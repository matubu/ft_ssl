#pragma once

#include "lib/rsa/rsa_key_serialization.h"
#include "lib/rsa/rsa_utils.h"
#include "lib/prime.h"

#define RSA_MIN_DIFF ((uint64_t)1 << 30)

// Online RSA private key parser
// https://lapo.it/asn1js/

uint64_t abs_diff(uint64_t a, uint64_t b) {
	return a > b ? a - b : b - a;
}

string_t genrsa() {
	uint64_t prime1 = random_prime(RSA_KEY_SIZE / 2);
	uint64_t prime2;
	do {
		prime2 = random_prime(RSA_KEY_SIZE / 2);
	} while (abs_diff(prime1, prime2) < RSA_MIN_DIFF);

	rsa_private_key_t key = rsa_private_key_from_primes(prime1, prime2, RSA_E);

	return serialize_rsa_private_key(&key);
}