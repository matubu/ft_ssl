#pragma once

#include "string.h"

string_t	sha256_hash(const string_t *str) {
	(void)str;
	return ((string_t){ .ptr = NULL, .len = 0 });
}