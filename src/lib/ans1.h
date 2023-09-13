#pragma once

#include "lib/string.h"

#include <stdarg.h>

#define ASN1_SEQUENCE		0x30
#define ASN1_INTEGER		0x02

// https://mbed-tls.readthedocs.io/en/latest/kb/cryptography/asn1-key-structures-in-der-and-pem/

string_t ans1_to_string(string_t *data, const char *begin_str, const char *end_str) {
	string_t ans1_base64 = base64_encode(data);

	free(data->ptr);

	return string_join(
		string_join(
			string_from_ptr(begin_str),
			ans1_base64,
			JOIN_FREE_B
		),
		string_from_ptr(end_str),
		JOIN_FREE_A
	);
}



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

string_t ans1_encode_integer_sequence(int seq_len, const uint64_t *seq) {
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
		ERROR("ans1_extract_integer_sequence: invalid input length");
		return (-1);
	}

	if (in.ptr[0] != ASN1_SEQUENCE) {
		ERROR("ans1_extract_integer_sequence: invalid input type");
		return (-1);
	}

	uint64_t	datalen = in.ptr[1];

	if (datalen != in.len - 2) {
		ERROR("ans1_extract_integer_sequence: invalid input length");
		return (-1);
	}

	uint8_t	*ptr = in.ptr + 2;

	for (int i = 0; i < out_seq_len; ++i) {
		if (ptr + 2 > in.ptr + in.len) {
			ERROR("ans1_extract_integer_sequence: invalid input length");
			return (-1);
		}

		if (*ptr++ != ASN1_INTEGER) {
			ERROR("ans1_extract_integer_sequence: invalid input type");
			return (-1);
		}

		uint64_t intlen = *ptr++;

		// The len can be 1 byte longer than a 64 bit unsigned int because of the sign bit
		if (intlen > (sizeof(uint64_t)+1)) {
			ERROR("ans1_extract_integer_sequence: integer too big");
			return (-1);
		}

		if (ptr + intlen > in.ptr + in.len) {
			ERROR("ans1_extract_integer_sequence: invalid input length");
			return (-1);
		}

		// CHECK OVERFLOW ?
		out_seq[i] = 0;

		for (uint64_t j = 0; j < intlen; ++j) {
			out_seq[i] <<= 8;
			out_seq[i] |= (uint64_t)*ptr++;
		}
	}

	return (0);
}





string_t ans1_encode_bit_string(string_t in) {
	string_t res = string_new(3 + in.len);

	res.ptr[0] = 0x03;
	res.ptr[1] = in.len + 1;
	res.ptr[2] = 0x00;

	for (size_t i = 0; i < in.len; ++i) {
		res.ptr[3 + i] = in.ptr[i];
	}

	return (res);
}

string_t ans1_extract_bit_string(string_t in) {
	if (in.len < 3) {
		ERROR("ans1_decode_bit_string: invalid input length");
		return ((string_t){ .ptr = NULL, .len = 0 });
	}

	if (in.ptr[0] != 0x03) {
		ERROR("ans1_decode_bit_string: invalid input type");
		return ((string_t){ .ptr = NULL, .len = 0 });
	}

	uint64_t	datalen = in.ptr[1];

	if (datalen != in.len - 2) {
		ERROR("ans1_decode_bit_string: invalid input length");
		return ((string_t){ .ptr = NULL, .len = 0 });
	}

	string_t res = string_new(datalen - 1);

	for (uint64_t i = 0; i < datalen - 1; ++i) {
		res.ptr[i] = in.ptr[3 + i];
	}

	return (res);
}




string_t ans1_encode_sequence(int seq_len, ...) {
	va_list		args;
	string_t	res = string_new(2);

	va_start(args, seq_len);

	for (int i = 0; i < seq_len; ++i) {
		string_t	*tmp = va_arg(args, string_t*);

		res = string_join(res, *tmp, JOIN_FREE_A);
	}

	va_end(args);

	res.ptr[0] = ASN1_SEQUENCE;
	res.ptr[1] = res.len - 2;

	return (res);
}
