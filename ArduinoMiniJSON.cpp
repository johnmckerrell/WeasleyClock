#include "ArduinoMiniJSON.h"

// Request current location

// Check if we're still getting a location

// Get currently stored location

void ArduinoMiniJSON::setup() {
  clearState();
}

/**
 * Clears the state before begining a new parse.
 */
void ArduinoMiniJSON::clearState() {
  // Only need to set the state, we
  // clear all other vars before use.
  parseState = 0;
  httpStatus = 0;
  httpSkip = 0;
}

/**
 * Basic getters for parse state, key and val properties.
 */

int ArduinoMiniJSON::getParseState() {
  return parseState;
}

char* ArduinoMiniJSON::getKey() {
  return jsonKey;
}

char* ArduinoMiniJSON::getVal() {
  return jsonVal;
}

int ArduinoMiniJSON::getHTTPStatus() {
  return httpStatus;
}

/**
 * 0 - before we've confirmed we have JSON
 * 1 - we have JSON, that is all
 * 2 - We're in a key string
 * 3 - We have a key string and are looking for value
 * 4 - We're in a value string
 * 5 - We have valid key and value pair - can be picked up elsewhere
 * 6/7 - String was too long, clear it out
 * go back to 1 after each key/value pair
 * if a key has a non-string value we ignore it
 #define JSON_PARSE_FINDJSON 0
#define JSON_PARSE_HAVEJSON 1
#define JSON_PARSE_INKEY 2
#define JSON_PARSE_HAVEKEY 3
#define JSON_PARSE_INVAL 4
#define JSON_PARSE_HAVEVAL 5
#define JSON_PARSE_KEYTOOLONG 6
#define JSON_PARSE_VALTOOLONG 7
 */
void ArduinoMiniJSON::handleInput(char c) {
  /*
  Serial.print(parseState);
  Serial.print( "\t" );
  Serial.print(jsonKeyPos);
  Serial.print("\t");
  Serial.print(jsonValPos);
  Serial.print("\t");
  Serial.print(escapeNextChar);
  Serial.print("\t");
  Serial.println(c);
  */
  if( parseState == JSON_PARSE_HAVEVAL ) {
    // We had a valid key/pair, now we start again.
    parseState = JSON_PARSE_HAVEJSON;
  }
  if( parseState == JSON_PARSE_CHECK_HTTP ) {
    if( httpSkip < 9 ) {
      // Skip HTTP/1.1<space>
      ++httpSkip;
    } else if( c >= '0' && c <= '9' ) {
      httpStatus = (httpStatus * 10) + (c - '0');
    } else {
      parseState = JSON_PARSE_FINDJSON;
    }
  } else if( parseState == JSON_PARSE_FINDJSON ) {
    if( c == '{' )
      parseState = JSON_PARSE_HAVEJSON;
  } else if( parseState == JSON_PARSE_HAVEJSON ) {
    if( c == '"' ) {
      parseState = JSON_PARSE_INKEY;
      jsonKey[0] = '\0';
      jsonVal[0] = '\0';
      jsonKeyPos = 0;
      jsonValPos = 0;
      escapeNextChar = false;
    }
  } else if( parseState == JSON_PARSE_INKEY ) {
    if( ! escapeNextChar && c == '"' ) {
      parseState = JSON_PARSE_HAVEKEY;
      escapeNextChar = false;
    } else if( ! escapeNextChar && c == '\\' ) {
      escapeNextChar = true;
    } else {
      escapeNextChar = false;
      jsonKey[jsonKeyPos] = c;
      jsonKey[++jsonKeyPos] = '\0';
      if( jsonValPos + 1 == JSON_MAX_STRING ) {
        parseState = JSON_PARSE_KEYTOOLONG;
      }
    }
  } else if( parseState == JSON_PARSE_HAVEKEY ) {
    if( c == '"' ) {
      parseState = JSON_PARSE_INVAL;
      escapeNextChar = false;
    } else if( c == '\n' || c == ' ' || c == '\t' || c == '\r' || c == ':' ) {
      // Ignore whitespace (or colon separator
    } else {
       // unexpected input, send us back to state 1
       //Serial.println("FOO!");
       parseState = JSON_PARSE_HAVEJSON;
    }
  } else if( parseState == JSON_PARSE_INVAL ) {
    if( ! escapeNextChar && c == '"' ) {
      parseState = JSON_PARSE_HAVEVAL;
      escapeNextChar = false;
    } else if( ! escapeNextChar && c == '\\' ) {
      escapeNextChar = true;
    } else {
      escapeNextChar = false;
      jsonVal[jsonValPos] = c;
      jsonVal[++jsonValPos] = '\0';
      if( jsonValPos + 1 == JSON_MAX_STRING ) {
        parseState = JSON_PARSE_VALTOOLONG;
      }
    }
  } else if( parseState == JSON_PARSE_KEYTOOLONG ) {
    if( ! escapeNextChar && c == '"' ) {
      parseState = JSON_PARSE_HAVEKEY;
      escapeNextChar = false;
    } else if( ! escapeNextChar && c == '\\' ) {
      escapeNextChar = true;
    }
  } else if( parseState == JSON_PARSE_VALTOOLONG ) {
    if( ! escapeNextChar && c == '"' ) {
      parseState = JSON_PARSE_HAVEVAL;
      escapeNextChar = false;
    } else if( ! escapeNextChar && c == '\\' ) {
      escapeNextChar = true;
    }
  }
}

