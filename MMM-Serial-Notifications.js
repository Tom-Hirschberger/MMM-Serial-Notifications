/* global Module

/* Magic Mirror
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

  socketNotificationReceived: function (notification, payload) {
    this.sendNotification(notification, payload)
  }
})
