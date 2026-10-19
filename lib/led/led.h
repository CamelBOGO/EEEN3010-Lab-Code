#ifndef LED_H
#define LED_H


class Led
{
  public: // variables or functions below can be accessed by others
    Led();  // constructor, must be same name as class: see the code segment below
    Led(int pin);  // another constructor with input pin
    ~Led(){}; //destructor, may be used to clear anything from memory, if necessary, 

    // other variables and member functions can be put here
    void on();
    void off();
    void toggle();
    int getState();

  private:
    int _pin;
    int _state; // state = 0 means it is OFF, 
    // you may also have some other private variables or functions here

};

#endif
