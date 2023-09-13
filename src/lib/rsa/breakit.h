#pragma once

#include "lib/rsa/rsa_key.h"
#include "lib/prime.h"
#include "lib/malloc.h"

#include <math.h>

int is_square(uint64_t n) {
	uint64_t a = ceil(sqrt(n));
	return a * a == n;
}

// https://en.wikipedia.org/wiki/Fermat's_factorization_method
uint64_t fermat_factor(uint64_t n) {
	uint64_t a = ceil(sqrt(n));
	uint64_t b2 = a * a - n;

	while (!is_square(b2)) {
		++a;
		b2 = a * a - n;
	}

	return a - sqrt(b2);
}

rsa_private_key_t *break_rsa_public_key(const rsa_public_key_t *public_key) {
	uint64_t n = public_key->modulus;

	uint64_t a = fermat_factor(n);
	uint64_t b = n / a;

	if (a * b != n) {
		ERROR("Fermat factorization failed");
		return NULL;
	}
	
	if (!miller_rabin(a, PRIME_PRECISION) || !miller_rabin(b, PRIME_PRECISION)) {
		ERROR("Fermat factorization failed");
		return NULL;
	}
	
	rsa_private_key_t private_key = rsa_private_key_from_primes(a, b, public_key->publicExponent);

	rsa_private_key_t *private_key_ptr = (rsa_private_key_t *)malloc(sizeof(rsa_private_key_t));
	*private_key_ptr = private_key;

	return private_key_ptr;
}


