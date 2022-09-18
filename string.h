#pragma once

#include <stdint.h>
#include <unistd.h>

#define PUTS(s) write(2, s "\n", sizeof(s))

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
#define ERROR(s) PUTS("ft_ssl: \x1b[91mError\x1b[0m: " s)
#define DIE(s) { ERROR(s); exit(1); }
#define HELP_AND_DIE(s) { ERROR(s); usage(); }

int	cmp(const char *a, const char *b) {
	while (*a && *a == *b) {
		++a;
		++b;
	}
	return (*(unsigned char *)a - *(unsigned char *)b);
}

uint64_t	slen(const uint8_t *s) {
	uint64_t	len = 0;

	while (*s)
	{
		++s;
		++len;
	}
	return (len);
}

typedef struct {
	uint8_t	*ptr;
	size_t	len;
}	string_t;

string_t	stringdup(const char *s) {
	uint64_t	len = slen((uint8_t *)s);
	uint8_t		*ptr = malloc(len + 1);
	uint64_t	i = 0;

	while (*s) {
		ptr[i++] = *s++;
	}
	ptr[i] = 0;
	return ((string_t){ .ptr = ptr, .len = len });
}