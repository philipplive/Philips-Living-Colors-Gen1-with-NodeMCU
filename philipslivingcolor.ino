#include<Arduino.h>
#include "PhilipsLampLib.h"

PhilipsLampLib plc;

void setup() {
  Serial.begin(115200);
  plc.setSerial(&Serial);
}

void loop() {
  plc.scanLamps(); 
  //plc.setLamps();
  delay(1000);
}
