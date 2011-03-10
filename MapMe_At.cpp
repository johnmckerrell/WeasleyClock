#include "MapMe_At.h"
#include <string.h>

#define DEBUG 1

// Request current location

// Check if we're still getting a location

// Get currently stored location

void MapMe_At::setup(const char *_username) {
  client = NULL;
  error = 0;
  strcpy( username, _username);
  location[0] = '\0';
}

boolean MapMe_At::wasError() {
  return error;
}

void MapMe_At::loop() {
  if( client ) {
    if( client->available() ) {
      char c = client->read();
      Serial.print(c);
      jsonParser.handleInput(c);
      if( jsonParser.getParseState() == JSON_PARSE_HAVEVAL ) {
        char *key = jsonParser.getKey();
        if( strcmp("error", key) == 0 ) {
          error = MAPME_AT_ERR_RESULT;
        } 
        else if( strcmp("label", key) == 0 ) {
          strcpy( location, jsonParser.getVal() );
        }
        /*
         Serial.print( key );
         Serial.print( "\t = \"" );
         Serial.print( jsonParser.getVal() );
         Serial.println( "\"" );
         */
      }
    }
    if( error || ! client->connected() ) {
#ifdef DEBUG
      if( error ) {
        Serial.print("error=");
        Serial.println(error);
      }
      Serial.println("disconnecting");
#endif
      client->stop();
      client = NULL;
      if( jsonParser.getParseState() == JSON_PARSE_FINDJSON )
        error = MAPME_AT_ERR_RESULT;

    }
  }
}

void MapMe_At::requestLocation() {
  byte server[] = { 
    188, 40, 54, 143 }; // mapme.at
  client = new Client;
#ifdef DEBUG
  Serial.println("connecting...");
#endif
  if( client->connect("mapme.at", 80) ) {
    jsonParser.clearState();
    location[0] = '\0';

#ifdef DEBUG
    Serial.println("connected");
#endif
    client->print("GET /api/where.json?username=");
    client->print(username);
    client->println();
    client->println();
  } 
  else {
    client = NULL;
    error = MAPME_AT_ERR_CONNECTION;
#ifdef DEBUG
    Serial.println("Connection failed");
#endif
  }
}

boolean MapMe_At::isActive() {
  return ( client ? true : false );
}

char* MapMe_At::getLocation() {
  return location;
}
