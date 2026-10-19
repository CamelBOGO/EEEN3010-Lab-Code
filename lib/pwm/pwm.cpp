// arduino built in library
#include "Arduino.h"

// our own header file
#include "pwm.h"


// constructors
Pwm::Pwm(int pin, int channel, int frequency, int resolutionBit)
{
  // assign parameters
  _pin = pin;
  _channel = channel;
  _frequency = frequency;
  _resolutionBit = resolutionBit;
  // compute the rest of parameters
  _period = 1000/_frequency; // note: the frequency should not be zero!
  _rangeMax = pow(2,_resolutionBit) - 1;

  
  // set to output pin first
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  // using internal library for PWM generation, it doesn't relate to led, although can be used to drive led
  ledcAttachPin(_pin, _channel);  
  ledcSetup(_channel, _frequency, _resolutionBit);

  // note: now the pwm channel is configured but still at low voltage, nothing output yet.
  
}
void Pwm::write(int value)
{
  // check if the value is out of range first
  if(value < 0)
  {
    value = 0;
  }
  else if(value > _rangeMax)
  {
    value = _rangeMax;
  }

  // update pwm value
  _pwmValue = value;
  _pwmratio = (float)_pwmValue/_rangeMax; // store ratio only
  
  // output if passed
  ledcWrite(_channel, _pwmValue);
}
void Pwm::writeRatio(float ratio)
{
  // check if the value is out of range first
  // note: this value is normalized, i.e. range = [0.0, 1.0]
  if(ratio < 0.0)
  {
    ratio = 0.0;
  }
  else if(ratio > 1.0)
  {
    ratio = 1.0;
  }
  
  // update pwm value
  _pwmratio = ratio;
  _pwmValue = (int)(_pwmratio*_rangeMax);
  
  // output if passed
  ledcWrite(_channel, _pwmValue);
}
float Pwm::getRatio()
{
  return _pwmratio;
}
