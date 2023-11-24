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
// if you use a third party sensor (not SPARKFUN) make sure to set the GGAIN value to GGAIN_1X
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
int gesture_ready = false;
int trigger_one = 3;
int echo_one = 4;
int trigger_two = 5;
int echo_two = 6;
int max_distance_one = 20;
int max_distance_two = 20;
unsigned long last_dist_check = millis();
int ms_between_dist_check = 500;
int delay_after_dist_match = 500;

// setup function is called on boot
void setup() {
  Serial.begin(9600);

  if(APDS9960_INT > 0){
    // init gesture sensor

    // Set interrupt pin as input for gesture sensor
    pinMode(APDS9960_INT, INPUT);
    // Initialize gesture sensor APDS-9960 (configure I2C and initial values)
    if (gestureSensor.init()) {
      
      Serial.println(F("INFO: APDS-9960 initialization complete"));
    
      attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
    
      if (gestureSensor.enableGestureSensor(true)) {
        Serial.println(F("INFO: Gesture sensor is now running"));
      } else {
        Serial.println(F("ERROR: Something went wrong during gesture sensor enable!"));
      }
      
      gesture_ready = true;
    } else {
      Serial.println(F("ERROR: Something went wrong during APDS-9960 init!"));
    }
  }

  if(trigger_one > 0){
    pinMode(trigger_one, OUTPUT);
    pinMode(echo_one, INPUT);
    digitalWrite(trigger_one, HIGH);
  }

  if(trigger_two > 0){
    pinMode(trigger_two, OUTPUT);
    pinMode(echo_two, INPUT);
    digitalWrite(trigger_two, HIGH);
  }
}

// main loop that is called repeatedly after setup
void loop() {
  if (gesture_ready){
    // check if there was an interrupt in the meantime
    handleInterrupt();
  } else {
    setup();
    delay(1000);
  }

  if(millis() > (last_dist_check + ms_between_dist_check)){
    if(trigger_one > 0){
      int cur_dist_one = getDistanceGD(trigger_one,echo_one);
      if(cur_dist_one <= max_distance_one ){
        Serial.println(F("In_Distance (1)"));
        delay(delay_after_dist_match);
      }
    }
    
    if(trigger_two > 0){
      int cur_dist_two = getDistanceGD(trigger_two,echo_two);
      if(cur_dist_two <= max_distance_two ){
        Serial.println(F("In_Distance (2)"));
        delay(delay_after_dist_match);
      }
    }
  }
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


int getDistance(int trigger, int echo){ 
  long dist=0;
  long cur_time=0;
  
  digitalWrite(trigger, LOW); 
  delayMicroseconds(3);
  noInterrupts();
  digitalWrite(trigger, HIGH); //Trigger Impuls 10 us
  delayMicroseconds(10);
  digitalWrite(trigger, LOW); 
  cur_time = pulseIn(echo, HIGH); // Echo-Zeit messen
  interrupts(); 
 
  cur_time = (cur_time/2); // Zeit halbieren
  dist = cur_time / 29.1; // Zeit in Zentimeter umrechnen
  return(dist); 
}


int getDistanceGD(int trigger, int echo){ 
  int old=0;
  int mid;
  int dist;
  int i;

  delay(10);
  old=getDistance(trigger,echo);
  delay(10);
  for (i=0; i<10; i++)
  {
    dist=getDistance(trigger,echo);
    mid=(0.8*old) + (0.2*dist);
    old=mid;
    delay(10);
  }
  return (mid);
}
