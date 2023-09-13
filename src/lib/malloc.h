#pragma once

#include <stdlib.h>

#include "lib/io.h"

#define malloc(n) ({ \
	void	*ptr = malloc(n); \
	if (ptr == NULL) { \
		DIE("allocation error", strerror(errno)); \
	} \
	ptr; \
})
