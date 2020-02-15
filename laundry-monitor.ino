#include <WiFi.h>
#include <functional>
#include <vector>

#include "appliance.h"
#include "constants.h"
#include "debounce-filter.h"
#include "median-filter.h"
#include "person.h"
#include "twilio.hpp"

// Note: must use ADC1, ADC2 becomes unusable during Wifi usage
static const uint8_t kWasherPin = 33;
static const uint8_t kDryerPin = 32;

// Set this to true to just log the current sensor values to the terminal. Used for setting up the current sensors.
static const bool calibrate = false;

// Defined in constants.h
extern std::vector<Person*> people;

Appliance* washer;
Appliance* dryer;
Twilio *twilio;

// Turn off all notifications if the appliances are both off for a while.
uint32_t notify_off_at = 0xFFFFFFFF;
static const uint32_t notify_off_delay = 60 * 60 * 1000; // One hour

void send_message(String to_number, String message) {
  String response;
  bool success = twilio->send_message(to_number, from_number, message, response);
  if (success) {
    Serial.println("Sent message successfully");
  } else {
    Serial.println(response);
  }
}

void setup() {
  Serial.begin(115200);

  washer = Appliance::Create<kWasherPin>(150 * 1000);
  dryer = Appliance::Create<kDryerPin>(30 * 1000);

  for (auto person : people) {
    digitalWrite(person->led_pin, HIGH);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {}
  delay(500);

  for (auto person : people) {
    digitalWrite(person->led_pin, LOW);
  }

  twilio = new Twilio(account_sid, auth_token);
}

void loop() {
  if (calibrate) {
    Serial.print(analogRead(kWasherPin));
    Serial.print(" ");
    Serial.println(analogRead(kDryerPin));
    delay(5);
    return;
  }

  washer->Run();
  dryer->Run();

  if (millis() > notify_off_at) {
    for (auto person : people) {
      person->notify = false;
      digitalWrite(person->led_pin, false);
    }
  }

  for (auto person : people) {
    person->button->Run();
    if (person->button->Rose()) {
      person->notify = !person->notify;
      digitalWrite(person->led_pin, person->notify);
      notify_off_at = millis() + notify_off_delay;
    }
  }

  if (washer->TurnedOff()) {
    for (auto person : people) {
      if (person->notify) {
        send_message(person->phone_number, "Washer is done");
      }
    }
    if (!dryer->State()) {
      notify_off_at = millis() + notify_off_delay;
    }
  }
  if (dryer->TurnedOff()) {
    for (auto person : people) {
      if (person->notify) {
        send_message(person->phone_number, "Dryer is done");
      }
    }
    if (!washer->State()) {
      notify_off_at = millis() + notify_off_delay;
    }
  }

  delay(1);
}
