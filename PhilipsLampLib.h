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
  void debug(unsigned char data);
  void debugLn(const char *data);

 protected:
  unsigned char counter;

 public:
  unsigned char lamps[MAX_LAMPS][8];  // Lampenadressen (je 8 Byte)
  PhilipsLampLib();
  void setSerial(Stream *serial);
  void reset();

  /**
      Suche X-Sekunden nach Sendersignalen und speichere die Empfangsadressen
      @param duration Anzahl Durchläufe (je 100ms)
      @return Wurden Lampen gefunden?
  */
  unsigned char searchLamps(unsigned char duration = 100);
  void setLamps(unsigned char cmd, unsigned char h = 0, unsigned char s = 0,
                unsigned char v = 0);
  void addLamp(unsigned char *address);
  void sendStrobe(byte strobe);
  unsigned char sendCommand(unsigned char command, unsigned char data);
  unsigned char sendBurstCommand(unsigned char command, unsigned char *data,
                                 unsigned char length);
  unsigned char sendByte(unsigned char data);
};