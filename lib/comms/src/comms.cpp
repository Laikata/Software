#include "comms.h"

static uint32_t crc32(const uint8_t data[], size_t data_length)
{
    uint32_t crc32 = 0xFFFFFFFFu;

    for (size_t i = 0; i < data_length; i++)
    {
        const uint32_t lookupIndex = (crc32 ^ data[i]) & 0xff;
        crc32 = (crc32 >> 8) ^ crc_table[lookupIndex]; // CRCTable is an array of 256 32-bit constants
    }

    // Finalize the CRC-32 value by inverting all the bits
    crc32 ^= 0xFFFFFFFFu;
    return crc32;
}

inline void read(uint8_t *buffer, int length)
{
    #if defined (TARGET_NANO_RP2040_CONNECT) || defined (TARGET_RASPBERRY_PI_PICO)
    Serial1.readBytes(buffer, length);
    #endif
    #ifdef ARDUINO_AVR_UNO
    Serial.readBytes(buffer, length);
    #endif
}

bool check_crc(uint8_t data[], size_t length, uint8_t expected[])
{
    uint32_t expected_crc = 0;
    memcpy(expected, &expected_crc, 4);

    return (crc32(data, length) == expected_crc);
}

void comms_send(uint8_t *data, size_t data_size, uint8_t header)
{
    static uint8_t counter = 0;
    size_t packet_size = 3 + data_size + 4;
    uint8_t packet[packet_size];
    packet[0] = 0x16;
    packet[1] = counter;
    packet[2] = header;

    memcpy(data, packet + 3, data_size);

    uint32_t hash = crc32(data, data_size);
    memcpy(&hash, packet + 3 + data_size, 4);

    #if defined (TARGET_NANO_RP2040_CONNECT) || defined (TARGET_RASPBERRY_PI_PICO)
    Serial1.write(packet, packet_size);
    #endif
    #ifdef ARDUINO_AVR_UNO
    Serial.write(packet, packet_size);
    #endif
}

// Start auto-generated code

void comms_gps(double latitude, double longitude, double altitude)
{
    static const int data_size = sizeof(double) + sizeof(double) + sizeof(double);
    uint8_t data[data_size];
    memcpy(&data[0], &latitude, sizeof(double));
    memcpy(&data[8], &longitude, sizeof(double));
    memcpy(&data[16], &altitude, sizeof(double));
    comms_send(data, data_size, 1);
}
void comms_unpack_gps(uint8_t data[], double *latitude, double *longitude, double *altitude)
{
    memcpy(latitude, data + 0, sizeof(double));
    memcpy(longitude, data + 8, sizeof(double));
    memcpy(altitude, data + 16, sizeof(double));
}
void comms_imu(vec3_t mag, vec3_t accel, vec3_t gyro, float hoz)
{
    static const int data_size = sizeof(vec3_t) + sizeof(vec3_t) + sizeof(vec3_t) + sizeof(float);
    uint8_t data[data_size];
    memcpy(&data[0], &mag, sizeof(vec3_t));
    memcpy(&data[12], &accel, sizeof(vec3_t));
    memcpy(&data[24], &gyro, sizeof(vec3_t));
    memcpy(&data[36], &hoz, sizeof(float));
    comms_send(data, data_size, 2);
}
void comms_unpack_imu(uint8_t data[], vec3_t *mag, vec3_t *accel, vec3_t *gyro, float *hoz)
{
    memcpy(mag, data + 0, sizeof(vec3_t));
    memcpy(accel, data + 12, sizeof(vec3_t));
    memcpy(gyro, data + 24, sizeof(vec3_t));
    memcpy(hoz, data + 36, sizeof(float));
}
packet_t comms_recv()
{
    uint8_t start = 0;
    read(&start, 1);
    Serial.print("Start ");
    Serial.println(start);
    if (start == 0x16)
    {
        uint8_t buffer[2];
        read(buffer, 2);
        uint8_t header = buffer[1];
        Serial.print("header ");
        Serial.println(header);
        if (header == 1)
        {
            uint8_t data[24 + 4];
            read(data, 24 + 4);
            if (!check_crc(data, 24, data + 24))
                return packet_error_crc;
            comms_unpack_gps(data, &gps_latitude, &gps_longitude, &gps_altitude);
            return packet_gps;
        }
        else if (header == 2)
        {
            uint8_t data[40 + 4];
            read(data, 40 + 4);
            if (!check_crc(data, 40, data + 40))
                return packet_error_crc;
            comms_unpack_imu(data, &imu_mag, &imu_accel, &imu_gyro, &imu_hoz);
            return packet_imu;
        }
    }
    return packet_none;
}
