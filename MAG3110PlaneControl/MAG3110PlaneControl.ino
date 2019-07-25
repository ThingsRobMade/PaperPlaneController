/*  *********************************************
 *   MAG3110PlaneControl.ino
 *   A quick re-coding of the SparkFun 9DoF sketch 
 *   to control a pair of servos.
 *   This one uses the MAG3110 Magnetometer, and
 *   the SparkFun Logic Level Translator.
 *   July, 2019, Rob Reynolds @ SparkFun Electronics.
 *   
 *   Based on the code originally written by
 *   George B. on behalf of SparkFun Electronics
 *   Created: Sep 22, 2016
 *   Updated: n/a
 *  
 *   This code is beerware; if you see me (or any other SparkFun employee) at the
 *   local, and you've found our code helpful, please buy us a round!
 *   Distributed as-is; no warranty is given.
 *   *********************************************/

#include <SparkFun_MAG3110.h>

MAG3110 mag = MAG3110(); //Instantiate MAG3110

#include "FastLED.h"

#define NUM_LEDS 10

#define DATA_PIN 2

CRGB leds[NUM_LEDS];

int b = 0;

#include <Servo.h>

Servo servoX; //Create object on X access
Servo servoY; //Create object on Y access

int xVal;
int yVal;

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  servoX.attach(8);
  servoY.attach(9);

  mag.initialize(); //Initializes the mag sensor
  mag.start();      //Puts the sensor in active mode
}

void loop() {

  if (b > NUM_LEDS || b < 0) b = 0  ;

  int x, y, z;
  //Only read data when it's ready
  if(mag.dataReady()) {
    //Read the data
    mag.readMag(&x, &y, &z);
  
    Serial.print("X: ");
    Serial.print(x);
    xVal = map(x, 0, 600, 55, 125);
    servoX.write(xVal);
    Serial.print(", Y: ");
    Serial.print(y);
    yVal = map(y, -650, -280, 90, 125);
    servoY.write(yVal);
    Serial.print(", Z: ");
    Serial.println(z);
  
    Serial.println("--------");

    leds[b] = CRGB::LightBlue;
    FastLED.show();
    leds[b] = CRGB::Black;

    b = b + 1;

    delay(30);

  }

}
