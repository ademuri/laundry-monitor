#!/bin/bash

set -euo pipefail

rm laundry-monitor/constants.h || true
cp laundry-monitor/constants.sample.h laundry-monitor/constants.h
platformio run
rm laundry-monitor/constants.h

