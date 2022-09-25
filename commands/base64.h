#pragma once

#include "utils.h"

// Documentation:
// https://www.rfc-editor.org/rfc/rfc4648.html

const char	base64_charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char	base64_padchar = '=';

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
	uint8_t	*output_ptr = malloc(input->len);
	size_t	j = 0;

	for (size_t i = 0; i < input->len; i += 4) {
		if (input->ptr[i] == '\n') {
			i++;
			continue ;
		}

		
	}

	return ((string_t){ .ptr = output_ptr, .len = j });
}