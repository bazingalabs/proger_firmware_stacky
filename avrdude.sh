#!/bin/bash

avrdude -cavr109 -patmega328p -P/dev/tty.usbmodem1a1231 -Uflash:w:ReadAnalogVoltage.cpp.hex:i -vvvvvv