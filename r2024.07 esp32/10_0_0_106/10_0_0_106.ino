//DEVICE https://tr.aliexpress.com/item/1005006269242344.html?spm=a2g0o.order_list.order_list_main.5.50923d12h0gi9A&gatewayAdapt=glo2tur
/*ESP WROOM 32
GPIO	Input	Output	Notes
0	pulled up	OK	outputs PWM signal at boot, must be LOW to enter flashing mode
1	TX pin	OK	debug output at boot
2	OK	OK	connected to on-board LED, must be left floating or LOW to enter flashing mode
3	OK	RX pin	HIGH at boot
4	OK	OK	
5	OK	OK	outputs PWM signal at boot, strapping pin
6	x	x	connected to the integrated SPI flash
7	x	x	connected to the integrated SPI flash
8	x	x	connected to the integrated SPI flash
9	x	x	connected to the integrated SPI flash
10	x	x	connected to the integrated SPI flash
11	x	x	connected to the integrated SPI flash
12	OK	OK	boot fails if pulled high, strapping pin
13	OK	OK	
14	OK	OK	outputs PWM signal at boot
15	OK	OK	outputs PWM signal at boot, strapping pin
16	OK	OK	
17	OK	OK	
18	OK	OK	
19	OK	OK	
21	OK	OK	
22	OK	OK	
23	OK	OK	
25	OK	OK	
26	OK	OK	
27	OK	OK	
32	OK	OK	
33	OK	OK	
34	OK		input only
35	OK		input only
36	OK		input only
39	OK		input only
*/




//ARDUINO.IMPORT
#include <Arduino.h>

//WIFI.IMPORT
#include <WiFi.h>
#include <WiFiMulti.h>
#include <esp_wifi.h>
//#include "AsyncTCP.h"//https://github.com/me-no-dev/AsyncTCP/archive/master.zip
#include <ESPAsyncWebServer.h>  // https://github.com/me-no-dev/ESPAsyncWebServer/archive/master.zip

//IMPORT.DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>

//IMPORT.DISPLAY
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

//GLOBAL.ARDUINO
bool arduino_serial_enable = false;
bool arduino_serial_verbose = false;
long arduino_serial_frequency = 115200;
unsigned long arduino_loop_begin_ms = 0;
unsigned long arduino_loop_prev_ms = 0;
unsigned long arduino_loop_interval_ms = 5000;
String arduino_readable_clock;
unsigned long arduino_readable_clock_offset_millis = 0;
String arduino_loop_readable_clock() {
  String readableTime;
  unsigned long currentMillis;
  unsigned long seconds;
  unsigned long minutes;
  unsigned long hours;
  unsigned long days;
  currentMillis = arduino_loop_begin_ms + arduino_readable_clock_offset_millis;
  seconds = currentMillis / 1000;
  minutes = seconds / 60;
  hours = minutes / 60;
  days = hours / 24;
  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  if (days > 1) {
    esp_restart();
  }
  if (hours < 10) {
    readableTime += "0";
  }
  readableTime += String(hours) + ".";
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
  if (arduino_serial_enable) Serial.println(arduino_readable_clock);
  if (arduino_serial_enable && arduino_serial_verbose) Serial.println(F("arduino_loop_begin.end"));
  return true;
}
void arduino_loop_end() {
  if (arduino_serial_enable && arduino_serial_verbose) Serial.println(F("arduino_loop_end.begin"));
  arduino_loop_prev_ms = arduino_loop_begin_ms;
  if (arduino_serial_enable && arduino_serial_verbose) Serial.println(F("arduino_loop_end.end"));
}

