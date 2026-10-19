// arduino built in library
#include "Arduino.h"

// our own header file
#include "led.h"

// constructor, it will use this constructor if there is no given input value
Led::Led() {
    _pin = 2;               // on board led on ESP32, use this default pin as LED if not given
    pinMode(_pin, OUTPUT);  // config as output pin
    _state = 0;
}
// constructor, it will use this constructor if an input pin is given
Led::Led(int pin) {
    _pin = pin;
    pinMode(_pin, OUTPUT);  // config as output pin
    _state = 0;
}
void Led::on() {
    digitalWrite(_pin, HIGH);
    _state = 1;
}
void Led::off() {
    digitalWrite(_pin, LOW);
    _state = 0;
}
void Led::toggle() {
    digitalWrite(_pin, !digitalRead(_pin));
    _state = !_state;
}
int Led::getState() {
    return _state;
}
