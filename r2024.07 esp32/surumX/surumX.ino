//LOOP
unsigned long loop_start_ms = 0;

//DISPLAY
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
//D21 GPIO21 - SDA
//D22 GPIO22 - SCL
//DISPLAY
U8G2_SSD1306_128X64_NONAME_F_SW_I2C
u8g2(U8G2_R0, /*clock*/ 22, /*data*/ 21, U8X8_PIN_NONE);

//ESP32
#include <WiFi.h>
// Set your Static IP address
//IPAddress local_IP(192, 168, 1, 184);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 0, 0);
//IPAddress primaryDNS(8, 8, 8, 8);   // optional
//IPAddress secondaryDNS(8, 8, 4, 4); // optional
// Replace with your network credentials
const char* ssid = "MesaMetalWF";
const char* password = "DateIs01062015";
void wifiInit() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  // Configures static IP address
  //if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  //  Serial.println("STA Failed to configure");
  //}
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
unsigned long wifi_loop_prev_ms = 0;
unsigned long wifi_loop_interval_ms = 30000;
void wifiReconnect() {
  if ((WiFi.status() != WL_CONNECTED) && (loop_start_ms - wifi_loop_prev_ms >= wifi_loop_interval_ms)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    wifi_loop_prev_ms = loop_start_ms;
  }
}

void setup() {
  u8g2.begin();
  Serial.begin(115200);
  wifiInit();
}



void printWifi() {
  String printWifi_0 = "  %";
  String printWifi_1 = " (";
  String printWifi_2 = " -dBm)";
  long rssi_mdbm = WiFi.RSSI();
  //long rssi_percent = map(-rssi_mdbm, 1, 60, 1, 255);  //usually 0-60 or 0-255
  //u8g2.drawStr(0, 10, "Wifi Strength:");
  //String msg = printWifi_0 + rssi_percent + printWifi_1 + rssi_mdbm + printWifi_2;
  //Serial.println(msg);
  //u8g2.drawStr(0, 30, msg.c_str());
}


void loop() {
  loop_start_ms = millis();
  wifiReconnect();
  //u8g2.clearBuffer();
  //u8g2.setFont(u8g2_font_7x14B_tr);
  //printWifi();
  //u8g2.drawStr(0, 10, "Hello");
  //u8g2.sendBuffer();
  delay(1000);
}
