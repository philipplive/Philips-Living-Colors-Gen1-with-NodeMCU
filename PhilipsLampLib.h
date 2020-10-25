#include <SPI.h>
#include <SoftwareSerial.h>

#define CC2500_CMD_SIDLE 0x36  // Exit RX / TX, turn off frequency synthesizer
#define SPI_CS 15

class PhilipsLampLib {
 private:
  Stream *_serial;

 protected:
  unsigned char counter;

 public:
  PhilipsLampLib();
  void setSerial(Stream *serial);
  void reset();
  void scanLamps();
  void setLamps();
  void sendStrobe(byte strobe);
  unsigned char sendCommand(unsigned char command, unsigned char data);
  unsigned char sendBurstCommand(unsigned char command, unsigned char *data,
                                 unsigned char length);
  unsigned char sendByte(unsigned char data);
};
