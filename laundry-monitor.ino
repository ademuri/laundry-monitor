#include "constants.h"
#include <WiFi.h>
#include "twilio.hpp"
#include "median-filter.h"

// Note: must use ADC1, ADC2 becomes unusable during Wifi usage
static const uint8_t kCurrentPin = 33;
static const uint8_t kLedPin = 2;

MedianFilter<uint32_t, uint32_t, 5> *easy_filter;
MedianFilter<uint32_t, uint32_t, 201> *hard_filter;

Twilio *twilio;

void setup() {
  Serial.begin(115200);

  pinMode(kCurrentPin, INPUT);
  // Require 3 on samples to stay on
  easy_filter = new MedianFilter<uint32_t, uint32_t, 5>(filter_functions::ForAnalogRead<kCurrentPin>());
  easy_filter->SetMinRunInterval(5);

  // Require 10s of on before turning on
  hard_filter = new MedianFilter<uint32_t, uint32_t, 201>(filter_functions::ForAnalogRead<kCurrentPin>());
  hard_filter->SetMinRunInterval(100);

  pinMode(kLedPin, OUTPUT);

  digitalWrite(kLedPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {}
  digitalWrite(kLedPin, HIGH);
  delay(500);
  digitalWrite(kLedPin, LOW);

  Serial.println("Sending...");
  twilio = new Twilio(account_sid, auth_token);
}

//int32_t kCycleDelayMs = 10;
int32_t kOnToOffThreshold = 150 * 1000; // 2.5 minutes
int32_t kOffToOnThreshold = 10 * 1000; // 10 seconds

bool laundryOn = false;
bool sensor_state = false;
uint32_t change_at = 0;
uint32_t print_at = 0;

void loop() {
  easy_filter->Run();
  hard_filter->Run();

  if (laundryOn) {
    if (easy_filter->GetFilteredValue() > 0) {
      change_at = millis() + kOnToOffThreshold;
    } else if (millis() > change_at) {
      laundryOn = false;
      sensor_state = false;
      digitalWrite(kLedPin, false);
      // DING
      String message = "Washer is done";
      String response;
      bool success = twilio->send_message(to_number, from_number, message, response);
      if (success) {
        Serial.println("Sent message successfully");
      } else {
        Serial.println(response);
      }
    }
  } else {
    // laundryOn == false
    if (hard_filter->GetFilteredValue() > 0) {
      laundryOn = true;
      digitalWrite(kLedPin, true);
      change_at = millis() + kOnToOffThreshold;
    }
  }

  if (millis() > print_at) {
    Serial.print(easy_filter->GetFilteredValue());
    Serial.print(" ");
    Serial.println(laundryOn ? "on" : "off");
    print_at = millis() + 500;
  }
}
