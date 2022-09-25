#pragma once

#include "utils.h"

// Documentation:
// https://www.rfc-editor.org/rfc/rfc4648.html

const uint8_t	base64_charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const uint8_t	base64_padchar = '=';

string_t	base64_encode(const string_t *input) {
	uint8_t	*output_ptr = malloc((input->len + 2) / 3 * 4);
	size_t	j = 0;

	for (size_t i = 0; i < input->len; i += 3) {
		int	b0 = input->ptr[i], b1 = -1, b2 = -1;

		if (i + 1 < input->len)
			b1 = input->ptr[i + 1];
		if (i + 2 < input->len)
			b2 = input->ptr[i + 2];

		output_ptr[j++] = base64_charset[(b0 >> 2)];

		if (b1 == -1) {
			output_ptr[j++] = base64_charset[(b0 & 0b11) << 4];
			output_ptr[j++] = base64_padchar;
			output_ptr[j++] = base64_padchar;
			continue ;
		}
		output_ptr[j++] = base64_charset[(b0 & 0b11) << 4 | (b1 >> 4)];

		if (b2 == -1) {
			output_ptr[j++] = base64_charset[(b1 & 15) << 2];
			output_ptr[j++] = base64_padchar;
			continue ;
		}
		output_ptr[j++] = base64_charset[(b1 & 15) << 2 | (b2 >> 6)];
		output_ptr[j++] = base64_charset[b2 & 63];
	}

	return ((string_t){ .ptr = output_ptr, .len = j });
}

string_t	base64_decode(const string_t *input) {
	int16_t	base64_looktable[256] = {-1};
	base64_looktable[(uint8_t)base64_padchar] = 0;
	for (size_t i = 0; i < 64; ++i) {
		base64_looktable[(uint8_t)base64_charset[i]] = i;
	}

	uint8_t	*output_ptr = malloc(input->len);
	size_t	j = 0;

	for (size_t i = 0; i < input->len; i += 4) {
		if (input->ptr[i] == '\n') {
			i++;
			continue ;
		}

		int16_t b0 = base64_looktable[input->ptr[i + 0]];
		int16_t b1 = base64_looktable[input->ptr[i + 1]];
		int16_t b2 = base64_looktable[input->ptr[i + 2]];
		int16_t b3 = base64_looktable[input->ptr[i + 3]];

		if (b0 < 0 || b1 < 0 || b2 < 0 || b3 < 0) {
			return ((string_t){ .ptr = output_ptr, .len = 0 });
		}

		// output_ptr[j++] = ;
		// output_ptr[j++] = ;
		// output_ptr[j++] = ;
	}

	return ((string_t){ .ptr = output_ptr, .len = j });
}