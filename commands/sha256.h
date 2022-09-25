#pragma once

#include "utils.h"

// Documentation:
// https://en.wikipedia.org/wiki/SHA-2

#define SHA256_CHUNK_OPT (&(chunk_opt){ \
	.chunk_byte_count = 512 / 8, \
	.length_class = 64, \
	.length_byte_order = BIG_ENDIAN, \
})
#define SHA256_DIGEST_LENGTH (8 * sizeof(uint32_t))
#define SHA224_DIGEST_LENGTH (7 * sizeof(uint32_t))

static const uint32_t	sha_primes_cube_root32[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void		sha256_chunk(uint32_t *digest, uint32_t *input) {
	uint32_t	w[64];
	// Copy the chunk 16 words
	for (size_t i = 0; i < 16; ++i) {
		w[i] = uint32_endianess(input[i], BIG_ENDIAN);
	}
	// Extend the first 16 words
	for (size_t i = 16; i < 64; ++i) {
		uint32_t s0 = rightrotate32(w[i-15], 7) ^ rightrotate32(w[i-15], 18) ^ (w[i-15] >> 3);
		uint32_t s1 = rightrotate32(w[i-2], 17) ^ rightrotate32(w[i-2], 19) ^ (w[i-2] >> 10);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
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
		uint32_t	S1 = rightrotate32(e, 6) ^ rightrotate32(e, 11) ^ rightrotate32(e, 25);
		uint32_t	ch = (e & f) ^ (~e & g);
		uint32_t	temp1 = h + S1 + ch + sha_primes_cube_root32[i] + w[i];
		uint32_t	S0 = rightrotate32(a, 2) ^ rightrotate32(a, 13) ^ rightrotate32(a, 22);
		uint32_t	maj = (a & b) ^ (a & c) ^ (b & c);
		uint32_t	temp2 = S0 + maj;

		h = g;
		g = f;
		f = e;
		e = d + temp1;
		d = c;
		c = b;
		b = a;
		a = temp1 + temp2;
	}

	digest[0] += a;
	digest[1] += b;
	digest[2] += c;
	digest[3] += d;
	digest[4] += e;
	digest[5] += f;
	digest[6] += g;
	digest[7] += h;
}

string_t	sha224_hash(const string_t *input) {
	uint32_t	*digest = malloc(SHA256_DIGEST_LENGTH);
	digest[0] = 0xc1059ed8;
	digest[1] = 0x367cd507;
	digest[2] = 0x3070dd17;
	digest[3] = 0xf70e5939;
	digest[4] = 0xffc00b31;
	digest[5] = 0x68581511;
	digest[6] = 0x64f98fa7;
	digest[7] = 0xbefa4fa4;

	LOOP_OVER_CHUNKS(input, SHA256_CHUNK_OPT, {
		sha256_chunk(digest, (uint32_t *)GET_CHUNK_BUFFER());
	});

	for (size_t i = 0; i < 7; ++i) {
		digest[i] = uint32_endianess(digest[i], BIG_ENDIAN);
	}
	return ((string_t){ .ptr = (uint8_t *)digest, .len = SHA224_DIGEST_LENGTH });
}

string_t	sha256_hash(const string_t *input) {
	uint32_t	*digest = malloc(SHA256_DIGEST_LENGTH);
	digest[0] = 0x6a09e667;
	digest[1] = 0xbb67ae85;
	digest[2] = 0x3c6ef372;
	digest[3] = 0xa54ff53a;
	digest[4] = 0x510e527f;
	digest[5] = 0x9b05688c;
	digest[6] = 0x1f83d9ab;
	digest[7] = 0x5be0cd19;

	LOOP_OVER_CHUNKS(input, SHA256_CHUNK_OPT, {
		sha256_chunk(digest, (uint32_t *)GET_CHUNK_BUFFER());
	});

	for (size_t i = 0; i < 8; ++i) {
		digest[i] = uint32_endianess(digest[i], BIG_ENDIAN);
	}
	return ((string_t){ .ptr = (uint8_t *)digest, .len = SHA256_DIGEST_LENGTH });
}
