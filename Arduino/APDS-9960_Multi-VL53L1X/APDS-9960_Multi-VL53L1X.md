## Topic ##
This example uses three VL53L1X sensors and one APDS-9960 sensor connected to an Arduino Nano v3. Other controllers will work as well only the wireing may be different. The Arduino Nano is connected to an Raspberry Pi via USB cable.
The goal is to transmit the gestures recognized by the APDS-9960 sensor to the Raspberry via serial console. Also if the distance messured by one VL53L1X sensor is smaller than a threshold (i.e. because a hand is in front of the sensor) an hit message containing the sensor id should be send to the Raspberry Pi.

## Preperation ##

### Raspberry Pi ###
#### User groups ####
The user "pi" needs to be in the group "dialout" to be able to access the serial interface.
You can check this with the command "groups" and the output should look like the following and contain "dialout":

    pi@MagicMirror:~ $ groups
    pi adm dialout cdrom sudo audio video plugdev games users input netdev gpio i2c spi

#### Udev rules ####
I use more than one Arduino board at my Raspberry at the same time and want each board to get an alias that does not change when connected to the Pi. To get this i use udev rules to create the alias automatically eacht time the boards get connected.

If you connect your Arduino to the Raspberry and run the "dmesg" command afterwards you can check which new device has been created (normally /dev/ttyACMXX for big Arduinos[Uno, Mega] and /dev/ttyUSBXX for the smaller ones).

You then can check the details of the device with the command where DEVICE is the id from above

    udevadm info --name=/dev/DEVICE --attribute-walk


Create a file "/etc/udev/rules.d/99-arduino.rules" with a text editor of your choise and sudo
    sudo nano /etc/udev/rules.d/99-arduino.rules

and use the lines below as templates for your one boards

    SUBSYSTEMS=="usb",KERNEL=="ttyACM*", ATTRS{serial}=="SERIAL_NUMBER", SYMLINK+="ard_mega"
    SUBSYSTEMS=="usb",KERNEL=="ttyACM*", ATTRS{serial}=="SERIAL_NUMBER", SYMLINK+="ard_uno"
    SUBSYSTEMS=="usb",KERNEL=="ttyUSB*", KERNELS=="1-1.1", SYMLINK+="ard_nano_1"
    SUBSYSTEMS=="usb",KERNEL=="ttyUSB*", KERNELS=="1-1.2", SYMLINK+="ard_nano_2"

Because my Nano boards all do have the same serial number i use the second two lines to create an alias based on the usb port number they are connected to.


### Arduino IDE ###
I assume that you already installed the Arduino IDE. If not follow the informations on https://www.arduino.cc/en/main/software.

#### APDS-9660 Library ####
I use the APDS Library of SparkFun (https://github.com/sparkfun/SparkFun_APDS-9960_Sensor_Arduino_Library). Follow the instractions on GitHub to install the library.

Because i do not use an original SparkFun Sensor i needed to modify some things in the library.
Open the ".h"-file of the library usally located in (~/Arduino/libraries/SparkFun_APDS-9960_Sensor_Arduino_Library-XXXX/src/SparkFun_APDS9960.h) where XXXX is the version number.

##### GGAIN #####
The GGAIN value controls Most of the third party sensors do not support GGAIN values of 4X, some only support 1X. I could not get any single gesture without changing this value.

Change to line

    #define DEFAULT_GGAIN           GGAIN_4X

to

    #define DEFAULT_GGAIN           GGAIN_1X


##### GLDRIVE #####
The GLDRIVE value controls the power of the led that is used to recognize the gestures. You can control the sensivity of the sensor with this definition. I reduced the power to reduce the count of "wrong" interrupts.

Change the line

    #define DEFAULT_GLDRIVE         LED_DRIVE_100MA

to

    #define DEFAULT_GLDRIVE         LED_DRIVE_50MA


#### VL53L1X ####
I use sensors of Pololu and the library for them can be installed with the library mangement of the Arduino IDE or via https://github.com/pololu/vl53l1x-arduino. I do not know if third party sensors do support all the features like setting the I2C adress!


## Wireing ##
I soldered all my stuff but you can surely use a breadboard as well. The following picture shows my circuit.

![alt text](https://github.com/Tom-Hirschberger/MMM-Serial-Notifications/blob/master/Arduino/APDS-9960_Multi-VL53L1X/APDS-9960_Multi-VL53L1X.jpg "APDS-9960_Multi-VL53L1X.jpg")


## Programming the ino ##

### VL53L1X ###
I my case i use three VL53L1X sensors. You can use less or more as well. The example ino (https://github.com/Tom-Hirschberger/MMM-Serial-Notifications/blob/master/Arduino/APDS-9960_Multi-VL53L1X/APDS-9960_Multi-VL53L1X.ino) defines some simple values which control how many sensors are connected.
It assumes that you connect the XSHUT pins to consecutive digital pins of the Arduino board (D4, D5, D6, etc.)

    #define SENSOR_CNT 3 //How many sensors are connected
    #define XSHUT_START_PIN 4 //we start with pin 4 because 2 and 3 are interrupt pins (on the nano)
    #define SENSORS_START_ADDRESS 53 //0x35 next sensor has 0x36, ...
    #define DISTANCE_THRESHOLD 200 //if object is closer than this millimeters an hit will be signaled
    #define DISTANCE_MODE VL53L1X::Long //the sensor supports Short, Mid, Long mode
    #define TIMING_BUDGET 50000
    #define TIMING_CONTINOUES 50
    #define TIMEOUT 100
    #define VL53L1X_DEBOUNCE 1000 //after a hit distance is messarued after this time of milliseconds again


### APDS-9960 ###
The interrupt pin of the sensor can be configured by changing the value of 

    #define APDS9960_INT    2 // Needs to be an interrupt pin


## CONCLUSION ##
If everything went well we do no get gestures and hits transmitted on the serial interface and can react to them on the Raspberry Pi.
If a hit happend for a VL53L1X sensor hits during the DEBOUNCE period will be ignored.