#pragma once

#include "lib/string.h"

#define ASN1_SEQUENCE		0x30
#define ASN1_INTEGER		0x02

// https://letsencrypt.org/docs/a-warm-welcome-to-asn1-and-der/#encoding-instructions

uint64_t ans1_int_bytes_size(uint64_t n) {
	uint64_t len = 8;

	while (len > 0) {
		if (n >> ((len - 1) * 8)) {
			break ;
		}
		len--;
	}
	return len + 1;
}

// The long form is at least two bytes long, and has bit 8 of the first byte set to 1.
// Bits 7-1 of the first byte indicate how many more bytes are in the length field itself.
// Then the remaining bytes specify the length itself, as a multi-byte integer.

string_t ans1_encode_integer_sequence(int seq_len, uint64_t *seq) {
	string_t res = string_new(2);

	for (int i = 0; i < seq_len; ++i) {
		uint64_t	n = seq[i];
		uint64_t	datalen = ans1_int_bytes_size(n);

		// type 0x02 (integer) + length + data
		string_t tmp = string_new(1 + 1 + datalen);

		tmp.ptr[0] = ASN1_INTEGER;
		tmp.ptr[1] = datalen;
		
		// big endian
		for (uint64_t j = 0; j < datalen; ++j) {
			tmp.ptr[2 + datalen - j - 1] = n & 0xff;
			n >>= 8;
		}

		res = string_join(res, tmp, JOIN_FREE_BOTH);
	}

	res.ptr[0] = ASN1_SEQUENCE;
	res.ptr[1] = res.len - 2;

	return (res);
}

int ans1_extract_integer_sequence(string_t in, int out_seq_len, uint64_t *out_seq) {
	if (in.len < 2) {
		return (-1);
	}

	if (in.ptr[0] != ASN1_SEQUENCE) {
		return (-1);
	}

	uint64_t	datalen = in.ptr[1];

	if (datalen != in.len - 2) {
		return (-1);
	}

	uint8_t	*ptr = in.ptr + 2;

	for (int i = 0; i < out_seq_len; ++i) {
		if (ptr + 2 > in.ptr + in.len) {
			return (-1);
		}

		if (*ptr++ != ASN1_INTEGER) {
			return (-1);
		}

		uint64_t intlen = *ptr++;

		if (intlen > sizeof(uint64_t)) {
			return (-1);
		}

		if (ptr + intlen > in.ptr + in.len) {
			return (-1);
		}

		out_seq[i] = 0;

		for (uint64_t j = 0; j < intlen; ++j) {
			out_seq[i] <<= 8;
			out_seq[i] |= (uint64_t)*ptr++;
		}
	}

	return (0);
}