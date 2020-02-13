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

// Defined in constants.h
extern std::vector<Person*> people;

Appliance* washer;
Twilio *twilio;

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

  washer = Appliance::Create<kWasherPin>(150 * 1000, 10 * 1000);

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
  washer->Run();

  for (auto person : people) {
    person->button->Run();
    if (person->button->Rose()) {
      person->notify = !person->notify;
      digitalWrite(person->led_pin, person->notify);
    }
  }

  if (washer->TurnedOff()) {
    for (auto person : people) {
      if (person->notify) {
        send_message(person->phone_number, "Washer is done");
      }
    }
  }
}
