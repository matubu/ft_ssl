#pragma once

#include <stdint.h>
#include </usr/include/string.h>
#include "malloc.h"

int	is_print(char c) {
	return (c >= ' ' && c <= '~');
}

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

string_t	string_ptr(uint8_t *s) {
	return ((string_t){ .ptr = s, .len = slen(s) });
}

string_t	string_dup(const char *s) {
	uint64_t	len = slen((uint8_t *)s);
	uint8_t		*ptr = malloc(len + 1);
	uint64_t	i = 0;

	while (*s) {
		ptr[i++] = *s++;
	}
	ptr[i] = 0;
	return ((string_t){ .ptr = ptr, .len = len });
}