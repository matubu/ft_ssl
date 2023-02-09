#pragma once

#include "lib/utils.h"
#include "lib/dump.h"

#include "commands/sha256.h"

#define BLOCK_SIZE 64
#define HASH_SIZE 32

// Documentation:
// https://www.rfc-editor.org/rfc/rfc8018

string_t computeBlockSizedKey(string_t key) {
	key = string_dup(key);

	if (key.len > BLOCK_SIZE)
		string_apply_inplace(&key, sha256_hash);

	if (key.len < BLOCK_SIZE) {
		string_t padded = string_new_initialized(BLOCK_SIZE, 0);

		for (size_t i = 0; i < key.len; ++i)
			padded.ptr[i] = key.ptr[i];

		free(key.ptr);
		key = padded;
	}

	return key;
}

string_t hmac256(string_t *key, const string_t *message) {
	string_t block_sized_key = computeBlockSizedKey(*key);

	string_t o_key_pad = string_new(BLOCK_SIZE + HASH_SIZE);
	string_t i_key_pad = string_new(BLOCK_SIZE + message->len);

	for (size_t i = 0; i < BLOCK_SIZE; ++i) {
		o_key_pad.ptr[i] = block_sized_key.ptr[i] ^ 0x5c;
		i_key_pad.ptr[i] = block_sized_key.ptr[i] ^ 0x36;
	}

	for (size_t i = 0; i < message->len; ++i)
		i_key_pad.ptr[BLOCK_SIZE + i] = message->ptr[i];

	string_t inner_hash = sha256_hash(&i_key_pad);

	for (size_t i = 0; i < inner_hash.len; ++i) {
		o_key_pad.ptr[BLOCK_SIZE + i]
			= inner_hash.ptr[i];
	}
	
	string_t outer_hash = sha256_hash(&o_key_pad);

	free(block_sized_key.ptr);
	free(o_key_pad.ptr);
	free(i_key_pad.ptr);

	return outer_hash;
}

string_t pbkdf2(string_t password, string_t salt, uint64_t iterations, uint64_t keylen) {
	string_t t = string_new(HASH_SIZE);

	uint8_t i[4] = {0, 0, 0, 1};
	string_t join = string_join(salt, (string_t){ .ptr = (uint8_t *)&i, .len = sizeof(i) }, JOIN_FREE_NONE);
	string_t u_curr = hmac256(&password, &join);

	free(join.ptr);

	for (size_t k = 0; k < u_curr.len; ++k)
		t.ptr[k] = u_curr.ptr[k];

	for (uint64_t j = 1; j < iterations; ++j) {
		string_t u_next = hmac256(&password, &u_curr);
		free(u_curr.ptr);
		u_curr = u_next;

		for (size_t k = 0; k < u_curr.len; ++k)
			t.ptr[k] ^= u_curr.ptr[k];
	}
	free(u_curr.ptr);

	t.len = keylen;

	return t;
}