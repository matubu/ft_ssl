#pragma once

#include <stdint.h>
#include <string.h>
#include "lib/malloc.h"

int	is_print(char c) {
	return (c >= ' ' && c <= '~');
}

int is_space(char c) {
	return (c == ' ' || (c >= '\t' && c <= '\f'));
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

string_t	string_new(size_t len) {
	return ((string_t){ .ptr = malloc(len), .len = len });
}

string_t	string_new_initialized(size_t len, uint8_t c) {
	string_t	s = string_new(len);

	for (size_t i = 0; i < len; ++i) {
		s.ptr[i] = c;
	}
	return (s);
}

string_t	string_dup(string_t s) {
	uint8_t	*ptr = malloc(s.len);

	for (size_t i = 0; i < s.len; ++i) {
		ptr[i] = s.ptr[i];
	}
	return ((string_t){ .ptr = ptr, .len = s.len });
}

string_t	string_from_ptr(uint8_t *s) {
	return ((string_t){ .ptr = s, .len = slen(s) });
}

string_t	string_from_chars(const char *s) {
	uint64_t	len = slen((uint8_t *)s);
	uint8_t		*ptr = malloc(len + 1);
	uint64_t	i = 0;

	while (*s) {
		ptr[i++] = *s++;
	}
	ptr[i] = 0;
	return ((string_t){ .ptr = ptr, .len = len });
}

void	string_apply_inplace(string_t *s, string_t (*fun)(const string_t *input)) {
	string_t decoded = fun(s);
	free(s->ptr);
	*(string_t *)s = decoded;
}