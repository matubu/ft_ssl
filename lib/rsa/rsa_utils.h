#pragma once

#include "lib/rsa/rsa_key.h"
#include "lib/io.h"

void print_rsa_field(const char *name, uint64_t value) {
	PUTSTR(1, "| \x1b[92m"); putstr(1, name); PUTSTR(1, "\x1b[0m: ");
	print_u64(1, value); PUTSTR(1, " \x1b[90m(0x"); print_u64_as_hex(1, value); PUTSTR(1, ")\x1b[0m\n");
}

void print_private_rsa_key(const rsa_private_key_t *private_key) {
	PUTSTR(1, "\x1b[93m+ Private-Key: (64 bit)\x1b[0m\n");
	print_rsa_field("modulus", private_key->modulus);
	print_rsa_field("publicExponent", private_key->publicExponent);
	print_rsa_field("privateExponent", private_key->privateExponent);
	print_rsa_field("prime1", private_key->prime1);
	print_rsa_field("prime2", private_key->prime2);
	print_rsa_field("exponent1", private_key->exponent1);
	print_rsa_field("exponent2", private_key->exponent2);
	print_rsa_field("coefficient", private_key->coefficient);
}

void print_public_rsa_key(const rsa_public_key_t *public_key) {
	PUTSTR(1, "\x1b[93m+ Public-Key: (64 bit)\x1b[0m\n");
	print_rsa_field("modulus", public_key->modulus);
	print_rsa_field("publicExponent", public_key->publicExponent);
}


rsa_public_key_t rsa_private_to_public_key(const rsa_private_key_t *private_key) {
	return ((rsa_public_key_t){
		.modulus = private_key->modulus,
		.publicExponent = private_key->publicExponent
	});
}

