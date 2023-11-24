/* global Module

/* MagicMirror²
 * Module: Serial-Notifications
 *
 * By Tom Hirschberger
 * MIT Licensed.
 */
Module.register('MMM-Serial-Notifications', {
  defaults: {
  },

  start: function () {
    Log.info("Starting module: " + this.name);
    this.sendSocketNotification('CONFIG', this.config)
  },

  notificationReceived: function(notification,payload) {
    if(notification === "CHANGED_PROFILE"){
      this.sendSocketNotification(notification,payload)
    }
  },

  socketNotificationReceived: function (notification, payload) {
    this.sendNotification(notification, payload)
  }
})
