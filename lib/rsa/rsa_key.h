#pragma once

#include <stdint.h>

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

typedef struct {
	uint64_t modulus;
	uint64_t publicExponent;
} rsa_public_key_t;
