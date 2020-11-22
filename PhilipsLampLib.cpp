#include "PhilipsLampLib.h"

// Referenz zu einem Stream für Debug
void PhilipsLampLib::setDebugStream(Stream* stream) { _stream = stream; }

PhilipsLampLib::PhilipsLampLib() {
  // SPI config
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);

  // reset device (0x30)
  reset();
  reset();

  // FSCTRL1 – Frequency Synthesizer Control
  sendCommand(0x0B, 0x09);

  // FSCTRL0 – Frequency Synthesizer Control
  sendCommand(0x0C, 0x00);

  // FREQ2 – Frequency Control Word, High Byte
  sendCommand(0x0D, 0x5D);

  // FREQ1 – Frequency Control Word, Middle Byte
  sendCommand(0x0E, 0x93);

  // FREQ0 – Frequency Control Word, Low Byte
  sendCommand(0x0F, 0xB1);

  // MDMCFG4 – Modem Configuration
  sendCommand(0x10, 0x2D);

  // MDMCFG3 – Modem Configuration
  sendCommand(0x11, 0x3B);

  // MDMCFG2 – Modem Configuration
  sendCommand(0x12, 0x73);

  // MDMCFG1 – Modem Configuration
  sendCommand(0x13, 0x22);

  // MDMCFG0 – Modem Configuration
  sendCommand(0x14, 0xF8);

  // CHANNR – Channel Number
  sendCommand(0x0A, 0x03);

  // DEVIATN – Modem Deviation Setting
  sendCommand(0x15, 0x00);

  // FREND1 – Front End RX Configuration
  sendCommand(0x21, 0xB6);

  // FREND0 – Front End TX configuration
  sendCommand(0x22, 0x10);

  // MCSM0 – Main Radio Control State Machine
  sendCommand(0x18, 0x18);

  // FOCCFG – Frequency Offset Compensation
  sendCommand(0x19, 0x1D);

  // BSCFG – Bit Synchronization Configuration
  sendCommand(0x1A, 0x1C);

  // AGCCTRL2 – AGC Control
  sendCommand(0x1B, 0xC7);

  // AGCCTRL1 – AGC Control
  sendCommand(0x1C, 0x00);

  // AGCCTRL0 – AGC Control
  sendCommand(0x1D, 0xB2);

  // FSCAL3 – Frequency Synthesizer Calibration
  sendCommand(0x23, 0xEA);

  // FSCAL2 – Frequency Synthesizer Calibration
  sendCommand(0x24, 0x0A);

  // FSCAL1 – Frequency Synthesizer Calibration
  sendCommand(0x25, 0x00);

  // FSCAL0 – Frequency Synthesizer Calibration
  sendCommand(0x26, 0x11);

  // TEST2 – Various Test Settings
  sendCommand(0x2C, 0x88);

  // TEST1 – Various Test Settings
  sendCommand(0x2D, 0x31);

  // TEST0 – Various Test Settings
  sendCommand(0x2E, 0x0B);

  // IOCFG2 – GDO2 Output Pin Configuration
  sendCommand(0x00, 0x06);

  // IOCFG0 – GDO0 Output Pin Configuration
  sendCommand(0x01, 0x01);

  // PKTCTRL1 – Packet Automation Control
  sendCommand(0x07, 0x04);

  // PKTCTRL0 – Packet Automation Control
  sendCommand(0x08, 0x45);

  // ADDR – Device Address
  sendCommand(0x09, 0x00);

  // PKTLEN – Packet Length
  sendCommand(0x06, 0xFF);

  // FIFOTHR – RX FIFO and TX FIFO Thresholds
  sendCommand(0x03, 0x0D);

  // PATABLE - Output Power Programming
  sendCommand(0x3E, 0xFF);

  // SIDLE
  sendStrobe(CC2500_CMD_SIDLE);

  // SPWD -  enter power down mode when CSn goes high
  sendStrobe(0x39);

  // SIDLE
  sendStrobe(CC2500_CMD_SIDLE);
}

uint8_t PhilipsLampLib::searchLamps(uint8_t duration) {
  uint8_t lamps = 0;

  listening(duration, [this, &lamps](uint8_t* data) {
    if (addLamp(data + 1)) lamps++;
  });

  return lamps;
};

