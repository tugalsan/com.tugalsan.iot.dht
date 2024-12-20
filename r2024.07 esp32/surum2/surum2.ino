//DEVICE https://tr.aliexpress.com/item/1005006269242344.html?spm=a2g0o.order_list.order_list_main.5.50923d12h0gi9A&gatewayAdapt=glo2tur

//ARDUINO.IMPORT
#include <Arduino.h>

//WIFI.IMPORT
#include <WiFi.h>
#include <esp_wifi.h>
//#include "AsyncTCP.h"//https://github.com/me-no-dev/AsyncTCP/archive/master.zip
#include <ESPAsyncWebServer.h> // https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip

//IMPORT.DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>

//IMPORT.DISPLAY
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

//GLOBAL.ARDUINO
bool arduino_serial_enable = true;
bool arduino_serial_verbose = false;
long arduino_serial_frequency = 115200;
unsigned long arduino_loop_begin_ms = 0;
unsigned long arduino_loop_prev_ms = 0;
unsigned long arduino_loop_interval_ms = 5000;
String arduino_readable_clock;
String arduino_loop_readable_clock() {
  String readableTime;
  unsigned long currentMillis;
  unsigned long seconds;
  unsigned long minutes;
  unsigned long hours;
  unsigned long days;

  currentMillis = arduino_loop_begin_ms;
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
    readableTime += String(hours) + ".";
  }

  if (minutes < 10) {
    readableTime += "0";
  }
  readableTime += String(minutes) + ".";

  if (seconds < 10) {
    readableTime += "0";
  }
  readableTime += String(seconds);
  return readableTime;
}
void arduino_setup() {
  delay(2000);
  if (!arduino_serial_enable) {
    return;
  }
  Serial.begin(arduino_serial_frequency);
  if (arduino_serial_enable) Serial.println(F("."));
  if (arduino_serial_enable) Serial.println(F("."));
  if (arduino_serial_enable) Serial.println(F("warm-up begin..."));
  delay(500);
  Serial.println(F("arduino_setup.begin"));
  Serial.print(F("arduino_setup.frequency="));
  Serial.println(arduino_serial_frequency);
  Serial.println(F("arduino_setup.end"));
}
bool arduino_loop_begin() {
  arduino_loop_begin_ms = millis();
  if (arduino_loop_prev_ms > arduino_loop_begin_ms) {
    arduino_loop_prev_ms = arduino_loop_begin_ms;
    return false;
  }
  arduino_readable_clock = arduino_loop_readable_clock();
  if (arduino_loop_begin_ms - arduino_loop_prev_ms < arduino_loop_interval_ms) {
    return false;
  }
  if (arduino_serial_enable && arduino_serial_verbose) Serial.println(F("arduino_loop_begin.begin"));
  if (arduino_serial_enable) Serial.print(F("Clock: "));
  if (arduino_serial_enable) Serial.println(arduino_loop_readable_clock());
  if (arduino_serial_enable && arduino_serial_verbose) Serial.println(F("arduino_loop_begin.end"));
  return true;
}
void arduino_loop_end() {
  if (arduino_serial_enable && arduino_serial_verbose) Serial.println(F("arduino_loop_end.begin"));
  arduino_loop_prev_ms = arduino_loop_begin_ms;
  if (arduino_serial_enable && arduino_serial_verbose) Serial.println(F("arduino_loop_end.end"));
}

//GLOBAL.DHT
bool dht_enable = false;
bool dht_verbose = false;
#define DHTPIN 5       //try 5 4 0 2 15, 16 14 12 13
#define DHTTYPE DHT11  // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);
float dht_t_flt = 0.0;  //last temperature
float dht_h_flt = 0.0;  //last humidity
char dht_t_chr[6];
char dht_h_chr[6];
void dht_setup() {
  dtostrf(dht_t_flt, 5, 1, dht_t_chr);
  dtostrf(dht_h_flt, 5, 1, dht_h_chr);
  if (!dht_enable) {
    if (arduino_serial_enable) Serial.println(F("dht_setup.disabled!"));
    return;
  }
  if (arduino_serial_enable) Serial.println(F("dht_setup.begin"));
  dht.begin();
  if (arduino_serial_enable) Serial.println(F("dht_setup.end"));
}
void dht_loop() {
  if (!dht_enable) {
    if (arduino_serial_enable && dht_verbose) Serial.println(F("dht_loop.disabled!"));
    return;
  }
  if (arduino_serial_enable && dht_verbose) Serial.println(F("dht_loop.begin"));
  float newT = dht.readTemperature();
  float newH = dht.readHumidity();
  if (isnan(newT) || isnan(newH)) {
    dht_t_flt = 0.0;
    dht_h_flt = 0.0;
  } else {
    dht_t_flt = newT;
    dht_h_flt = newH;
  }
  dtostrf(dht_t_flt, 5, 2, dht_t_chr);
  dtostrf(dht_h_flt, 5, 2, dht_h_chr);
  if (arduino_serial_enable) Serial.print(F("Sic: "));
  if (arduino_serial_enable) Serial.println(dht_t_chr);
  if (arduino_serial_enable) Serial.print(F("Nem: "));
  if (arduino_serial_enable) Serial.println(dht_h_chr);
  if (arduino_serial_enable && dht_verbose) Serial.println(F("dht_loop.end"));
}

