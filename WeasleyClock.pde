#include <SPI.h>
#include <stdlib.h>
#include <EEPROM.h>
#include <Client.h>
#include <Ethernet.h>
#include <Server.h>

#include "ClockDriver.h"
#include "MapMe_At.h"


void * operator new(size_t size);
void operator delete(void * ptr);

void * operator new(size_t size)
{
  return malloc(size);
}

void operator delete(void * ptr)
{
  free(ptr);
}

/*
 * Weasley Clock
 *
 * Supposed to take input from mapme.at via the ethernet shield
 * and point the hands at the correct spots for the locations.
 */

byte mac[] = { 0xDE, 0xAD, 0xB3, 0xED, 0xCE, 0xEA };
//byte ip[] = { 10, 56, 56, 20 };
byte ip[] = { 192, 168, 10, 17 };
//byte ip[] = { 192, 168, 10, 17 };

#define DEBUG 1

ClockDriver cd;
MapMe_At mm_john, mm_other;
int forwardsPush = 6;
int backwardsPush = 7;
int thirdOption = 8;
int ledPin = 13;
int geraldineHand = 0, johnHand = 0;
int lastGHand = 0, lastJHand = 0;
unsigned long lastMillis = 0;
boolean changed = false;
boolean outputted = false;

void setup()                    // run once, when the sketch starts
{
  Ethernet.begin(mac, ip);
#ifdef DEBUG
  Serial.begin(9600);
#endif
  cd.setup();
  mm_john.setup("snowdrop");
  mm_other.setup("royaliris");
  pinMode(forwardsPush, INPUT);
  pinMode(backwardsPush, INPUT);
  pinMode(thirdOption, INPUT);
  pinMode(ledPin, OUTPUT);
}

int locationToHour( char *location ) {
  if( location[0] != '\0' && strcasestr( location, "liverpool" ) != NULL ) {
    return 11;
  }
  if( location[0] != '\0' && strcasestr( location, "seacombe" ) != NULL ) {
    return 3;
  }
  if( location[0] != '\0' && strcasestr( location, "west-float" ) != NULL ) {
    return 5;
  }
  if( location[0] != '\0' && strcasestr( location, "east-float" ) != NULL ) {
    return 5;
  }
  if( location[0] != '\0' && strcasestr( location, "alexandra-dock" ) != NULL ) {
    return 5;
  }
  if( location[0] != '\0' && strcasestr( location, "woodchurch" ) != NULL ) {
    return 7;
  }
  if( location[0] != '\0' && strcasestr( location, "manchester-ship-canal" ) != NULL ) {
    return 9;
  }

  return 1;
}

void loop()                     // run over and over again
{
  //if(digitalRead(forwardsPush) == HIGH) {
  //for(int i = 0; i < 150; ++i ) {
  //  cd.step(1);
  //}
  //delay(2000);
  //}
  //return;
  cd.loop();
  mm_john.loop();
  mm_other.loop();
  unsigned long nowMillis = millis();
  boolean update = false;
  if( lastMillis != 0 ) {
    if( nowMillis < lastMillis ) {
      // handle the fact that it rolled over.
      update = true;
    } else if( nowMillis - lastMillis > 60000 ) {
      update = true;
    }
  } else {
    update = true;
  }
  if( update ) {
    lastJHand = johnHand;
    lastGHand = geraldineHand;
    johnHand = -1;
    geraldineHand = -1;
#ifdef DEBUG
    Serial.print("Timeout");
    Serial.println(nowMillis);
#endif
    if( ! mm_john.isActive() ) {
      mm_john.requestLocation();
    }
    if( ! mm_other.isActive() ) {
      mm_other.requestLocation();
    }
    lastMillis = nowMillis;
  }
  if( johnHand == -1 || geraldineHand == -1 ) {
    digitalWrite(ledPin, LOW);
    boolean wasError = false;
    if( johnHand == -1 && ! mm_john.isActive() ) {
      if( mm_john.wasError() ) {
        wasError = true;
        johnHand = lastJHand;
      } else {
        johnHand = locationToHour( mm_john.getLocation() );
      }
    }
    if( geraldineHand == -1 && ! mm_other.isActive() ) {
      if( mm_other.wasError() ) {
        geraldineHand = lastGHand;
        wasError = true;
      } else {
        geraldineHand = locationToHour( mm_other.getLocation() );
      }
    }
    if( geraldineHand != -1 && johnHand != -1 ) {
      if( wasError )
        digitalWrite( ledPin, HIGH );
#ifdef DEBUG
      Serial.print( "Setting Hands to " );
      Serial.print( mm_john.getLocation() );
      Serial.print( " (" );
      Serial.print( johnHand );
      Serial.print( ") and " );
      Serial.print( mm_other.getLocation() );
      Serial.print( " (" );
      Serial.print( geraldineHand );
      Serial.println( ")" );
#endif
      cd.setClockHands( johnHand, geraldineHand );
    }
  }
  /*
  if( ! changed ) {
    changed = true;
    Serial.println("About to request location");
    mm_john.requestLocation();
  }
  if( changed && ! outputted && ! mm_john.isActive() ) {
    outputted = true;
    if( mm_john.wasError() ) {
      Serial.println( "There was an error finding your location." );
    } else {
      Serial.print("Found your location, it's: ");
      char* loc = mm_john.getLocation(); 
      if( loc[0] == '\0' ) {
        Serial.println( "arbitrary" );
      }
        Serial.println( loc );
    }
  }
  */
  /*
  if( digitalRead(forwardsPush) == HIGH ) {
    // 0:40
    cd.stepTo(100,false);
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
  } else if( digitalRead(backwardsPush) == HIGH ) {
    // 6:40
    cd.setTarget(1000,true);
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
  } else if( digitalRead(thirdOption) == HIGH ) {
    // 11:20
    cd.stepTo(1700,false);
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
  }
  */
  //cd.step(true);
  /*
  if( digitalRead(forwardsPush) == HIGH ) {
    cd.step(true);
    changed = true;
  } else if( digitalRead(backwardsPush) == HIGH ) {
    cd.step(false);
    changed = true;
  } else if( changed ) {
    cd.writeSteps();
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    changed = false;
  }
  */
}
