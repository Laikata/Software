#include <Arduino.h>
#include <comms.h>

void test_gps(void) {
    double latitude = 69.420;
    double longitude = 4563.86465;
    double altitude = 1;

    comms_gps(latitude, longitude, altitude);
    Serial.println(comms_recv());
    //assert(comms_recv() == packet_gps);
    //assert(latitude == gps_latitude);
    //assert(longitude == gps_longitude);
    //assert(altitude == gps_altitude);
}

void setup() {
    Serial.begin(9600);
    #if defined (TARGET_NANO_RP2040_CONNECT) || defined (TARGET_RASPBERRY_PI_PICO)
    Serial1.begin(9600);
    #endif
    //gps_latitude = 0;
    //gps_longitude = 0;
    //gps_altitude = 0;
}

void loop() {
    test_gps();
    delay(500);
}
