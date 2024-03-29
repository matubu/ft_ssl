#pragma once

#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>

#include "lib/string.h"

#define write(fd, buf, len) ({ \
	ssize_t	ret = write(fd, buf, len); \
	if (ret < 0) { \
		exit(42); \
	} \
	ret; \
})

#define TO_STRING(x) #x
#define TO_STRING_EXPAND(x) TO_STRING(x)

#define PUTSTR(fd, s) write(fd, s, sizeof(s) - 1)
#define PUTS(fd, s) PUTSTR(fd, s "\n")

void	putstr(int fd, const char *s) {
	write(fd, s, slen((const uint8_t *)s));
}

void	hex(int fd, uint8_t c) {
	write(fd, &"0123456789abcdef"[c >> 4], 1);
	write(fd, &"0123456789abcdef"[c & 0x0f], 1);
}

void	print_u64(int fd, uint64_t n) {
	char buf[20];
	char *ptr = (buf + sizeof(buf));

	do {
		*--ptr = '0' + n % 10;
		n /= 10;
	} while (n);

	write(fd, ptr, (buf + sizeof(buf)) - ptr);
}

void	print_u64_as_hex(int fd, uint64_t n) {
	char buf[16];
	char *ptr = (buf + sizeof(buf));

	do {
		*--ptr = "0123456789abcdef"[n & 0x0f];
		n >>= 4;
	} while (n);

	write(fd, ptr, (buf + sizeof(buf)) - ptr);
}

void	print_escape(int fd, const string_t *s) {
	for (size_t i = 0; i < s->len; ++i) {
		switch (s->ptr[i])
		{
			case '\0': PUTSTR(fd, "\x1b[91m\\0\x1b[0m"); break;
			case '\f': PUTSTR(fd, "\x1b[91m\\f\x1b[0m"); break;
			case '\n': PUTSTR(fd, "\x1b[91m\\n\x1b[0m"); break;
			case '\r': PUTSTR(fd, "\x1b[91m\\r\x1b[0m"); break;
			case '\t': PUTSTR(fd, "\x1b[91m\\t\x1b[0m"); break;
			case '\v': PUTSTR(fd, "\x1b[91m\\v\x1b[0m"); break;
			case '\\': PUTSTR(fd, "\x1b[91m\\\\\x1b[0m"); break;
		
			default:
				if (is_print(s->ptr[i])) {
					write(fd, s->ptr + i, 1);
				} else {
					PUTSTR(fd, "\x1b[91m\\x");
					hex(fd, s->ptr[i]);
					PUTSTR(fd, "\x1b[0m");
				}
		}
	}
}

#define ERROR(...) { \
	const char	*args[] = { __VA_ARGS__ }; \
	PUTSTR(2, "ft_ssl: \x1b[91mError\x1b[0m"); \
	size_t	count = sizeof(args) / sizeof(*args); \
	for (size_t i = 0; i < count; ++i) { \
		PUTSTR(2, ": "); \
		putstr(2, args[i]); \
	} \
	write(2, "\n", 1); \
}
#define DIE(...) { ERROR(__VA_ARGS__); exit(1); }
