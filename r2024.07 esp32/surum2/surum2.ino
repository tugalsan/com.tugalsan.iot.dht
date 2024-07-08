//DEVICE https://tr.aliexpress.com/item/1005006269242344.html?spm=a2g0o.order_list.order_list_main.5.50923d12h0gi9A&gatewayAdapt=glo2tur

//ARDUINO.IMPORT
#include <Arduino.h>

//WIFI.IMPORT
#include <WiFi.h>

//GLOBAL.DISPLAY
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

//GLOBAL.SERIAL

//GLOBAL.ARDUINO
bool arduino_serial_enable = true;
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
void arduino_setup() {
  delay(2000);
  if (!arduino_serial_enable) {
    return;
  }
  Serial.begin(arduino_serial_frequency);
  delay(500);
  Serial.println(F("arduino_setup.begin"));
  Serial.print(F("arduino_setup.frequency="));
  Serial.println(arduino_serial_frequency);
  Serial.println(F("arduino_setup.end"));
}
bool arduino_loop_begin() {
  arduino_loop_begin_ms = millis();
  arduino_readable_clock = arduino_loop_readable_clock();
  if (arduino_loop_begin_ms - arduino_loop_prev_ms < arduino_loop_interval_ms) {
    return false;
  }
  if (arduino_serial_enable) Serial.println(F("arduino_loop_begin.begin"));
  if (arduino_serial_enable) Serial.print(F("Clock: "));
  if (arduino_serial_enable) Serial.println(arduino_loop_readable_clock());
  if (arduino_serial_enable) Serial.println(F("arduino_loop_begin.end"));
  return true;
}
void arduino_loop_end() {
  if (arduino_serial_enable) Serial.println(F("arduino_loop_end.begin"));
  arduino_loop_prev_ms = arduino_loop_begin_ms;
  if (arduino_serial_enable) Serial.println(F("arduino_loop_end.end"));
}

//GLOBAL.DISPLAY
bool displayEnable = true;
//D21 GPIO21 - SDA
//D22 GPIO22 - SCL
U8G2_SSD1306_128X64_NONAME_F_SW_I2C
u8g2(U8G2_R0, /*clock*/ 22, /*data*/ 21, U8X8_PIN_NONE);
void display_setup() {
  if (!displayEnable) {
    if (arduino_serial_enable) Serial.println(F("display_setup.disabled!"));
    return;
  }
  if (arduino_serial_enable) Serial.println(F("display_setup.begin"));
  u8g2.begin();
  if (arduino_serial_enable) Serial.println(F("display_setup.end"));
}
void display_loop_begin() {
  if (!displayEnable) {
    if (arduino_serial_enable) Serial.println(F("display_loop_begin.disabled!"));
    return;
  }
  if (arduino_serial_enable) Serial.println(F("display_loop_begin.begin"));
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tr);
  if (arduino_serial_enable) Serial.println(F("display_loop_begin.end"));
}
void display_loop_end() {
  if (!displayEnable) {
    if (arduino_serial_enable) Serial.println(F("display_loop_end.disabled!"));
    return;
  }
  if (arduino_serial_enable) Serial.println(F("display_loop_end.begin"));
  u8g2.sendBuffer();
  if (arduino_serial_enable) Serial.println(F("display_loop_end.end"));
}


//GLOBAL.WIFI
bool wifiEnable = false;
const char* ssid = "MesaMetalWF";
const char* password = "DateIs01062015";
unsigned long wifi_loop_prev_ms = 0;
unsigned long wifi_loop_interval_ms = 30000;
IPAddress wifi_ip_current;
String wifi_setup_mac;
//IPAddress local_IP(192, 168, 1, 184);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 0, 0);
//IPAddress primaryDNS(8, 8, 8, 8);   // optional
//IPAddress secondaryDNS(8, 8, 4, 4); // optional
void wifi_setup() {
  if (!wifiEnable) {
    if (arduino_serial_enable) Serial.println(F("wifi_setup.disabled!"));
    return;
  }
  if (arduino_serial_enable) Serial.println(F("wifi_setup.begin"));
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  //  Serial.println("STA Failed to configure");
  //}
  if (arduino_serial_enable) Serial.println(F("wifi_setup.connecting..."));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  wifi_ip_current = WiFi.localIP();
  wifi_setup_mac;
  WiFi.macAddress();
  if (arduino_serial_enable) Serial.println(F("wifi_setup.end"));
}
void wifi_loop() {
  if (!wifiEnable) {
    if (arduino_serial_enable) Serial.println(F("wifi_loop.disabled!"));
    return;
  }
  if (arduino_serial_enable) Serial.println(F("wifi_loop.begin"));
  if ((WiFi.status() != WL_CONNECTED) && (arduino_loop_begin_ms - wifi_loop_prev_ms >= wifi_loop_interval_ms)) {
    Serial.print(millis());
    if (arduino_serial_enable) Serial.println(F("wifi_loop.connecting..."));
    WiFi.disconnect();
    WiFi.reconnect();
    wifi_loop_prev_ms = arduino_loop_begin_ms;
    wifi_ip_current = WiFi.localIP();
  }
  if (arduino_serial_enable) Serial.println(F("wifi_loop.end"));
}

void setup() {
  arduino_setup();
  wifi_setup();
  display_setup();
}

void loop() {
  if (!arduino_loop_begin()){
    return;
  }
  wifi_loop();
  display_loop_begin();
  if (displayEnable) {
    u8g2.drawStr(0, 10, "Clock:");
    u8g2.drawStr(42, 10, arduino_readable_clock.c_str());
  }
  display_loop_end();
  arduino_loop_end();
}
