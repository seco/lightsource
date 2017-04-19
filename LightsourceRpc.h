#ifndef LIGHTSOURCE_RPC_H
#define LIGHTSOURCE_RPC_H

#include <ESP8266WebServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <FS.h>

#define DBG_OUTPUT_PORT Serial

class LightsourceRpc
{
  public:
    LightsourceRpc();
    void process(ESP8266WebServer &srv);
};

#endif
