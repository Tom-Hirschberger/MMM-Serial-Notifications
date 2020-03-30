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
        (function(innerDev){
          console.log("Creating port for usb dev: "+innerDev)
          curPort = new SerialPort(innerDev,{
            baudRate: 9600,
          })
          curParser = curPort.pipe(new Readline({ delimiter: '\n' }))
          curParser.on("data", function(data){
            self.sendAllNotifications(innerDev, data)
          });
        })(curUsbDev)
        
      }
      self.started = true
    }
  },

  sendAllNotifications: function (curDev, curData) {
    const self = this
    for(var curExptMessage in self.config.devices[curDev].messages){
      if(curData.indexOf(curExptMessage) == 0){
        for(var curNotification in self.config.devices[curDev].messages[curExptMessage]){
          if(typeof self.config.devices[curDev].messages[curExptMessage][curNotification].notification !== 'undefined'){
            if(typeof self.config.devices[curDev].messages[curExptMessage][curNotification].payload === 'undefined'){
              var payload = {}
            } else {
              var payload = self.config.devices[curDev].messages[curExptMessage][curNotification].payload
              if(typeof self.config.devices[curDev].messages[curExptMessage][curNotification].payloadReplacement !== 'undefined'){
                var payloadReplacementString = self.config.devices[curDev].messages[curExptMessage][curNotification].payloadReplacement
                if(typeof self.config.devices[curDev].messages[curExptMessage][curNotification].replacementPrefix !== 'undefined'){
                  replacementPrefix = self.config.devices[curDev].messages[curExptMessage][curNotification].replacementPrefix
                } else {
                  replacementPrefix = ""
                }

                var curReplacementValue = curData.substring(replacementPrefix.length-1,curData.length-1)
                
                var newPayload = {}
                for(var curPayloadKey in payload){
                  var curPayloadValue = payload[curPayloadKey]
                  if(typeof curPayloadValue === 'string'){
                    newPayload[curPayloadKey] = curPayloadValue.replace(payloadReplacementString,curReplacementValue)
                  } else {
                    newPayload[curPayloadKey] = curPayloadValue
                  }
                }

                payload = newPayload
              }
            }
            self.sendSocketNotification(self.config.devices[curDev].messages[curExptMessage][curNotification].notification,newPayload)
          }
        }
      }
    }
  },
})