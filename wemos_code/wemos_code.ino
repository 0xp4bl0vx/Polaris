#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "index.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// Macros for LCD, LEDs & SENSOR pins
#define SCL D1
#define SDA D2
#define MEMORY_LED D5
#define TEMP_SENSOR D6
#define RESET D5

const byte measureMode = 40;
const int modeAddress = 0;
int address = 1;
const byte stringEnd = 255;

// WIFI creds
const char* APSsid = "Polaris"; //APSSID;
const char* APPass = "polaris123"; //APPSK;

// Network config
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

//Create server
ESP8266WebServer server(80);

// Form variables
int8_t maxTemp, minTemp;
uint8_t measuresSendFreq;
String ssid, pass, postServer, apiKey;

// Temperature Sensor config
OneWire oneWire(TEMP_SENSOR);
DallasTemperature sensors(&oneWire);

int measuresFreq = 20000;
bool previousAlarm = false;
int actualTime = 0;
int previousTime = 0;

// LCD Display config
LiquidCrystal_I2C lcd(0x27, 16, 2);
byte DegreesDot[8] =
{
0b00000,
0b00111,
0b00101,
0b00111,
0b00000,
0b00000,
0b00000,
0b00000
};

byte SmileFace[8] =
{
0b00000,
0b01010,
0b01010,
0b01010,
0b00000,
0b10001,
0b01110,
0b00000
};

int mode = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  
  EEPROM.begin(256);
  mode = EEPROM.read(modeAddress);

  pinMode(MEMORY_LED, OUTPUT);
  digitalWrite(RESET, HIGH);
  pinMode(RESET, OUTPUT);

  // LCD Display setup
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.createChar(0, DegreesDot);
  lcd.createChar(1, SmileFace);

  // Sensor setup
  sensors.begin();

  if (mode != measureMode) {
    // Init WIFI Network
    WiFi.softAP(APSsid, APPass);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    Serial.println("AP configurado");

    //Init server
    server.on("/", handleRoot);
    server.on("/postconfig/", handlePostConfig);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("Servidor abierto");  
  } else if (mode == measureMode) {

    Serial.println("Leyendo la configuración de la memoria");
    maxTemp = EEPROM.read(address);
    address++;
    minTemp = EEPROM.read(address);
    address++;
    measuresSendFreq = EEPROM.read(address);
    address++;
    readString(&address, &ssid);
    readString(&address, &pass);
    readString(&address, &postServer);
    readString(&address, &apiKey);

    sensors.setHighAlarmTemp(0, maxTemp);
    sensors.setLowAlarmTemp(0, minTemp);
    sensors.setAlarmHandler(&newAlarmHandler);
    measuresFreq = measuresSendFreq * 60 * 1000; 

    Serial.print("Conectandose a ");
    Serial.print(ssid);
    WiFi.mode(WIFI_STA);
    lcd.setCursor(0, 0);
    lcd.print("Cargando");

    WiFi.begin(ssid.c_str(), pass.c_str());
    int dots = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      
      lcd.print(".");
      dots++;
      if (dots >= 3) {
        delay(500);
        dots = 0;
        lcd.setCursor(8, 0);
        lcd.print("   ");
        lcd.setCursor(8, 0);
      }
    }
    Serial.println("");
    Serial.println("Conectado correctamente");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  sensors.requestTemperatures();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(sensors.getTempCByIndex(0));
  lcd.write(0);
  lcd.print("C");

  if (mode != measureMode) {
    server.handleClient();

    lcd.setCursor(0, 1);
    lcd.print("Sin Configurar");
  } else if (mode == measureMode) {
    actualTime = millis();
    if (actualTime > previousTime && actualTime - previousTime >= measuresFreq) {
      // Data Sent
    }
    previousTime = actualTime;
  }
  delay(measuresFreq);
}

void handleRoot() {
  server.send(200, "text/html", indexPage);
}

