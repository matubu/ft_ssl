#pragma once

#include "string.h"

void	write_boolean(int boolean) {
	write(1, &"10"[!boolean], 1);
}

void	bindump(string_t s) {
	for (size_t i = 0; i < s.len; ++i) {
		write_boolean(s.ptr[i] >> 7 & 1);
		write_boolean(s.ptr[i] >> 6 & 1);
		write_boolean(s.ptr[i] >> 5 & 1);
		write_boolean(s.ptr[i] >> 4 & 1);
		write_boolean(s.ptr[i] >> 3 & 1);
		write_boolean(s.ptr[i] >> 2 & 1);
		write_boolean(s.ptr[i] >> 1 & 1);
		write_boolean(s.ptr[i] >> 0 & 1);
		write(1, " ", 1);
	}
	write(1, "\n", 1);
}
