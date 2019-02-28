/* Magic Mirror
 * Module: Serial-Notifications
 *
 * By Tom Hirschberger
 * MIT Licensed.
 */

const NodeHelper = require('node_helper')
const Readline = require('@serialport/parser-readline')
const SerialPort = require("serialport");

module.exports = NodeHelper.create({

  start: function () {
    this.started = false
  },

  socketNotificationReceived: function (notification, payload) {
    const self = this
    if (notification === 'CONFIG' && self.started === false) {
      self.config = payload

      for (var curUsbDev in self.config.devices){
        console.log("Creating port for usb dev: "+curUsbDev)
        self.config.devices[curUsbDev].port = new SerialPort(curUsbDev,{
          baudRate: 9600,
        })

      self.config.devices[curUsbDev].parser = self.config.devices[curUsbDev].port.pipe(new Readline({ delimiter: '\n' }))
      self.config.devices[curUsbDev].parser.on("data", function(data){
        for(var curExptMessage in self.config.devices[curUsbDev].messages){
          if(data.indexOf(curExptMessage) == 0){
            for(var curNotification in self.config.devices[curUsbDev].messages[curExptMessage]){
              if(typeof self.config.devices[curUsbDev].messages[curExptMessage][curNotification].notification !== 'undefined'){
                if(typeof self.config.devices[curUsbDev].messages[curExptMessage][curNotification].payload === 'undefined'){
                  var payload = {}
                  } else {
                    var payload = self.config.devices[curUsbDev].messages[curExptMessage][curNotification].payload
                  }
                  self.sendSocketNotification(self.config.devices[curUsbDev].messages[curExptMessage][curNotification].notification,payload)
                }
              }
            }
          }
        });
      }

      self.started = true
    }
  }
})
