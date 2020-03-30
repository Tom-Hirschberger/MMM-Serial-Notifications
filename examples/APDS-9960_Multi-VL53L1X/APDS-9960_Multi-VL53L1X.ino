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

#include <VL53L1X.h>
#define SENSOR_CNT 3
#define XSHUT_START_PIN 4 //we start with pin 4 because 2 and 3 are interrupt pins (on the nano)
#define SENSORS_START_ADDRESS 53 //0x35 next sensor has 0x36, ...
#define DISTANCE_THRESHOLD 200 //if object is closer than this millimeters an hit will be signaled
#define DISTANCE_MODE VL53L1X::Long //the sensor supports Short, Mid, Long mode
#define TIMING_BUDGET 50000
#define TIMING_CONTINOUES 50
#define TIMEOUT 100
#define VL53L1X_DEBOUNCE 1000 //after a hit distance is messarued after this time of milliseconds again
VL53L1X sensors[SENSOR_CNT];
unsigned long sensorsLastHit[SENSOR_CNT];


/*
  Magic Mirror 2.0
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
#define APDS9960_DEBOUNCE 500 //Time between two accepted interrupts
#define APDS9960_AMBIENTLIGHT_INTERVAL 10000 //Time between two accepted interrupts
unsigned long APDS9960LastHit = 0;
unsigned long APDS9960LastAmbientLight = 0;
uint16_t APDS9960AmbientLight = 0;

// GLOBAL VARIABLES

// gesture sensor
SparkFun_APDS9960 gestureSensor = SparkFun_APDS9960();


// CONSTANTS

// STATE

// interrupt was received and gesture should be read from sensor
int isr_flag = 0;
int gesture_ready = false;

// setup function is called on boot
void setup() {
  for (int i = 0; i < SENSOR_CNT; i++)
  {
    sensors[i] = VL53L1X();
    sensorsLastHit[i] = 0;
    pinMode(XSHUT_START_PIN + i, OUTPUT);
    digitalWrite(XSHUT_START_PIN + i, LOW);
  }
  
   // Initalisiert I2C
  delay(500);
  Wire.begin();
  Wire.beginTransmission(0x29);
  
  Serial.begin(9600);

  for (int i = 0; i < SENSOR_CNT; i++)
  {
    digitalWrite(XSHUT_START_PIN + i,HIGH);
    delay(500);
    sensors[i].init();
    Serial.print("VL53L1X");
    Serial.print(i);
    Serial.println(" - SetAdress");
    delay(500);
    sensors[i].setAddress(SENSORS_START_ADDRESS+i);
    Serial.print("VL53L1X");
    Serial.print(i);
    Serial.println(" - SetAdress finished");
  }

  for (int i = 0; i < SENSOR_CNT; i++)
  {
    sensors[i].setDistanceMode(DISTANCE_MODE);
    sensors[i].setMeasurementTimingBudget(TIMING_BUDGET);
    sensors[i].startContinuous(TIMING_CONTINOUES);
    sensors[i].setTimeout(TIMEOUT);
  }

  if(APDS9960_INT > 0){
    // init gesture sensor

    // Set interrupt pin as input for gesture sensor
    pinMode(APDS9960_INT, INPUT);
    // Initialize gesture sensor APDS-9960 (configure I2C and initial values)
    if (gestureSensor.init()) {
      
      Serial.println(F("INFO: APDS-9960 initialization complete"));

      gestureSensor.enableLightSensor(false);
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

  
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;


  for (byte i = 1; i < 120; i++)
  {

    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
    } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}

// main loop that is called repeatedly after setup
void loop() {
  unsigned long curTime = millis();
  for (int i = 0; i < SENSOR_CNT; i++)
  {
    if ((curTime - sensorsLastHit[i]) > VL53L1X_DEBOUNCE){
      int curValue = sensors[i].read(true);
      if (curValue <= DISTANCE_THRESHOLD){
        Serial.print("VL53L1X");
        Serial.print(i);
        if (sensors[i].timeoutOccurred()) { 
          Serial.print(": TIMEOUT"); 
        } else {
          Serial.print(": HIT");
          sensorsLastHit[i] = curTime;
        }
        Serial.println();
      } 
      
      delay(200);
    }
  }  
  
  if (gesture_ready){
    unsigned long curTime = millis();
    if((curTime - APDS9960LastAmbientLight) > APDS9960_AMBIENTLIGHT_INTERVAL){
      if(!gestureSensor.readAmbientLight(APDS9960AmbientLight)){
        Serial.print("Problem with AmbientLight");
      } else {
        Serial.print("AmbientLight: ");
        Serial.println(APDS9960AmbientLight);
        APDS9960LastAmbientLight = curTime;
      }
    }
    // check if there was an interrupt in the meantime
    handleInterrupt();
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
  unsigned long curTime = millis();
  if ((curTime - APDS9960LastHit) > APDS9960_DEBOUNCE){
    isr_flag = 1;  
  }
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
