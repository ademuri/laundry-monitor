# Laundry Monitor

This is an Arduino sketch that sends an SMS when an appliance has finished
running. It uses an analog current sensor.

## Dashboard

The code creates a web dashboard. You can use mDNS to easily access this
dashboard. From any browser except on Android, navigate to
[laundry-monitor.local/](http://laundry-monitor.local/) to see the sensor
values, appliance states, and uptime.

## Current sensor

I use [this Modern Devices
sensor](https://moderndevice.com/product/current-sensor/). This sensor is
unique among current sensors in that it doesn't require modifying the cord of
the appliance. Traditional (split-core) current sensors must be attached to a
single conductor, rather than the entire cord, or they won't work. The Modern
Devices sensor sends an analog voltage between GND and VIN depending on the
measured current. It seems to have relatively low noise (i.e. when the
appliance is off, it returns 0).

### Tuning

The current sensors are very sensitive to position - you may need to fiddle
around a bit to find the position that works. All you need is for the sensor to
consistently be non-zero when the appliance is running. The sensor should
mostly report zero when the appliance is off. There is filtering on the sensor,
so small amounts of noise are OK.

## Platform

I run this on an ESP32 dev board, the DOIT ESP32 Devkit v1, using the Arduino
environment. It should work on any ESP32-based board. Note that ADC2 is
unusable while WiFi is enabled, so you'll need to connect the sensor to ADC1.

This uses the [smart input
filter](https://github.com/ademuri/smart-input-filter) and [Twilio ESP32
client](https://github.com/ademuri/twilio-esp32-client) libraries.

## Configuration

You'll need to provide some user-specific configuration values. Copy
`constants.sample.h` to `constants.h` (which is automatically ignored by Git,
for safety). Then fill out the following fields.

### WiFi

Enter the `ssid` and `password` for your wifi network.

### Twilio

This sketch uses Twilio to send SMS. The `account_sid`, `auth_token`, and
`from_number` are all available on the Twilio dashboard. You need to rent a
phone number from Twilio to be able to send SMS (the `from_number`).

### People

This project can notify multiple people. Each person has a phone number and
pins for input and output. The input pin is used to turn on and off
notifications. The output pin drives an LED to display the notification status
(on or off).

## Programming

The first time you load this sketch, you'll need to program as normal:

```
$ pio run -t upload
```

You may need to specify the port using `--upload-port`.

After that, you can use ArduinoOTA to update the sketch over WiFi:

```
$ pio run -t upload --upload-port laundry-monitor.local
```

This can be a little flaky, so you may have to try a few times.
