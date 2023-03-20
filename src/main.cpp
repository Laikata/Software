#include <Arduino.h>
#include <comms.h>

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  if(comms_recv()) {
    Serial.println(gps_longitude);
    Serial.println(gps_latitude);
    Serial.println(gps_altitude);
  }
}