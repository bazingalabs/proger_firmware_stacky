#!/bin/bash

/Users/erikkallen/Projects/Bazingalabs/avrdude-5.11/avrdude -c proger -patmega328p -P/dev/tty.usbmodem1a1231 -Uflash:w:Blink.cpp.hex:i -vvvvvv -C /Users/erikkallen/Projects/Bazingalabs/avrdude-5.11/avrdude.conf -a 41