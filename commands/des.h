#pragma once

#include "lib/utils.h"
#include "lib/pbkdf2.h"

#include "commands/base64.h"

#include "ft_ssl.h"

// Documentation:
// https://en.wikipedia.org/wiki/Data_Encryption_Standard
// https://en.wikipedia.org/wiki/DES_supplementary_material

const uint8_t	des_sbox[8][64] = {
	{	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
		 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 },
	{	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		 13, 8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 },
	{	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 },
	{	 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 },
	{	 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 },
	{	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 },
	{	 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 },
	{	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }
};

const uint8_t	des_ip_pbox[64] = {
	58, 50, 42, 34, 26, 18, 10,	2,
	60, 52, 44, 36, 28, 20, 12,	4,
	62, 54, 46, 38, 30, 22, 14,	6,
	64, 56, 48, 40, 32, 24, 16,	8,
	57, 49, 41, 33, 25, 17, 9 ,	1,
	59, 51, 43, 35, 27, 19, 11,	3,
	61, 53, 45, 37, 29, 21, 13,	5,
	63, 55, 47, 39, 31, 23, 15,	7, 
};

const uint8_t	des_inverse_ip_pbox[64] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9 , 49, 17, 57, 25, 
};

const uint8_t	des_p_pbox[32] = {
	16, 7 , 20, 21,29, 12, 28, 17,
	1 , 15, 23, 26, 5, 18, 31, 10,
	2 , 8 , 24, 14,32, 27, 3 , 9,
	19, 13, 30, 6 ,22, 11, 4 , 25, 
};

const uint8_t	des_pc_1[] = {
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12, 4
};

const uint8_t	des_pc_2[] = {
	14, 17, 11, 24, 1,  5, 3,  28, 15, 6,  21, 10,
	23, 19, 12, 4, 26, 8, 16, 7,  27, 20, 13, 2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

const uint8_t	des_rotation_table[] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

const uint8_t des_exp[48] = {
	32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
};

static inline uint64_t	permute(uint64_t n, const uint8_t *pbox, size_t in_size, size_t out_size) {
	uint64_t result = 0;

	for (size_t i = 0; i < out_size; ++i) {
		result |= ((n >> (in_size - pbox[i])) & 1) << (out_size - i - 1);
	}

	return result;
}

uint32_t	des_feistel_f_function(uint32_t block, uint64_t subkey) {
	// Step 1: Expansion permutation
	uint64_t	expanded_block = permute(block, des_exp, 32, 48);

	// Step 2: Key mixing
	uint64_t	mixed_block = expanded_block ^ subkey;

	// Step 3: S-box substitution
	uint32_t	substituted_block = 0;
	for (size_t i = 0; i < 8; ++i) {
		int idx = 32 * (mixed_block >> (47 - i * 6) & 1)
				+ 16 * (mixed_block >> (47 - i * 6 - 5) & 1)
				+ 8 * (mixed_block >> (47 - i * 6 - 1) & 1) 
				+ 4 * (mixed_block >> (47 - i * 6 - 2) & 1) 
				+ 2 * (mixed_block >> (47 - i * 6 - 3) & 1) 
				+ 1 * (mixed_block >> (47 - i * 6 - 4) & 1);

		substituted_block
			|= des_sbox[i][idx] << ((7-i) * 4);
	}

	// Step 4: Permutation
	uint32_t	permutated_block = permute(substituted_block, des_p_pbox, 32, 32);

	return permutated_block;
}

uint64_t	des_feistel(uint64_t block, uint64_t *subkeys) {
	block = permute(block, des_ip_pbox, 64, 64);

	for (size_t i = 0; i < 16; ++i) {
		uint32_t	left = block >> 32;
		uint32_t	right = block & 0xFFFFFFFF;

		uint32_t	f = des_feistel_f_function(right, subkeys[i]);

		block = ((uint64_t)right << 32) | (left ^ f);
	}

	block = ((block & 0xFFFFFFFF) << 32) | (block >> 32);

	block = permute(block, des_inverse_ip_pbox, 64, 64);

	return block;
}

void key_schedule(uint64_t *subkeys, uint64_t key, int reverse) {
	key = permute(key, des_pc_1, 64, 56);

	uint64_t	left = (key >> 28) & 0xFFFFFFF;
	uint64_t	right = key & 0xFFFFFFF;

	for (size_t i = 0; i < 16; ++i) {
		right = leftrotate(right, des_rotation_table[i], 28);
		left = leftrotate(left, des_rotation_table[i], 28);

		uint64_t concat = (left << 28) | right;
		subkeys[i] = permute(concat, des_pc_2, 56, 48);
	}

	if (reverse) {
		for (size_t i = 0; i < 8; ++i) {
			uint64_t	tmp = subkeys[i];
			subkeys[i] = subkeys[15 - i];
			subkeys[15 - i] = tmp;
		}
	}
}

#define FOR_DES_BLOCK(output) for (size_t i = 0; i < (output).len; i += 8)
#define GET_DES_BLOCK_INDEX() (i / 8)
#define GET_DES_BLOCK(input) ({ \
		char block[8]; \
		for (size_t j = 0; j < 8; ++j) { \
			if (i + j < (input)->len) \
				block[7 - j] = (input)->ptr[i + j]; \
			else \
				block[7 - j] = (i + 8) - (input)->len; \
		} \
		*(uint64_t *)block; \
})

uint64_t	parse_hex_digit(char c) {
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	return 0;
}

uint64_t	parse_hex(const char *s) {
	uint64_t	n = 0;

	for (size_t i = 0; i < 16; ++i) {
		n <<= 4;
		if (*s)
			n |= parse_hex_digit(*s++);
	}
	return n;
}

