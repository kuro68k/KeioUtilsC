#pragma once


extern void crc16_set(uint16_t initial_value, uint16_t polynomial, uint16_t final_xor, bool reverse_input, bool reverse_output);
extern uint8_t crc8(const void *buffer, unsigned int buffer_length);
extern uint16_t crc16(const void *buffer, unsigned int buffer_length);
extern uint32_t crc32(const void *buffer, unsigned int buffer_length);
extern uint32_t xmega_nvm_crc32(uint8_t *buffer, unsigned int buffer_length);
