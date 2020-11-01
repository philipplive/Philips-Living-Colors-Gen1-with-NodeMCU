#include <Arduino.h>

#include "PhilipsLampLib.h"

PhilipsLampLib plc;

void setup() {
  pinMode(16, OUTPUT);
  pinMode(2, OUTPUT);

  Serial.begin(115200);
  delay(1000);
  Serial.println("START");
  digitalWrite(2, true);
  plc.setSerial(&Serial);
  // delay(1000);
  plc.searchLamps();
  digitalWrite(2, false);

  digitalWrite(16, true);
}

void loop() {
  plc.setLamps(CMD_SET_COLOR, 100, 100, 100);
  delay(500);
  // Serial.println("OK4");
}
