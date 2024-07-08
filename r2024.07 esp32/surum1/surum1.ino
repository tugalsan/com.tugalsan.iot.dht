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

bool serialEnable = true;

void setup() {
  delay(1000);
  if (serialEnable) Serial.begin(115200);

  if (serialEnable) Serial.println(F("display.begin..."));
  u8g2.begin();
  if (serialEnable) Serial.println(F("display.begin.ok"));
}

void loop() {
  delay(1000);
  loop_start_ms = millis();
  if (serialEnable) {
    Serial.print(F("loop.begin@"));
    Serial.println(loop_start_ms);
  }
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tr);
  u8g2.drawStr(0, 10, "Hello");
  u8g2.sendBuffer();
}
