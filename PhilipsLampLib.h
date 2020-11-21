#ifndef CLASS_PHILIPSLAMPLIB
#define CLASS_PHILIPSLAMPLIB

#include <SPI.h>
#include <SoftwareSerial.h>

#define CC2500_CMD_SIDLE 0x36  // Exit RX / TX, turn off frequency synthesizer
#define SPI_CS 15
#define MAX_LAMPS 10

#define CMD_SET_COLOR 3
#define CMD_ON 5
#define CMD_OFF 7

class PhilipsLampLib {
 private:
  Stream *_serial;
  void debug(uint8_t data);
  void debugLn(const char *data);

 protected:
  uint8_t counter;

 public:
  uint8_t lamps[MAX_LAMPS][8];  // Lampenadressen (je 8 Byte)
  PhilipsLampLib();
  void setSerial(Stream *serial);
  void reset();

  /**
  Suche X-Sekunden nach Sendersignalen und speichere die Empfangsadressen
  @param duration Anzahl Durchläufe (je 100ms)
  @return Wurden Lampen gefunden?
  */
  uint8_t searchLamps(uint8_t duration = 100);
  void setLamps(uint8_t cmd, uint8_t h = 0, uint8_t s = 0, uint8_t v = 0);
  bool addLamp(uint8_t *address);

  /**
  @return Wieviele Lampen sind aktuell registriert?
  */
  uint8_t countLamps();
  void sendStrobe(byte strobe);
  uint8_t sendCommand(uint8_t command, uint8_t data);
  uint8_t sendBurstCommand(uint8_t command, uint8_t *data, uint8_t length);

  /**
  Suche X-Sekunden nach Paketen
  @param duration Anzahl Durchläufe (je 100ms)
  @param callback Callback welcher bei einem empfangenen Paket aufgerufen
  wird
  */
  void listening(uint8_t duration, std::function<void(uint8_t *)> callback);
};

#endif