//GLOBAL.DHT
bool dht_enable = true;
bool dht_verbose = false;
#define DHTPIN 23      //esp8266 5 4 0 2 15, 16 14 12 13 | esp32: 23, 21, 27...
#define DHTTYPE DHT22  // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);
float dht_t_flt = 0.0;  //last temperature
float dht_h_flt = 0.0;  //last humidity
char dht_t_chr[8];
char dht_h_chr[8];
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
  //newT = 100;
  //newH = 100;
  if (isnan(newT) || isnan(newH)) {
    dht_t_flt = 0.0;
    dht_h_flt = 0.0;
  } else {
    dht_t_flt = newT;
    dht_h_flt = newH;
  }
  dtostrf(dht_t_flt, 5, 1, dht_t_chr);
  dtostrf(dht_h_flt, 5, 1, dht_h_chr);
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
//uint8_t wifi_mac_custom[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
uint8_t wifi_mac_custom[] = { 0x0A, 0x00, 0x00, 0x01, 0x00, 0x06 };//10.0.0.106
const uint32_t wifi_loop_interval_ms = 10000;
String wifi_ip_current;
String wifi_ssid_current;
//String wifi_rssi_current;
String wifi_setup_mac;
WiFiMulti wifiMulti;
AsyncWebServer server(80);
//IPAddress local_IP(192, 168, 1, 184);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 0, 0);
//IPAddress primaryDNS(8, 8, 8, 8);   // optional
//IPAddress secondaryDNS(8, 8, 4, 4); // optional
String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i) {
    char buf[3];
    sprintf(buf, "%02X", ar[i]);  // J-M-L: slight modification, added the 0 in the format for padding
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}
String wifi_mac_custom_str = mac2String(wifi_mac_custom);
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
</head>
<body>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Sic</span> 
    <span id="temperature">%TMP%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Nem</span>
    <span id="humidity">%HUM%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <i class="fas fa-tv" style="color:#03128a;"></i> 
    <span class="dht-labels">SN#</span> 
    <span id="ip">%MAC%</span>
  </p>
  <p>
    <i class="fas fa-clock" style="color:#000000;"></i> 
    <span class="dht-labels">CLK</span>
    <span id="clock">%CLK%</span>
  </p>
  <p>
    <i class="fas fa-wifi" style="color:#000000;"></i> 
    <span class="dht-labels">WIFI</span>
    <span id="clock">%SSID%</span>
  </p>
  <p>
    <i class="fas fa-paperclip" style="color:#05228a;"></i> 
    <span class="dht-labels">IP#</span> 
    <span id="device">%IP%</span>
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
  xhttp.open("GET", "/t", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/h", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("clock").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/c", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";
String index_html_processor(const String& var) {
  if (var == "IP") {
    return wifi_ip_current;
  }
  if (var == "MAC") {
    return wifi_setup_mac;
  }
  if (var == "TMP") {
    return String(dht_t_chr);
  }
  if (var == "HUM") {
    return String(dht_h_chr);
  }
  if (var == "SSID") {
    return String(wifi_ssid_current);
  }
  if (var == "CLK") {
    return arduino_readable_clock;
  }
  return String();
}
String wifi_ipaddress_2_str(const IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]);
}
bool wifi_connected = false;
void _wifi_global_clear() {
  wifi_connected = false;
  wifi_ssid_current = String(F("not connected!"));
  //  wifi_rssi_current = String(F("not connected!"));
  wifi_ip_current = String(F("not connected!"));
}
void _wifi_global_load() {
  wifi_connected = true;
  wifi_setup_mac = WiFi.macAddress();
  wifi_ssid_current = String(WiFi.SSID());
  //  wifi_rssi_current = String(WiFi.RSSI());
  wifi_ip_current = wifi_ipaddress_2_str(WiFi.localIP());
  if (arduino_serial_enable) Serial.print(F("_wifi_global_load: "));
  if (arduino_serial_enable) Serial.print(wifi_ssid_current);
  //  if (arduino_serial_enable) Serial.print(F(" | "));
  //  if (arduino_serial_enable) Serial.print(wifi_rssi_current);
  if (arduino_serial_enable) Serial.print(F(" | "));
  if (arduino_serial_enable) Serial.print(wifi_setup_mac);
  if (arduino_serial_enable) Serial.print(F(" | "));
  if (arduino_serial_enable) Serial.print(wifi_ip_current);
  if (arduino_serial_enable) Serial.println();
}
void wifi_setup() {
  if (arduino_serial_enable) Serial.println(F("wifi_setup.connecting.init"));
  _wifi_global_clear();
  if (!wifi_enable) {
    if (arduino_serial_enable) Serial.println(F("wifi_setup.disabled!"));
    return;
  }
  if (arduino_serial_enable) Serial.println(F("wifi_setup.begin"));
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, wifi_mac_custom);
  wifiMulti.addAP("Mebosa", "Bugun19112018");
  wifiMulti.addAP("Mesametal", "DateIs01062015");
  wifiMulti.addAP("MesaMetalWF", "DateIs01062015");
  Serial.println(F("wifi_setup.connecting..."));
  if (wifiMulti.run() == WL_CONNECTED) {
    _wifi_global_load();
  } else {
    if (arduino_serial_enable) Serial.println("wifi_setup.connecting.failed");
    _wifi_global_clear();
  }
  if (arduino_serial_enable) Serial.println(F("wifi_setup.end"));
}
bool _wifi_warmup() {
  if (arduino_serial_enable) Serial.println("wifi_warmup.scaning...");
  int n = WiFi.scanNetworks();
  if (arduino_serial_enable) Serial.println("wifi_warmup.scan done");
  if (n == 0) {
    if (arduino_serial_enable) Serial.println("wifi_warmup.no networks found");
  } else {
    if (arduino_serial_enable) Serial.print("wifi_warmup.");
    if (arduino_serial_enable) Serial.print(n);
    if (arduino_serial_enable) Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      if (arduino_serial_enable) Serial.print("wifi_warmup.itm[");
      if (arduino_serial_enable) Serial.print(i);
      if (arduino_serial_enable) Serial.print("]: ");
      if (arduino_serial_enable) Serial.print(WiFi.SSID(i));
      if (arduino_serial_enable) Serial.println();
      delay(10);
    }
  }
  if (wifiMulti.run(wifi_loop_interval_ms) != WL_CONNECTED) {
    if (arduino_serial_enable) Serial.println("wifi_warmup.connecting.failed");
    _wifi_global_clear();
    delay(1000);
    return false;
  }
  _wifi_global_load();
  return true;
}
bool wifi_config_done = false;
bool _wifi_config() {
  if (wifi_config_done) {
    return true;
  }
  if (arduino_serial_enable) Serial.println();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (arduino_serial_enable) Serial.println(request->url());
    request->send_P(200, "text/html", index_html, index_html_processor);
  });
  server.on("/t", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (arduino_serial_enable) Serial.println(request->url());
    request->send_P(200, "text/plain", dht_t_chr);
  });
  server.on("/h", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (arduino_serial_enable) Serial.println(request->url());
    request->send_P(200, "text/plain", dht_h_chr);
  });
  server.on("/c", HTTP_GET, [](AsyncWebServerRequest* request) {
    if (arduino_serial_enable) Serial.println(request->url());
    int offset_hour_int = 0;
    if (request->hasParam("offset_hour")) {
      String offset_hour_str = request->getParam("offset_hour")->value();
      if (arduino_serial_enable) Serial.println(F("offset_hour_str"));
      if (arduino_serial_enable) Serial.println(offset_hour_str);
      offset_hour_int = offset_hour_str.toInt();
    }
    int offset_min_int = 0;
    if (request->hasParam("offset_min")) {
      String offset_min_str = request->getParam("offset_min")->value();
      if (arduino_serial_enable) Serial.println(F("offset_min_str"));
      if (arduino_serial_enable) Serial.println(offset_min_str);
      offset_min_int = offset_min_str.toInt();
    }
    int offset_sec_int = 0;
    if (request->hasParam("offset_sec")) {
      String offset_sec_str = request->getParam("offset_sec")->value();
      if (arduino_serial_enable) Serial.println(F("offset_sec_str"));
      if (arduino_serial_enable) Serial.println(offset_sec_str);
      offset_sec_int = offset_sec_str.toInt();
    }
    if (arduino_serial_enable) Serial.println(F("arduino_readable_clock_offset_millis (prv)"));
    if (arduino_serial_enable) Serial.println(arduino_readable_clock_offset_millis);
    arduino_readable_clock_offset_millis += offset_sec_int * 1000L + offset_min_int * 60 * 1000L + offset_hour_int * 60 * 60 * 1000L;
    if (arduino_serial_enable) Serial.println(F("arduino_readable_clock_offset_millis (pst)"));
    if (arduino_serial_enable) Serial.println(arduino_readable_clock_offset_millis);
    arduino_readable_clock = arduino_loop_readable_clock();
    request->send_P(200, "text/plain", arduino_readable_clock.c_str());
  });
  server.begin();
  wifi_config_done = true;
  if (arduino_serial_enable) Serial.println(F("wifi_config.end"));
  return true;
}
void wifi_loop() {
  if (!wifi_enable) {
    if (arduino_serial_enable && wifi_verbose) Serial.println(F("wifi_loop.disabled!"));
    return;
  }
  if (!_wifi_warmup()) {
    return;
  }
  _wifi_config();
}

