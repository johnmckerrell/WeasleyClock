#ifndef ClockDriver_h
#define ClockDriver_h

#include "WProgram.h"

class ClockDriver {
  public:  
    void setup();
    void setup(int a, int b, int c, int d, int e);
    void step(boolean direction);
    void stepTo(int step, boolean allowBackwards);
    int getCurrentStep();
    void writeSteps();
    void setTarget(int targetStep, boolean allowBackwards);
    void setClockHands( int bigHand, int smallHand );
    void loop();
    boolean isActive();

  private:
    int step1A;                // LED connected to digital pin 13
    int step2A;
    int step3A;
    int step4A;
    int pause;
    int maxSteps;
    int currentSteps;
    int targetStep;
    int targetDirection;
    boolean active;
    
    void _step1();
    void _step2();
    void _step3();
    void _step4();
    void _readSteps();

};

#endif
