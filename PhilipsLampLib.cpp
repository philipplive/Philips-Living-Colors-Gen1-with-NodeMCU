#include "PhilipsLampLib.h"

// Referenz zu einem Stream für Debug
void PhilipsLampLib::setSerial(Stream* serial) { _serial = serial; }

PhilipsLampLib::PhilipsLampLib() {
  // SPI config
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);

  delay(1);

  // reset device (0x30)
  reset();

  //FSCTRL1 – Frequency Synthesizer Control
  sendCommand(0x0B, 0x09);

  //FSCTRL0 – Frequency Synthesizer Control
  sendCommand(0x0C, 0x00);

  //FREQ2 – Frequency Control Word, High Byte
  sendCommand(0x0D, 0x5D);

  //FREQ1 – Frequency Control Word, Middle Byte
  sendCommand(0x0E, 0x93);

  //FREQ0 – Frequency Control Word, Low Byte
  sendCommand(0x0F, 0xB1);

  //MDMCFG4 – Modem Configuration
  sendCommand(0x10, 0x2D);

  //MDMCFG3 – Modem Configuration
  sendCommand(0x11, 0x3B);

  //MDMCFG2 – Modem Configuration
  sendCommand(0x12, 0x73);

  //MDMCFG1 – Modem Configuration
  sendCommand(0x13, 0x22);

  //MDMCFG0 – Modem Configuration
  sendCommand(0x14, 0xF8);

  //CHANNR – Channel Number
  sendCommand(0x0A, 0x03);

  //DEVIATN – Modem Deviation Setting
  sendCommand(0x15, 0x00);

  //FREND1 – Front End RX Configuration
  sendCommand(0x21, 0xB6);

  //FREND0 – Front End TX configuration
  sendCommand(0x22, 0x10);

  //MCSM0 – Main Radio Control State Machine
  // Configuration
  sendCommand(0x18, 0x18);

  //FOCCFG – Frequency Offset Compensation
  // Configuration
  sendCommand(0x19, 0x1D);

  //BSCFG – Bit Synchronization Configuration
  sendCommand(0x1A, 0x1C);

  //AGCCTRL2 – AGC Control
  sendCommand(0x1B, 0xC7);

  //AGCCTRL1 – AGC Control
  sendCommand(0x1C, 0x00);

  //AGCCTRL0 – AGC Control
  sendCommand(0x1D, 0xB2);

  //FSCAL3 – Frequency Synthesizer Calibration
  sendCommand(0x23, 0xEA);

  //FSCAL2 – Frequency Synthesizer Calibration
  sendCommand(0x24, 0x0A);

  //FSCAL1 – Frequency Synthesizer Calibration
  sendCommand(0x25, 0x00);

  //FSCAL0 – Frequency Synthesizer Calibration
  sendCommand(0x26, 0x11);

  //TEST2 – Various Test Settings
  sendCommand(0x2C, 0x88);

  //TEST1 – Various Test Settings
  sendCommand(0x2D, 0x31);

  //TEST0 – Various Test Settings
  sendCommand(0x2E, 0x0B);

  //IOCFG2 – GDO2 Output Pin Configuration
  sendCommand(0x00, 0x06);

  //IOCFG0 – GDO0 Output Pin Configuration
  sendCommand(0x01, 0x01);

  //PKTCTRL1 – Packet Automation Control
  sendCommand(0x07, 0x04);

  //PKTCTRL0 – Packet Automation Control
  sendCommand(0x08, 0x45);

  //ADDR – Device Address
  sendCommand(0x09, 0x00);

  //PKTLEN – Packet Length
  sendCommand(0x06, 0xFF);

  //FIFOTHR – RX FIFO and TX FIFO Thresholds
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

void PhilipsLampLib::reset() {
  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(2);
  SPI.transfer(0x30);
  delayMicroseconds(45);
  digitalWrite(SPI_CS, HIGH);
}

