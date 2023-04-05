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

  

  if (millis() % 10 == 0) {
    Serial.println("Sending");
    comms_gps(0.1, 0.2, 0.3);
  }

}