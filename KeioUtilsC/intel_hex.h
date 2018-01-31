// intel_hex.h

#ifndef __INTEL_HEX_H
#define __INTEL_HEX_H


extern uint32_t firmware_crc;
extern uint32_t firmware_size;


extern bool ReadHexFile(char *filename, uint8_t *buffer, unsigned int buffer_size);


#endif