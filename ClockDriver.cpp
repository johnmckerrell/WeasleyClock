#include <EEPROM.h>
#include "WProgram.h"
#include "ClockDriver.h"

void ClockDriver::setup() {
  this->setup(4,5,6,7,30);
}

void ClockDriver::_readSteps() {
  int address = 0;
  int memorySteps = 0;
  int value = 0;
  while( value = EEPROM.read(address) ) {
    memorySteps = (memorySteps * 255) + value;
#ifdef DEBUG
    Serial.print(address);
    Serial.print("\t");
    Serial.print(value);
    Serial.print("\t");
    Serial.print(memorySteps);
    Serial.println("");
#endif
    ++address;
  }
  currentSteps = memorySteps;
#ifdef DEBUG
  Serial.print("read currentSteps from memory: ");
  Serial.println(currentSteps);
#endif
}

void ClockDriver::writeSteps() {
  int address = 0;
  int memorySteps = currentSteps;
  while( memorySteps > 255 ) {
    EEPROM.write(address,memorySteps / 255);
    memorySteps = memorySteps % 255;
    ++address;
  }
  EEPROM.write(address,memorySteps);
  EEPROM.write(address+1,0);
#ifdef DEBUG
  Serial.print("written currentSteps to memory: ");
  Serial.println(currentSteps);
#endif
}

void ClockDriver::stepTo(int step, boolean allowBackwards) {
  int direction = true;
  if( allowBackwards ) {
    int backwards = ( currentSteps - step ) % maxSteps;
    int forwards = ( step - currentSteps ) % maxSteps;
    if( backwards < 0 )
      backwards += maxSteps;
    if( forwards < 0 )
      forwards += maxSteps;
    if( backwards < forwards ) {
      direction = false;
    }
  }

/*
  while( currentSteps != step ) {
    this->step(direction);
  }
  */
  Serial.print("step = ");
  Serial.println(step);
  Serial.print("currentSteps = ");
  Serial.println(currentSteps);
  int steps = step - currentSteps;
  Serial.print("steps = ");
  Serial.println(steps);
  if( steps < 0 )
    steps += (600*12);
  steps = - steps;
  Serial.print("steps = ");
  Serial.println(steps);
  Serial.print("stepper = ");
  Serial.println((int)(this->stepper));
  while( steps < -100 ) {
    stepper->step(-100);
    steps += 100;
  }
  stepper->step(steps);
  currentSteps = step;
  this->writeSteps();
}

void ClockDriver::setClockHands( int bigHand, int smallHand ) {
  if( bigHand < 0 || bigHand >= 12 || smallHand < 0 || smallHand >= 12 )
    return;
  int newStep = 600 * smallHand + ( bigHand * ( 600 / 12 ) );
  stepTo( newStep, false );
}

boolean ClockDriver::isActive() {
  return active;
}

int ClockDriver::getCurrentStep() {
  return currentSteps;
}


void ClockDriver::setup(int a, int b, int c, int d, int e) {
  stepper = new Stepper(600,a,b,c,d);
  stepper->setSpeed(e);
  maxSteps = 600*12;
  currentSteps = 0;
  this->_readSteps();
  targetStep = -1;
  targetDirection = true;
  active = false;
}
