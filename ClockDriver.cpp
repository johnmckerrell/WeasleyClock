#include <EEPROM.h>
#include "WProgram.h"
#include "ClockDriver.h"

void ClockDriver::step(boolean dir) {
  if( dir ) {
    _step1();
    delay(pause);
    _step2();
    delay(pause);
    _step3();
    delay(pause);
    _step4();
    delay(pause);
    ++currentSteps;
  } else {
    _step4();
    delay(pause);
    _step3();
    delay(pause);
    _step2();
    delay(pause);
    _step1();
    delay(pause);
    --currentSteps;
  }
  if( currentSteps < 0 ) {
    currentSteps += maxSteps;
  } else if( currentSteps >= maxSteps ) {
    currentSteps -= maxSteps;
  }
}

void ClockDriver::_step1() {
 digitalWrite(step1A, LOW);
 digitalWrite(step2A, LOW);
 digitalWrite(step3A, LOW);
 digitalWrite(step4A, HIGH);
}
    
void ClockDriver::_step2() {
 digitalWrite(step1A, LOW);
 digitalWrite(step2A, HIGH);
 digitalWrite(step3A, LOW);
 digitalWrite(step4A, LOW);
}
    
void ClockDriver::_step3() {
 digitalWrite(step1A, LOW);
 digitalWrite(step2A, LOW);
 digitalWrite(step3A, HIGH);
 digitalWrite(step4A, LOW);
}
    
void ClockDriver::_step4() {
 digitalWrite(step1A, HIGH);
 digitalWrite(step2A, LOW);
 digitalWrite(step3A, LOW);
 digitalWrite(step4A, LOW);
}

void ClockDriver::setup() {
  this->setup(2,3,4,5,1);
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
#ifdef DEBUG
  Serial.print( "going from " );
  Serial.print( currentSteps );
  Serial.print( " to " );
  Serial.print( step );
  Serial.println( direction ? " forwards" : " backwards" );
#endif
  while( currentSteps != step ) {
    this->step(direction);
  }
  this->writeSteps();
}

void ClockDriver::setClockHands( int bigHand, int smallHand ) {
  if( bigHand < 0 || bigHand >= 12 || smallHand < 0 || smallHand >= 12 )
    return;
  int newStep = 150 * smallHand + ( bigHand * ( 150 / 12 ) );
  stepTo( newStep, false );
}

boolean ClockDriver::isActive() {
  return active;
}

void ClockDriver::setTarget(int target, boolean allowBackwards) {
  if( target < 0 || target >= maxSteps ) {
    targetStep = -1;
    active = false;
    this->writeSteps();
  }
  int direction = true;
  if( allowBackwards ) {
    int backwards = ( currentSteps - target ) % maxSteps;
    int forwards = ( target - currentSteps ) % maxSteps;
    if( backwards < 0 )
      backwards += maxSteps;
    if( forwards < 0 )
      forwards += maxSteps;
    if( backwards < forwards ) {
      direction = false;
    }
  }
  if( target != currentSteps ) {
    active = true;
  }
  targetStep = target;
  targetDirection = direction;
#ifdef DEBUG
  Serial.print( "going from " );
  Serial.print( currentSteps );
  Serial.print( " to " );
  Serial.print( target );
  Serial.println( direction ? " forwards" : " backwards" );
#endif
}

void ClockDriver::loop() {
  if( targetStep < 0 || targetStep >= maxSteps ) {
  } else if( currentSteps != targetStep ) {
    this->step(targetDirection);
  } else if( active ) {
    this->writeSteps();
    active = false;
  }
}

int ClockDriver::getCurrentStep() {
  return currentSteps;
}


void ClockDriver::setup(int a, int b, int c, int d, int e) {
  step1A = a;                // LED connected to digital pin 13
  step2A = b;
  step3A = c;
  step4A = d;
  pause = e;
  maxSteps = 150*12;
  currentSteps = 0;
  this->_readSteps();
  targetStep = -1;
  targetDirection = true;
  active = false;
  pinMode(step1A, OUTPUT);      // sets the digital pin as output
  pinMode(step2A, OUTPUT);
  pinMode(step3A, OUTPUT);
  pinMode(step4A, OUTPUT);
}
