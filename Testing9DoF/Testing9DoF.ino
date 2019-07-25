/****************************************************************
 * Testing 9DoF.ino
 * ICM 20948 Arduino Library Demo 
 * Use the default configuration to stream 9-axis IMU data
 * and control a pair of servos based on the
 * position of the breakout board.
 * Rob Reynolds @ SparkFun Electronics
 * July, 2019
 * Based on the examole by Owen Lyke @ SparkFun Electronics
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * 
 * Distributed as-is; no warranty is given.
 ***************************************************************/
#include "ICM_20948.h"  // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU

#include "FastLED.h"   // Library fof the RGB LED strip

#define NUM_LEDS 10   // Define the number of LEDs in your strip

#define DATA_PIN 2    // Define the pin which received data from the 9Dof

CRGB leds[NUM_LEDS];

int b = 0;  // Used this for looping the LEDs

//#define USE_SPI       // Uncomment this to use SPI

#define SERIAL_PORT Serial

#define SPI_PORT SPI    // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN 2        // Which pin you connect CS to. Used only when "USE_SPI" is defined

#define WIRE_PORT Wire  // Your desired Wire port.      Used when "USE_SPI" is not defined
#define AD0_VAL   1     // The value of the last bit of the I2C address. 
                        // On the SparkFun 9DoF IMU breakout the default is 1, and when 
                        // the ADR jumper is closed the value becomes 0

#ifdef USE_SPI
  ICM_20948_SPI myICM;  // If using SPI create an ICM_20948_SPI object
#else
  ICM_20948_I2C myICM;  // Otherwise create an ICM_20948_I2C object
#endif

#include <Servo.h>

Servo servoX; //Create object on X access
Servo servoY; //Create object on Y access

int xVal;  // Variable for the X-axis servo's position
int yVal;  // Variable for the Y-axis servo's position

int ledPin = 13;  // Not necessary in this code

void setup() {

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  servoX.attach(8);  // Define the pin used for the X-axis servo
  servoY.attach(9);  // Define the pin used for the Y-axis servo

  SERIAL_PORT.begin(115200);
  while(!SERIAL_PORT){};
  
  bool initialized = false;
  while( !initialized ){

#ifdef USE_SPI
    SPI_PORT.begin();
    myICM.begin( CS_PIN, SPI_PORT ); 
#else
    WIRE_PORT.begin();
    WIRE_PORT.setClock(400000);
    myICM.begin( WIRE_PORT, AD0_VAL );
#endif

    SERIAL_PORT.print( F("Initialization of the sensor returned: ") );
    SERIAL_PORT.println( myICM.statusString() );
    if( myICM.status != ICM_20948_Stat_Ok ){
      SERIAL_PORT.println( "Trying again..." );
      delay(500);
    }else{
      initialized = true;
    }
  }
  pinMode(ledPin, OUTPUT);

}

void loop() {

  if (b > NUM_LEDS || b < 0) b = 0  ;
  
  if( myICM.dataReady() ){
    myICM.getAGMT();                // The values are only updated when you call 'getAGMT'
    printScaledAGMT( myICM.agmt);   // This function takes into account the sclae settings from when the measurement was made to calculate the values with units
    delay(30);

    leds[b] = CRGB::LightBlue;
    FastLED.show();
    leds[b] = CRGB::Black;

    b = b + 1;
  }
  else{
    Serial.println("Waiting for data");
    delay(500);
  }
  
}


// Below here are some helper functions to print the data nicely!

void printPaddedInt16b( int16_t val ){
  if(val > 0){
    SERIAL_PORT.print(" ");
    if(val < 10000){ SERIAL_PORT.print("0"); }
    if(val < 1000 ){ SERIAL_PORT.print("0"); }
    if(val < 100  ){ SERIAL_PORT.print("0"); }
    if(val < 10   ){ SERIAL_PORT.print("0"); }
  }else{
    SERIAL_PORT.print("-");
    if(abs(val) < 10000){ SERIAL_PORT.print("0"); }
    if(abs(val) < 1000 ){ SERIAL_PORT.print("0"); }
    if(abs(val) < 100  ){ SERIAL_PORT.print("0"); }
    if(abs(val) < 10   ){ SERIAL_PORT.print("0"); }
  }
  SERIAL_PORT.print(abs(val));
}


void printFormattedFloat(float val, uint8_t leading, uint8_t decimals){
  float aval = abs(val);
  if(val < 0){
    SERIAL_PORT.print("-");
  }else{
    SERIAL_PORT.print(" ");
  }
  for( uint8_t indi = 0; indi < leading; indi++ ){
    uint32_t tenpow = 0;
    if( indi < (leading-1) ){
      tenpow = 1;
    }
    for(uint8_t c = 0; c < (leading-1-indi); c++){
      tenpow *= 10;
    }
    if( aval < tenpow){
      SERIAL_PORT.print("0");
    }else{
      break;
    }
  }
  if(val < 0){
    SERIAL_PORT.print(-val, decimals);
  }else{
    SERIAL_PORT.print(val, decimals);
  }
}

void printScaledAGMT( ICM_20948_AGMT_t agmt){
  SERIAL_PORT.print(" Mag (uT) [ ");
  printFormattedFloat( myICM.magX(), 3, 2 );
  SERIAL_PORT.print(", ");
  printFormattedFloat( myICM.magY(), 3, 2 );
  SERIAL_PORT.print(", ");
  printFormattedFloat( myICM.magZ(), 3, 2 );
  SERIAL_PORT.print(" ]");
  
  //SERIAL_PORT.println();

/* Found the range being fed from the 9D0F, then used the
 *  map function to translate that into servo position.
 *  Example - on the X plane, the data ranged from 10 to -40,
 *  and I wanted my servo to rotate from 80° to 155°
 */

  xVal = myICM.magX();
  xVal = map(xVal, 10, -40, 80, 155);
  servoX.write(xVal);
  yVal = myICM.magY();
  yVal = map(yVal, 28, -46, 35, 130);
  servoY.write(yVal);

  SERIAL_PORT.print("  xVAL = ");
  SERIAL_PORT.print(xVal);
  SERIAL_PORT.print("  yVal = ");
  SERIAL_PORT.print(yVal);
  SERIAL_PORT.println();
 
  
 }
