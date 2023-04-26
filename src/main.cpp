#include <Arduino.h>
#include <vector.h>
#include <motor.h>
#include <comms.h>


void test_gps(void) {
    double latitude = 69.420;
    double longitude = 4563.86465;
    double altitude = 1;

    if (Serial.available() == 0 ) {
        comms_gps(latitude, longitude, altitude);
        comms_imu({3, 4, 5}, {6, 7, 8}, {9, 10, 11}, 4);
    }
    switch(comms_recv()) {
        case packet_none: break;
        case packet_gps: {
            Serial.println(gps_latitude);
            Serial.println(gps_longitude);
            Serial.println(gps_altitude);
            break;
        }
        case packet_imu: {
            for(int i = 0; i < 3; i++) {
                vec3_t *imu;
                switch(i) {
                    case 0: imu = &imu_mag; break;
                    case 1: imu = &imu_gyro; break;
                    case 2: imu = &imu_accel; break;
                }
                Serial.print(imu->x);
                Serial.print(" ");
                Serial.print(imu->y);
                Serial.print(" ");
                Serial.println(imu->z);
            }
            Serial.println(imu_hoz);
            break;
        }
        case packet_error_crc: {
            Serial.println("CRC Mismatch!");
            break;
        }
    }
    //assert(comms_recv() == packet_gps);
    //assert(latitude == gps_latitude);
    //assert(longitude == gps_longitude);
    //assert(altitude == gps_altitude);
}

void setup() {
    motor_init();
    Serial.begin(9600);
    #if defined (TARGET_NANO_RP2040_CONNECT) || defined (TARGET_RASPBERRY_PI_PICO)
    Serial1.begin(9600);
    #endif
}

bool motor_override = false;
long last_send = 0;

void loop() {
    for(int i = 1000; i < 2000; i += 100) {
        motor_speed(i);
        delay(100);
    }
    return;
    // Recibir
    switch (comms_recv()) {
        case packet_none: break;
        case packet_ctl: {
            
            // Ajustar motores
            break;
        }
        case packet_error_crc: {
            Serial.println("CRC Mismatch!");
            break;
        }
    }
    // Leer sensores

    // Calcular dirección
    if(!motor_override) {

    } else {

    }

    // Motores

    // Enviar todo
    if(millis() - last_send > 500 ) {
        // blablabla
        last_send = millis();
    }
}
