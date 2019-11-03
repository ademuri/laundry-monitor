# Laundry Monitor

This is an Arduino sketch that sends an SMS when an appliance has finished running. It uses an analog current sensor.

## Current sensor

I use [this Modern Devices sensor](https://moderndevice.com/product/current-sensor/). This sensor is unique among current sensors in that it doesn't require modifying the cord of the appliance. Traditional (split-core) current sensors must be attached to a single conductor, rather than the entire cord, or they won't work. The Modern Devices sensor sends an analog voltage between GND and VIN depending on the measured current. It seems to have relatively low noise (i.e. when the aplpliance is off, it returns 0).

## Platform

I run this on an ESP32 dev board, the DOIT ESP32 Devkit v1, using the Arduino environment. It should work on any ESP32-based board. Note that ADC2 is unusable while WiFi is enabled, so you'll need to connect the sensor to ADC1.

## Configuration

You'll need to provide some user-specific configuration values. Copy `constants.sample.h` to `constants.h` (which is automatically ignored by Git, for safety). Then fill out the following fields.

### WiFi

Enter the `ssid` and `password` for your wifi network.

### Twilio

This sketch uses Twilio to send SMS. The `account_sid`, `auth_token`, and `from_number` are all available on the Twilio dashboard. You need to rent a phone number from Twilio to be able to send SMS (the `from_number`). The `to_number` is the phone number to send notifications to.
