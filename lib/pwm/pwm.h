#ifndef PWM_H
#define PWM_H



class Pwm
{
  public:
  // constructor
  Pwm(int pin, int channel, int frequency, int resolutionBit);
  // destructor
  ~Pwm(){}; 

  // variables or member functions
  void write(int value);  
  void writeRatio(float ratio); 
  float getRatio();
  

  // private variables
  private:
  int _pin;
  int _channel; // timer/pwm channel
  int _frequency; // unit = Hz
  int _period;  // unit = ms, time for one complete cycle
  int _resolutionBit;  // resolution(counts) for 1 period, e.g. 10 means 10 bits resolutions
  int _rangeMax; // this is the max count value, e.g. 2^_resolutionBit -1, 
  // status below, for storing readings only
  int _pwmValue;
  float _pwmratio;
};



#endif
