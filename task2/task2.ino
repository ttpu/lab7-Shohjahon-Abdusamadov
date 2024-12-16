#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoOTA.h>  // ArduinoOTA kutubxonasini qo'shish

const char* ssid = "Galaxy A52 832C";   // Wi-Fi SSID
const char* password = "77777777";      // Wi-Fi Password
const char* firmwareUrl = "https://github.com/ttpu/lab7-Shohjahon-Abdusamadov/blob/main/task2/esp32framewrok.bin";  // URL to .bin file
const float currentVersion = 1.0;       // Current firmware version
const char* versionUrl = "https://github.com/ttpu/lab7-Shohjahon-Abdusamadov/blame/main/version.txt";  // URL to version text file

// Pin configuration for LEDs and Button
const int buttonPin = 33;    // Button input pin (change this based on your setup)
const int redLedPin = 4;     // Red LED pin
const int greenLedPin = 5;   // Green LED pin
const int yellowLedPin = 19; // Yellow LED pin
const int blueLedPin = 22;   // Blue LED pin

// Variables for button press detection
bool buttonState = false;        // Current state of the button
bool lastButtonState = false;    // Last state of the button
unsigned long lastMillis = 0;    // Last time for LED blink interval

// Interval durations for LED blinking
unsigned long redInterval = 1000;
unsigned long greenInterval = 500;
unsigned long yellowInterval = 3000;
unsigned long blueInterval = 5000;

// Variables to manage blinking timings
unsigned long redLastMillis = 0;
unsigned long greenLastMillis = 0;
unsigned long yellowLastMillis = 0;
unsigned long blueLastMillis = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");
  
  // Set LED pins as OUTPUT
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  // Set button pin as INPUT
  pinMode(buttonPin, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Start OTA (Over-the-Air) service
  ArduinoOTA.begin();

  // Check for firmware updates
  checkForUpdates();
}

void loop() {
  // Regular code: Check for updates periodically
  static unsigned long lastCheck = 0;
  const unsigned long interval = 10000;  // Check every 10 seconds
  if (millis() - lastCheck > interval) {
    lastCheck = millis();
    checkForUpdates();
  }

  // Read button state and handle LED blinking on press/release
  bool currentButtonState = digitalRead(buttonPin);

  // Check if button press state has changed
  if (currentButtonState != lastButtonState) {
    lastMillis = millis();  // Update debounce timer
    lastButtonState = currentButtonState;
  }

  // If button is pressed, perform actions when released
  if (currentButtonState == HIGH && buttonState == false && millis() - lastMillis > 50) {
    buttonState = true; // Button press detected
  } else if (currentButtonState == LOW && buttonState == true) {
    buttonState = false; // Button released

    // Print message and start LED blinking when button is released
    Serial.println("Button released! Blinking LEDs...");
    // Reset the timers for LED blinking
    redLastMillis = millis();
    greenLastMillis = millis();
    yellowLastMillis = millis();
    blueLastMillis = millis();
  }

  // Handle OTA update in the loop
  ArduinoOTA.handle();
  delay(10);  // Small delay to prevent excessive CPU usage

  // Blink LEDs parallel, each with its own interval
  if (millis() - redLastMillis >= redInterval) {
    redLastMillis = millis();
    digitalWrite(redLedPin, !digitalRead(redLedPin));  // Toggle red LED
  }
  if (millis() - greenLastMillis >= greenInterval) {
    greenLastMillis = millis();
    digitalWrite(greenLedPin, !digitalRead(greenLedPin));  // Toggle green LED
  }
  if (millis() - yellowLastMillis >= yellowInterval) {
    yellowLastMillis = millis();
    digitalWrite(yellowLedPin, !digitalRead(yellowLedPin));  // Toggle yellow LED
  }
  if (millis() - blueLastMillis >= blueInterval) {
    blueLastMillis = millis();
    digitalWrite(blueLedPin, !digitalRead(blueLedPin));  // Toggle blue LED
  }
}

// Function to check for firmware updates
void checkForUpdates() {
  Serial.println("Checking for firmware updates...");
  HTTPClient http;
  // Fetch the latest version number from the server
  http.begin(versionUrl);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String newVersionStr = http.getString();
    newVersionStr.trim();  // Remove any whitespace or newline characters
    float newVersion = newVersionStr.toFloat();
    Serial.print("Current firmware version: ");
    Serial.println(currentVersion, 2);
    Serial.print("Available firmware version: ");
    Serial.println(newVersion, 2);
    if (newVersion > currentVersion) {
      Serial.println("New firmware available. Starting update...");
      http.end();  // Close the connection before proceeding
      downloadAndUpdate();
    } else {
      Serial.println("Already on the latest version.");
    }
  } else {
    Serial.print("Failed to check for updates. HTTP error code: ");
    Serial.println(httpCode);
  }
  http.end();
}

// Function to download and update firmware
void downloadAndUpdate() {
  HTTPClient http;
  // Begin downloading the new firmware
  http.begin(firmwareUrl);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    if (contentLength > 0) {
      bool canBegin = Update.begin(contentLength);
      if (canBegin) {
        Serial.println("Beginning firmware update...");
        WiFiClient* client = http.getStreamPtr();
        size_t written = Update.writeStream(*client);
        if (written == contentLength) {
          Serial.println("Firmware update successfully written.");
        } else {
          Serial.print("Only ");
          Serial.print(written);
          Serial.print(" out of ");
          Serial.print(contentLength);
          Serial.println(" bytes were written. Update failed.");
          http.end();
          return;
        }
        if (Update.end()) {
          Serial.println("Update finished successfully.");
          if (Update.isFinished()) {
            Serial.println("Update successfully completed. Rebooting...");
            ESP.restart();
          } else {
            Serial.println("Update did not complete. Something went wrong.");
          }
        } else {
          Serial.print("Error Occurred. Error #: ");
          Serial.println(Update.getError());
        }
      } else {
        Serial.println("Not enough space to begin OTA update.");
      }
    } else {
      Serial.println("Content length is not valid.");
    }
  } else {
    Serial.print("Failed to download firmware. HTTP error code: ");
    Serial.println(httpCode);
  }
  http.end();
}
