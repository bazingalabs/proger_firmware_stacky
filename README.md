proger_firmware_stacky
======================

Stacky Proger firmware

This firmware allows a Proger to upload code to any Stacky device using i2c with avrdude

== Example
avrdude -cavr109 -patmega328p -P/dev/tty.usbmodem1a1231 -Uflash:w:Blink.cpp.hex:i

* -cavr109 this is the programmer type to use
* -P this is the usb port the proger is on
* -U specify the file to flash

