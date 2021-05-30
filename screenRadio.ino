/*
  TFT LCD - TFT Simple driving
  modified on 21 Feb 2019
  by Saeed Hosseini
  https://electropeak.com/learn/
*/

#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include "TouchScreen.h"
#include <stdint.h>

#include<Wire.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x07FF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF

#define YP A2
#define XM A3
#define YM 8
#define XP 9

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
String channel="";
int s=0;
void setup() {

  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));

#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.4\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 2.4\" TFT Breakout Board Pinout"));
#endif
  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();



  tft.begin(identifier);

  tft.fillScreen(RED);

  tft.setCursor(90, 20);
  tft.setTextSize(2);
  tft.println("prev");
  tft.setCursor(90, 100);
  tft.setTextSize(2);
  tft.println("Pause");
  tft.setCursor(90, 180);
  tft.setTextSize(2);
  tft.println("Play");
  tft.setCursor(90, 260);
  tft.setTextSize(2);
  tft.println("Next");
   //tft.setCursor(90, 260);
  //tft.setTextSize(2);
 // tft.println(channel);
  setUpCommunication();

}

void setUpCommunication() {
  
  Wire.begin(5);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void receiveEvent(int howMany)
{

  char x=(char)Wire.read();
  char x1=(char)Wire.read();
  char x2=(char)Wire.read();
  char x3=(char)Wire.read();
  char x4=(char)Wire.read();
  
  
  // s=x;
    tft.fillRoundRect(30, 250, 60, 65, 0, RED);
//count++;
  tft.setCursor(30, 250);
  tft.setTextSize(2);
  tft.println(String(x)+String(x1)+String(x2)+String(x3)+String(x4));
   //Serial.println(s);
          // print the integer
}

void requestEvent() {
  TSPoint p = ts.getPoint();
  int x=p.x;
  int y=p.y;
  
  //Serial.println(x);
  if (p.z > ts.pressureThreshhold) {
    if(x>=100 && x<=250){
        Wire.write('1');
    }
    if(x>250 && x<=500){
      Serial.println("-----here----");
      Wire.write('2');
    }
     if(x>520 && x<=700){
      Serial.println("-----here----");
      Wire.write('3');
    }
     if(x>720 ){
      Serial.println("-----here----");
      Wire.write('4');
    }
  
  }
}
int count=0;
void loop() {
 

  if(count==0){
    count=-2;
    Wire.write('1');
  }

//Serial.println(s);

  
  
  
   TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);     //.kbv these pins are shared with TFT
        pinMode(XM, OUTPUT); 
  if (p.z > ts.pressureThreshhold) {
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);
  }
  //channel="";

  delay(100);
}
