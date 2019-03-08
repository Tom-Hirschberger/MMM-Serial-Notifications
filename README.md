# MMM-Serial-Notifications
MMM-Serial-Notifications is a module for the [MagicMirror](https://github.com/MichMich/MagicMirror) project by [Michael Teeuw](https://github.com/MichMich).

It watches the serial consoles and sends notifications based on the received messages;

This module is inspired by https://github.com/thobach/MMM-Gestures!

## Installation
```sh
  cd ~/MagicMirror/modules
  git clone 
  cd MMM-Serial-Notifications
  npm install
```


## Configuration
  To use the module insert it in the config.js file. Here is an example:
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
