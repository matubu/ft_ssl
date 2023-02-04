#pragma once

#include "lib/io.h"

void	hexdump(int fd, const string_t *s) {
	for (size_t i = 0; i < s->len; ++i) {
		hex(fd, s->ptr[i]);
	}
}

void	hexdump_with_preview(int fd, const string_t *s) {
	for (size_t i = 0; i < s->len; i += 16) {
		for (size_t j = 0; j < 16; ++j) {
			if (i + j < s->len)
				hex(fd, s->ptr[i + j]);
			else
				write(fd, "  ", 2);

			if (j % 2)
				write(fd, " ", 1);
		}
		write(fd, "| ", 2);
		for (size_t j = 0; j < 16; ++j) {
			if (i + j >= s->len)
				write(fd, " ", 1);
			else if (is_print(s->ptr[i + j]))
				write(fd, &s->ptr[i + j], 1);
			else
				write(fd, ".", 1);
		}
		write(fd, "\n", 1);
	}
}

void	write_boolean(int boolean) {
	write(1, &"10"[!boolean], 1);
}

void	bindump(const string_t *s) {
	for (size_t i = 0; i < s->len; ++i) {
		write_boolean(s->ptr[i] >> 7 & 1);
		write_boolean(s->ptr[i] >> 6 & 1);
		write_boolean(s->ptr[i] >> 5 & 1);
		write_boolean(s->ptr[i] >> 4 & 1);
		write_boolean(s->ptr[i] >> 3 & 1);
		write_boolean(s->ptr[i] >> 2 & 1);
		write_boolean(s->ptr[i] >> 1 & 1);
		write_boolean(s->ptr[i] >> 0 & 1);
		write(1, " ", 1);
	}
	write(1, "\n", 1);
}

void	textdump(int fd, const string_t *s) {
	for (size_t i = 0; i < s->len; i += 64) {
		size_t	len = s->len - i;

		if (len > 64)
			len = 64;
		write(fd, s->ptr + i, len);
		write(fd, "\n", 1);
	}
}

void	stringdump(int fd, const string_t *s) {
	write(fd, s->ptr, s->len);
}