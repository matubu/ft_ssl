#pragma once

#include "malloc.h"
#include "string.h"
#include "endianess.h"
#include "utils.h"

// command: openssl sha3-512
// nice links:
// https://summerschool-croatia.cs.ru.nl/2014/slides/SHA-3%20and%20permutation-based%20cryptography.pdf

#define SHA3_512_PADDING_OPT (&(padding_opt){ \
	.chunk_byte_count = 64, \
	.length_byte_order = BIG_ENDIAN, \
})
#define SHA3_512_DIGEST_LENGTH (8 * sizeof(uint32_t))

static void		sha3_512_chunk(uint32_t *digest, uint32_t *input) {
	(void)digest;
	(void)input;
}

string_t	sha3_512_hash(const string_t *input) {
	uint32_t	*digest = malloc(SHA3_512_DIGEST_LENGTH);

	// 1 extra byte for the separator and 8 for the 64 bits length
	size_t		byte_count = input->len + 9;

	uint8_t		buffer[64];

	for (size_t i = 0; i < byte_count; i += 64) {
		init_chunk_buffer(buffer, input, i, SHA3_512_PADDING_OPT);
		sha3_512_chunk(digest, (uint32_t *)buffer);
	}

	return ((string_t){ .ptr = (uint8_t *)digest, .len = SHA3_512_DIGEST_LENGTH });
}