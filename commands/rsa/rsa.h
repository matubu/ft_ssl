#pragma once

#include "lib/rsa/rsa_utils.h"
#include "lib/rsa/rsa_key_serialization.h"

int	is_supported_format(const char *format) {
	if (!format) {
		return (1);
	}
	return (cmp(format, "PEM") == 0);
}

// https://www.openssl.org/docs/man1.1.1/man3/RSA_check_key.html
void	check_rsa_key(const rsa_private_key_t *key) {
	if (!miller_rabin(key->prime1, PRIME_PRECISION)) {
		ERROR("rsa validate: prime1 is not prime");
	}

	if (!miller_rabin(key->prime2, PRIME_PRECISION)) {
		ERROR("rsa validate: prime2 is not prime");
	}

	if (key->modulus != key->prime1 * key->prime2) {
		ERROR("rsa validate: modulus != prime1 * prime2");
	}

	if ((uint128_t)key->privateExponent * key->publicExponent
			% ((key->prime1 - 1) * (key->prime2 - 1))
		!= 1) {
		ERROR("rsa validate: d*e != 1 mod (p-1*q-1)");
	}

	uint128_t exponent1 = key->privateExponent % (key->prime1 - 1);
	uint128_t exponent2 = key->privateExponent % (key->prime2 - 1);
	uint128_t coefficient = mul_inv(key->prime2, key->prime1);

	if (key->exponent1 != exponent1) {
		ERROR("rsa validate: exponent1 != d mod (p-1)");
	}

	if (key->exponent2 != exponent2) {
		ERROR("rsa validate: exponent2 != d mod (q-1)");
	}

	if (key->coefficient != coefficient) {
		ERROR("rsa validate: coefficient != q^-1 mod p");
	}
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
		if (get_flag(args->flags, "-modulus")->present) {
			print_rsa_field("Modulus", key->modulus);
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
		if (get_flag(args->flags, "-modulus")->present) {
			print_rsa_field("Modulus", key->modulus);
		}
		if (get_flag(args->flags, "-check")->present) {
			check_rsa_key(key);
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