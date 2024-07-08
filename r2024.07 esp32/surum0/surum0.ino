//LOOP
unsigned long loop_start_ms = 0;

//DEBUG
bool serialEnable = true;

void setup() {
  delay(1000);
  if (serialEnable) Serial.begin(115200);
}

void loop() {
  delay(1000);
  loop_start_ms = millis();
  if (serialEnable) {
    Serial.print(F("loop.begin@"));
    Serial.println(loop_start_ms);
  }
}
