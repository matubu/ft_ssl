#pragma once

#include "lib/ans1.h"
#include "lib/prime.h"

#include "commands/base64.h"

#define RSA_KEY_SIZE 64
#define RSA_E 65537

typedef struct {
	uint64_t version;
	uint64_t modulus;
	uint64_t publicExponent;
	uint64_t privateExponent;
	uint64_t prime1;
	uint64_t prime2;
	uint64_t exponent1;
	uint64_t exponent2;
	uint64_t coefficient;
} rsa_private_key_t;

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

	printf("prime1: %lu\n", prime1);
	printf("prime2: %lu\n", prime2);

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

	string_t ans1 = ans1_encode_integer_sequence(9, (uint64_t *)&key);

	string_t ans1_base64 = base64_encode(&ans1);

	free(ans1.ptr);

	return string_join(
		string_join(
			string_from_ptr("-----BEGIN RSA PRIVATE KEY-----\n"),
			ans1_base64,
			JOIN_FREE_B
		),
		string_from_ptr("\n-----END RSA PRIVATE KEY-----\n"),
		JOIN_FREE_A
	);
}