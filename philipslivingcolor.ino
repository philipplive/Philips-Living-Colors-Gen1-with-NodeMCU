#include <Arduino.h>
#include <EEPROM.h>

#include "PhilipsLampLib.h"

#define LED_RED 2    // Setup LED
#define LED_BLUE 16  // Runing LED
#define BTN_FLASH 0

PhilipsLampLib plc;

void setup() {
  EEPROM.begin(512);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Serial.begin(115200);
  digitalWrite(LED_RED, true);
  plc.setSerial(&Serial);

  // Beim Start nach Signalen suchen
  if (plc.searchLamps()) {
    // Falls Lampen gefunden, diese im EEPROM speichern
    saveLamps();
  } else {
    // Falls keine neuen Lampen gefunden, Lampen aus EEPROM setzen
    loadLamps();
  }

  digitalWrite(LED_RED, false);
  digitalWrite(LED_BLUE, true);
}

void loop() {
  plc.setLamps(CMD_ON, 100, 200, 200);
  delay(500);
  plc.setLamps(CMD_OFF, 100, 100, 100);
  delay(500);

  digitalWrite(LED_RED, digitalRead(BTN_FLASH));
}

void saveLamps() {
  int addr = 0;

  for (int l = 0; l < MAX_LAMPS; l++) {
    for (int i = 0; i < 8; i++) {
      EEPROM.write(addr++, plc.lamps[l][i]);
    }
  }

  EEPROM.commit();
}

void loadLamps() {
  int addr = 0;
  Serial.println("EEPROM READ");

  for (int l = 0; l < MAX_LAMPS; l++) {
    for (int i = 0; i < 8; i++) {
      plc.lamps[l][i] = EEPROM.read(addr++);
    }
  }
}