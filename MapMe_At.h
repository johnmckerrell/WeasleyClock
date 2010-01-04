#ifndef MapMe_At_h
#define MapMe_At_h

#include "WProgram.h"
#include <Client.h>
#include <Ethernet.h>
#include <Server.h>
#include "ArduinoMiniJSON.h"

#define MAPME_AT_MAX_STRING 40
#define MAPME_AT_ERR_CONNECTION 1
#define MAPME_AT_ERR_RESULT 2

class MapMe_At {
  public:
    void setup(const char *_username);
    void loop();
    void requestLocation();
    boolean isActive();
    char* getLocation();
    boolean wasError();
  
  private:
    Client *client;
    int error;
    char username[MAPME_AT_MAX_STRING];
    char location[MAPME_AT_MAX_STRING];
    ArduinoMiniJSON jsonParser;    

};

#endif
