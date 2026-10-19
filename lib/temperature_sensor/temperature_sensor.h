#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

// include necessary libraryzzxzzx
#include <DallasTemperature.h>
#include <OneWire.h>

class TemperatureSensor {
   public:
    // constructor
    // need to have the initializer list to initialize the onewire object
    TemperatureSensor(int pinOneWire) : oneWire(pinOneWire), temp(&oneWire) {
        // assign pins
        _pinOneWire = pinOneWire;
    }
    // destructor, may update if necessary in the future
    ~TemperatureSensor() {};

    // member functions
    void requestTemperature();  // send request temperature commands and read back readings,
    float getTempC();           // return temperature in degree C,

   private:
    int _pinOneWire;  // sensor pin, need to both input and output

    OneWire oneWire;         // oneWire object from class, as communication ptotocol
    DallasTemperature temp;  // temperature sensor object
};

#endif
