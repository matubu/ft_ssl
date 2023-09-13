#pragma once

#include "lib/ans1.h"
#include "lib/string.h"

#include "lib/rsa/rsa_key.h"

#include "commands/base64.h"


//
// PRIVATE KEY
//

#define BEGIN_RSA_PRIVATE_KEY "-----BEGIN RSA PRIVATE KEY-----\n"
#define END_RSA_PRIVATE_KEY "\n-----END RSA PRIVATE KEY-----\n"

string_t serialize_rsa_private_key(const rsa_private_key_t *key) {

	string_t ans1 = ans1_encode_integer_sequence(9, (const uint64_t *)key);

	return ans1_to_string(
		&ans1,
		BEGIN_RSA_PRIVATE_KEY,
		END_RSA_PRIVATE_KEY
	);

}

rsa_private_key_t *deserialize_rsa_private_key(string_t key) {

	if (!string_starts_with(key, string_from_ptr(BEGIN_RSA_PRIVATE_KEY))) {
		ERROR("Invalid RSA private key (prefix)");
		return (NULL);
	}

	if (!string_ends_with(key, string_from_ptr(END_RSA_PRIVATE_KEY))) {
		ERROR("Invalid RSA private key (suffix)");
		return (NULL);
	}

	rsa_private_key_t *res = malloc(sizeof(rsa_private_key_t));
	string_t substr = (string_t){
		.ptr = key.ptr + STATIC_LEN(BEGIN_RSA_PRIVATE_KEY),
		.len = key.len - STATIC_LEN(BEGIN_RSA_PRIVATE_KEY END_RSA_PRIVATE_KEY)
	};
	string_t ans1 = base64_decode(&substr);

	if (ans1_extract_integer_sequence(ans1, 9, (uint64_t *)res) == -1) {
		free(res);
		free(ans1.ptr);
		return (NULL);
	}

	free(ans1.ptr);

	return (res);

}


//
// PUBLIC KEY
//

#define BEGIN_PUBLIC_KEY "-----BEGIN PUBLIC KEY-----\n"
#define END_PUBLIC_KEY "\n-----END PUBLIC KEY-----\n"

#define ASN1_RSA_PUBLIC_KEY_IDENTIFIER		(uint8_t []){ \
	0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00 \
}

// Source: https://mbed-tls.readthedocs.io/en/latest/kb/cryptography/asn1-key-structures-in-der-and-pem/

// PublicKeyInfo ::= SEQUENCE {
// 	algorithm   SEQUENCE {
// 		algorithm   OBJECT IDENTIFIER,
// 		parameters  ANY DEFINED BY algorithm OPTIONAL
// 	},
// 	PublicKey   BIT STRING {
// 		SEQUENCE {
// 			modulus   INTEGER,  -- n
// 			publicExponentINTEGER   -- e
// 		}
// 	}
// }

string_t serialize_public_key(const rsa_public_key_t *key) {
	string_t tmp = ans1_encode_integer_sequence(2, (const uint64_t *)key);
	string_t rsa_public_key_ans1 = ans1_encode_bit_string(tmp);
	free(tmp.ptr);

	string_t algorithm_ans1 = (string_t){
		.ptr = ASN1_RSA_PUBLIC_KEY_IDENTIFIER,
		.len = sizeof(ASN1_RSA_PUBLIC_KEY_IDENTIFIER)
	};

	string_t public_key_ans1 = ans1_encode_sequence(2, &algorithm_ans1, &rsa_public_key_ans1);

	free(rsa_public_key_ans1.ptr);

	return ans1_to_string(
		&public_key_ans1,
		BEGIN_PUBLIC_KEY,
		END_PUBLIC_KEY
	);
}

rsa_public_key_t *deserialize_public_key(string_t key) {

	if (!string_starts_with(key, string_from_ptr(BEGIN_PUBLIC_KEY))) {
		ERROR("Invalid public key (prefix)");
		return (NULL);
	}

	if (!string_ends_with(key, string_from_ptr(END_PUBLIC_KEY))) {
		ERROR("Invalid public key (suffix)");
		return (NULL);
	}

	string_t substr = (string_t){
		.ptr = key.ptr + STATIC_LEN(BEGIN_PUBLIC_KEY),
		.len = key.len - STATIC_LEN(BEGIN_PUBLIC_KEY END_PUBLIC_KEY)
	};
	string_t ans1 = base64_decode(&substr);

	if (ans1.len < 2) {
		ERROR("Invalid public key (too short)");
		free(ans1.ptr);	
		return (NULL);
	}

	if (ans1.ptr[0] != ASN1_SEQUENCE) {
		ERROR("Invalid public key (does not start with ASN1_SEQUENCE)");
		free(ans1.ptr);
		return (NULL);
	}

	uint64_t seq_len = ans1.ptr[1];

	if (seq_len != ans1.len - 2) {
		ERROR("Invalid public key (invalid length)");
		free(ans1.ptr);
		return (NULL);
	}

	string_t public_key_ans1 = (string_t){
		.ptr = ans1.ptr + 2,
		.len = ans1.len - 2
	};

	string_t algorithm_ans1 = (string_t){
		.ptr = ASN1_RSA_PUBLIC_KEY_IDENTIFIER,
		.len = sizeof(ASN1_RSA_PUBLIC_KEY_IDENTIFIER)
	};

	if (!string_starts_with(public_key_ans1, algorithm_ans1)) {
		ERROR("Unrecognized public key algorithm");
		free(ans1.ptr);
		return (NULL);
	}

	public_key_ans1.ptr += algorithm_ans1.len;
	public_key_ans1.len -= algorithm_ans1.len;

	string_t rsa_public_key_ans1 = ans1_extract_bit_string(public_key_ans1);
	free(ans1.ptr);

	rsa_public_key_t *res = malloc(sizeof(rsa_public_key_t));
	
	if (ans1_extract_integer_sequence(rsa_public_key_ans1, 2, (uint64_t *)res) == -1) {
		free(res);
		free(rsa_public_key_ans1.ptr);
		return (NULL);
	}

	free(rsa_public_key_ans1.ptr);

	return (res);

}
