#include "Arduino.h"

void setup() {
 Serial.begin(57600); 
}

char str_buffer[100];

void loop() {
  Serial.readBytesUntil('\n', str_buffer, 100);
  Serial.println(str_buffer);
}
