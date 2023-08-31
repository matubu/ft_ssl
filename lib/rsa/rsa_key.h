#pragma once

#include <stdint.h>

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
