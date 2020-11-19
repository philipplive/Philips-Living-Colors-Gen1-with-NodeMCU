#include <Arduino.h>
#include <EEPROM.h>

#include "PhilipsLampLib.h"

#define LED_RED 16    // LED Rot
#define LED_BLUE 2  // LED Blau
#define BTN_FLASH 0  // Button Flash

PhilipsLampLib plc;

void setup() {
  EEPROM.begin(512);

  //pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Serial.begin(115200);
  digitalWrite(LED_RED, false);
  digitalWrite(LED_BLUE, false);
  //digitalWrite(LED_BLUE, false);
  plc.setSerial(&Serial);

  // Beim Start nach Signalen suchen
  if (plc.searchLamps()) {
    // Falls Lampen gefunden, diese in EEPROM speichern
    saveLamps();
  } else {
    // Falls keine neuen Lampen gefunden, Lampen aus EEPROM setzen
    loadLamps();
  }

  digitalWrite(LED_RED, true);

  // Lampe mit letztem bekannten Farbe starten
  uint8_t* color = getColors();
  plc.setLamps(CMD_ON, color[0], color[1], color[2]);
}

void loop() {
  uint8_t h = 0;
  uint8_t s = 0;
  uint8_t v = 0;

  plc.listening(100, [&h, &s, &v](uint8_t* data) {
    h = data[12];
    s = data[13];
    v = data[14];
  });

  if (h != 0 && s != 0 && v != 0) {
    uint8_t data[] = {h, s, v};
    saveColors(data);
  }
}

void saveLamps() {
  Serial.println("Write EEPROM");
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

  for (int l = 0; l < MAX_LAMPS; l++) {
    for (int i = 0; i < 8; i++) {
      plc.lamps[l][i] = EEPROM.read(addr++);
    }
  }
}

void saveColors(uint8_t* hsv) {
  Serial.println("WRITE EEPROM");
  for (int c = 0; c < 3; c++) EEPROM.write(500 + c, hsv[c]);

  EEPROM.commit();
}

uint8_t* getColors() {
  uint8_t data[2];

  for (int c = 0; c < 3; c++) data[c] = EEPROM.read(500 + c);

  return data;
}