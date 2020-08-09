#ifndef __CONSTANTS__
#define __CONSTANTS

#include "person.h"

const char* ssid = "";
const char* password = "";
const char* account_sid = "";
const char* auth_token = "";

// Phone number to send from, from Twilio. Starts with '+<countrycode>'.
String from_number = "";

// People to notify when the appliances finish running.
// Format: new Person("phone number", sensor_pin_number, led_pin_number)
// Phone numbers start with '+<countrycode>'
//
// Note: check the pin table here when assigning pins, not all are safe to use!
// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
std::vector<Person*> people = {
    new Person("+13038675309", 25, 14),
};

#endif
