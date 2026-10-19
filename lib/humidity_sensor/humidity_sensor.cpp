// arduino built in library
#include "Arduino.h"

// our own header file
#include "humidity_sensor.h"

// constructor is inside header file

// read measurements
void HumiditySensor::readMeasurements() {
    _humidity = dht.readHumidity();
    _temperature = dht.readTemperature();  // Read temperature as Celsius (the default)
}

// return stored value
float HumiditySensor::getHumidity() {
    return _humidity;
}
float HumiditySensor::getTemperature() {
    return _temperature;
}
