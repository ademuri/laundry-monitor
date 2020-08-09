#!/bin/bash

set -euo pipefail

constants_created=
if [ ! -f "laundry-monitor/constants.h" ]; then
  cp laundry-monitor/constants.sample.h laundry-monitor/constants.h
  constants_created=true
fi

platformio run

if [ "$constants_created" = true ]; then
  rm laundry-monitor/constants.h
fi
