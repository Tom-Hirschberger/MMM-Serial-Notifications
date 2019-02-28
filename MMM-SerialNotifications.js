/* global Module

/* Magic Mirror
 * Module: Serial-Notifications
 *
 * By Tom Hirschberger
 * MIT Licensed.
 */
Module.register('MMM-Serial-Notifications', {
  defaults: {
    devices : {
      '/dev/ttyACM0':{
        messages: {
          'UP':[
            {
              notification:'VOLUME_UP', 
              payload:{
                step:5
              }
            },
            {
              notification:'USER_PRESENCE',
              payload: true
            }
          ],
          'DOWN':[
            {
              notification:'VOLUME_DOWN',
              payload:{
                step:2
              }
            },
            {
              notification:'USER_PRESENCE',
              payload: true
            }
          ]
        }
      }
    }
  },

  start: function () {
    Log.info("Starting module: " + this.name);
    this.sendSocketNotification('CONFIG', this.config)
  },

  socketNotificationReceived: function (notification, payload) {
    this.sendNotification(notification, payload)
  }
})
