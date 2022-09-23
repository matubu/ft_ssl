#pragma once

#include "malloc.h"
#include "string.h"
#include "endianess.h"
#include "utils.h"

// command: openssl shake256
// nice links:
// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf
// https://en.wikipedia.org/wiki/SHA-3

#define SHAKE256_PADDING_OPT (&(padding_opt){ \
	.chunk_byte_count = 64, \
	.length_byte_order = BIG_ENDIAN, \
})
#define SHAKE256_DIGEST_LENGTH (8 * sizeof(uint32_t))

static void		shake256_chunk(uint32_t *digest, uint32_t *input) {
	(void)digest;
	(void)input;
}

string_t	shake256_hash(const string_t *input) {
	uint32_t	*digest = malloc(SHAKE256_DIGEST_LENGTH);

	// 1 extra byte for the separator and 8 for the 64 bits length
	size_t		byte_count = input->len + 9;

	uint8_t		buffer[64];

	for (size_t i = 0; i < byte_count; i += 64) {
		init_chunk_buffer(buffer, input, i, SHAKE256_PADDING_OPT);
		shake256_chunk(digest, (uint32_t *)buffer);
	}

	return ((string_t){ .ptr = (uint8_t *)digest, .len = SHAKE256_DIGEST_LENGTH });
}