/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//try 5 4 0 2 15, 16 14 12 13
#define DHTPIN 5     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  Serial.println(F("dht will begin..."));
  dht.begin();

  Serial.println(F("display will begin..."));
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  Serial.println(F("loop new..."));
  delay(5000);

  Serial.println(F("dht reading..."));
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }

  Serial.println(F("display clearing..."));
  display.clearDisplay();
  
  Serial.print(F("display temp as "));
  Serial.println(t);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print(F("Temperature: "));
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(t);
  display.print(F(" "));
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print(F("C"));
  
  Serial.print(F("display humid as "));
  Serial.println(h);
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print(F("Humidity: "));
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(h);
  display.print(F(" %")); 


  Serial.println(F("display render..."));
  delay(1000);
  display.display(); 
}