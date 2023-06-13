#pragma once

#include <LittleFS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#define EEPROM_BNO_OFFSET 0
#define EEPROM_BNO_ID_OFFSET EEPROM_BNO_OFFSET
#define EEPROM_BNO_CONFIG_OFFSET EEPROM_BNO_OFFSET + sizeof(long)

void eeprom_writeBnoConfig(long id, adafruit_bno055_offsets_t config);
long eeprom_readBnoId();
adafruit_bno055_offsets_t eeprom_readBnoConfig();