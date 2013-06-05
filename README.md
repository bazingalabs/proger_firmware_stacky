Proger firmware for stacky
======================

This firmware allows a Proger to upload code to any Stacky device using i2c with avrdude
To build the firmware use INO tool from http://inotool.org/ or the Arduino IDE

**When using the Arduino IDE copy the libraries in the lib directory to your Arduino libraries forder!!**

#### Example
```bash
avrdude -cavr109 -patmega328p -P/dev/tty.usbmodem1a1231 -Uflash:w:Blink.cpp.hex:i
```
* -cavr109 this is the programmer type to use
* -P this is the usb port the proger is on
* -U specify the file to flash

