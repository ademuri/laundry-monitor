#ifndef __CONSTANTS__
#define __CONSTANTS

const char* ssid = "";
const char* password = "";
const char* account_sid = "";
const char* auth_token = "";

String to_number = "";
String from_number = "";

// People to notify when the appliances finish running.
// Format: {"phone number", sensor_pin_number, led_pin_number}
// Phone numbers start with '+<countrycode>'
std::vector<Person*> people = {
  {"+13038675309", 25, 14},
};

#endif
