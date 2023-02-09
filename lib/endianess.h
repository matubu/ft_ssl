#pragma once

#include <stdint.h>

#if defined(__linux__)
# include <endian.h>
#elif defined(__APPLE__)
# include <machine/endian.h>
#endif

// LITTLE_ENDIAN
// BIG_ENDIAN
// BYTE_ORDER

uint16_t	swap_uint16(uint16_t n) {
	n = (n >> 8) | (n << 8);
	return (n);
}
uint32_t	swap_uint32(uint32_t n) {
	                                                        // 1234
	n = (n >> 16) | (n << 16);                              // 3412
	n = ((n & 0xFF00FF00) >> 8) | ((n & 0x00FF00FF) << 8);  // 4321
	return (n);
}
uint64_t	swap_uint64(uint64_t n) {
	n = (n >> 32) | (n << 32);
	n = ((n & 0xFFFF0000FFFF0000) >> 16) | ((n & 0x0000FFFF0000FFFF) << 16);
	n = ((n & 0xFF00FF00FF00FF00) >>  8) | ((n & 0x00FF00FF00FF00FF) <<  8);
	return (n);
}

uint16_t	uint16_endianess(uint16_t n, int target_endian) {
	if (target_endian == BYTE_ORDER)
		return (n);
	return (swap_uint16(n));
}
uint32_t	uint32_endianess(uint32_t n, int target_endian) {
	if (target_endian == BYTE_ORDER)
		return (n);
	return (swap_uint32(n));
}
uint64_t	uint64_endianess(uint64_t n, int target_endian) {
	if (target_endian == BYTE_ORDER)
		return (n);
	return (swap_uint64(n));
}