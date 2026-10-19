#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

// installed library for DHT humidity sensors
#include "DHT.h"

class HumiditySensor {
   public:
    // variables or functions below can be accessed by others
    // constructor with input pin and sensor type
    // it needs to use initializer list for dht object to run correctly
    HumiditySensor(int pin, int type) : dht(pin, type) {
        _pin = pin;
        _type = type;
        dht.begin();
    }

    // destructor, may be used to clear anything from memory, if necessary,
    ~HumiditySensor() {};

    // other variables and member functions can be put here
    void readMeasurements();  // read humidity and temperature
    float getHumidity();      // return stored humidity
    float getTemperature();   // return stored temperature

   private:
    int _pin;   // sensor input/output pin
    int _type;  // sensor type
    float _humidity;
    float _temperature;

    DHT dht;  // dht sensor object, will be initialized in the initializer list in constructor
};

#endif
