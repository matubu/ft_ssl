#pragma once

#include <stdlib.h>

#include "string.h"

#define malloc(n) ({ \
	void	*ptr = malloc(n); \
	if (ptr == NULL) { \
		DIE("allocation error"); \
	} \
	ptr; \
})
