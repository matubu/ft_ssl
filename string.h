#pragma once

#include <stdint.h>

#define PUTS(s) write(2, s "\n", sizeof(s))

typedef struct {
	uint8_t	*ptr;
	size_t	len;
}	string_t;

int	cmp(const char *a, const char *b) {
	while (*a && *a == *b) {
		++a;
		++b;
	}
	return (*(unsigned char *)a - *(unsigned char *)b);
}