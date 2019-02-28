/* Magic Mirror
 * Module: Serial-Notifications
 *
 * By Tom Hirschberger
 * MIT Licensed.
 */

const NodeHelper = require('node_helper')
const serialport = require("serialport");
const SerialPort = require("serialport").SerialPort;

module.exports = NodeHelper.create({

  start: function () {
    this.started = false
  },

  socketNotificationReceived: function (notification, payload) {
    const self = this
    if (notification === 'CONFIG' && self.started === false) {
      self.config = payload

      console.log(this.name+": Got config!")

      for (var curUsbDev in self.config.devices){
        console.log("Creating port for usb dev: "+curUsbDev)
        self.config.devices[curUsbDev].port = new SerialPort(curUsbDev,{
          baudrate: 9600,
          parser: serialport.parsers.readline("\n")
        })

        self.config.devices[curUsbDev].port.on("open", function(){
          console.log('serial port' +curUsbDev+ ' opened')
          self.config.devices[curUsbDev].port.on("data", function(curData){
            for(var curExptMessage in self.config.devices[curUsbDev].messages){
              if(data.indexOf(curExptMessage) == 0){
                console.log("Received "+data)
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
        });
      }

      self.started = true
    } else {
      console.log(this.name+": Received "+notification)
    }
  }
})
