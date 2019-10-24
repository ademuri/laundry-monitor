#include "constants.h"
#include "digicert.h" 
#include <WiFi.h>
#include "twilio.hpp"

// Note: must use ADC1, ADC2 becomes unusable during Wifi usage
static const uint8_t kCurrentPin = 33;
static const uint8_t kLedPin = 2;

Twilio *twilio;

void setup() {
  Serial.begin(115200);

  pinMode(kCurrentPin, INPUT);
  pinMode(kLedPin, OUTPUT);

  digitalWrite(kLedPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {}
  digitalWrite(kLedPin, HIGH);
  delay(500);
  digitalWrite(kLedPin, LOW);

  Serial.println("Sending...");
  twilio = new Twilio(account_sid, auth_token, DigiCertGlobalRootCA_crt);
  String message = "test message";
  String response;
  //bool success = twilio->send_message(to_number, from_number, message, response);
  //Serial.println(response);
  //Serial.println(success);
}

int32_t kCycleDelayMs = 10;
//int32_t kChangeCountThreshold = 60 * 1000 / kCycleDelayMs; // One minute
int32_t kChangeCountThreshold = 5 * 1000 / kCycleDelayMs; // 5 seconds

uint32_t average;
bool laundryOn = false;
int32_t changeCount = 0;
int printCount = 0;

void loop() {
  average = (average * 3 + analogRead(kCurrentPin)) / 4;
  if (laundryOn) {
    if (average == 0) {
      changeCount++;
      if (changeCount > kChangeCountThreshold) {
        laundryOn = false;
        changeCount = 0;
        digitalWrite(kLedPin, false);
        // DING
      }
    }
  } else {
    if (average > 0) {
      changeCount++;
      if (changeCount > kChangeCountThreshold) {
        laundryOn = true;
        changeCount = 0;
        digitalWrite(kLedPin, true);
      }
    }
  }

  printCount++;
  if (printCount > 10) {
    printCount = 0;
    Serial.println(average);
  }
  delay(kCycleDelayMs);
}
