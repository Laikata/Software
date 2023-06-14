//#include "motor.h"
//#include "comms.h"
//#include "vector.h"
#include <LittleFS.h>
#include "bno.h"
#include "gps.h"
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

// void test_gps(void) {
//     double latitude = 69.420;
//     double longitude = 4563.86465;
//     double altitude = 1;

//     if (Serial.available() == 0 ) {
//         comms_gps(latitude, longitude, altitude);
//         comms_imu({3, 4, 5}, {6, 7, 8}, {9, 10, 11}, 4);
//     }
//     switch(comms_recv()) {
//         case packet_none: break;
//         case packet_gps: {
//             Serial.println(gps_latitude);
//             Serial.println(gps_longitude);
//             Serial.println(gps_altitude);
//             break;
//         }
//         case packet_imu: {
//             for(int i = 0; i < 3; i++) {
//                 vec3_t *imu;
//                 switch(i) {
//                     case 0: imu = &imu_mag; break;
//                     case 1: imu = &imu_gyro; break;
//                     case 2: imu = &imu_accel; break;
//                 }
//                 Serial.print(imu->x);
//                 Serial.print(" ");
//                 Serial.print(imu->y);
//                 Serial.print(" ");
//                 Serial.println(imu->z);
//             }
//             Serial.println(imu_hoz);
//             break;
//         }
//         case packet_error_crc: {
//             Serial.println("CRC Mismatch!");
//             break;
//         }
//     }
//     //assert(comms_recv() == packet_gps);
//     //assert(latitude == gps_latitude);
//     //assert(longitude == gps_longitude);
//     //assert(altitude == gps_altitude);
// }
// C:\Users\BOT\.platformio\packages\tool-rp2040tools
void setup(){
  LittleFS.begin();
  Serial.begin(115200);
  bno_init();
  gps_init();
}

bool motor_override = false;
long last_send = 0;

void loop(){
  gps_info();
  imu::Quaternion quat = bno.getQuat();
  Serial.print("qW: ");
  Serial.print(quat.w(), 4); 
  Serial.print(" qX: ");
  Serial.print(quat.x(), 4);
  Serial.print(" qY: ");
  Serial.print(quat.y(), 4);
  Serial.print(" qZ: ");
  Serial.print(quat.z(), 4);
  double siny_cosp = 2 * (quat.w() * quat.z() + quat.x() * quat.y());
  double cosy_cosp = 1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z());
  double yaw = atan2 (siny_cosp, cosy_cosp);
  double a = yaw * 57.21 + 180;
  Serial.print("  ");
  Serial.print(a, 2);
  Serial.println("\t\t");
    Serial.printf("BNO: %g\n", bno_getAzimuth());

    // for(int i = 1000; i < 2000; i += 100) {
    //     motor_speed(i);
    //     delay(100);
    // }
    // return;
    // // Recibir
    // switch (comms_recv()) {
    //     case packet_none: break;
    //     case packet_ctl: {
            
    //         // Ajustar motores
    //         break;
    //     }
    //     case packet_error_crc: {
    //         Serial.println("CRC Mismatch!");
    //         break;
    //     }
    // }
    // // Leer sensores

    // // Calcular dirección
    // if(!motor_override) {

    // } else {

    // }

    // // Motores

    // // Enviar todo
    // if(millis() - last_send > 500 ) {
    //     // blablabla
    //     last_send = millis();
    // }
}
