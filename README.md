# MMM-Serial-Notifications
MMM-Serial-Notifications is a module for the [MagicMirror](https://github.com/MichMich/MagicMirror) project by [Michael Teeuw](https://github.com/MichMich).

It watches the serial consoles and sends notifications based on the received messages;

This module is inspired by https://github.com/thobach/MMM-Gestures. This module uses a newer version of the "serialport" library and does not need the additional webserver anymore!

The example directory contains some implementations i use for my arduino boards (i.e. APDS-9960, VL53L0X and HC-SR04 sensors).

In newer release also support for profiles has been included. You can add an profiles string to each notification. This makes it possible to use the same sensors for different purposes in different profiles. (i.e. use the APDS-9960 either to control the MMM-Spotify module and in a different page to control i.e. a radio plugin or something totally different).

## Installation
```sh
  cd ~/MagicMirror/modules
  git clone https://github.com/Tom-Hirschberger/MMM-Serial-Notifications.git
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
                profile: "pageOneEveryone pageTwoNobody"
                payload: {
                  step: 2
                }
              },
              {
                notification: 'DO_SOMETHING_ELSE',
                profile: "pageOneNobody"
                payload: {
                  step: 2
                }
              },
              {
                notification: 'USER_PRESENCE',
                payload: true
              }
            ],
            'AmbientLight: ': [
              {
                notification: 'AMBIENT_LIGHT',
								payloadReplacement: '###VALUE###',
								replacementPrefix: 'AmbientLight: ',
								payload: {
										value: '###VALUE###'
								}
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
