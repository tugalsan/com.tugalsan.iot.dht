//https://tr.aliexpress.com/item/4001147892727.html?spm=a2g0o.order_list.order_list_main.5.684d3d12UrQbTI&gatewayAdapt=glo2tur

//IMPORT.WIFI
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

//IMPORT.SCREEN
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//IMPORT.DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>

//GLOBAL.WIFI
const char* ssid = "MesaMetalWF";
const char* password = "DateIs01062015";
AsyncWebServer server(80);

//GLOBAL.SCREEN
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//GLOBAL.DHT
#define DHTPIN 5       //try 5 4 0 2 15, 16 14 12 13
#define DHTTYPE DHT11  // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

//GLOBAL.PAGE
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: left;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 5px;
    }
  </style>
</head>
<body>
  <p>
    <i class="fas fa-tv" style="color:#05228a;"></i> 
    <span class="dht-labels">Device:</span> 
    <span id="device">%PH_DEVICE%</span>
  </p>
  <p>
    <i class="fas fa-paperclip" style="color:#03128a;"></i> 
    <span class="dht-labels">HTTP:</span> 
    <span id="ip">%PH_IP%</span>
  </p>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature:</span> 
    <span id="temperature">%PH_TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity:</span>
    <span id="humidity">%PH_HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <i class="fas fa-clock" style="color:#000000;"></i> 
    <span class="dht-labels">Clock:</span>
    <span id="clock">%PH_CLOCK%</span>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("clock").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/clock", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";
float t = 0.0;  //last temperature
float h = 0.0;  //last humidity
IPAddress ip;
String mac;
String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}
String getReadableClock() {
  String readableTime;
  unsigned long currentMillis;
  unsigned long seconds;
  unsigned long minutes;
  unsigned long hours;
  unsigned long days;

  currentMillis = millis();
  seconds = currentMillis / 1000;
  minutes = seconds / 60;
  hours = minutes / 60;
  days = hours / 24;
  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;

  if (days > 0) {
    readableTime = String(days) + " ";
  }

  if (hours > 0) {
    readableTime += String(hours) + ":";
  }

  if (minutes < 10) {
    readableTime += "0";
  }
  readableTime += String(minutes) + ":";

  if (seconds < 10) {
    readableTime += "0";
  }
  readableTime += String(seconds);
  return readableTime;
}
String processor(const String& var) {  // Replaces placeholder with DHT values
  if (var == "PH_DEVICE") {
    return mac;
  }
  if (var == "PH_IP") {
    return IpAddress2String(ip);
  }
  if (var == "PH_TEMPERATURE") {
    return String(t);
  }
  if (var == "PH_HUMIDITY") {
    return String(h);
  }
  if (var == "PH_CLOCK") {
    return getReadableClock();
  }
  return String();
}

//GLOBAL.SETUP
bool serialEnable = true;
bool wifiEnable = true;
bool displayEnable = true;

void setup() {
  if (serialEnable) Serial.begin(115200);

  if (serialEnable) Serial.println(F("."));
  if (serialEnable) Serial.println(F("."));
  if (serialEnable) Serial.println(F("warm-up begin..."));
  delay(500);

  if (serialEnable) Serial.println(F("dht will begin..."));
  dht.begin();

  if (displayEnable) {
    if (serialEnable) Serial.println(F("display will begin..."));
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      if (serialEnable) Serial.println(F("SSD1306 allocation failed"));
      for (;;)
        ;
    }
    delay(2000);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.display();
  }

  if (wifiEnable) {
    if (serialEnable) Serial.println(F("wifi will begin..."));
    WiFi.begin(ssid, password);

    if (serialEnable) Serial.println(F("wifi connecting..."));
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      if (serialEnable) Serial.println(".");
    }

    if (serialEnable) Serial.println(F("printing mac..."));
    mac = WiFi.macAddress();
    if (serialEnable) Serial.println(mac);

    if (serialEnable) Serial.println(F("printing ip..."));
    ip = WiFi.localIP();
    if (serialEnable) Serial.println(ip);

    if (serialEnable) Serial.println(F("binding servlet /..."));
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send_P(200, "text/html", index_html, processor);
    });

    if (serialEnable) Serial.println(F("binding servlet /temperature..."));
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send_P(200, "text/plain", String(t).c_str());
    });

    if (serialEnable) Serial.println(F("binding servlet /humidity..."));
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send_P(200, "text/plain", String(h).c_str());
    });

    if (serialEnable) Serial.println(F("binding servlet /clock..."));
    server.on("/clock", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send_P(200, "text/plain", getReadableClock().c_str());
    });

    if (serialEnable) Serial.println(F("starting server..."));
    server.begin();
  }
}

//GLOBAL.LOOP
const long interval = 10000;  // Updates DHT readings every 10 seconds
unsigned long previousMillis = 0;

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis < interval) {
    return;
  }
  previousMillis = currentMillis;
  if (serialEnable) Serial.println(F("loop.new"));

  if (serialEnable) Serial.println(F("dht.reading..."));
  {
    float newT = dht.readTemperature();
    float newH = dht.readHumidity();
    if (isnan(newT) || isnan(newH)) {
      t = 0.0;
      h = 0.0;
    } else {
      t = newT;
      h = newH;
    }
  }

  if (displayEnable) {
    if (serialEnable) Serial.println(F("display clearing..."));
    display.clearDisplay();

    if (serialEnable) Serial.print(F("display temp as "));
    if (serialEnable) Serial.println(t);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("Temperature: "));
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print(t);
    display.print(F(" "));
    display.setTextSize(1);
    display.cp437(true);
    display.write(167);
    display.setTextSize(2);
    display.print(F("C"));

    if (serialEnable) Serial.print(F("display humid as "));
    if (serialEnable) Serial.println(h);
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.print(F("Humidity: "));
    display.setTextSize(2);
    display.setCursor(0, 45);
    display.print(h);
    display.print(F(" %"));

    if (serialEnable) Serial.println(F("display render..."));
    delay(2000);
    display.display();
  }
}