const char	*des_get_password(const arguments_t *args) {
	if (args->flags['p'].argument)
		return args->flags['p'].argument;

	char *pass = getpass("Enter password: ");

	if (pass == NULL)
		DIE("getpass() failed");

	return pass;
}

uint64_t	des_get_salt(string_t *input, const arguments_t *args) {
	if (args->flags['d'].present && input->len >= 16
		&& string_starts_with(*input, string_from_ptr("Salted__"))) {
		uint64_t salt = *(uint64_t *)(input->ptr + 8);
		string_t cleared_input = string_dup((string_t){ .ptr = input->ptr + 16, .len = input->len - 16 });

		free(input->ptr);
		*input = cleared_input;

		return salt;
	}

	if (args->flags['s'].present)
		return uint64_endianess(parse_hex(args->flags['s'].argument), BIG_ENDIAN);

	srand(time(NULL) ^ clock());

	uint64_t salt = 0;

	for (size_t i = 0; i < 64; ++i)
		salt ^= ((uint64_t)rand()) << i;

	return salt;
}

uint64_t	*des_get_key(string_t *input, const arguments_t *args, string_t *salt_str) {
	static uint64_t keys[2];

	keys[0] = 0;
	keys[1] = 0;

	if (args->flags['k'].present) {
		keys[0] = parse_hex(args->flags['k'].argument);
		if (slen((uint8_t *)args->flags['k'].argument) > 16)
			keys[1] = parse_hex(args->flags['k'].argument + 16);
		return keys;
	}

	string_t pass = string_from_ptr(des_get_password(args));
	uint64_t salt = des_get_salt(input, args);

	string_t hash = pbkdf2(pass, (string_t){ .len = sizeof(salt), .ptr = (uint8_t *)&salt }, 10000, 16);

	keys[0] = uint64_endianess(*(uint64_t *)hash.ptr, BIG_ENDIAN);
	keys[1] = uint64_endianess(*(uint64_t *)(hash.ptr + 8), BIG_ENDIAN);

	*salt_str = string_join(
		string_from_ptr("Salted__"),
		(string_t){ .len = sizeof(salt), .ptr = (uint8_t *)&salt },
		JOIN_FREE_NONE
	);

	free(hash.ptr);

	return keys;
}

typedef struct {
	string_t	salt_str;
	string_t	output;
	uint64_t	*keys;
	uint64_t 	subkeys[16];
}	des_t;

string_t	des_postprocessing(des_t *des, const arguments_t *args) {
	if (!args->flags['d'].present) {
		des->output = string_join(des->salt_str, des->output, JOIN_FREE_B);
	}
	free(des->salt_str.ptr);

	if (args->flags['d'].present && des->output.len > 0) {
		size_t padding = des->output.ptr[des->output.len - 1];

		if (padding > 8 || padding > des->output.len)
			DIE("Invalid padding");

		des->output.len -= padding;
	}

	if (args->flags['a'].present && !args->flags['d'].present)
		string_apply_inplace(&des->output, base64_encode);

	return des->output;
}

des_t	des_init(string_t *input, const arguments_t *args) {
	des_t des;

	if (args->flags['a'].present && args->flags['d'].present)
		string_apply_inplace((string_t *)input, base64_decode);

	des.salt_str = (string_t){ .len = 0, .ptr = NULL };
	des.keys = des_get_key((string_t *)input, args, &des.salt_str);
	key_schedule(des.subkeys, des.keys[0], args->flags['d'].present);

	des.output = string_new((input->len / 8 + !(args->flags['d'].present)) * 8);

	return des;
}

string_t	des_ecb_cipher(const string_t *input, const arguments_t *args) {
	des_t des = des_init((string_t *)input, args);

	FOR_DES_BLOCK(des.output) {
		uint64_t block = des_feistel(GET_DES_BLOCK(input), des.subkeys);

		((uint64_t *)des.output.ptr)[GET_DES_BLOCK_INDEX()] = uint64_endianess(block, BIG_ENDIAN);
	}

	return des_postprocessing(&des, args);
}

string_t	des_cbc_cipher(const string_t *input, const arguments_t *args) {
	des_t des = des_init((string_t *)input, args);

	if (!args->flags['v'].argument) {
		DIE("No IV specified");
	}

	uint64_t iv = parse_hex(args->flags['v'].argument);
	uint64_t xor;
	uint64_t xor_next = iv;

	FOR_DES_BLOCK(des.output) {
		uint64_t in = GET_DES_BLOCK(input);

		xor = xor_next;

		if (!(args->flags['d'].present))
			in ^= xor;
		else
			xor_next = in;

		uint64_t block = des_feistel(in, des.subkeys);

		if (args->flags['d'].present)
			block ^= xor;
		else
			xor_next = block;

		((uint64_t *)des.output.ptr)[GET_DES_BLOCK_INDEX()] = uint64_endianess(block, BIG_ENDIAN);
	}

	return des_postprocessing(&des, args);
}

string_t	des_ede_cipher(const string_t *input, const arguments_t *args) {
	des_t des = des_init((string_t *)input, args);

	uint64_t subkeys2[16];

	key_schedule(subkeys2, des.keys[1], !args->flags['d'].present);

	FOR_DES_BLOCK(des.output) {
		uint64_t block = des_feistel(GET_DES_BLOCK(input), des.subkeys);
		block = des_feistel(block, subkeys2);
		block = des_feistel(block, des.subkeys);

		((uint64_t *)des.output.ptr)[GET_DES_BLOCK_INDEX()] = uint64_endianess(block, BIG_ENDIAN);
	}

	return des_postprocessing(&des, args);
}