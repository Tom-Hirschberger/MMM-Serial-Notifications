# MMM-Serial-Notifications
MMM-Serial-Notifications is a module for the [MagicMirror](https://github.com/MichMich/MagicMirror) project by [Michael Teeuw](https://github.com/MichMich).

It watches the serial consoles and sends notifications based on the received messages;

This module is inspired by https://github.com/thobach/MMM-Gestures. This module uses a newer version of the "serialport" library and does not need the additional webserver anymore!

The Arduino directory contains examples i had written for my Arduino UNO R3 and my Arduino Mega 2560; the current examples use the APDS-9960 and HC-SR04 sensors to send messages which then will be watched and result in notifications.

## Installation
```sh
  cd ~/MagicMirror/modules
  git clone git@github.com:Tom-Hirschberger/MMM-Serial-Notifications.git
  cd MMM-Serial-Notifications
  npm install
```


## Configuration
  To use the module insert something like this into the config.js file:
```js
  {
    module: 'MMM-Serial-Notifications',
    config: {
      devices: {
        '/dev/ttyACM0': {
          messages: {
            'Gesture: UP': [
              {
                notification: 'VOLUME_UP',
                payload: {
                  step: 5
                }
              },
              {
                notification: 'USER_PRESENCE',
                payload: true
              }
            ],
          'Gesture: DOWN': [
              {
                notification: 'VOLUME_DOWN',
                payload: {
                step: 2
                }
              },
              {
                notification: 'USER_PRESENCE',
                payload: true
              }
            ]
          }
        }
      }
    }
  },
```

| Option  | Description | Type | Default
|-------- | ----------- | ---- | -------
| devices | An Object containing the serial devices, the messages and notifications | Object | {}
