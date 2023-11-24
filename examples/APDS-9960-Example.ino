/*
  Arduino Wire library, included by default
  https://www.arduino.cc/en/Reference/Wire
*/ 
#include <Wire.h>

/*
  SparkFun_APDS9960 library for APDS-9960 gesture sensor, taken from
  https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor
*/
#include <SparkFun_APDS9960.h>

/*
  MagicMirror²

  Collects gesture events from gesture sensor APDS-9960 which are then forwarded on the serial port as text.

  The circuit for an Arduino Uno:
  * input 1: APDS-9960 on digital pin 2 (interrupt) + I2C (SDA on pin A4, SCL on pin A5) + GND & VCC (3.3V)
  * output: serial out on USB
  
  In order to compile you'll need to copy the libraries from above to Arduino's library folder.

  Created 2019-02-28 by Tom Hirschberger (https://github.com/Tom-Hirschberger)

  License: MIT

*/

// LIBRARY MODIFICATIONS

// Caution: You'll need to overwrite the fowlloing in the SparkFun_APDS9960 lib to
// prevent gesture sensor interrupt when just standing in front of mirror, this will
// reduce infrared LED power:
// #define DEFAULT_GGAIN           GGAIN_2X // was: GGAIN_4X
// #define DEFAULT_GLDRIVE         LED_DRIVE_50MA // was: LED_DRIVE_100MA


// PINS

// gesture sensor interrup pin
#define APDS9960_INT    2 // Needs to be an interrupt pin

// GLOBAL VARIABLES

// gesture sensor
SparkFun_APDS9960 gestureSensor = SparkFun_APDS9960();


// CONSTANTS

// STATE

// interrupt was received and gesture should be read from sensor
int isr_flag = 0;

// setup function is called on boot
void setup() {

  // init gesture sensor

  // Set interrupt pin as input for gesture sensor
  pinMode(APDS9960_INT, INPUT);

  Serial.begin(9600);

  // Initialize gesture sensor APDS-9960 (configure I2C and initial values)
  if (gestureSensor.init()) {

    Serial.println(F("INFO: APDS-9960 initialization complete"));

    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);

    if (gestureSensor.enableGestureSensor(true)) {
      Serial.println(F("INFO: Gesture sensor is now running"));
    } else {
      Serial.println(F("ERROR: Something went wrong during gesture sensor enable!"));
    }

    } else {

      Serial.println(F("ERROR: Something went wrong during APDS-9960 init!"));
    }
}

// main loop that is called repeatedly after setup
void loop() {
  // check if there was an interrupt in the meantime
  handleInterrupt();
}

// check for interrupt, if one is present it means gesture is present,
// in that case stop interrupt handler, handle gesture and attach
// interrupt handler again
void handleInterrupt() {

  // if interrupt was set, read and print gesture, reset interrupt flag
  if (isr_flag == 1) {

    detachInterrupt(digitalPinToInterrupt(APDS9960_INT));

    handleGesture();

    isr_flag = 0;

    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);

  }

}

// interrupt function for interrupt pin that APDS9960 is attached to
void interruptRoutine() {

  isr_flag = 1;

}

// on gestor sensor interrupt send serial message with gesture
void handleGesture() {

  if (gestureSensor.isGestureAvailable()) {

    switch (gestureSensor.readGesture()) {
      case DIR_UP:
        Serial.println(F("Gesture: UP"));
        break;
      case DIR_DOWN:
        Serial.println(F("Gesture: DOWN"));
        break;
      case DIR_LEFT:
        Serial.println(F("Gesture: LEFT"));
        break;
      case DIR_RIGHT:
        Serial.println(F("Gesture: RIGHT"));
        break;
      case DIR_NEAR:
        Serial.println(F("Gesture: NEAR"));
        break;
      case DIR_FAR:
        Serial.println(F("Gesture: FAR"));
        break;
      default:
        Serial.println(F("Gesture: NONE"));
    }

  }

}
