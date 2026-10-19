#ifndef FAN_H
#define FAN_H

// need to drive the fan with pwm
#include "pwm.h"


// special handling for interrupt routine, because static interrupt function cannot be put
// define a function pointer type for pointing to the static interrupt routine functions
typedef void fanRpmInterruptFunctionPointerType(void);





// main class for fan 
class Fan
{
  public:
  Pwm pwm; // pwm sensor object, will be initialized in the initializer list in constructor
  
  // constructor
  // it needs to use initializer list for pwm object to run correctly
  Fan(int rpmPin, volatile unsigned long *rpmCountCurrentPointer, fanRpmInterruptFunctionPointerType interruptRoutinePtr, int pwmPin, int pwmChannel, int pwmFrequency, int pwmResolutionBit):pwm(pwmPin,pwmChannel,pwmFrequency,pwmResolutionBit)
  {
    // assign pin for rpm (interrupt)
    _rpmPin = rpmPin;
    _rpmCountCurrentPointer = rpmCountCurrentPointer;
    _rpmInterruptRoutinePointer = interruptRoutinePtr;  // pointer to the interrupt routine
    
    // initialize rpm related paramenters, 
    // init rpm imput interrupt pins and interrupt routines
    pinMode(_rpmPin, INPUT);  // must set the pin as input first
    attachInterrupt(_rpmPin, _rpmInterruptRoutinePointer, FALLING);
    
    // pwm init is done as its pwm object constructor already

    
    // initialize all variabbles for computing rpm
    _rpmCountCurrent = 0;  
    _rpmCountPrevious = 0; 
    _rpmCountDelta = _rpmCountCurrent - _rpmCountPrevious; 
    _TimeCurrent = 1; // just to avoid bugs for dividing by  zero
    _TimePrevious = 0;
    _TimeDelta = _TimeCurrent - _TimePrevious;
    _rpm = 0; 
  }
  // desctuctor, can modify it if necessary 
  ~Fan(){};

  // public variables and member functions
  void updateCountAndTime(); 
  void computeRpm();
  void updateRpm();
  
  unsigned int getRpmCountCurrent();
  unsigned int getRpm();

  void computeHz();
  void updateHz();

  unsigned int getHz();
  
  
  private:
  // here most of variables are for RPM, pwm related stuffs will be stored in the pwm object
  int _rpmPin; // rpm input interrupt pin
  fanRpmInterruptFunctionPointerType *_rpmInterruptRoutinePointer;

  volatile unsigned long *_rpmCountCurrentPointer; // this pointer points to the count being incremented in the interrupt function
  unsigned long _rpmCountCurrent;  // current rpm count 
  unsigned long _rpmCountPrevious;  // store the previous rpm count for computing delta count
  unsigned long _rpmCountDelta; // difference of count, must be positive as fan only spins in one direction
  unsigned long _TimeCurrent; // current time, unit = ms
  unsigned long _TimePrevious;
  unsigned long _TimeDelta; // difference of time
  unsigned int _rpm; // actual rpm value, unit = Revolutions Per Minute
  int rpmCoefficient = 30000; // rpm = deltaCount/time(unit=ms) * 1000 * 60 / 2(# of magnetic pole and falling edge triggered)
  unsigned int _Hz;  // bumber of revolution per second, a smaller value as rpm
};

// need to put below stuffs inside fan.cpp to avoid multiple declarations, but have to make them 'extern' so fan.h and main.cpp can find them
extern volatile unsigned long fanInRpmCount;
extern volatile unsigned long fanOutRpmCount;
extern void IRAM_ATTR fanInRpmInterruptRoutine();
extern void IRAM_ATTR fanOutRpmInterruptRoutine();

#endif
