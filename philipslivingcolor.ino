#include <Arduino.h>
#include <EEPROM.h>

#include "PhilipsLampLib.h"

#define LED_RED 2
#define LED_BLUE 16

PhilipsLampLib plc;

void setup() {
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Serial.begin(115200);
  digitalWrite(LED_RED, true);
  plc.setSerial(&Serial);

  if (plc.searchLamps()) {
    Serial.println("FOUND");
  } else {
    Serial.println("NIX");
  }

  digitalWrite(LED_RED, false);
  digitalWrite(LED_BLUE, true);
}

void loop() {
  plc.setLamps(CMD_ON, 100, 200, 200);
  delay(500);
  plc.setLamps(CMD_OFF, 100, 100, 100);
  delay(500);
}
