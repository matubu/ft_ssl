#pragma once

#include "malloc.h"
#include "string.h"
#include "endianess.h"
#include "utils.h"

// command: openssl rmd160
// nice links:
// https://en.wikipedia.org/wiki/RIPEMD
// https://en.bitcoin.it/wiki/RIPEMD-160
// https://homes.esat.kuleuven.be/~bosselae/ripemd160/pdf/AB-9601/AB-9601.pdf

#define RMD160_PADDING_OPT (&(padding_opt){ \
	.chunk_byte_count = 64, \
	.length_byte_order = LITTLE_ENDIAN, \
})
#define RMD160_DIGEST_LENGTH (5 * sizeof(uint32_t))

void		rmd160_chunk(uint32_t *dgst, uint32_t *input) {
	uint32_t	la = dgst[0], lb = dgst[1], lc = dgst[2], ld = dgst[3], le = dgst[4];
	uint32_t	ra = dgst[0], rb = dgst[1], rc = dgst[2], rd = dgst[3], re = dgst[4];

	for (uint32_t j = 0; j < 80; ++j) {

	}

	uint32_t	t = dgst[1] ^ lc ^ rd;
	dgst[1] =       dgst[2] ^ ld ^ re;
	dgst[2] =       dgst[3] ^ le ^ ra;
	dgst[3] =       dgst[4] ^ la ^ rb;
	dgst[4] =       dgst[0] ^ lb ^ rc;
	dgst[0] = t;
}

string_t	rmd160_hash(const string_t *input) {
	uint32_t	*digest = malloc(RMD160_DIGEST_LENGTH);
	digest[0] = 0x67452301;
	digest[1] = 0xEFCDAB89;
	digest[2] = 0x98BADCFE;
	digest[3] = 0x10325476;
	digest[4] = 0xC3D2E1F0;

	// 1 extra byte for the separator and 8 for the 64 bits length
	size_t		byte_count = input->len + 9;

	uint8_t		buffer[64];

	for (size_t i = 0; i < byte_count; i += 64) {
		init_chunk_buffer(buffer, input, i, RMD160_PADDING_OPT);
		rmd160_chunk(digest, (uint32_t *)buffer);
	}

	return ((string_t){ .ptr = (uint8_t *)digest, .len = RMD160_DIGEST_LENGTH });
}