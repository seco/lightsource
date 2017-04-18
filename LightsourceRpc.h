#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

class LightsourceRpc
{
  public:
    LightsourceRpc();
    void process(ESP8266WebServer &srv);
};
