// arduino built in library
#include "Arduino.h"

// our own header file
#include "pwm_servo.h"



// constructors
PwmServo::PwmServo(int pin, int channel, int frequency, int resolutionBit, int offset)
{
  // assign parameters
  _pin = pin;
  _channel = channel;
  _frequency = frequency;
  _resolutionBit = resolutionBit;
  // compute the rest of parameters
  _period = 1000/_frequency; // note: the frequency should not be zero!
  _rangeMax = pow(2,_resolutionBit) - 1;
  // special computation for RC servo standard, +ve_width:=[1ms 2ms], default:=50Hz
  _countOffset = offset;
  _countFor1ms = _rangeMax/_period + _countOffset; 
  _countFor2ms = _rangeMax/_period*2 + _countOffset;
  
  
  // set to output pin first
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  // using internal library for PWM generation, it doesn't relate to led, although can be used to drive led
  ledcAttachPin(_pin, _channel);
  ledcSetup(_channel, _frequency, _resolutionBit);

  // note: now the pwm channel is configured but still at low voltage, nothing output yet.
  
}
void PwmServo::write(int value)
{
  // check if the value is out of range first
  if(value < _countFor1ms)
  {
    value = _countFor1ms;
  }
  else if(value > _countFor2ms)
  {
    value = _countFor2ms;
  }
  // update pwm value
  _servoPwmValue = value;
  _servoPwmRatio = (float)(_countFor2ms - _servoPwmValue)/(_countFor2ms - _countFor1ms);
  
  // output if passed
  ledcWrite(_channel, _servoPwmValue);
}
void PwmServo::writeRatio(float ratio)
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
  _servoPwmRatio = ratio;
  _servoPwmValue = _servoPwmRatio * (_countFor2ms - _countFor1ms) + _countFor1ms;
  
  // output if passed
  ledcWrite(_channel, _servoPwmValue);
}