//GLOBAL.DISPLAY
bool display_enable = true;
bool display_verbose = false;
//D21 GPIO21 - SDA
//D22 GPIO22 - SCL
U8G2_SSD1306_128X64_NONAME_F_SW_I2C
u8g2(U8G2_R0, /*clock*/ 22, /*data*/ 21, U8X8_PIN_NONE);
void display_setup() {
  if (!display_enable) {
    if (arduino_serial_enable) Serial.println(F("display_setup.disabled!"));
    return;
  }
  if (arduino_serial_enable) Serial.println(F("display_setup.begin"));
  u8g2.begin();
  u8g2.setFont(u8g2_font_7x14B_tr);  //u8g2_font_unifont_t_chinese2 | u8g2_font_7x14B_tr
  //u8g2.enableUTF8Print();
  //u8g2.drawUTF8(0, 13, "Sic");
  if (arduino_serial_enable) Serial.println(F("display_setup.end"));
}
void display_loop_begin() {
  if (!display_enable) {
    if (arduino_serial_enable && display_verbose) Serial.println(F("display_loop_begin.disabled!"));
    return;
  }
  if (arduino_serial_enable && display_verbose) Serial.println(F("display_loop_begin.begin"));
  u8g2.clearBuffer();
  if (arduino_serial_enable && display_verbose) Serial.println(F("display_loop_begin.end"));
}
void display_loop_end() {
  if (!display_enable) {
    if (arduino_serial_enable && display_verbose) Serial.println(F("display_loop_end.disabled!"));
    return;
  }
  if (arduino_serial_enable && display_verbose) Serial.println(F("display_loop_end.begin"));
  u8g2.sendBuffer();
  if (arduino_serial_enable && display_verbose) Serial.println(F("display_loop_end.end"));
}

//GLOBAL.WIFI
bool wifi_enable = true;
bool wifi_verbose = false;
//const char* wifi_ssid = "MesaMetalWF";
//const char* wifi_pass = "DateIs01062015";
const char* wifi_ssid = "Mebosa";
const char* wifi_pass = "Bugun19112018";
uint8_t wifi_mac_custom[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
unsigned long wifi_loop_prev_ms = 0;
unsigned long wifi_loop_interval_ms = 30000;
String wifi_ip_current;
String wifi_setup_mac;
NetworkServer server(80);
//IPAddress local_IP(192, 168, 1, 184);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 0, 0);
//IPAddress primaryDNS(8, 8, 8, 8);   // optional
//IPAddress secondaryDNS(8, 8, 4, 4); // optional
String wifi_ipaddress_2_str(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]);
}
void wifi_setup() {
  if (!wifi_enable) {
    if (arduino_serial_enable) Serial.println(F("wifi_setup.disabled!"));
    return;
  }
  if (arduino_serial_enable) Serial.println(F("wifi_setup.begin"));
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, wifi_mac_custom);
  WiFi.begin(wifi_ssid, wifi_pass);
  wifi_setup_mac = WiFi.macAddress();
  if (arduino_serial_enable) Serial.print(F("wifi_config.wifi_setup_mac: "));
  if (arduino_serial_enable) Serial.println(wifi_setup_mac);
  if (arduino_serial_enable) Serial.println(F("wifi_setup.end"));
}
bool wifi_config_done = false;
void wifi_config() {
  if (wifi_config_done) {
    return;
  }
  if (!wifi_enable) {
    if (arduino_serial_enable) Serial.println(F("wifi_config.disabled!"));
    return;
  }
  //if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  //  Serial.println("STA Failed to configure");
  //}
  if (arduino_serial_enable) Serial.println(F("wifi_config.connecting..."));
  if (WiFi.status() != WL_CONNECTED) {
    if (arduino_serial_enable) Serial.print('.');
//    delay(1000);
    return;
  }
  if (arduino_serial_enable) Serial.println();
  wifi_ip_current = wifi_ipaddress_2_str(WiFi.localIP());
  if (arduino_serial_enable) Serial.print(F("wifi_config.wifi_ip_current: "));
  if (arduino_serial_enable) Serial.println(wifi_ip_current);
  server.begin();
  wifi_config_done = true;
  if (arduino_serial_enable) Serial.println(F("wifi_config.end"));
}
void wifi_loop() {
//  if (!wifi_enable) {
//    if (arduino_serial_enable && wifi_verbose) Serial.println(F("wifi_loop.disabled!"));
//    return;
//  }
  wifi_config();
  if (!wifi_config_done){
    return;
  }
  if (arduino_serial_enable && wifi_verbose) Serial.println(F("wifi_loop.begin"));
  if ((WiFi.status() != WL_CONNECTED) && (arduino_loop_begin_ms - wifi_loop_prev_ms >= wifi_loop_interval_ms)) {
    if (arduino_serial_enable) Serial.print(millis());
    if (arduino_serial_enable) Serial.println(F("wifi_loop.connecting..."));
    WiFi.disconnect();
    WiFi.reconnect();
    wifi_loop_prev_ms = arduino_loop_begin_ms;
    wifi_ip_current = wifi_ipaddress_2_str(WiFi.localIP());
    if (arduino_serial_enable) Serial.print(F("wifi_loop.wifi_ip_current: "));
    if (arduino_serial_enable) Serial.println(wifi_ip_current);
  }
  if (arduino_serial_enable && wifi_verbose) Serial.println(F("wifi_loop.end"));
}

