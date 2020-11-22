#include <Arduino.h>
#include <EEPROM.h>

#include "PhilipsLampLib.h"

#define LED_RED 16   // LED Rot
#define LED_BLUE 2   // LED Blau
#define BTN_FLASH 0  // Button Flash

PhilipsLampLib plc;

void setup() {
  EEPROM.begin(512);
  pinMode(LED_RED, OUTPUT);
  Serial.begin(115200);
  plc.setDebugStream(&Serial);

  digitalWrite(LED_RED, false);

  // Beim Start nach Sendersignalen suchen
  if (plc.searchLamps()) {
    // Falls Lampen gefunden, diese in EEPROM speichern
    saveLamps();
    Serial.print("Neue Lampenadressen gespeichert\r\n");
  } else {
    // Falls keine neuen Lampen gefunden, Lampen aus EEPROM setzen
    loadLamps();
    Serial.print("Lampenadressen aus EEPROM geladen\r\n");
  }

  digitalWrite(LED_RED, true);

  // Lampe mit letzter bekannter Farbe starten
  uint8_t* color = getColors();
  plc.setLamps(CMD_ON, color[0], color[1], color[2]);
}

uint8_t hsv[] = {0, 0, 0};
uint8_t wait = 0;

void loop() {
  // Empfange ein Signal
  plc.listening(1, [](uint8_t* data) {
    hsv[0] = data[12];
    hsv[1] = data[13];
    hsv[2] = data[14];
    wait = 0;
  });

  // Knopf betätigt, neue Farbe auswählen
  if (!digitalRead(BTN_FLASH)) {
    hsv[0] += 20;
    hsv[1] = 255;
    hsv[2] = 240;
    wait = 0;
    plc.setLamps(CMD_ON, hsv[0], hsv[1], hsv[2]);
  }

  // Nach X Sekunden Farbe speichern
  if (hsv[0] != 0 && hsv[1] != 0 && hsv[2] != 0 && wait++ > 100) {
    saveColors(hsv);
    hsv[0] = 0;
    hsv[1] = 0;
    hsv[2] = 0;
    wait = 0;
    Serial.print("Neue Farbwerte gespeichert\r\n");
  }
}

/**
Alle Lampenadressen im EEPROM ablegen
*/
void saveLamps() {
  int addr = 0;

  for (int l = 0; l < MAX_LAMPS; l++) {
    for (int i = 0; i < 8; i++) {
      EEPROM.write(addr++, plc.lamps[l][i]);
    }
  }

  EEPROM.commit();
}

/**
Alle Lampenadressen aus EEPROM lesen
*/
void loadLamps() {
  int addr = 0;

  for (int l = 0; l < MAX_LAMPS; l++) {
    for (int i = 0; i < 8; i++) {
      plc.lamps[l][i] = EEPROM.read(addr++);
    }
  }
}

/**
Default-Farbe im EEPROM ablegen
*/
void saveColors(uint8_t* hsv) {
  for (int c = 0; c < 3; c++) {
    EEPROM.write(500 + c, hsv[c]);
  }

  EEPROM.commit();
}

/**
Default-Farbe aus EEPROM ablegen
*/
uint8_t* getColors() {
  uint8_t data[2];

  for (int c = 0; c < 3; c++) data[c] = EEPROM.read(500 + c);

  return data;
}