#pragma once

#include <unistd.h>
#include <errno.h>
#include <stdint.h>

#define PUTSTR(s) write(1, s, sizeof(s) - 1)
#define PUTS(s) PUTSTR(s "\n")

void	usage(void) {
	PUTS("usage: ft_ssl command [flags] [file/string]");
	PUTS("");
	PUTS("Flags:");
	PUTS("  -p          echo STDIN to STDOUT and append the checksum to STDOUT");
	PUTS("  -q          quiet mode");
	PUTS("  -r          reverse the format of the output");
	PUTS("  -s STRING   print the sum of the given string");
	PUTS("");
	PUTS("Commands: md5, sha256");
	exit(1);
}
#define ERROR(...) { \
	const char	*args[] = { __VA_ARGS__ }; \
	PUTSTR("ft_ssl: \x1b[91mError\x1b[0m"); \
	size_t	count = sizeof(args) / sizeof(*args); \
	for (size_t i = 0; i < count; ++i) { \
		PUTSTR(": "); \
		putstr(args[i]); \
	} \
	write(1, "\n", 1); \
}
#define DIE(...) { ERROR(__VA_ARGS__); exit(1); }
#define HELP_AND_DIE(...) { ERROR(__VA_ARGS__); usage(); }

void	hex(uint8_t c) {
	write(1, &"0123456789abcdef"[c >> 4], 1);
	write(1, &"0123456789abcdef"[c & 0x0f], 1);
}

#include "string.h"

void	putstr(const char *s) {
	write(1, s, slen((const uint8_t *)s));
}

void	print_escape(const string_t *s) {
	for (size_t i = 0; i < s->len; ++i) {
		switch (s->ptr[i])
		{
			case '\0': PUTSTR("\x1b[91m\\0\x1b[0m"); break;
			case '\f': PUTSTR("\x1b[91m\\f\x1b[0m"); break;
			case '\n': PUTSTR("\x1b[91m\\n\x1b[0m"); break;
			case '\r': PUTSTR("\x1b[91m\\r\x1b[0m"); break;
			case '\t': PUTSTR("\x1b[91m\\t\x1b[0m"); break;
			case '\v': PUTSTR("\x1b[91m\\v\x1b[0m"); break;
			case '\\': PUTSTR("\x1b[91m\\\\\x1b[0m"); break;
		
			default:
				if (is_print(s->ptr[i])) {
					write(1, s->ptr + i, 1);
				} else {
					PUTSTR("\x1b[91m\\x");
					hex(s->ptr[i]);
					PUTSTR("\x1b[0m");
				}
		}
	}
}