void handlePostConfig() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    String error;
    if (checkFormInput(&error)) {
      maxTemp = server.arg("max_temp").toInt();
      minTemp = server.arg("min_temp").toInt();
      measuresSendFreq = server.arg("measures_freq").toInt();
      ssid = inputTrim(server.arg("ssid"));
      pass = inputTrim(server.arg("pass"));
      postServer = inputTrim(server.arg("post_server"));
      apiKey = inputTrim(server.arg("api_key"));
      server.send(200, "text/plain", "Polaris configurado correctamente");
      
      EEPROM.write(modeAddress, measureMode);
      EEPROM.write(address, maxTemp);
      address++;
      EEPROM.write(address, minTemp);
      address++;
      EEPROM.write(address, measuresSendFreq);
      address++;
      writeString(&address, ssid);
      writeString(&address, pass);
      writeString(&address, postServer);
      writeString(&address, apiKey);
      EEPROM.commit();

      lcd.setCursor(0, 0);
      lcd.print("Configurado ");
      lcd.write(1);
      lcd.setCursor(0, 1);
      lcd.print("Reiniciando...");
      delay(4000);
      digitalWrite(RESET, LOW);
    } else {
      server.send(200, "text/plain", error);
    }
  }
}

void handleNotFound() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plane", "");
}

bool checkFormInput(String *error) {
  for (int i = 0; i < server.args(); i++) {
    if (server.arg(i) == "") {
      *error = "Ningun campo puede estar vacio";
      return false;
    }
  }

  if (!checkNum(server.arg(0))) {
    *error = "Formato de número incorrecto (Ej: 12, 12.5, 0.30)";
    return false;
  } else if (server.arg(0).toInt() < -50 || server.arg(0).toInt() > 120) {
    *error = "Los límites de temperatura deben de estar entre -50 y 120";
    return false;
  }

  if (!checkNum(server.arg(1))) {
    *error = "Formato de número incorrecto (Ej: 12, 12.5, 0.30)";
    return false;
  } else if (server.arg(1).toInt() < -50 || server.arg(1).toInt() > 120) {
    *error = "Los límites de temperatura deben de estar entre -50 y 120";
    return false;
  }

  if (!checkNum(server.arg(2))) {
    *error = "Formato de número incorrecto (Ej: 12, 12.5, 0.30)";
    return false;
  } else if (server.arg(2).toInt() <= 0 || server.arg(2).toInt() > 255) {
    *error = "La frequencia de las medidas debe ser de entre 1 y 255 minutos";
    return false;
  }

  if (server.arg(0) <= server.arg(1)) {
    *error = "La temperatura máxima debe ser mayor que la mínima";
    return false;
  }

  if (!checkUrl(server.arg(5))) {
    *error = "Formato de URL incorrecto (Ej: https://ejemplo.com/, http://10.10.10.10:8080/)";
    return false;
  }

  return true;
}

bool checkNum(String num) {
  for (int i = 0; i < num.length(); i++) {
    if (!isDigit(num[i]) && !(i == 0 && num[i] == '-')) {
      if (i > 0 && num[i] == '.') {
        for (int j = i + 1; j < num.length(); j++) {
          if (!isDigit(num[j])) {
            return false;
          }
        }
      } else {
        return false;
      }
      break;
    }
  }
  return true;
}

bool checkUrl(String url) {
  if (url.substring(0, 7) != "http://" && url.substring(0, 8) != "https://") {
    return false;
  } else if (url.indexOf(".") < 8) {
    return false;
  }
  return true;
}

String inputTrim(String user_input) {
  user_input.trim();
  return user_input;
}

void writeString(int *address, String string) {
  if (*address + string.length() + 1 >= 512) {
      Serial.println("Data is too long");
      return;
  } else {
    for (int i = 0; i < string.length(); i++) {
    EEPROM.write(*address, string[i]);
    (*address)++;
    }
    EEPROM.write(*address, stringEnd);
    (*address)++;
  }
}

void readString(int *address, String *string) {
  int i = 0;
  while (EEPROM.read(*address) != 255) {
    (*string).concat((char)EEPROM.read(*address));;
    (*address)++;
    i++;
  }
  (*address)++;
}

void newAlarmHandler(const uint8_t* deviceAddress) {
  int temp = sensors.getTempCByIndex(0);
  int max = sensors.getHighAlarmTemp(0);
  max++;
  int low = sensors.getLowAlarmTemp(0);
  low++;
  if (previousAlarm) {
    //Send alert to server
    previousAlarm = false;
    return;
  }

  if (temp != DEVICE_DISCONNECTED_C) {
    if (temp > max) {
      Serial.println("Temperatura muy alta");
      previousAlarm = true;
    } else if (temp < low) {
      Serial.println("Temperatura muy baja");
      previousAlarm = true;
    }
  } else {
    Serial.print("Sensor desconectado");
  }
}
