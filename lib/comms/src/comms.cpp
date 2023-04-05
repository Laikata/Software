#include "comms.h"

static uint32_t crc32(const uint8_t data[], size_t data_length) {
    uint32_t crc32 = 0xFFFFFFFFu;
    
    for (size_t i = 0; i < data_length; i++) {
        const uint32_t lookupIndex = (crc32 ^ data[i]) & 0xff;
        crc32 = (crc32 >> 8) ^ crc_table[lookupIndex];  // CRCTable is an array of 256 32-bit constants
    }
    
    // Finalize the CRC-32 value by inverting all the bits
    crc32 ^= 0xFFFFFFFFu;
    return crc32;
}

void comms_send(uint8_t data[], uint8_t data_length, uint8_t header){
    
	// SYN char + header + counter + data + crc32 checksum
	uint16_t packet_size = 1 + 1 + 1 + data_length + 4;
	static uint8_t counter = 0;
	uint8_t packet[packet_size];

	packet[0] = 0x16;
	packet[1] = header;
	packet[2] = counter;

	for (uint32_t i = 0; i < data_length; i++) {
		packet[i + 3] = data[i];
	}

	uint32_t checksum = crc32(data, data_length);

	//TODO: Test whether this is sent in big endian or not
	packet[2 + data_length + 1] = (checksum >> 24) & 0xff;
	packet[2 + data_length + 2] = (checksum >> 16) & 0xff;
	packet[2 + data_length + 3] = (checksum >> 8) & 0xff;
	packet[2 + data_length + 4] = checksum & 0xff;

	static int lost_packets = 0;
	// Discard package if there isn't space for writing
	if(Serial.availableForWrite() >= packet_size) {
		Serial.write(packet, packet_size);
                counter++; // This will overflow and that is okay
	} else {
		lost_packets++;
		//Serial.printf("LOST PACKETS %i\n", lost_packets);
	}
}

inline void read(uint8_t *buffer, int length) {
    Serial.readBytes(buffer, length);
}

bool check_crc(uint8_t data[], size_t length, uint8_t expected[]) {
    uint32_t expected_crc = 0;
    memcpy(expected, &expected_crc, 4);

    return (crc32(data, length) == expected_crc);
}

void comms_unpack_gps(uint8_t data[], double *latitude, double *longitude, double *altitude) {
        memcpy(latitude, data + 0, sizeof(double));
        memcpy(longitude, data + 8, sizeof(double));
        memcpy(altitude, data + 16, sizeof(double));
}

void comms_unpack_imu(uint8_t data[], vec3_t *mag, vec3_t *accel, vec3_t *gyro, float *hoz) {
        memcpy(mag, data + 0, sizeof(vec3_t));
        memcpy(accel, data + 12, sizeof(vec3_t));
        memcpy(gyro, data + 24, sizeof(vec3_t));
        memcpy(hoz, data + 36, sizeof(float));
}

PacketType comms_recv() { 
	uint8_t start = 0x00;
	read(&start, 1);
	if(start == 0x16) {
		uint8_t buffer[2];
		read(buffer, 2);
		uint8_t header = buffer[1];
		if (header == 1) {
			uint8_t data[24 + 4];
			read(data, 24 + 4);
			if(check_crc(data, 24, data + 24)){ return none; }
			comms_unpack_gps(data, &gps_latitude, &gps_longitude, &gps_altitude);
		}
		else if (header == 2) {
			uint8_t data[40 + 4];
			read(data, 40 + 4);
			if(check_crc(data, 40, data + 40)){ return none; }
			comms_unpack_imu(data, &imu_mag, &imu_accel, &imu_gyro, &imu_hoz);
		}
	}
	return none;
}

void comms_gps(double latitude, double longitude, double altitude) {
        static const int data_size = sizeof(double) + sizeof(double) + sizeof(double);
        uint8_t data[data_size];
        memcpy(&data[0], &latitude, sizeof(double));
        memcpy(&data[8], &longitude, sizeof(double));
        memcpy(&data[16], &altitude, sizeof(double));
        comms_send(data, data_size, 0x01);
}


void comms_imu(vec3_t mag, vec3_t accel, vec3_t gyro, float hoz) {
        static const int data_size = sizeof(vec3_t) + sizeof(vec3_t) + sizeof(vec3_t) + sizeof(float);
        uint8_t data[data_size];
        memcpy(&data[0], &mag, sizeof(vec3_t));
        memcpy(&data[12], &accel, sizeof(vec3_t));
        memcpy(&data[24], &gyro, sizeof(vec3_t));
        memcpy(&data[36], &hoz, sizeof(float));
        comms_send(data, data_size, 0x02);
}