void setup() {
  arduino_setup();
  wifi_setup();
  display_setup();
  dht_setup();
}


void loop() {
  if (!arduino_loop_begin()) {
    return;
  }
  dht_loop();
  wifi_loop();
  display_loop_begin();
  if (display_enable) {
    u8g2.setFont(u8g2_font_7x14B_tr);

    int y = 13;
    u8g2.drawStr(0, y, "Sic");
    u8g2.drawStr(18, y, dht_t_chr);
    u8g2.drawStr(58, y, ",Nem");
    u8g2.drawStr(85, y, dht_h_chr);

    int yo = 26;
    int yl = 12;
    int x3 = 22;
    int x4 = x3 + 11;

    y = yo + yl * 0;
    u8g2.setFont(u8g2_font_6x13B_tr);
    u8g2.drawStr(0, y, "SN#");
    u8g2.drawStr(x3, y, wifi_setup_mac.c_str());
    u8g2.setFont(u8g2_font_7x14B_tr);

    y = yo + yl * 1;
    u8g2.drawStr(0, y, "CLK");
    u8g2.drawStr(x4, y, arduino_readable_clock.c_str());

    y = yo + yl * 2;
    u8g2.drawStr(0, y, "WIFI");
    u8g2.drawStr(x4, y, wifi_ssid);

    y = yo + yl * 3;
    if (true) {
      u8g2.drawStr(0, y, "IP#");
      u8g2.drawStr(x4, y, wifi_ip_current.c_str());
    } else {
      u8g2.drawStr(0, y, "MOD");
      u8g2.drawStr(x4, y, "Olcum Modu");
    }
  }
  if (wifi_enable && wifi_config_done) {

    NetworkClient client = server.accept();  // listen for incoming clients

    if (client) {                     // if you get a client,
      Serial.println("New Client.");  // print a message out the serial port
      String currentLine = "";        // make a String to hold incoming data from the client
      while (client.connected()) {    // loop while the client's connected
        if (client.available()) {     // if there's bytes to read from the client,
          char c = client.read();     // read a byte, then
          Serial.write(c);            // print it out the serial monitor
          if (c == '\n') {            // if the byte is a newline character

            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();

              // the content of the HTTP response follows the header:
              client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
              client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

              // The HTTP response ends with another blank line:
              client.println();
              // break out of the while loop:
              break;
            } else {  // if you got a newline, then clear currentLine:
              currentLine = "";
            }
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
          }

          // Check to see if the client request was "GET /H" or "GET /L":
          if (currentLine.endsWith("GET /H")) {
            digitalWrite(5, HIGH);  // GET /H turns the LED on
          }
          if (currentLine.endsWith("GET /L")) {
            digitalWrite(5, LOW);  // GET /L turns the LED off
          }
        }
      }
      // close the connection:
      client.stop();
      Serial.println("Client Disconnected.");
    }
  }
  display_loop_end();
  arduino_loop_end();
}
