#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "Galaxy A52 832C";          // Replace with your Wi-Fi SSID
const char* password = "77777777";            // Replace with your Wi-Fi Password

unsigned long redTimer = 0, greenTimer = 0, yellowTimer = 0, blueTimer = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  pinMode(2, OUTPUT);  // Red LED
  pinMode(4, OUTPUT);  // Green LED
  pinMode(16, OUTPUT); // Yellow LED
  pinMode(17, OUTPUT); // Blue LED

  // Connect to your Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for the connection to establish
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Configure OTA
  ArduinoOTA.setHostname("esp32-ota");   // Optional: Set a custom hostname

  // OTA Event Handlers
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nUpdate Complete!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress * 100) / total);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error [%u]: ", error);
    if      (error == OTA_AUTH_ERROR)    Serial.println("Authentication Failed");
    else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connection Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)     Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("Ready for OTA updates.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();

  unsigned long currentMillis = millis();

  // Red LED (1 second)
  if (currentMillis - redTimer >= 1000) {
    redTimer = currentMillis;
    digitalWrite(2, !digitalRead(2));
  }

  // Green LED (0.5 second)
  if (currentMillis - greenTimer >= 500) {
    greenTimer = currentMillis;
    digitalWrite(4, !digitalRead(4));
  }

  // Yellow LED (3 seconds)
  if (currentMillis - yellowTimer >= 3000) {
    yellowTimer = currentMillis;
    digitalWrite(16, !digitalRead(16));
  }

  // Blue LED (5 seconds)
  if (currentMillis - blueTimer >= 5000) {
    blueTimer = currentMillis;
    digitalWrite(17, !digitalRead(17));
  }
}
