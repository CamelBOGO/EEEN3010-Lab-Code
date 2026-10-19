// arduino built in library
#include "Arduino.h"

// our own header file
#include "fan.h"

// two global variables for interrupt functions to increment.
// they will be passed into the fan objects for computing rpm
volatile unsigned long fanInRpmCount = 0;
volatile unsigned long fanOutRpmCount = 0;

// static interrupt routine functions for fan RPM pin x2
// IRAM_ATTR is a special modifier to indicate that these two routines are to be put in the internal RAM for faster execution.
void IRAM_ATTR fanInRpmInterruptRoutine() {
    fanInRpmCount++;
}
void IRAM_ATTR fanOutRpmInterruptRoutine() {
    fanOutRpmCount++;
}

// constructor declaration is done inside class definition with initialize list for pwm object

// member functions
void Fan::updateCountAndTime() {
    // move the current values into previous values
    _rpmCountPrevious = _rpmCountCurrent;
    _TimePrevious = _TimeCurrent;

    // copy the current count value from the pointer, immediately release pointer after copy
    _rpmCountCurrent = *_rpmCountCurrentPointer;
    _TimeCurrent = millis();  // get current time value

    // compute delta count and delta time
    _rpmCountDelta = _rpmCountCurrent - _rpmCountPrevious;
    _TimeDelta = _TimeCurrent - _TimePrevious;
}

// simple compute rpm value by count / time
void Fan::computeRpm() {
    // simple compute rpm, unit = rev per minutes
    _rpm = (unsigned int)((float)_rpmCountDelta / _TimeDelta * rpmCoefficient);
}
// simple function to update and computer the result rpm
void Fan::updateRpm() {
    updateCountAndTime();
    computeRpm();
}
// simple wrapper functions to get values
unsigned int Fan::getRpmCountCurrent() {
    return _rpmCountCurrent;
}

unsigned int Fan::getRpm() {
    return _rpm;
}

void Fan::computeHz() {
    // simple compute Hz of rotation speed, unit= revolution per second
    _Hz = (unsigned int)((float)_rpmCountDelta / _TimeDelta * 1000);
}
void Fan::updateHz() {
    updateCountAndTime();
    computeHz();
}
unsigned int Fan::getHz() {
    return _Hz;
}
