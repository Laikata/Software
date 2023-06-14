#include "bno.h"

extern Adafruit_BNO055 bno;

#define BNO055_SAMPLERATE_DELAY_MS (100)

void displaySensorOffsets(const adafruit_bno055_offsets_t &calibData)
{
  Serial.print("Accelerometer: ");
  Serial.print(calibData.accel_offset_x); Serial.print(" ");
  Serial.print(calibData.accel_offset_y); Serial.print(" ");
  Serial.print(calibData.accel_offset_z); Serial.print(" ");

  Serial.print("\nGyro: ");
  Serial.print(calibData.gyro_offset_x); Serial.print(" ");
  Serial.print(calibData.gyro_offset_y); Serial.print(" ");
  Serial.print(calibData.gyro_offset_z); Serial.print(" ");

  Serial.print("\nMag: ");
  Serial.print(calibData.mag_offset_x); Serial.print(" ");
  Serial.print(calibData.mag_offset_y); Serial.print(" ");
  Serial.print(calibData.mag_offset_z); Serial.print(" ");

  Serial.print("\nAccel Radius: ");
  Serial.print(calibData.accel_radius);

  Serial.print("\nMag Radius: ");
  Serial.print(calibData.mag_radius);
}

void displayCalStatus(void)
{
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }

  /* Display the individual values */
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.print(mag, DEC);
}

void bno_init(){
  Serial.println("Initializing BNO");
  delay(1000); // TODO: This may be unnecessary, test it once Laikata is working


  if(!bno.begin()){
    Serial.println("ERROR: No BNO detected...");
  }

  sensor_t sensor;
  bno.getSensor(&sensor);
  bno.setExtCrystalUse(true);
  sensors_event_t event;

  if(!LittleFS.exists("bno_calib")) {
    Serial.println("No calibration found");
    delay(1500);
    Serial.println("Please Calibrate Sensor: ");
    while (!bno.isFullyCalibrated())
    {
        bno.getEvent(&event);

        Serial.print("X: ");
        Serial.print(event.orientation.x, 4);
        Serial.print("\tY: ");
        Serial.print(event.orientation.y, 4);
        Serial.print("\tZ: ");
        Serial.print(event.orientation.z, 4);

        /* Optional: Display calibration status */
        displayCalStatus();

        /* New line for the next sample */
        Serial.println("");

        /* Wait the specified delay before requesting new data */
        delay(BNO055_SAMPLERATE_DELAY_MS);
    }
    Serial.println("\nFully calibrated!");
    Serial.println("--------------------------------");
    Serial.println("Calibration Results: ");
    adafruit_bno055_offsets_t newCalib;
    bno.getSensorOffsets(newCalib);
    displaySensorOffsets(newCalib);

    Serial.println("\n\nStoring calibration data to EEPROM...");
    File f = LittleFS.open("bno_calib", "w");
    char* buffer = (char*) &newCalib;
    f.write(buffer, sizeof(adafruit_bno055_offsets_t));
    f.close();
  }
  else {
    Serial.println("Calibration found");
    Serial.println("Restoring Calibration data to the BNO055...");

    adafruit_bno055_offsets_t calibData;
    char* buffer = (char*) &calibData;
    File f = LittleFS.open("bno_calib", "r");
    f.readBytes(buffer, sizeof(adafruit_bno055_offsets_t));
    bno.setSensorOffsets(calibData);
    displaySensorOffsets(calibData);

    Serial.println("Calibrating magnetometer...");
    // while (!bno.isFullyCalibrated())
    // {
    //  bno.getEvent(&event);
    //  //displayCalStatus();
    //  //Serial.println("");
    //  delay(BNO055_SAMPLERATE_DELAY_MS);
    // }
  }
}

double bno_getAzimuth(){
  imu::Quaternion quat = bno.getQuat();
  double siny_cosp = 2 * (quat.w() * quat.z() + quat.x() * quat.y());
  double cosy_cosp = 1 - 2 * (quat.y() * quat.y() + quat.z() * quat.z());
  double yaw = atan2 (siny_cosp, cosy_cosp);
  double a = yaw * 57.21 + 180;
  return a;
}