#pragma once

#include "malloc.h"
#include "string.h"
#include "endianess.h"
#include "utils.h"

// nice links:
// https://en.wikipedia.org/wiki/Whirlpool_(hash_function)
// https://www2.seas.gwu.edu/~poorvi/Classes/CS381_2007/Whirlpool.pdf

#define WHIRLPOOL_PADDING_OPT (&(padding_opt){ \
	.chunk_byte_count = 64, \
	.length_byte_order = LITTLE_ENDIAN, \
})
#define WHIRLPOOL_DIGEST_LENGTH (16 * sizeof(uint32_t))

// do 10 times:
#define RF(K, r) SubBytes(); ShiftColums(); MixRows(); AddRoundKey(K[r]);
// main algo for chunk
#define W(K) AddRoundKey(K[0]); for (size_t i = 0; i <= 10; ++i) RF(K, i);

static const uint8_t	WhirlpoolSBox[] = {
	0x18, 0x23, 0xC6, 0xE8, 0x87, 0xB8, 0x01, 0x4F, 0x36, 0xA6, 0xD2, 0xF5, 0x79, 0x6F, 0x91, 0x52,
	0x60, 0xBC, 0x0B, 0x8E, 0xA3, 0x0C, 0x7B, 0x35, 0x1D, 0xE0, 0xD7, 0xC2, 0x2E, 0x4B, 0xFE, 0x57,
	0x15, 0x77, 0x37, 0xE5, 0x9F, 0xF0, 0x4A, 0xCA, 0x58, 0xC9, 0x29, 0x0A, 0xB1, 0xA0, 0x6B, 0x85,
	0xBD, 0x5D, 0x10, 0xF4, 0xCB, 0x3E, 0x05, 0x67, 0xE4, 0x27, 0x41, 0x8B, 0xA7, 0x7D, 0x95, 0xC8,
	0xFB, 0xEE, 0x7C, 0x66, 0xDD, 0x17, 0x47, 0x9E, 0xCA, 0x2D, 0xBF, 0x07, 0xAD, 0x5A, 0x83, 0x33,
	0x63, 0x02, 0xAA, 0x71, 0xC8, 0x19, 0x49, 0xC9, 0xF2, 0xE3, 0x5B, 0x88, 0x9A, 0x26, 0x32, 0xB0,
	0xE9, 0x0F, 0xD5, 0x80, 0xBE, 0xCD, 0x34, 0x48, 0xFF, 0x7A, 0x90, 0x5F, 0x20, 0x68, 0x1A, 0xAE,
	0xB4, 0x54, 0x93, 0x22, 0x64, 0xF1, 0x73, 0x12, 0x40, 0x08, 0xC3, 0xEC, 0xDB, 0xA1, 0x8D, 0x3D,
	0x97, 0x00, 0xCF, 0x2B, 0x76, 0x82, 0xD6, 0x1B, 0xB5, 0xAF, 0x6A, 0x50, 0x45, 0xF3, 0x30, 0xEF,
	0x3F, 0x55, 0xA2, 0xEA, 0x65, 0xBA, 0x2F, 0xC0, 0xDE, 0x1C, 0xFD, 0x4D, 0x92, 0x75, 0x06, 0x8A,
	0xB2, 0xE6, 0x0E, 0x1F, 0x62, 0xD4, 0xA8, 0x96, 0xF9, 0xC5, 0x25, 0x59, 0x84, 0x72, 0x39, 0x4C,
	0x5E, 0x78, 0x38, 0x8C, 0xC1, 0xA5, 0xE2, 0x61, 0xB3, 0x21, 0x9C, 0x1E, 0x43, 0xC7, 0xFC, 0x04,
	0x51, 0x99, 0x6D, 0x0D, 0xFA, 0xDF, 0x7E, 0x24, 0x3B, 0xAB, 0xCE, 0x11, 0x8F, 0x4E, 0xB7, 0xEB,
	0x3C, 0x81, 0x94, 0xF7, 0xB9, 0x13, 0x2C, 0xD3, 0xE7, 0x6E, 0xC4, 0x03, 0x56, 0x44, 0x7F, 0xA9,
	0x2A, 0xBB, 0xC1, 0x53, 0xDC, 0x0B, 0x9D, 0x6C, 0x31, 0x74, 0xF6, 0x46, 0xAC, 0x89, 0x14, 0xE1,
	0x16, 0x3A, 0x69, 0x09, 0x70, 0xB6, 0xC0, 0xED, 0xCC, 0x42, 0x98, 0xA4, 0x28, 0x5C, 0xF8, 0x86,
};
static const uint8_t	Whirlpool_E_SBox[] =  { 0x1, 0xB, 0x9, 0xC, 0xD, 0x6, 0xF, 0x3, 0xE, 0x8, 0x7, 0x4, 0xA, 0x2, 0x5, 0x0 };
static const uint8_t	Whirlpool_E1_SBox[] = { 0xF, 0x0, 0xD, 0x7, 0xB, 0xE, 0x5, 0xA, 0x9, 0x2, 0xC, 0x1, 0x3, 0x4, 0x8, 0x6 };
static const uint8_t	Whirlpool_R_SBox[] =  { 0x7, 0xC, 0xB, 0xD, 0xE, 0x4, 0x9, 0xF, 0x6, 0x3, 0x8, 0xA, 0x2, 0x5, 0x1, 0x0 };

static void		whirlpool_chunk(uint32_t *digest, uint32_t *input) {
	// Round keys
	uint32_t	rc[11][8] = {
		[0] = { 0x18, 0x23, 0xC6, 0xE8, 0x87, 0xB8, 0x01, 0x4F }
	};

	(void)digest;
	(void)input;
}

string_t	whirlpool_hash(const string_t *input) {
	uint32_t	*digest = malloc(WHIRLPOOL_DIGEST_LENGTH);

	// 1 extra byte for the separator and 8 for the 64 bits length
	size_t		byte_count = input->len + 9;

	uint8_t		buffer[64];

	for (size_t i = 0; i < byte_count; i += 64) {
		init_chunk_buffer(buffer, input, i, WHIRLPOOL_PADDING_OPT);
		whirlpool_chunk(digest, (uint32_t *)buffer);
	}

	return ((string_t){ .ptr = (uint8_t *)digest, .len = WHIRLPOOL_DIGEST_LENGTH });
}