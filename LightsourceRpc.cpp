#include "LightsourceRpc.h"

void LightsourceRpc::begin()
{
  DBG("LightsourceRpc::begin> \n");

  lightsourceStrips = new LightsourceStrips();
}

LightsourceRpc::~LightsourceRpc()
{
	delete lightsourceStrips;
}

void LightsourceRpc::process(ESP8266WebServer &srv)
{  
  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& jsonRpcRequest = jsonBuffer.parseObject(srv.arg(0));
  
  if (strcmp(jsonRpcRequest["method"], "lightsource.init") == 0)
    srv.send(200, "application/json-rpc", init(jsonRpcRequest));
  if (strcmp(jsonRpcRequest["method"], "lightsource.testConfig") == 0)
    srv.send(200, "application/json-rpc", testConfig(jsonRpcRequest));
  else
    srv.send(200, "application/json-rpc", "{\"jsonrpc\": \"2.0\", \"id\": \"" + String((const char *)jsonRpcRequest["id"]) + "\", \"error\": {\"code\": -32601, \"message\": \"Method not found\"}");
}

String LightsourceRpc::init(JsonObject &request)
{
  DBG("LightsourceRpc::init> request content\n---------------------------------------------\n");
  request.prettyPrintTo(Serial);
  DBG("\n---------------------------------------------\n");
  return "{\"jsonrpc\": \"2.0\", \"id\": \"" + String((const char *)request["id"]) + "\", \"result\": " + getFileContents("/config.json") + "}";
}

String LightsourceRpc::testConfig( JsonObject &request)
{
  DBG("LightsourceRpc::testConfig> request content\n---------------------------------------------\n");
  request.prettyPrintTo(Serial);
  DBG("\n---------------------------------------------\n");
  return "{\"jsonrpc\": \"2.0\", \"id\": \"" + String((const char *)request["id"]) + "\", \"result\": \"0\"}";
}
String LightsourceRpc::writeConfig( JsonObject &request)
{
  DBG("LightsourceRpc::writeConfig> request content\n---------------------------------------------\n");
  request.prettyPrintTo(Serial);
  DBG("\n---------------------------------------------\n");
  return "{\"jsonrpc\": \"2.0\", \"id\": \"" + String((const char *)request["id"]) + "\", \"result\": \"0\"}";
}

