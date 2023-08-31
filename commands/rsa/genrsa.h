#pragma once

#include "lib/rsa/rsa_key_serialization.h"
#include "lib/prime.h"


#define RSA_KEY_SIZE 64
#define RSA_E 65537

// Online RSA private key parser
// https://lapo.it/asn1js/

string_t genrsa() {
	uint64_t prime1 = random_prime(RSA_KEY_SIZE / 2);
	uint64_t prime2 = random_prime(RSA_KEY_SIZE / 2);

	uint64_t phi = (prime1 - 1) * (prime2 - 1);
	uint64_t e = RSA_E;
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

	return serialize_rsa_private_key(&key);
}