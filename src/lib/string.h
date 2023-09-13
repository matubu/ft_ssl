#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "lib/malloc.h"

#define STATIC_LEN(x) (sizeof(x) - 1)

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

	if (s == NULL)
		return (0);
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

string_t	string_from_ptr(const char *s) {
	return ((string_t){ .ptr = (uint8_t *)s, .len = slen((uint8_t *)s) });
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

#define JOIN_FREE_NONE	0b00
#define JOIN_FREE_A	    0b01
#define JOIN_FREE_B	    0b10
#define JOIN_FREE_BOTH	0b11

string_t	string_join(string_t a, string_t b, int free_mask) {
	string_t new = string_new(a.len + b.len);

	for (size_t i = 0; i < a.len; ++i)
		new.ptr[i] = a.ptr[i];
	for (size_t i = 0; i < b.len; ++i)
		new.ptr[a.len + i] = b.ptr[i];

	if (free_mask & JOIN_FREE_A)
		free(a.ptr);
	if (free_mask & JOIN_FREE_B)
		free(b.ptr);

	return (new);
}

int	string_starts_with(string_t s, string_t prefix) {
	if (s.len < prefix.len)
		return (0);

	for (size_t i = 0; i < prefix.len; ++i) {
		if (s.ptr[i] != prefix.ptr[i]) {
			return (0);
		}
	}

	return (1);
}

int string_ends_with(string_t s, string_t suffix) {
	if (s.len < suffix.len)
		return (0);

	for (size_t i = 0; i < suffix.len; ++i) {
		if (s.ptr[s.len - suffix.len + i] != suffix.ptr[i])
			return (0);
	}

	return (1);
}