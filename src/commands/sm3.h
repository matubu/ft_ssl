#pragma once

#include "lib/utils.h"

// Documentation:
// https://www.researchgate.net/figure/Padding-and-parsing-on-different-lengths-of-messages_fig1_335848054

#define SM3_CHUNK_OPT (&(chunk_opt){ \
	.chunk_byte_count = 512 / 8, \
	.length_class = 64, \
	.length_byte_order = BIG_ENDIAN, \
})
#define SM3_DIGEST_LENGTH (8 * sizeof(uint32_t))

static uint32_t	sm3_p0(uint32_t x) {
	return (x ^ leftrotate32(x, 9) ^ leftrotate32(x, 17));
}

static uint32_t	sm3_p1(uint32_t x) {
	return (x ^ leftrotate32(x, 15) ^ leftrotate32(x, 23));
}

#define SM3_FF(j, a, b, c) (i < 16 ? (a ^ b ^ c) : (a & b) | (a & c) | (b & c))
#define SM3_GG(j, a, b, c) (i < 16 ? (a ^ b ^ c) : (a & b) | (~a & c))

static void		sm3_chunk(uint32_t *digest, uint32_t *input) {
	uint32_t	w[68];
	// Copy the chunk (16 words)
	for (size_t i = 0; i < 16; ++i) {
		w[i] = uint32_endianess(input[i], BIG_ENDIAN);
	}
	// Extend the first 16 words into the remaining 48 words w[16..63]
	for (size_t i = 16; i < 68; ++i) {
		w[i] = sm3_p1(w[i - 16] ^ w[i - 9] ^ leftrotate32(w[i - 3], 15)) ^ leftrotate32(w[i - 13], 7) ^ w[i - 6];
	}
	// Permute
	uint32_t	w_[64];
	for (size_t i = 0; i < 64; ++i) {
		w_[i] = w[i] ^ w[i + 4];
	}

	uint32_t	a = digest[0];
	uint32_t	b = digest[1];
	uint32_t	c = digest[2];
	uint32_t	d = digest[3];
	uint32_t	e = digest[4];
	uint32_t	f = digest[5];
	uint32_t	g = digest[6];
	uint32_t	h = digest[7];

	for (size_t i = 0; i < 64; ++i) {
		uint32_t	ss1 = leftrotate32(leftrotate32(a, 12) + e + leftrotate32(i < 16 ? 0x79cc4519 : 0x7a879d8a, i % 32), 7);
		uint32_t	ss2 = ss1 ^ leftrotate32(a, 12);
		uint32_t	tt1 = SM3_FF(i, a, b, c) + d + ss2 + w_[i];
		uint32_t	tt2 = SM3_GG(i, e, f, g) + h + ss1 + w[i];

		d = c;
		c = leftrotate32(b, 9);
		b = a;
		a = tt1;
		h = g;
		g = leftrotate32(f, 19);
		f = e;
		e = sm3_p0(tt2);
	}

	digest[0] ^= a;
	digest[1] ^= b;
	digest[2] ^= c;
	digest[3] ^= d;
	digest[4] ^= e;
	digest[5] ^= f;
	digest[6] ^= g;
	digest[7] ^= h;
}

string_t	sm3_hash(const string_t *input) {
	uint32_t	*digest = malloc(SM3_DIGEST_LENGTH);
	digest[0] = 0x7380166f;
	digest[1] = 0x4914b2b9;
	digest[2] = 0x172442d7;
	digest[3] = 0xda8a0600;
	digest[4] = 0xa96f30bc;
	digest[5] = 0x163138aa;
	digest[6] = 0xe38dee4d;
	digest[7] = 0xb0fb0e4e;

	LOOP_OVER_CHUNKS(input, SM3_CHUNK_OPT, {
		sm3_chunk(digest, (uint32_t *)GET_CHUNK_BUFFER());
	});

	for (size_t i = 0; i < 8; ++i) {
		digest[i] = uint32_endianess(digest[i], BIG_ENDIAN);
	}
	return ((string_t){ .ptr = (uint8_t *)digest, .len = SM3_DIGEST_LENGTH });
}