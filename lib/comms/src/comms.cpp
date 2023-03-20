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

inline void read(uint8_t *buffer, int length) {
    Serial.readBytes(buffer, length);
}

bool check_crc(uint8_t data[], size_t length, uint8_t expected[]) {
    uint32_t expected_crc = 0;
    memcpy(expected, &expected_crc, 4);

    return (crc32(data, length) == expected_crc);
}



void comms_unpack_gps(uint8_t data[], double *latitude, double *longitude, double *altitude) {
    memcpy(latitude, &data[0], sizeof(double));
    memcpy(longitude, &data[8], sizeof(double));
    memcpy(altitude, &data[16], sizeof(double));
}

bool comms_recv() {
    uint8_t start = 0x00;
    read(&start, 1);
    if(start == 0x16) {
        uint8_t buffer[2];
        read(buffer, 2);
        uint8_t header = buffer[1];
        if (header == 0x01) { // GPS
            uint8_t data[24 + 4];
            read(data, 24 + 4);
            if(!check_crc(data, 24, data + 24)) { return false; }
            comms_unpack_gps(data, &gps_latitude, &gps_longitude, &gps_altitude);
        }
    }
    return true;
}