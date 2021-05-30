#include <Arduino_FreeRTOS.h>

#include "task.h"
#include "timers.h"
#include "semphr.h"

//------------------------------------------------Radio includes



#include <TEA5767.h>
#include <Wire.h>

//Constants:
TEA5767 Radio;

#include <Servo.h>


//--------------------------------end of includes--------------------------------
TaskHandle_t xAllHandle = NULL;
TaskHandle_t xParking = NULL;


void all( void *pvParameters );

void parking( void *pvParameters );
void radio( void *pvParameters );
void fuelLevel( void *pvParameters );
void wipers( void *pvParameters );

const int pingPin1 = 10; // Trigger Pin of Ultrasonic Sensor
const int echoPin1 = 11; // Echo Pin of Ultrasonic Sensor
const int pingPin = 9; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 8; // Echo Pin of Ultrasonic Sensor

const int pingPinB = 26; // Trigger Pin of Ultrasonic Sensor
const int echoPinB = 24; // Echo Pin of Ultrasonic
long cmS1;
long cmS2;
long cmB;
// Motor A

int enA = 7;
int in1 = 5;
int in2 = 6;

// Motor B

int enB = 2;
int in3 = 3;
int in4 = 4 ;

int buzzer = 22;

bool park = false;
bool parked = false;
bool obst = false;

//---------------------------------------------------------------radio vars
double old_frequency;
double frequency;
int search_mode = 0;
int search_direction;
unsigned long last_pressed;
unsigned char buf[5];
int stereo;
int signal_level;
double current_freq;
unsigned long current_millis = millis();
int inByte;
int flag = 0;
int power = 12;

int prev = LOW;
int pause = LOW;
int play = LOW;
int next = LOW;

bool onOff = false;
//----------------------------------------------------------------------------------------------------












// fuel vars --------------------------start-----------------------------


int resval = 0;  // holds the value
int respin = A1; // sensor pin used
int d1 = 53;
int d2 = 52;
int d3 = 51;
int d4 = 50;
int d5 = 49;
int d6 = 48;
int d7 = 47;
//fuel vars ---------------------------------end



// wiper vars---------------------------------------start
Servo myservo;

int pos = 0;
int sensorValue = 0;



// wipers vars end------------------------------------------

