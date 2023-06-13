#include "eeprom.h"

void eeprom_writeBnoConfig(long id, adafruit_bno055_offsets_t config){
  File f = LittleFS.open("bno_conf", "w");
  char* buffer = (char*) &config;
  f.write(buffer, sizeof(adafruit_bno055_offsets_t));
  f.close();
}

adafruit_bno055_offsets_t eeprom_readBnoConfig(){
  adafruit_bno055_offsets_t config;
  char* buffer = (char*) &config;
  File f = LittleFS.open("bno_conf", "r");
  f.readBytes(buffer, sizeof(adafruit_bno055_offsets_t));
  return config;
}