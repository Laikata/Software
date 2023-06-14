#pragma once
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <LittleFS.h>
//https://forum.pjrc.com/threads/58592-Reliably-reloading-the-calibration-data-of-IMU-BNO055
#define BNO055_SAMPLERATE_DELAY_MS (100)

void bno_init();
double bno_getAzimuth();