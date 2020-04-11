#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <periodic-runner.h>

#include <functional>
#include <vector>

#include "appliance.h"
#include "constants.h"
#include "dashboard.h"
#include "debounce-filter.h"
#include "median-filter.h"
#include "person.h"
#include "twilio.hpp"

// Note: must use ADC1, ADC2 becomes unusable during Wifi usage
static const uint8_t kWasherPin = 33;
static const uint8_t kDryerPin = 32;

// Set this to true to just log the current sensor values to the terminal. Used
// for setting up the current sensors.
static const bool calibrate = false;

// Defined in constants.h
extern std::vector<Person *> people;

Appliance *washer;
Appliance *dryer;
Twilio *twilio;
Dashboard *dashboard;
AsyncWebServer *server;

// Turn off all notifications if the appliances are both off for a while.
uint32_t notify_off_at = 0xFFFFFFFF;
static const uint32_t notify_off_delay = 60 * 60 * 1000;  // One hour

static const uint32_t refresh_mdns_delay = 60 * 1000;

// How long to disable WiFi sleep for when the dashboard is used.
static const uint32_t kDashboardSleepDelay = 60 * 1000;

PeriodicRunner runner;

void send_message(String to_number, String message) {
  String response;
  bool success =
      twilio->send_message(to_number, from_number, message, response);
  if (success) {
    Serial.println("Sent message successfully");
  } else {
    Serial.println(response);
  }
}

void setup() {
  Serial.begin(115200);

  // Turn on lights while connecting to WiFi to show we're alive
  for (auto person : people) {
    digitalWrite(person->led_pin, HIGH);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  }
  delay(500);

  for (auto person : people) {
    digitalWrite(person->led_pin, LOW);
  }

  twilio = new Twilio(account_sid, auth_token);

  ArduinoOTA
      .onStart([]() {
        WiFi.setSleep(false);
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
          type = "sketch";
        } else {  // U_SPIFFS
          type = "filesystem";
        }

        Serial.println("Start updating " + type);
        SPIFFS.end();
      })
      .onEnd([]() { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });

  ArduinoOTA.begin();

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (MDNS.begin("laundry-monitor")) {
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  washer = Appliance::Create<kWasherPin>(150 * 1000);
  dryer = Appliance::Create<kDryerPin>(30 * 1000);

  server = new AsyncWebServer(80);
  dashboard = new Dashboard(server);
  dashboard->Add<uint32_t>("Uptime", millis, 5000);
  dashboard->Add<bool>(
      "Washer on", []() { return washer->State(); }, 2000);
  dashboard->Add<bool>(
      "Dryer on", []() { return dryer->State(); }, 2000);
  dashboard->Add<uint16_t>(
      "Washer level", []() { return analogRead(kWasherPin); }, 2000);
  dashboard->Add<uint16_t>(
      "Dryer level", []() { return analogRead(kDryerPin); }, 2000);
  server->begin();

  runner.Add(refresh_mdns_delay, []() {
    if (!MDNS.begin("laundry-monitor")) {
      Serial.println("Error while refreshing MDNS");
    }
  });
}

void loop() {
  ArduinoOTA.handle();
  runner.Run();

  if (calibrate) {
    Serial.print(analogRead(kWasherPin));
    Serial.print(" ");
    Serial.println(analogRead(kDryerPin));
    delay(5);
    return;
  }

  washer->Run();
  dryer->Run();

  for (auto person : people) {
    person->button->Run();
    if (person->button->Rose()) {
      person->notify = !person->notify;
      digitalWrite(person->led_pin, person->notify);
      notify_off_at = millis() + notify_off_delay;
    }
  }

  if (washer->State() || dryer->State()) {
    notify_off_at = millis() + notify_off_delay;
  }

  if (washer->TurnedOff()) {
    for (auto person : people) {
      if (person->notify) {
        send_message(person->phone_number, "Washer is done");
      }
    }
  }
  if (dryer->TurnedOff()) {
    for (auto person : people) {
      if (person->notify) {
        send_message(person->phone_number, "Dryer is done");
      }
    }
  }

  if (millis() > notify_off_at) {
    for (auto person : people) {
      person->notify = false;
      digitalWrite(person->led_pin, false);
    }
  }

  if (millis() - dashboard->last_used() > kDashboardSleepDelay) {
    WiFi.setSleep(true);
  } else {
    WiFi.setSleep(false);
  }
}
