#include <Adafruit_GFX_AS.h>
#include <Adafruit_ILI9341_fast_as.h>
#include <Ticker.h>
#include <Adafruit_NeoPixel.h>
#include "LightsourceHTTPHandlers.h"

const char* ssid = "twinkle";
const char* password = "analInvader";
IPAddress ip(10, 10, 10, 1);
IPAddress gw(10, 10, 10, 1);
IPAddress mask(255, 255, 255, 0);
static bool inSetup=false;

#define LIGHT_HEART_LED   D0
#define LIGHT_STRIP1_PIN  D1
#define LIGHT_STRIP2_PIN  D2
#define LIGHT_STRIP3_PIN  D3
#define LIGHT_STRIP4_PIN  D6

#define LIGHT_TFT_SCK     D5
#define LIGHT_TFT_MOSI    D7
#define LIGHT_TFT_CS      D8
#define LIGHT_TFT_DC      D4

Adafruit_ILI9341 tft = Adafruit_ILI9341();
#define LIGHT_ALLPIXELS for (int i=0; i<4; i++)
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(255, LIGHT_STRIP1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(255, LIGHT_STRIP2_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(255, LIGHT_STRIP3_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip4 = Adafruit_NeoPixel(255, LIGHT_STRIP4_PIN, NEO_GRB + NEO_KHZ800);
Ticker heartbeat;
Adafruit_NeoPixel *pixels[4] = { &strip1, &strip2, &strip3, &strip4 };
extern ESP8266WebServer httpServer;
extern ESP8266HTTPUpdateServer httpUpdater;

LightsourceRpc lightsourceRpc;

/*
 *  MAIN PROGRAM
 */
void setup(void){
  DBG_OUTPUT_PORT.begin(115200);
  DBG_OUTPUT_PORT.println();
  DBG_OUTPUT_PORT.println("Booting Sketch...");
  heartbeat.attach(0.1, heartbeatLed);

  DBG_OUTPUT_PORT.println("Init TDFT");
  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, gw, mask);
  WiFi.softAP(ssid, password, 11);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  tft.println("AP Name: ");
  tft.println(ssid);
  tft.println("AP IP address: ");
  tft.println(myIP);
  
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
  
  DBG_OUTPUT_PORT.println("Root handler");
  httpServer.on("/", httpLightConfig);

  // Attach FS/http handlers
  httpServer.on("/list", HTTP_GET, httpFileList);
  //load editor
  httpServer.on("/edit", HTTP_GET, [](){
    if(!httpFileRead("/edit.htm")) httpServer.send(404, "text/plain", "FileNotFound");
  });
  //create file
  httpServer.on("/edit", HTTP_PUT, httpFileCreate);
  //delete file
  httpServer.on("/edit", HTTP_DELETE, httpFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  httpServer.on("/edit", HTTP_POST, [](){ httpServer.send(200, "text/plain", ""); }, httpFileUpload);

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
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_OUTPUT_PORT.printf("\r\n");
  }

  DBG_OUTPUT_PORT.println("init led strips");
  LIGHT_ALLPIXELS { pixels[i]->begin(); }
  LIGHT_ALLPIXELS { pixels[i]->setPixelColor(0, Adafruit_NeoPixel::Color(16,0,0)); }
  LIGHT_ALLPIXELS { pixels[i]->show(); }
  DBG_OUTPUT_PORT.println("Enter loop");

  heartbeat.detach();
  heartbeat.attach(0.4, heartbeatLed);
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

void heartbeatLed()
{
  int state = digitalRead(LIGHT_HEART_LED);  // get the current state of GPIO1 pin
  digitalWrite(LIGHT_HEART_LED, !state);     // set pin to the opposite state
}


