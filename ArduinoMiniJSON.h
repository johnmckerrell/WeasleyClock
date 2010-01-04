#ifndef ARDUINOMINIJSON_H
#define ARDUINOMINIJSON_H

#include "WProgram.h"

/**
 * The JSON parser can be in one of the following states:
 * 0 - before we've confirmed we have JSON
 * 1 - we have JSON, that is all
 * 2 - We're in a key string
 * 3 - We have a key string and are looking for value
 * 4 - We're in a value string
 * 5 - We have valid key and value pair - can be picked up elsewhere
 * 6/7 - String was too long, clear it out
 * go back to 1 after each key/value pair
 * if a key has a non-string value we ignore it
 */
#define JSON_MAX_STRING 40
#define JSON_PARSE_FINDJSON 0
#define JSON_PARSE_HAVEJSON 1
#define JSON_PARSE_INKEY 2
#define JSON_PARSE_HAVEKEY 3
#define JSON_PARSE_INVAL 4
#define JSON_PARSE_HAVEVAL 5
#define JSON_PARSE_KEYTOOLONG 6
#define JSON_PARSE_VALTOOLONG 7

class ArduinoMiniJSON {
  public:
    void setup();
    void clearState();
    void handleInput(char c);
    char* getKey();
    char* getVal();
    int getParseState();
  
  private:
    char jsonKey[JSON_MAX_STRING];
    char jsonVal[JSON_MAX_STRING];
    int jsonKeyPos;
    int jsonValPos;
    int parseState;
    boolean escapeNextChar;
    

};

#endif
