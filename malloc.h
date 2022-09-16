#pragma once

#include <stdlib.h>

#include "string.h"

#define malloc(n) ({ \
	void	*ptr = malloc(n); \
	if (ptr == NULL) { \
		PUTS("ft_ssl: Error: allocation failed"); \
		exit(1); \
	} \
	ptr; \
})