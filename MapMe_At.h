#ifndef MapMe_At_h
#define MapMe_At_h

#include "Arduino.h"
#include <IPAddress.h>
#include <Client.h>
#include <Ethernet.h>
#include <Server.h>
#include <Dns.h>
#include "ArduinoMiniJSON.h"

#define MAPME_AT_MAX_STRING 40
#define MAPME_AT_ERR_CONNECTION 1
#define MAPME_AT_ERR_RESULT 2

class MapMe_At {
  public:
    void setup(const char *_username, const char *_sharing_code);
    void loop();
    void requestLocation();
    boolean isActive();
    char* getLabel();
    char* getPOIType();
    boolean wasError();
  
  private:
    EthernetClient client;
    int error;
    char username[MAPME_AT_MAX_STRING];
    char sharing_code[MAPME_AT_MAX_STRING];
    char label[MAPME_AT_MAX_STRING];
    char poi_type[MAPME_AT_MAX_STRING];
    ArduinoMiniJSON jsonParser;    

};

#endif
