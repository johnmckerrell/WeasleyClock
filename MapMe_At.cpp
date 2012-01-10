#include "MapMe_At.h"
#include <string.h>

#define DEBUG 1

// Request current location

// Check if we're still getting a location

// Get currently stored location

void MapMe_At::setup(const char *_username, const char *_sharing_code) {
  error = 0;
  strcpy( username, _username);
  strcpy( sharing_code, _sharing_code);
  label[0] = '\0';
  poi_type[0] = '\0';
}

boolean MapMe_At::wasError() {
  return error;
}

void MapMe_At::loop() {
  if( client.connected() ) {
    if( client.available() ) {
      char c = client.read();
      Serial.print(c);
      jsonParser.handleInput(c);
      if( jsonParser.getParseState() == JSON_PARSE_HAVEVAL && jsonParser.getHTTPStatus() == 200 ) {
        char *key = jsonParser.getKey();
        if( strcmp("error", key) == 0 ) {
          error = MAPME_AT_ERR_RESULT;
        } 
        else if( strcmp("label", key) == 0 ) {
          strcpy( label, jsonParser.getVal() );
        }
        else if( strcmp("poi_type", key) == 0 ) {
          strcpy( poi_type, jsonParser.getVal() );
        }
        /*
         Serial.print( key );
         Serial.print( "\t = \"" );
         Serial.print( jsonParser.getVal() );
         Serial.println( "\"" );
         */
      }
    }
    if( error || ! client.connected() ) {
#ifdef DEBUG
      if( error ) {
        Serial.print("error=");
        Serial.println(error);
      }
      Serial.println("disconnecting");
#endif
      client.stop();
      if( jsonParser.getParseState() == JSON_PARSE_FINDJSON )
        error = MAPME_AT_ERR_RESULT;

    }
  }
}

// Crude cachebusting to get round the Maker Faire proxy
int gRequestCount = 0;

void MapMe_At::requestLocation() {
  byte server[] = { 
    188, 40, 54, 143 }; // mapme.at
#ifdef DEBUG
  Serial.println("connecting...");
#endif
  if( client.connect("mapme.at", 80) ) {
    jsonParser.clearState();
    label[0] = '\0';
    poi_type[0] = '\0';

#ifdef DEBUG
    Serial.println("connected");
#endif
    client.print("GET /api/where.json?username=");
    client.print(username);
    // Crude cachebusting to get round makerfaire proxy
    client.print("&mfuk=");
    client.print(gRequestCount++);
    client.print("&sharing_code=");
    client.print(sharing_code);
    client.println(" HTTP/1.0");
    client.print("Host: mapme.at");
    client.println();
    client.println();
  } 
  else {
    error = MAPME_AT_ERR_CONNECTION;
#ifdef DEBUG
    Serial.println("Connection failed");
#endif
  }
}

boolean MapMe_At::isActive() {
  return ( client.connected() ? true : false );
}

char* MapMe_At::getLabel() {
  return label;
}

char * MapMe_At::getPOIType() {
  return poi_type;
}
