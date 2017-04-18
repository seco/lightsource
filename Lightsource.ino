#include <Ticker.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FS.h>
#include "LightsourceRpc.h"

const char* ssid = "FAST3764-ED38";
const char* password = "54dkl182";

#define LIGHT_ALLPIXELS for (int i=0; i<4; i++)
#define LIGHT_STRIP1_PIN D1
#define LIGHT_STRIP2_PIN D2
#define LIGHT_STRIP3_PIN D5
#define LIGHT_STRIP4_PIN D6
#define DBG_OUTPUT_PORT Serial

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(255, LIGHT_STRIP1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(255, LIGHT_STRIP2_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(255, LIGHT_STRIP3_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(255, LIGHT_STRIP4_PIN, NEO_GRB + NEO_KHZ800);

Ticker heartbeat;

Adafruit_NeoPixel *pixels[4] = { &strip1, &strip2, &strip3, &strip4 };
IPAddress ip(192, 168, 1, 123);
IPAddress gw(192, 168, 1, 1);
IPAddress dns(8,8,8,8);
IPAddress mask(255, 255, 255, 0);

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
LightsourceRpc lightsourceRpc;

/* HTTP callbacks */
static void httpLightConfig();
static void httpFileList();
static String httpGetContentType(String filename);
static bool httpFileRead(String path);
static void httpNotRegistered();
static void httpRpc();

/*
 *  MAIN PROGRAM
 */
void setup(void){
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.println();
  DBG_OUTPUT_PORT.println("Booting Sketch...");
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(D0, LOW);
    delay(55);
    digitalWrite(D0, HIGH);
    delay(55);
    digitalWrite(D0, LOW);
    DBG_OUTPUT_PORT.print(".");
  }

  WiFi.config(ip, dns, gw, mask);
  
  DBG_OUTPUT_PORT.println("");
  DBG_OUTPUT_PORT.println("WiFi connected");

  digitalWrite(D0, HIGH);
  
  // Print the IP address
  DBG_OUTPUT_PORT.print("Use this URL to connect: ");
  DBG_OUTPUT_PORT.print("http://");
  DBG_OUTPUT_PORT.print(WiFi.localIP());
  DBG_OUTPUT_PORT.println("/");

  DBG_OUTPUT_PORT.print("Adding callbkacks");

  DBG_OUTPUT_PORT.println("Not registered location handler");
  httpServer.onNotFound(httpNotRegistered);
  
  DBG_OUTPUT_PORT.print("Root handler");
  httpServer.on("/", httpLightConfig);

  DBG_OUTPUT_PORT.println("/list");
  httpServer.on("/list", HTTP_GET, httpFileList);

  DBG_OUTPUT_PORT.println("/rpc");
  httpServer.on("/rpc", HTTP_GET, httpRpc);
  
  DBG_OUTPUT_PORT.println("/update");
  httpUpdater.setup(&httpServer);
  
  DBG_OUTPUT_PORT.println("start web server");
  httpServer.begin();

  DBG_OUTPUT_PORT.println("init spiffs");
  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {    
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_OUTPUT_PORT.printf("\n");
  }

  DBG_OUTPUT_PORT.println("init led strips");
  LIGHT_ALLPIXELS { pixels[i]->begin(); }
  LIGHT_ALLPIXELS { pixels[i]->setPixelColor(i, Adafruit_NeoPixel::Color(16,0,0)); }
  LIGHT_ALLPIXELS { pixels[i]->show(); }
  
  heartbeat.attach(0.6, heartbeatLed);

  DBG_OUTPUT_PORT.println("attach MDNS and loop");
}

/*
 * Main Loop
 */
void loop(void)
{
  httpServer.handleClient();
}

/* utilities */
String formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

static void heartbeatLed()
{
  int state = digitalRead(D0);  // get the current state of GPIO1 pin
  digitalWrite(D0, !state);     // set pin to the opposite state
}

void httpLightConfig()
{
  DBG_OUTPUT_PORT.println("HTTP> root handler");
  httpFileRead("/index.html");
}

void httpFileList()
{
  DBG_OUTPUT_PORT.println("HTTP> file list handler");
  
  if(!httpServer.hasArg("dir")) {httpServer.send(500, "text/plain", "BAD ARGS"); return;}
  
  String path = httpServer.arg("dir");
  DBG_OUTPUT_PORT.println("HTTP> " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next()){
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }
  
  output += "]";
  httpServer.send(200, "text/json", output);
}

bool httpFileRead(String path)
{
  DBG_OUTPUT_PORT.println("HTTP> handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";
  String contentType = httpGetContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = httpServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

static void httpNotRegistered()
{
  if (!httpFileRead(httpServer.uri()))
  {
    String message = "Location not found in SPIFFS or not registsred as a callback \n\n";
    message += "URI: ";
    message += httpServer.uri();
    message += "\nMethod: ";
    message += (httpServer.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += httpServer.args();
    message += "\n";
    for (uint8_t i=0; i<httpServer.args(); i++){
     message += " NAME:"+httpServer.argName(i) + "\n VALUE:" + httpServer.arg(i) + "\n";
    }
    httpServer.send(404, "text/plain", message);
    DBG_OUTPUT_PORT.print(message);
  }
}

String httpGetContentType(String filename)
{
  if(httpServer.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

static void httpRpc()
{
  lightsourceRpc.process(httpServer);
}


