#ifndef PERSON
#define PERSON

#include <functional>

#include "debounce-filter.h"

struct Person {
  const String phone_number;
  DebounceFilter* button;
  bool notify;
  const uint8_t led_pin;

  Person(String phone_number, const uint8_t pin, const uint8_t led_pin)
      : phone_number(phone_number), notify(false), led_pin(led_pin) {
    pinMode(pin, INPUT_PULLUP);
    pinMode(led_pin, OUTPUT);
    button = new DebounceFilter([=]() { return !digitalRead(pin); });
  };
};

#endif
