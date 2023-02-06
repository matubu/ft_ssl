#pragma once

#include "lib/utils.h"

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

		output_ptr[j++] = base64_charset[b0 >> 2];

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
	int16_t	base64_looktable[256];
	for (size_t i = 0; i < 256; ++i) {
		base64_looktable[i] = -1;
	}
	base64_looktable[(uint8_t)base64_padchar] = 0;
	for (size_t i = 0; i < 64; ++i) {
		base64_looktable[(uint8_t)base64_charset[i]] = i;
	}

	uint8_t	*output_ptr = malloc(input->len);
	size_t	j = 0;
	size_t	i = 0;

	while (1) {
		int block_size = 3;

		while (i < input->len && base64_looktable[input->ptr[i]] == -1)
			i++;
		if (i >= input->len)
			break ;
		int16_t b0 = base64_looktable[input->ptr[i++]];

		while (i < input->len && base64_looktable[input->ptr[i]] == -1)
			i++;
		if (i >= input->len)
			goto error;
		int16_t b1 = base64_looktable[input->ptr[i++]];

		while (i < input->len && base64_looktable[input->ptr[i]] == -1)
			i++;
		if (i >= input->len)
			goto error;
		if (input->ptr[i] == base64_padchar)
			block_size--;
		int16_t b2 = base64_looktable[input->ptr[i++]];

		while (i < input->len && base64_looktable[input->ptr[i]] == -1)
			i++;
		if (i >= input->len)
			goto error;
		if (input->ptr[i] == base64_padchar)
			block_size--;
		int16_t b3 = base64_looktable[input->ptr[i++]];

		output_ptr[j + 0] = (b0 << 2) | ((b1 >> 4) & 0b11);
		output_ptr[j + 1] = ((b1 & 15) << 4) | ((b2 >> 2) & 15);
		output_ptr[j + 2] = ((b2 & 0b11) << 6) | b3;

		j += block_size;
	}

	return ((string_t){ .ptr = output_ptr, .len = j });

error:
	return ((string_t){ .ptr = output_ptr, .len = 0 });
}
