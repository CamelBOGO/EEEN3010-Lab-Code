#ifndef PWM_SERVO_H
#define PWM_SERVO_H

class PwmServo {
   public:
    // constructor
    PwmServo(int pin, int channel, int frequency, int resolutionBit, int offset);
    // destructor
    ~PwmServo() {};

    // variables or member functions
    void write(int value);
    void writeRatio(float ratio);

    // private variables
   private:
    int _pin;
    int _channel;        // timer/pwm channel
    int _frequency;      // unit = Hz
    int _period;         // unit = ms, time for one complete cycle
    int _resolutionBit;  // resolution(counts) for 1 period, e.g. 10 means 10 bits resolutions
    int _rangeMax;       // this is the max count value, e.g. 2^_resolutionBit -1,
    // more related to servo motor pwm
    int _countFor1ms;  // counts representing 1ms for servo start position
    int _countFor2ms;  // counts representing 2ms for servo end position
    int _countOffset;  // offset counts to shift 1ms and 2ms, used for trimming of installation angle offset
    // status below,
    int _servoPwmValue;
    float _servoPwmRatio;
};

#endif