void PhilipsLampLib::listening(uint8_t duration,
                               std::function<void(uint8_t*)> callback) {
  uint8_t result = 0;

  // SIDLE
  sendStrobe(CC2500_CMD_SIDLE);

  // RX FIFO länge auslesen
  result = sendCommand(0xFB, 0x00);

  for (int i = 0; i < result; ++i) {
    // Einzelnes Byte RX FIFO auslesen
    sendCommand(0xBF, 0x00);
  }

  // Einige Durchläufe lang versuchen
  for (int i = 0; i < duration; ++i) {
    // SRX: enable RX
    sendStrobe(0x34);

    // Kurz warten, bis RX gefüllt wird (falls jemand sendet)
    delay(50);

    // Anzahl Bytes im RX (sollten 17 sein)
    result = sendCommand(0xFB, 0x00);

    if (result >= 17) {
      uint8_t data[17];

      // Gesamtes Restpaket auslesen
      sendBurstCommand(0xFF, data, 17);

      // Paket auf Gültigkeit prüfen
      if (data[0] == 14 && data[9] == 17 &&
          (data[10] == 3 || data[10] == 5 || data[10] == 7)) {
        callback(data);
      } else {
        // Paket ungültig
      }
    }

    // SIDLE
    sendStrobe(CC2500_CMD_SIDLE);
  }

  // Restlänge auslesen
  result = sendCommand(0xFB, 0x00);

  for (int i = 0; i < result; ++i) {
    // Einzelnes Byte RX FIFO auslesen
    sendCommand(0xBF, 0x00);
  }
}

bool PhilipsLampLib::addLamp(uint8_t* address) {
  // Debug
  debug("Suche Adresse: ");

  for (int i = 0; i < 8; i++) {
    Serial.print(address[i]);
    Serial.print(" ");
  }

  debug("\r\nVorhandene Adressen im Speicher:\r\n");

  for (int l = 0; l < MAX_LAMPS; l++) {
    for (int i = 0; i < 8; i++) {
      debug(lamps[l][i]);
      debug(" ");
    }
    debug("\r\n");
  }

  // Prüfe ob Adresse schon vorhanden
  bool exist = true;

  for (int l = 0; l < MAX_LAMPS; l++) {
    if (lamps[l][0] == 0) continue;

    bool exist = true;

    for (int i = 0; i < 8; i++) {
      if (lamps[l][i] != address[i]) exist = false;
    }

    if (exist) return false;
  }

  // Füge Adresse hinzu
  for (int l = 0; l < MAX_LAMPS; l++) {
    // Leeres Adressfeld suchen und hier einfügen
    if (lamps[l][0] == 0) {
      for (int i = 0; i < 8; i++) {
        lamps[l][i] = address[i];
      }

      return true;
    }
  }

  // Hier angekommen, keine Platz mehr für neue Adressen!
  debug("Adressspeicer voll!");
}

uint8_t PhilipsLampLib::countLamps() {
  uint8_t count = 0;

  for (int l = 0; l < MAX_LAMPS; l++) {
    if (lamps[l][0] != 0 && lamps[l][1] != 0 && lamps[l][2] != 0 &&
        lamps[l][3] != 0) {
      count++;
    }
  }

  return count;
}

void PhilipsLampLib::setLamps(uint8_t cmd, uint8_t h, uint8_t s, uint8_t v) {
  for (int l = 0; l < MAX_LAMPS; l++) {
    if (lamps[l][0] == 0) continue;

    uint8_t data[15];

    // Fix
    data[0] = 0x0E;

    // Adresse
    data[1] = lamps[l][0];
    data[2] = lamps[l][1];
    data[3] = lamps[l][2];
    data[4] = lamps[l][3];
    data[5] = lamps[l][4];
    data[6] = lamps[l][5];
    data[7] = lamps[l][6];
    data[8] = lamps[l][7];

    // Fix
    data[9] = 17;

    // Befehl
    data[10] = cmd;

    // Counter
    data[11] = counter++;

    // HSV Farbwerte
    data[12] = h;
    data[13] = s;
    data[14] = v;

    // SIDLE
    sendStrobe(CC2500_CMD_SIDLE);

    // Fülle TX FIFO
    sendBurstCommand(0x7F, data, 15);

    // STX: enable TX
    sendStrobe(0x35);
    delay(100);
  }
};

void PhilipsLampLib::reset() {
  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(2);
  SPI.write(0x30);
  delayMicroseconds(45);
  digitalWrite(SPI_CS, HIGH);
}

void PhilipsLampLib::sendStrobe(byte strobe) {
  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(1);
  SPI.write(strobe);
  delayMicroseconds(1);
  digitalWrite(SPI_CS, HIGH);
};

uint8_t PhilipsLampLib::sendCommand(uint8_t command, uint8_t data) {
  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(1);
  SPI.write(command);
  delayMicroseconds(5);
  uint8_t result = SPI.transfer(data);
  delayMicroseconds(5);
  digitalWrite(SPI_CS, HIGH);
  return result;
};

void PhilipsLampLib::sendBurstCommand(uint8_t command, uint8_t* data,
                                      uint8_t length) {
  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(1);
  SPI.write(command);
  delayMicroseconds(1);

  uint8_t result = 0;

  // Daten senden
  for (int i = 0; i < length; ++i) {
    result = SPI.transfer(data[i]);
    delayMicroseconds(5);
    data[i] = result;
  }

  digitalWrite(SPI_CS, HIGH);
};

void PhilipsLampLib::debug(uint8_t data) {
  if (_stream) _stream->print(data);
};

void PhilipsLampLib::debugLn(const char* data) {
  if (_stream) _stream->println(data);
};

void PhilipsLampLib::debug(const char* data) {
  if (_stream) _stream->print(data);
};