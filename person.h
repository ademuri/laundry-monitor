#ifndef PERSON
#define PERSON

#include "debounce-filter.h"
#include <functional>

struct Person {
  String phone_number;
  DebounceFilter* button;
  bool notify;

  Person(String phone_number, const uint8_t pin) : phone_number(phone_number), notify(false) {
    pinMode(pin, INPUT_PULLUP);
    button = new DebounceFilter([=]() {
        return !digitalRead(pin); });
  };
};

#endif
