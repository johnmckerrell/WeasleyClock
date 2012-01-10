#include "Arduino.h"
#include <SPI.h>
#include <stdlib.h>
#include <EEPROM.h>
#include <IPAddress.h>
#include <Client.h>
#include <Ethernet.h>
#include <Server.h>
#include <Stepper.h>
#include <Dhcp.h>
#include <Dns.h>
#include "ClockDriver.h"
#include "MapMe_At.h"

/*
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
*/

/*
 * Weasley Clock
 *
 * Supposed to take input from mapme.at via the ethernet shield
 * and point the hands at the correct spots for the locations.
 */

byte mac[] = { 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a };
//byte ip[] = { 10, 56, 56, 20 };
//byte ip[] = { 192, 168, 2, 2 };
//byte ip[] = { 192, 168, 10, 97 };

#define DEBUG 1
#define TRAVELLING 11

ClockDriver cd;
MapMe_At mm_john, mm_other;
int forwardsPush = 6;
int backwardsPush = 7;
int thirdOption = 8;
int ledPin = 13;
// geraldineHand is the hour hand, johnHand is the minute hand
int geraldineHand = 0, johnHand = 0;
int lastGHand = 0, lastJHand = 0;
unsigned long lastMillis = 0;
boolean changed = false;
boolean outputted = false;

void setup()                    // run once, when the sketch starts
{
#ifdef DEBUG
  Serial.begin(9600);
#endif
  Serial.println("Setting up Ethernet");
  if (Ethernet.begin(mac) == 0) {
#ifdef DEBUG
    Serial.println("Failed to configure Ethernet using DHCP");
#endif
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

  cd.setup();
  mm_john.setup("snowdrop","");
  mm_other.setup("royaliris","");
  //pinMode(forwardsPush, INPUT);
  //pinMode(backwardsPush, INPUT);
  //pinMode(thirdOption, INPUT);
  //pinMode(ledPin, OUTPUT);
}

int locationToHour( char *location ) {
  // mortal peril home work cafe pub station travelling family airport hotel restaurant hospital

  if( location[0] != '\0' && strcasestr( location, "home" ) != NULL ) {
    return 1;
  }
  if( location[0] != '\0' && strcasestr( location, "airport" ) != NULL ) {
    return 2;
  }
  if( location[0] != '\0' && strcasestr( location, "cafe" ) != NULL ) {
    return 3;
  }
  if( location[0] != '\0' && strcasestr( location, "pub" ) != NULL ) {
    return 4;
  }
  if( location[0] != '\0' && strcasestr( location, "bar" ) != NULL ) {
    return 4;
  }
  if( location[0] != '\0' && strcasestr( location, "station" ) != NULL ) {
    return 5;
  }
  if( location[0] != '\0' && strcasestr( location, "anne" ) != NULL ) {
    return 6;
  }
  if( location[0] != '\0' && strcasestr( location, "dad" ) != NULL ) {
    return 6;
  }
  if( location[0] != '\0' && strcasestr( location, "hazel" ) != NULL ) {
    return 6;
  }
  if( location[0] != '\0' && strcasestr( location, "angela" ) != NULL ) {
    return 6;
  }
  if( location[0] != '\0' && strcasestr( location, "gwen" ) != NULL ) {
    return 6;
  }
  if( location[0] != '\0' && strcasestr( location, "family" ) != NULL ) {
    return 6;
  }
  if( location[0] != '\0' && strcasestr( location, "work" ) != NULL ) {
    return 7;
  }
  if( location[0] != '\0' && strcasestr( location, "office" ) != NULL ) {
    return 7;
  }
  if( location[0] != '\0' && strcasestr( location, "hotel" ) != NULL ) {
    return 8;
  }
  if( location[0] != '\0' && strcasestr( location, "restaurant" ) != NULL ) {
    return 10;
  }
  if( location[0] != '\0' && strcasestr( location, "shop" ) != NULL ) {
    return 9;
  }
  if( location[0] != '\0' && strcasestr( location, "store" ) != NULL ) {
    return 9;
  }
  if( location[0] != '\0' && strcasestr( location, "market" ) != NULL ) {
    return 9;
  }
  

  return TRAVELLING;
}

void loop()                     // run over and over again
{
  /*
  delay(5000);
  Serial.println("half six");
  cd.setClockHands(6,6);
  delay(5000);
  Serial.println("ten past eight");
  cd.setClockHands(2,8);
  delay(5000);
  Serial.println("twenty past eleven");
  cd.setClockHands(4,11);
  delay(5000);
  Serial.println("five past five");
  cd.setClockHands(1, 5);
  delay(5000);
  Serial.println("midnight");
  cd.setClockHands(0,0);
  delay(86400000);
  */
  /*
  Serial.println("Hands to 4 & 7");
  cd.setClockHands(4,7);
  delay(1000);
  Serial.println("Hands to 8 & 2");
  cd.setClockHands(8,2);
  delay(1000);
  Serial.println("Hands to 1 & 5");
  cd.setClockHands(1,5);
  delay(1000);
  return;
  */
  //if(digitalRead(forwardsPush) == HIGH) {
  //for(int i = 0; i < 150; ++i ) {
  //  cd.step(1);
  //}
  //delay(2000);
  //}
  //return;

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
        johnHand = locationToHour( mm_john.getLabel() );
        if (johnHand == TRAVELLING) {
          johnHand = locationToHour( mm_john.getPOIType() );
        }
      }
    }
    if( geraldineHand == -1 && ! mm_other.isActive() ) {
      if( mm_other.wasError() ) {
        geraldineHand = lastGHand;
        wasError = true;
      } else {
        geraldineHand = locationToHour( mm_other.getLabel() );
        if (geraldineHand == TRAVELLING) {
          geraldineHand = locationToHour( mm_other.getPOIType() );
        }
      }
    }
    if( geraldineHand != -1 && johnHand != -1 ) {
      if( wasError )
        digitalWrite( ledPin, HIGH );
#ifdef DEBUG
      Serial.print( "Setting Hands to " );
      Serial.print( mm_john.getLabel() );
      Serial.print( " - " );
      Serial.print( mm_john.getPOIType() );
      Serial.print( " (" );
      Serial.print( johnHand );
      Serial.print( " minutes) and " );
      Serial.print( mm_other.getLabel() );
      Serial.print( " - " );
      Serial.print( mm_other.getPOIType() );
      Serial.print( " (" );
      Serial.print( geraldineHand );
      Serial.println( " hours)" );
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
