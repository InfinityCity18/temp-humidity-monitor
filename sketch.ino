
#include "DHT.h"
#include <LiquidCrystal.h>
#include <WiFi.h>

#define DHTPIN 28
#define LDR_PIN 2
#define DHTTYPE DHT22
#define MOTION_PIN 3

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Hello World!");
  Serial1.begin(115200);
  Serial1.println("Hello, Raspberry Pi Pico W!");
  pinMode(LDR_PIN, INPUT);
  pinMode(MOTION_PIN, INPUT);  
  dht.begin();
}

void loop() {
  bool is_light = (digitalRead(LDR_PIN) == LOW);
  bool motion_detected = (digitalRead(MOTION_PIN) == HIGH);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial1.print("Temp:");
  Serial1.println(t);
  Serial1.print("Humid:");
  Serial1.println(h);
  Serial1.println(is_light);
  Serial1.println(motion_detected);

  delay(3000);
}