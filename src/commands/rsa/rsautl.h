#pragma once

#include "lib/rsa/rsa_utils.h"
#include "lib/rsa/rsa_key_serialization.h"
#include "lib/readfile.h"

void	rsautl_print(int fd, const string_t *s, const input_t *curr, const arguments_t *args) {
	(void)curr;

	if (get_flag(args->flags, "-hexdump")->present) {
		hexdump_with_preview(fd, s);
	} else {
		write(fd, s->ptr, s->len);
	}
}

string_t	rsautl_readfile(const char *inkey_filename) {
	if (!inkey_filename) {
		DIE("Missing -inkey argument");
	}
	string_t inkey;
	if (readfile(inkey_filename, &inkey) != 0) {
		DIE("Failed to load key", inkey_filename);
	}
	return inkey;
}

string_t	rsautl(const string_t *in, const arguments_t *args) {
	(void)in;

	string_t inkey = rsautl_readfile(get_flag(args->flags, "-inkey")->argument);
	uint64_t in_value = 0;
	uint64_t out_value;
	string_t out = (string_t){0};

	if (in->len > 8) {
		DIE("Input too long");
	}

	if (in->len < 8) {
		DIE("Input too short");
	}

	for (size_t i = 0; i < in->len; i++) {
		in_value <<= 8;
		in_value |= in->ptr[i];
	}

	if (get_flag(args->flags, "-decrypt")->present) {
		if (get_flag(args->flags, "-pubin")->present) {
			DIE("A private key is required for decryption");
		}

		rsa_private_key_t *key = deserialize_rsa_private_key(inkey);

		if (key == NULL) {
			goto cleanup;
		}

		out_value = power(in_value, key->privateExponent, key->modulus);

		free(key);

	} else {
		rsa_public_key_t *key;

		if (get_flag(args->flags, "-pubin")->present) {
			key = deserialize_public_key(inkey);
		} else {
			key = deserialize_rsa_private_key_as_public_key(inkey);
		}

		if (key == NULL) {
			goto cleanup;
		}

		if (in_value >= key->modulus) {
			ERROR("Data too large for key size");
			free(key);
			goto cleanup;
		}

		out_value = power(in_value, key->publicExponent, key->modulus);

		free(key);
	}

	out = string_new_initialized(8, 0);

	for (size_t i = 0; i < 8; i++) {
		out.ptr[7 - i] = out_value & 0xff;
		out_value >>= 8;
	}

cleanup:
	free(inkey.ptr);
	return out;
}