void setUpForRadio() {
  pinMode(power, OUTPUT);
  digitalWrite(power, HIGH);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  xTaskCreate(parking, "parking", 1000, NULL, 3, NULL);
  xTaskCreate(radio, "radio", 1000, NULL, 1, NULL);
  xTaskCreate(fuelLevel, "fuel", 1000, NULL, 1, NULL);
  xTaskCreate(wipers, "wipers", 1000, NULL, 1, NULL);
  vTaskStartScheduler();
  //  xTaskCreate(parking, "parking", 1000, NULL, 2, xParking);

}
void wipers(void *pvParameters)  {
  myservo.attach(13);
  while (1)
  { sensorValue = analogRead(A0);
    Serial.println(sensorValue);
    if (sensorValue > 800) {
      myservo.write(180);

      Serial.print("    NO RAIN ");
      vTaskDelay(pdMS_TO_TICKS(1000));
      // delay(1000);
    }
    if (sensorValue <= 800 && sensorValue > 600) {
      Serial.print("AMOUNT: LOW ");
      Serial.print("IT IS RAINING   ");
      for (pos = 180; pos >= 0; pos -= 1) {
        myservo.write(180);
        //  delay(3);
        vTaskDelay(pdMS_TO_TICKS(3));
      }
      for (pos = 0; pos <= 180; pos += 1) {
        myservo.write(pos);
        vTaskDelay(pdMS_TO_TICKS(3));
        //delay(3);
      }
      vTaskDelay(pdMS_TO_TICKS(2000));
      //delay(2000);
    }

    if (sensorValue <= 600 && sensorValue > 460) {
      Serial.print("IT IS RAINING   ");
      Serial.print("AMOUNT: MEDIUM ");
      for (pos = 180; pos >= 0; pos -= 1) {
        myservo.write(180);
        vTaskDelay(1);
        //delay(3);
      }
      for (pos = 0; pos <= 180; pos += 1) {
        myservo.write(pos);
        vTaskDelay(1);
        // delay(3);
      }
      vTaskDelay(4);
      // delay(1000);
    }

    if (sensorValue < 460) {
      Serial.print("IT IS RAINING   ");
      Serial.print("AMOUNT: HIGH    ");
      for (pos = 180; pos >= 0; pos -= 1) {
        myservo.write(pos);
        vTaskDelay(1);
        //delay(3);
      }
      for (pos = 0; pos <= 180; pos += 1) {
        myservo.write(pos);
        vTaskDelay(1);
        //delay(3);
      }
      vTaskDelay(2);
      //delay(100);
    }
 vTaskDelay(1);
  }
  
}
void fuelLevel(void *pvParameters)  {
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(d5, OUTPUT);
  pinMode(d6, OUTPUT);
  pinMode(d7, OUTPUT);
  while (1)
  {


    resval = analogRead(respin); //Read data from analog pin and store it to resval variable
    Serial.print(resval);

    if (resval <= 500) {
      Serial.println("Water Level: Empty");

      digitalWrite(d4, HIGH);
      digitalWrite(d6, HIGH);
      digitalWrite(d7, LOW);
      digitalWrite(d5, HIGH);
      digitalWrite(d3, HIGH);
      digitalWrite(d1, HIGH);
      digitalWrite(d2, HIGH);
    } else if (resval > 500 && resval <= 555) {

      digitalWrite(d4, LOW);
      digitalWrite(d6, LOW);
      digitalWrite(d7, LOW);
      digitalWrite(d5, LOW);
      digitalWrite(d3, LOW);
      digitalWrite(d1, HIGH);
      digitalWrite(d2, HIGH);

      Serial.println("Water Level: Low");
    } else if (resval > 555 && resval <= 650) {
      digitalWrite(d4, HIGH);
      digitalWrite(d6, HIGH);
      digitalWrite(d7, HIGH);
      digitalWrite(d5, LOW);
      digitalWrite(d3, HIGH);
      digitalWrite(d1, HIGH);
      digitalWrite(d2, LOW);

      Serial.println("Water Level: Medium");
    } else if (resval > 650) {
      digitalWrite(d4, HIGH);
      digitalWrite(d6, HIGH);
      digitalWrite(d7, HIGH);
      digitalWrite(d5, HIGH);
      digitalWrite(d3, HIGH);
      digitalWrite(d1, LOW);
      digitalWrite(d2, LOW);
      Serial.println("Water Level: High");
    }
    //delay(1000);
    vTaskDelay(pdMS_TO_TICKS(1000));



  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
int co = 0;
void requestRadio() {

  Wire.requestFrom(5, 1);
  while (Wire.available()) {
    char c = Wire.read();
    if (c == '1') {
      Serial.println(c);
      prev = HIGH;
      pause = LOW;
      play = LOW;
      next = LOW;
      inByte = '-';
      //      if(onOff){
      //
      //        highLow=HIGH;
      //        onOff=!onOff;
      //      }else{
      //
      //          highLow=LOW;
      //        onOff=!onOff;
      //      }

    }

    if (c == '2') {
      Serial.println(c);
      prev = LOW;
      pause = HIGH;
      play = LOW;
      next = LOW;
      digitalWrite(power, LOW);
    }
    if (c == '3') {
      Serial.println(c);
      prev = LOW;
      pause = LOW;
      play = HIGH;
      next = LOW;
      digitalWrite(power, HIGH);
    }
    if (c == '4') {
      Serial.println(c);
      prev = LOW;
      pause = LOW;
      play = LOW;
      next = HIGH;
      inByte = '+';
    }

  }



}
void sendToSlave() {
  if (Radio.read_status(buf) == 1) {
    current_freq =  floor (Radio.frequency_available (buf) / 100000 + .5) / 10;
  }
  Wire.beginTransmission(5);
  double f = current_freq;
  // Serial.println(f);
  String temp = String(f) + "0";
  // Serial.println(temp.charAt(0));
  Wire.write(temp.charAt(0));
  Wire.write(temp.charAt(1));
  Wire.write(temp.charAt(2));
  Wire.write(temp.charAt(3));
  Wire.write(temp.charAt(4));

  Wire.endTransmission();

}
void radio(void *pvParameters)  {
  setUpForRadio();
  Radio.init();
  Radio.set_frequency(104.2); //On power on go to station 95.2
  Wire.begin();


  while (1)
  {

    if (inByte != 'x') {
      //inByte = Serial.read();

      if (inByte == '+' || inByte == '-') { //accept only + and - from keyboard
        flag = 0;
      }
    }


    if (Radio.read_status(buf) == 1) {
      current_freq =  floor (Radio.frequency_available (buf) / 100000 + .5) / 10;
      stereo = Radio.stereo(buf);
      signal_level = Radio.signal_level(buf);
      //By using flag variable the message will be printed only one time.
      if (flag == 0) {
        Serial.print("Current freq: ");
        Serial.print(current_freq);
        Serial.print("MHz Signal: ");
        //Strereo or mono ?
        if (stereo) {
          Serial.print("STEREO ");
        }
        else {
          Serial.print("MONO ");
        }
        Serial.print(signal_level);
        Serial.println("/15");
        flag = 1;
      }
    }

    //When button pressed, search for new station
    if (search_mode == 1) {
      if (Radio.process_search (buf, search_direction) == 1) {
        search_mode = 0;
      }
    }
    //If forward button is pressed, go up to next station
    if (inByte == '+') {
      last_pressed = current_millis;
      search_mode = 1;
      search_direction = TEA5767_SEARCH_DIR_UP;
      Radio.search_up(buf);
      sendToSlave();
    }
    //If backward button is pressed, go down to next station
    if (inByte == '-') {
      last_pressed = current_millis;
      search_mode = 1;
      search_direction = TEA5767_SEARCH_DIR_DOWN;
      Radio.search_down(buf);
      sendToSlave();
    }
    current_freq =  floor (Radio.frequency_available (buf) / 100000 + .5) / 10;
    String channelfreq = "";
    channelfreq = channelfreq + current_freq;
    //Serial.print(channelfreq+":message");

    inByte = 'x';
    requestRadio();
    vTaskDelay(pdMS_TO_TICKS(100));
    //delay(100);


  }
}
void parking(void *pvParameters)  {
  pinMode(buzzer, OUTPUT);

  // Set all the motor control pins to outputs
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  TickType_t xLastWakeTime ;
  xLastWakeTime = xTaskGetTickCount ( ) ;

  while (1)
  {
    buzzerSensor();

    if (!parked) {
      checkParking();

    }


    if (!parked) {
      if (!park) {
        sensor1();
        sensor();
        if (cmB < 40) {
          Serial.print("STOP");

          digitalWrite(in1, LOW);
          digitalWrite(in2, LOW);
          digitalWrite(in3, LOW);
          digitalWrite(in4, LOW);
          digitalWrite(buzzer, HIGH);
          //delay(500);
          vTaskDelay(pdMS_TO_TICKS(500));

          digitalWrite(buzzer, LOW);
          //delay(500);
          vTaskDelay(pdMS_TO_TICKS(500));
          digitalWrite(buzzer, HIGH);
          //delay(500);
          vTaskDelay(pdMS_TO_TICKS(500));
          digitalWrite(buzzer, LOW);

        } else {
          move();

        }

      } else {
        moveBack();
        demoPark();
        digitalWrite(buzzer, LOW);

        parked = true;
      }
    } else {
      if (cmB < 40) {
        digitalWrite(buzzer, HIGH);
        //delay(30);
        vTaskDelay(pdMS_TO_TICKS(30));
        digitalWrite(buzzer, LOW);
        vTaskDelay(pdMS_TO_TICKS(cmB * cmB));
        //delay(cmB * cmB);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void move()

{

  // This function will run the motors in both directions at a fixed speed

  // Turn on motor A

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  // Set speed to 200 out of possible range 0~255

  analogWrite(enA, 120);

  // Turn on motor B

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  // Set speed to 200 out of possible range 0~255

  analogWrite(enB, 120);



  // Now turn off motors



}

void demoTwo()

{

  // This function will run the motors across the range of possible speeds
  // Note that maximum speed is determined by the motor itself and the operating voltage

  // Turn on motors

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  // Accelerate from zero to maximum speed

  for (int i = 0; i < 256; i++)

  {

    analogWrite(enA, i);
    analogWrite(enB, i);

    //delay(20);
    vTaskDelay(pdMS_TO_TICKS(20));

  }

  // Decelerate from maximum speed to zero

  for (int i = 255; i >= 0; --i)

  {

    analogWrite(enA, i);
    analogWrite(enB, i);

    //  delay(20);
    vTaskDelay(pdMS_TO_TICKS(20));

  }

  // Now turn off motors

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

}
void demoPark()

{

  // This function will run the motors in both directions at a fixed speed


  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  analogWrite(enA, 150);

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  analogWrite(enB, 150);
  vTaskDelay(pdMS_TO_TICKS(700));
  //delay(700);//clock

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  vTaskDelay(pdMS_TO_TICKS(750));
  //delay(750);

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  vTaskDelay(pdMS_TO_TICKS(600));
  // delay(600);//rev
  //  digitalWrite(in1, LOW);
  //  digitalWrite(in2, LOW);
  //  digitalWrite(in3, LOW);
  //  digitalWrite(in4, LOW);
  //  delay(400);

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  vTaskDelay(pdMS_TO_TICKS(550));
  // delay(550);//anti
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  vTaskDelay(pdMS_TO_TICKS(300));
  //delay(300);



}

void moveBack()

{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  // Set speed to 200 out of possible range 0~255

  analogWrite(enA, 150);

  // Turn on motor B

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  // Set speed to 200 out of possible range 0~255

  analogWrite(enB, 150);
  vTaskDelay(pdMS_TO_TICKS(300));
  //delay(300);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  // Now turn off motors



}

long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
void sensor1() {
  long duration, inches, cm;
  pinMode(pingPin1, OUTPUT);
  digitalWrite(pingPin1, LOW);
  vTaskDelay(pdMS_TO_TICKS(2));
  //delayMicroseconds(2);
  digitalWrite(pingPin1, HIGH);
  vTaskDelay(pdMS_TO_TICKS(10));
  //delayMicroseconds(10);
  digitalWrite(pingPin1, LOW);
  pinMode(echoPin1, INPUT);
  duration = pulseIn(echoPin1, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  Serial.print("Sensor 2, ");
  cmS2 = cm;

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  vTaskDelay(pdMS_TO_TICKS(100));
  // delay(100);
}

void sensorB() {
  long duration, inches, cm;
  pinMode(pingPinB, OUTPUT);
  digitalWrite(pingPinB, LOW);
  vTaskDelay(pdMS_TO_TICKS(2));
  //delayMicroseconds(2);

  digitalWrite(pingPinB, HIGH);
  vTaskDelay(pdMS_TO_TICKS(10));
  //delayMicroseconds(10);
  digitalWrite(pingPinB, LOW);
  pinMode(echoPinB, INPUT);
  duration = pulseIn(echoPinB, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  //Serial.print("Sensor Buzzer, ");
  cmB = cm;

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  vTaskDelay(pdMS_TO_TICKS(100));
  // delay(100);
}

void sensor() {
  long duration, inches, cm;

  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);

  vTaskDelay(pdMS_TO_TICKS(2));
  //delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);

  vTaskDelay(pdMS_TO_TICKS(10));
  // delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  cmS1 = cm;
  Serial.print("Sensor 1, ");

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  vTaskDelay(pdMS_TO_TICKS(100));
  //delay(100);
}

void checkParking() {
  if (cmS1 > 15 && cmS2 > 15) {
    Serial.print("BARKING");
    Serial.println();
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    park = true;
    digitalWrite(buzzer, HIGH);
    vTaskDelay(pdMS_TO_TICKS(2000));
    //delay(2000);
    digitalWrite(buzzer, LOW);

  }
}

void buzzerSensor() {
  sensorB();


}
