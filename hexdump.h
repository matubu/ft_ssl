#pragma once

#include "string.h"

void	hex(uint8_t c) {
	write(1, &"0123456789abcdef"[c >> 4], 1);
	write(1, &"0123456789abcdef"[c & 0x0f], 1);
}

int	is_print(char c) {
	return (c >= ' ' && c <= '~');
}

void	hexdump(string_t s) {
	for (size_t i = 0; i < s.len; ++i) {
		hex(s.ptr[i]);
	}
	write(1, "\n", 1);
}

void	hexdump_with_preview(string_t s) {
	for (size_t i = 0; i < s.len; i += 16) {
		for (size_t j = 0; j < 16; ++j) {
			if (i + j < s.len)
				hex(s.ptr[i + j]);
			else
				write(1, "  ", 2);

			if (j % 2)
				write(1, " ", 1);
		}
		write(1, "| ", 2);
		for (size_t j = 0; j < 16; ++j) {
			if (i + j >= s.len)
				write(1, " ", 1);
			else if (is_print(s.ptr[i + j]))
				write(1, &s.ptr[i + j], 1);
			else
				write(1, ".", 1);
		}
		write(1, "\n", 1);
	}
}