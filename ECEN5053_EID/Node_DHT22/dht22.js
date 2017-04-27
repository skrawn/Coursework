
var sensorLib = require("node-dht-sensor")
var averageTemp
var averageHum

var sensor = { 
    sensors: [ {
        name: "Outdoor",
        type: 22,
        pin: 4
    } ],

    read: function() {
        var b = sensorLib.read(this.sensors.type, this.sensors.pin)
        averageTemp = b.temperature.toFixed(1)
        averageHum = b.humidity.toFixed(1)
        console.log(this.sensors.name + ": " + b.temperature.toFixed(1) +
             "°C, " + b.humidity.toFixed(1) + "%")
        setTimeout(function() {
            sensor.read()
        }, 10000)
    }
}

var counter = 0;

while (1) {
    sensor.read()
    if (counter++ > 10) {
        counter = 0
        console.log("Average Temperature: " + averageTemp / 10 + "°C, " +
            "Average Humidity: " + averageHum / 10 + "%")
        averageHum = 0
        averageTemp = 0
    }
}