#include "Arduino.h"
#include "temperature_sensor.h"


// constructor declared inside the header for initializer list


// member functions
void TemperatureSensor::requestTemperature()
{
  // see DallasTemperature library for details
  temp.requestTemperatures();
}

float TemperatureSensor::getTempC()
{
  // see DallasTemperature library for details
  return temp.getTempCByIndex(0);
}
