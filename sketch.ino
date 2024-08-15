#include "DHT.h"
#include <LiquidCrystal.h>
#include <WiFi.h>

#define DHTPIN 28
#define LDR_PIN 2
#define DHTTYPE DHT22
#define MOTION_PIN 3
#define PORT 80

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
char servername[] = "google.com"; //server url
WiFiClient client;

char ssid[] = "networkSSID";          //  your network SSID (name)
char pass[] = "myPassword";   // your network password

struct PostData {
  bool is_light;
  bool motion_detected;
  float h;
  float t;
};

void setup() {
  lcd.begin(16, 2);
  Serial1.begin(115200);
  pinMode(LDR_PIN, INPUT);
  pinMode(MOTION_PIN, INPUT);  
  dht.begin();
  lcd.print("Initializing...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) { //wait and try again if connection to wifi failed
    delay(1500);
  }
}

void loop() {

  bool is_light = (digitalRead(LDR_PIN) == LOW);
  bool motion_detected = (digitalRead(MOTION_PIN) == HIGH);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  struct PostData data = {
    is_light, motion_detected, h, t
  };

  lcd.clear();
  lcd.print("Temp: ");
  lcd.println(t);
  lcd.print("Humidity: ");
  lcd.println(h);

  if (is_light) {
    lcd.display();
  } else {
    lcd.noDisplay();
  }

  if (WiFi.status() == WL_CONNECTED) {
    send_http_post(data);
  } else {
    WiFi.begin(ssid, pass);
  }

  delay(3000);
}

void send_http_post(struct PostData data) {
  client.stop();
  
  if (client.connect(servername, 80)) {
    client.println("POST /monitor HTTP/1.1");
    client.print("{\"l\":");
    client.print(data.is_light);
    client.print(",\"m\":");
    client.print(data.motion_detected);
    client.print(",\"h\":");
    client.print(data.h);
    client.print(",\"t\":");
    client.print(data.t);
    client.println("}");
    client.println(); 
  }
}
