#pragma once

#include "lib/utils.h"

// Documentation:
// https://en.wikipedia.org/wiki/SHA-2

#define SHA512_CHUNK_OPT (&(chunk_opt){ \
	.chunk_byte_count = 1024 / 8, \
	.length_class = 128, \
	.length_byte_order = BIG_ENDIAN, \
})
#define SHA512_DIGEST_LENGTH (8 * sizeof(uint64_t))
#define SHA384_DIGEST_LENGTH (6 * sizeof(uint64_t))

static const uint64_t	sha_primes_cube_root64[80] = {
	0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538, 
	0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe, 
	0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 
	0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 
	0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab, 
	0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725, 
	0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 
	0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b, 
	0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218, 
	0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 
	0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 
	0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec, 
	0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c, 
	0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6, 
	0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 
	0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

static void		sha512_chunk(uint64_t *digest, uint64_t *input) {
	uint64_t	w[80];
	// Copy the chunk 16 words
	for (size_t i = 0; i < 16; ++i) {
		w[i] = uint64_endianess(input[i], BIG_ENDIAN);
	}
	// Extend the first 16 words
	for (size_t i = 16; i < 80; ++i) {
		uint64_t s0 = rightrotate64(w[i-15], 1) ^ rightrotate64(w[i-15], 8) ^ (w[i-15] >> 7);
		uint64_t s1 = rightrotate64(w[i-2], 19) ^ rightrotate64(w[i-2], 61) ^ (w[i-2] >> 6);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
	}

	uint64_t	a = digest[0];
	uint64_t	b = digest[1];
	uint64_t	c = digest[2];
	uint64_t	d = digest[3];
	uint64_t	e = digest[4];
	uint64_t	f = digest[5];
	uint64_t	g = digest[6];
	uint64_t	h = digest[7];

	for (size_t i = 0; i < 80; ++i) {
		uint64_t	S1 = rightrotate64(e, 14) ^ rightrotate64(e, 18) ^ rightrotate64(e, 41);
		uint64_t	ch = (e & f) ^ (~e & g);
		uint64_t	temp1 = h + S1 + ch + sha_primes_cube_root64[i] + w[i];
		uint64_t	S0 = rightrotate64(a, 28) ^ rightrotate64(a, 34) ^ rightrotate64(a, 39);
		uint64_t	maj = (a & b) ^ (a & c) ^ (b & c);
		uint64_t	temp2 = S0 + maj;

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

string_t	sha384_hash(const string_t *input) {
	uint64_t	*digest = malloc(SHA512_DIGEST_LENGTH);
	digest[0] = 0xcbbb9d5dc1059ed8;
	digest[1] = 0x629a292a367cd507;
	digest[2] = 0x9159015a3070dd17;
	digest[3] = 0x152fecd8f70e5939;
	digest[4] = 0x67332667ffc00b31;
	digest[5] = 0x8eb44a8768581511;
	digest[6] = 0xdb0c2e0d64f98fa7;
	digest[7] = 0x47b5481dbefa4fa4;

	LOOP_OVER_CHUNKS(input, SHA512_CHUNK_OPT, {
		sha512_chunk(digest, (uint64_t *)GET_CHUNK_BUFFER());
	});

	for (size_t i = 0; i < 6; ++i) {
		digest[i] = uint64_endianess(digest[i], BIG_ENDIAN);
	}
	return ((string_t){ .ptr = (uint8_t *)digest, .len = SHA384_DIGEST_LENGTH });
}

string_t	sha512_hash(const string_t *input) {
	uint64_t	*digest = malloc(SHA512_DIGEST_LENGTH);
	digest[0] = 0x6a09e667f3bcc908;
	digest[1] = 0xbb67ae8584caa73b;
	digest[2] = 0x3c6ef372fe94f82b;
	digest[3] = 0xa54ff53a5f1d36f1;
	digest[4] = 0x510e527fade682d1;
	digest[5] = 0x9b05688c2b3e6c1f;
	digest[6] = 0x1f83d9abfb41bd6b;
	digest[7] = 0x5be0cd19137e2179;

	LOOP_OVER_CHUNKS(input, SHA512_CHUNK_OPT, {
		sha512_chunk(digest, (uint64_t *)GET_CHUNK_BUFFER());
	});

	for (size_t i = 0; i < 8; ++i) {
		digest[i] = uint64_endianess(digest[i], BIG_ENDIAN);
	}
	return ((string_t){ .ptr = (uint8_t *)digest, .len = SHA512_DIGEST_LENGTH });
}