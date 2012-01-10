#ifndef ClockDriver_h
#define ClockDriver_h

#include <Stepper.h>
#include "Arduino.h"

class ClockDriver {
  public:  
    void setup();
    void setup(int a, int b, int c, int d, int e);
    void stepTo(int step, boolean allowBackwards);
    int getCurrentStep();
    void writeSteps();
    void setClockHands( int bigHand, int smallHand );
    void loop();
    boolean isActive();

  private:
    int pin1, pin2, pin3, pin4;
    int pause;
    int maxSteps;
    int currentSteps;
    int targetStep;
    int targetDirection;
    boolean active;
    Stepper  *stepper;
    
    void _readSteps();

};

#endif
