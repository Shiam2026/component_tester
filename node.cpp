#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "আপনার_ওয়াইফাই_নাম";
const char *password = "আপনার_ওয়াইফাই_পাসওয়ার্ড";

ESP8266WebServer server(80);

#define TEST_PIN D1
#define BATTERY_PIN A0

void setup()
{
    Serial.begin(115200);
    pinMode(TEST_PIN, OUTPUT);
    digitalWrite(TEST_PIN, LOW);

    WiFi.begin(ssid, password);
    Serial.print("\nConnecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected!");
    Serial.print("আপনার ESP8266 এর IP Address: ");
    Serial.println(WiFi.localIP()); // এই আইপিটি ওয়েব অ্যাপে বসাতে হবে

    // ডেটা পাঠানোর রাউট
    server.on("/data", HTTP_GET, handleData);
    // টগল করার রাউট
    server.on("/toggle", HTTP_GET, handleToggle);

    // CORS Error সমাধান (গিটহাব থেকে কানেক্ট করার জন্য)
    server.onNotFound([]()
                      {
    if (server.method() == HTTP_OPTIONS) {
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
      server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
      server.send(204);
    } else {
      server.send(404, "text/plain", "Not Found");
    } });

    server.begin();
}

void loop()
{
    server.handleClient();
}

void handleData()
{
    server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS পারমিশন

    int adcValue = analogRead(BATTERY_PIN);
    float pinVoltage = (adcValue / 1023.0) * 3.3;
    float batteryVoltage = pinVoltage * 11.0; // 100k ও 10k ডিভাইডারের জন্য

    int percent = 0;

    // স্মার্ট ব্যাটারি ডিটেকশন (ভোল্টেজ দেখে ব্যাটারি চেনা)
    if (batteryVoltage > 8.0)
    {
        // যদি ভোল্টেজ ৮ ভোল্টের বেশি হয়, তবে এটি 12V সোলার/লেড-এসিড ব্যাটারি
        // সোলার ব্যাটারির চার্জ (12.7V = 100%, 11.5V = 0%)
        percent = map(batteryVoltage * 100, 1150, 1270, 0, 100);
    }
    else if (batteryVoltage > 1.0)
    {
        // যদি ভোল্টেজ ১ থেকে ৮ ভোল্টের মাঝে হয়, তবে এটি 3.7V ব্যাটারি
        // 3.7V ব্যাটারির চার্জ (4.2V = 100%, 3.2V = 0%)
        percent = map(batteryVoltage * 100, 320, 420, 0, 100);
    }

    // পার্সেন্টেজ যেন 100 এর বেশি বা 0 এর কম না দেখায়
    if (percent > 100)
        percent = 100;
    if (percent < 0)
        percent = 0;

    // JSON ডেটা তৈরি করে ওয়েবপেজে পাঠানো
    String json = "{\"voltage\":\"" + String(batteryVoltage, 2) + "\", \"percent\":" + String(percent) + "}";
    server.send(200, "application/json", json);
}

void handleToggle()
{
    server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS পারমিশন
    digitalWrite(TEST_PIN, !digitalRead(TEST_PIN));
    server.send(200, "text/plain", "Toggled");
}
