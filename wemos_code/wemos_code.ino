#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "index.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// Macros for LCD, LEDs & SENSOR pins
#define SCL D1
#define SDA D2
#define MEMORY_LED D5
#define TEMP_SENSOR D6

// WIFI creds
const char* AP_ssid = "Polaris"; //APSSID;
const char* AP_pass = "polaris123"; //APPSK;

// Network config
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

//Create server
ESP8266WebServer server(80);


// Form variables
int max_temp;
int min_temp;
int measures_freq;
char* ssid;
char* pass;
char* post_server;
char* api_key;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  // Init WIFI Network
  WiFi.softAP(AP_ssid, AP_pass);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.print("AP configured");

  //Init server
  server.on("/", handleRoot);
  server.on("/postconfig/", handlePostConfig);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.print("Server opened");

  pinMode(MEMORY_LED, OUTPUT);
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", index_page);
}

void handlePostConfig() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    max_temp = server.args("max_temp").toInt();
    min_temp = server.args("min_temp").toInt();
    measures_freq = server.args("measures_freq").toInt();
    ssid = server.args("ssid");
    pass = server.args("pass");
    post_server = server.args("post_server");
    api_key = server.args("api_key");
    server.send(200, "text/plain", "Config OK");
  }
}

void handleNotFound() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plane", "");
}