void PhilipsLampLib::scanLamps() {
  unsigned char result = 0;

  // SIDLE
  sendStrobe(CC2500_CMD_SIDLE);

  // RX FIFO länge auslesen
  result = sendCommand(0xFB, 0x00);

  for (int i = 0; i < result; ++i) {
    // Einzelnes Byte RX FIFO auslesen
    sendCommand(0xBF, 0x00);
  }

  // Einige Durchläufe lang versuchen
 for (int i = 0; i < 30; ++i) {
    // SRX: enable RX
    sendStrobe(0x34);

    // Kurz warten, bis RX gefüllt wird (falls jemand sendet)
    delay(100);

    // Anzahl Bytes im RX (17)
    result = sendCommand(0xFB, 0x00);

    if (result >= 17) {
      Serial.print("-");
      Serial.println(result);

      // Erstes Byte lesen aus RX FIFO
      result = sendCommand(0xBF, 0x00);

      Serial.print("--");
      Serial.println(result);

      // Paketstart immer mit 0x0E (14)
      if (result == 0x0E) {
        unsigned char data[16];

        // Gesamtes Restpaket auslesen
        sendBurstCommand(0xFF, data, result);

        for (int i = 0; i < 16; ++i) {
          Serial.print("---");
          Serial.print(i);
          Serial.print("-");
          Serial.print(data[i]);
          if (i == 8) Serial.print("(fix)");
          if (i == 9) Serial.print("(cmd)");
          if (i == 10) Serial.print("(ink)");

          if (i == 13) Serial.print("(v)");
          Serial.println("");
        }
      } else {
        // Falsche Pakete verwerfen
        // Restlänge auslesen
        result = sendCommand(0xFB, 0x00);

        for (int i = 0; i < result; ++i) {
          // Einzelnes Byte RX FIFO auslesen
          sendCommand(0xBF, 0x00);
        }
      }
    }

    // SIDLE
    sendStrobe(CC2500_CMD_SIDLE);

    delay(2);
  }

  // SIDLE
  sendStrobe(CC2500_CMD_SIDLE);

  // Restlänge auslesen
  result = sendCommand(0xFB, 0x00);

  for (int i = 0; i < result; ++i) {
    // Einzelnes Byte RX FIFO auslesen
    sendCommand(0xBF, 0x00);
  }
};

unsigned char count;

void PhilipsLampLib::setLamps() {
  // Warte bis fertig gesendet
  while ((sendByte(0xF5) & 0x1F) > 1) {
  };

  unsigned char data[15];

  data[0] = 0x0E;  // Paketlänge

  data[1] = 208;
  data[2] = 152;
  data[3] = 119;
  data[4] = 152;
  data[5] = 195;
  data[6] = 64;
  data[7] = 213;
  data[8] = 180;
  data[9] = 17;

  // CMD 0x07 = off,  0x05 = on, 0x03 = hsv
  data[10] = 0x03;

  // Counter
  data[11] = count++;

  // HSV
  data[12] = random(0, 255);
  data[13] = random(0, 255);
  data[14] = random(0, 255);

  // SIDLE
  sendStrobe(CC2500_CMD_SIDLE);

  // Fülle TX FIFO
  sendBurstCommand(0x7F, data, 15);

  // STX: enable TX
  sendStrobe(0x35);
};

void PhilipsLampLib::sendStrobe(byte strobe) {
  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(1);
  SPI.transfer(strobe);
  delayMicroseconds(1);
  digitalWrite(SPI_CS, HIGH);
};

unsigned char PhilipsLampLib::sendCommand(unsigned char command,
                                          unsigned char data) {
  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(1);
  SPI.write(command);
  delayMicroseconds(5);
  unsigned char result = SPI.transfer(data);
  delayMicroseconds(5);
  digitalWrite(SPI_CS, HIGH);
  return result;
};

unsigned char PhilipsLampLib::sendBurstCommand(unsigned char command,
                                               unsigned char* data,
                                               unsigned char length) {
  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(1);
  SPI.write(command);
  delayMicroseconds(1);

  unsigned char result = 0;

  // Daten senden
  for (int i = 0; i < length; ++i) {
    result = SPI.transfer(data[i]);
    delayMicroseconds(5);
    data[i] = result;
  }

  digitalWrite(SPI_CS, HIGH);
  return result;
};

unsigned char PhilipsLampLib::sendByte(unsigned char data) {
  unsigned char result = 0;

  digitalWrite(SPI_CS, LOW);
  delayMicroseconds(1);
  result = SPI.transfer(data);
  delayMicroseconds(5);
  digitalWrite(SPI_CS, HIGH);

  return result;
};
