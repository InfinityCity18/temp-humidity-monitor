
#include "DHT.h"
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define DHTPIN 22
#define LDR_PIN 16
#define DHTTYPE DHT11
#define MOTION_PIN 14
#define BACKLIGHT_PIN 15
#define PORT 443

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(2,3,4,5,6,7);
//IPAddress server(10,69,69,52);
WiFiClientSecure client;

char ssid[] = "*"; // your network SSID (name)
char pass[] = "*"; // your network password

const char* host = "*";

struct PostData {
    bool motion_detected;
    float h;
    float t;
    float light;
};

void setup() {
    lcd.begin(16, 2);
    Serial1.begin(115200);
    pinMode(LDR_PIN, INPUT);
    pinMode(MOTION_PIN, INPUT);
    pinMode(BACKLIGHT_PIN, OUTPUT);
    dht.begin();
    digitalWrite(BACKLIGHT_PIN,HIGH);
    lcd.print("Initializing...");

    while (WiFi.begin(ssid, pass) != WL_CONNECTED) { //wait and try again if connection to wifi failed
        delay(1500);
    }
}

void loop() {

    bool is_light = (digitalRead(LDR_PIN) == LOW);
    float lux = is_light ? 100000.0 : 0.0;
    // {
    //   const float GAMMA = 0.7;
    //   const float RL10 = 50;
    //   int analogValue = analogRead(A0);
    //   float voltage = analogValue / 1024. * 5;
    //   float resistance = 2000 * voltage / (1 - voltage / 5);
    //   lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
    // }
    bool motion_detected = (digitalRead(MOTION_PIN) == HIGH);
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    struct PostData data = {
        motion_detected, h, t, lux
    };

    lcd.clear();
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(h);
    lcd.print(motion_detected);

    if (is_light) {
        digitalWrite(BACKLIGHT_PIN,HIGH);
    } else {
        digitalWrite(BACKLIGHT_PIN,LOW);
    }

    if (WiFi.status() == WL_CONNECTED) {
        send_http_post(data);
    } else {
        WiFi.begin(ssid, pass);
    }

    delay(2000);
}

void send_http_post(struct PostData data) {
    client.stop();
    client.setInsecure();

    if (client.connect(host, PORT)) {
        lcd.setCursor(11, 0);
        lcd.print("meow");

        String body = "{";
        body += "\"l\":";
        body += data.light;
        body += ",\"m\":";
        body += (data.motion_detected ? "true" : "false");
        body += ",\"h\":";
        body += data.h;
        body += ",\"t\":";
        body += data.t;
        body += "}";

        // Send HTTP headers
        client.println("POST /monitor HTTP/1.1");
        client.print("Host: ");
        client.println(host);
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(body.length());
        client.println("Connection: close");
        client.println();
        client.print(body);
    } else {
        lcd.setCursor(11, 0);
        lcd.print("no meow");
    }
}
