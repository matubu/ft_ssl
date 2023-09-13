#pragma once

#include <stdint.h>

#include "lib/prime.h"


#define RSA_KEY_SIZE 64
#define RSA_E 65537


typedef struct {
	uint64_t version;
	uint64_t modulus;          // n
	uint64_t publicExponent;   // e
	uint64_t privateExponent;  // d
	uint64_t prime1;           // p
	uint64_t prime2;           // q
	uint64_t exponent1;
	uint64_t exponent2;
	uint64_t coefficient;
} rsa_private_key_t;

typedef struct {
	uint64_t modulus;
	uint64_t publicExponent;
} rsa_public_key_t;





rsa_private_key_t rsa_private_key_from_primes(uint64_t prime1, uint64_t prime2, uint64_t e) {
	uint64_t phi = (prime1 - 1) * (prime2 - 1);
	uint64_t d = mul_inv(e, phi);
	uint64_t d1 = d % (prime1 - 1);
	uint64_t d2 = d % (prime2 - 1);
	uint64_t qinv = mul_inv(prime2, prime1);

	rsa_private_key_t key = {
		.version = 0,
		.modulus = prime1 * prime2,
		.publicExponent = e,
		.privateExponent = d,
		.prime1 = prime1,
		.prime2 = prime2,
		.exponent1 = d1,
		.exponent2 = d2,
		.coefficient = qinv
	};

	return (key);
}