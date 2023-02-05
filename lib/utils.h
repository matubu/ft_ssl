#pragma once

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "endianess.h"
#include "lib/string.h"
#include "lib/malloc.h"

uint64_t	leftrotate(uint64_t n, uint8_t offset, size_t length) {
	return ((n << offset) | (n >> (length - offset))) & ((1 << length) - 1);
}
uint32_t	leftrotate32(uint32_t n, uint8_t offset) {
	return ((n << offset) | (n >> (32 - offset)));
}
uint64_t	leftrotate64(uint64_t n, uint8_t offset) {
	return ((n << offset) | (n >> (64 - offset)));
}

uint64_t	rightrotate(uint64_t n, uint8_t offset, size_t length) {
	return ((n >> offset) | (n << (length - offset))) & ((1 << length) - 1);
}
uint32_t	rightrotate32(uint32_t n, uint8_t offset) {
	return ((n >> offset) | (n << (32 - offset)));
}
uint64_t	rightrotate64(uint64_t n, uint8_t offset) {
	return ((n >> offset) | (n << (64 - offset)));
}

typedef struct {
	size_t	chunk_byte_count;
	size_t	length_class;      // either 64 or 128 (32 not supported)
	int		length_byte_order; // either LITTLE or BIG endian
}	chunk_opt;
// Note: does not support 128 Little
void		init_chunk_buffer(uint8_t *buffer, const string_t *input, size_t i, const chunk_opt *opt) {
	size_t j = 0;
	
	while (j < opt->chunk_byte_count && i < input->len) {
		// TODO need to change depending on endianess ?
		buffer[j] = input->ptr[i];
		++j;
		++i;
	}

	if (j >= opt->chunk_byte_count)
		return ;

	buffer[j++] = 0x80;

	while (j < opt->chunk_byte_count && j != opt->chunk_byte_count - (opt->length_class / 8)) {
		buffer[j++] = 0;
	}

	if (j >= opt->chunk_byte_count)
		return ;

	while (j != opt->chunk_byte_count - sizeof(uint64_t)) {
		buffer[j++] = 0;
	}

	*(uint64_t *)&buffer[j] = uint64_endianess(input->len * 8, opt->length_byte_order);
}


#define LOOP_OVER_CHUNKS(input, opt, callback) { \
	size_t		byte_count = input->len + 1 + opt->length_class / 8; \
	uint8_t		buffer[opt->chunk_byte_count]; \
 \
	for (size_t i = 0; i < byte_count; i += opt->chunk_byte_count) { \
		init_chunk_buffer(buffer, input, i, opt); \
		callback; \
	} \
}
#define GET_CHUNK_BUFFER() buffer

// const uint8_t	AESSBox[] = {
// 	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
// 	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
// 	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
// 	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
// 	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
// 	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
// 	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
// 	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
// 	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
// 	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
// 	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
// 	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
// 	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
// 	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
// 	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
// 	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
// };

uint32_t	SubWord(uint32_t n, const uint8_t *s_box) {
	uint8_t	*ptr = (uint8_t *)&n;

	ptr[0] = s_box[ptr[0]];
	ptr[1] = s_box[ptr[1]];
	ptr[2] = s_box[ptr[2]];
	ptr[3] = s_box[ptr[3]];

	return (n);
}

void	SubBytes(uint32_t *ptr, const uint8_t *s_box) {
	for (uint32_t i = 0; i < 16; ++i) {
		((uint8_t *)ptr)[i] = s_box[((uint8_t *)ptr)[i]];
	}
}

// void	AESShiftRows(uint32_t *ptr) {
// 	ptr[1] = ((ptr[1] & 0x00FFFFFF) <<  8) | ((ptr[1] & 0xFF000000) >> 24);
// 	ptr[2] = ((ptr[2] & 0x0000FFFF) << 16) | ((ptr[2] & 0xFFFF0000) >> 16);
// 	ptr[3] = ((ptr[3] & 0x000000FF) << 24) | ((ptr[3] & 0xFFFFFF00) >>  8);
// }

uint8_t	MultiplyBy2(uint8_t n) {
	uint8_t res = n << 1;
	if ((n & 128) != 0) {
		res ^= 0x1b;
	}
	return (res);
}

uint8_t	MultiplyBy3(uint8_t n) {
	return (MultiplyBy2(n) ^ n);
}

// void	AESMixColumn(uint8_t *a, uint8_t *b, uint8_t *c, uint8_t *d) {
// 	uint8_t	a_tmp = *a;
// 	uint8_t	b_tmp = *b;
// 	uint8_t	c_tmp = *c;
// 	uint8_t	d_tmp = *d;

// 	*a = MultiplyBy2(a_tmp) ^ MultiplyBy3(b_tmp) ^ c_tmp ^ d_tmp;
// 	*b = MultiplyBy2(b_tmp) ^ MultiplyBy3(c_tmp) ^ d_tmp ^ a_tmp;
// 	*c = MultiplyBy2(c_tmp) ^ MultiplyBy3(d_tmp) ^ a_tmp ^ b_tmp;
// 	*d = MultiplyBy2(d_tmp) ^ MultiplyBy3(a_tmp) ^ b_tmp ^ c_tmp;
// }

// 0001 -> (0010 ^ 0110 ^ 0100 ^ 1000)
// 0010
// 0100
// 1000

// void	AESMixColumns(uint32_t *ptr) {
// 	for (uint32_t i = 0; i < 4; ++i) {
// 		MixColumn(
// 			(uint8_t *)(ptr + 0) + i,
// 			(uint8_t *)(ptr + 1) + i,
// 			(uint8_t *)(ptr + 2) + i,
// 			(uint8_t *)(ptr + 3) + i
// 		);
// 	}
// }

void	AddRoundKey(uint32_t *ptr, uint32_t *key) {
	ptr[0] ^= key[0];
	ptr[1] ^= key[1];
	ptr[2] ^= key[2];
	ptr[3] ^= key[3];
}
