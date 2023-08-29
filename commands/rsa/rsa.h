#pragma once

string_t	rsa(const string_t *_, const arguments_t *args) {
	(void)_;
	(void)args;
	printf("test\n");
	return ((string_t){ .ptr = NULL, .len = 0 });
}