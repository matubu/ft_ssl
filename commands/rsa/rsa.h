#pragma once

#include "lib/rsa/rsa_utils.h"
#include "lib/rsa/rsa_key_serialization.h"

int	is_supported_format(const char *format) {
	if (!format) {
		return (1);
	}
	return (cmp(format, "PEM") == 0);
}

string_t	rsa(const string_t *in, const arguments_t *args) {
	if (!is_supported_format(get_flag(args->flags, "-inform")->argument)) {
		DIE("given inform format not supported");
	}

	if (!is_supported_format(get_flag(args->flags, "-outform")->argument)) {
		DIE("given outform format not supported");
	}

	string_t out = (string_t){ .ptr = NULL, .len = 0 };





	
	if (get_flag(args->flags, "-pubin")->present) {
		rsa_public_key_t *key = deserialize_public_key(*in);

		if (key == NULL) {
			return ((string_t){ .ptr = NULL, .len = 0 });
		}

		if (get_flag(args->flags, "-text")->present) {
			print_public_rsa_key(key);
		}

		out = serialize_public_key(key);

		free(key);
	} else {
		rsa_private_key_t *key = deserialize_rsa_private_key(*in);

		if (key == NULL) {
			return ((string_t){ .ptr = NULL, .len = 0 });
		}

		if (get_flag(args->flags, "-text")->present) {
			print_private_rsa_key(key);
		}

		if (get_flag(args->flags, "-pubout")->present) {
			rsa_public_key_t pub = rsa_private_to_public_key(key);
			out = serialize_public_key(&pub);
		} else {
			out = serialize_rsa_private_key(key);
		}

		free(key);
	}



	if (get_flag(args->flags, "-noout")->present) {
		free(out.ptr);
		return ((string_t){ .ptr = NULL, .len = 0 });
	}

	PUTS(1, "writing RSA key");
	return out;
}