void setup() {
  arduino_setup();
  wifi_setup();
  display_setup();
  dht_setup();
}


const char DEGREE_SYMBOL[] = { 0xB0, '\0' };
void loop() {
  if (!arduino_loop_begin()) {
    return;
  }
  dht_loop();
  wifi_loop();
  display_loop_begin();
  if (display_enable) {
    //u8g2_font_6x13B_tr | u8g2_font_7x14B_tr | u8g2_font_helvR12_tf
    int y = 13;
    if (wifi_connected) {
      u8g2.setFont(u8g2_font_7x14B_tr);
      u8g2.drawStr(4, y, wifi_setup_mac.c_str());
    } else {
      u8g2.setFont(u8g2_font_7x14B_tr);
      u8g2.drawStr(4, y, wifi_mac_custom_str.c_str());
    }

    int yo = 27;
    int yl = 12;

    y = yo + yl * 0;
    u8g2.setFont(u8g2_font_helvR12_tf);
    u8g2.drawUTF8(0, y + 3, "°");
    u8g2.setFont(u8g2_font_7x14B_tr);
    u8g2.drawStr(8, y, "C:");
    u8g2.setFont(u8g2_font_7x14_tr);
    u8g2.drawStr(21, y, dht_t_chr);
    u8g2.setFont(u8g2_font_7x14B_tr);
    u8g2.drawStr(62, y, "%RH:");
    u8g2.setFont(u8g2_font_7x14_tr);
    u8g2.drawStr(89, y, dht_h_chr);

    y = yo + yl * 1;
    u8g2.setFont(u8g2_font_7x14B_tr);
    u8g2.drawStr(0, y, "CLK");
    u8g2.setFont(u8g2_font_7x14_tr);
    u8g2.drawStr(33, y, arduino_readable_clock.c_str());

    y = yo + yl * 2;
    u8g2.setFont(u8g2_font_7x14B_tr);
    u8g2.drawStr(0, y, "WIFI");
    u8g2.setFont(u8g2_font_7x14_tr);
    u8g2.drawStr(33, y, wifi_ssid_current.c_str());

    y = yo + yl * 3;
    if (wifi_connected) {
      u8g2.setFont(u8g2_font_7x14B_tr);
      u8g2.drawStr(0, y, "IP#");
      u8g2.setFont(u8g2_font_7x14_tr);
      u8g2.drawStr(33, y, wifi_ip_current.c_str());
    } else {
      u8g2.setFont(u8g2_font_7x14B_tr);
      u8g2.drawStr(0, y, "MOD");
      u8g2.setFont(u8g2_font_7x14_tr);
      u8g2.drawStr(33, y, "Calibration");
    }
  }
  display_loop_end();
  arduino_loop_end();
}
