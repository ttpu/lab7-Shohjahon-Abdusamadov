#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

const char* ssid = "Galaxy A52 832C";        // Your Wi-Fi SSID
const char* password = "77777777";          // Your Wi-Fi Password

bool buttonPressed = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  pinMode(2, OUTPUT);  // Red LED
  pinMode(4, OUTPUT);  // Green LED
  pinMode(16, OUTPUT); // Yellow LED
  pinMode(17, OUTPUT); // Blue LED
  pinMode(18, INPUT);  // Button

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  static bool buttonLastState = HIGH;

  // Check button state
  bool buttonCurrentState = digitalRead(18);
  if (buttonLastState == HIGH && buttonCurrentState == LOW) {
    buttonPressed = true;
    Serial.println("Button pressed!");
  } else if (buttonLastState == LOW && buttonCurrentState == HIGH) {
    if (buttonPressed) {
      Serial.println("Button released!");
      performLEDSequence();
      buttonPressed = false;
    }
  }
  buttonLastState = buttonCurrentState;
}

void performLEDSequence() {
  // Blink Red LED 5 times (1 second each)
  for (int i = 0; i < 5; i++) {
    digitalWrite(2, HIGH);
    delay(500);
    digitalWrite(2, LOW);
    delay(500);
  }

  // Blink Green LED 10 times (0.5 seconds each)
  for (int i = 0; i < 10; i++) {
    digitalWrite(4, HIGH);
    delay(250);
    digitalWrite(4, LOW);
    delay(250);
  }

  // Blink Yellow LED 3 times (3 seconds each)
  for (int i = 0; i < 3; i++) {
    digitalWrite(16, HIGH);
    delay(1500);
    digitalWrite(16, LOW);
    delay(1500);
  }

  // Blink Blue LED 2 times (5 seconds each)
  for (int i = 0; i < 2; i++) {
    digitalWrite(17, HIGH);
    delay(2500);
    digitalWrite(17, LOW);
    delay(2500);
  }
}
