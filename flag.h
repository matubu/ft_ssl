#pragma once

#include <stdio.h>

#include "lib/string.h"

typedef struct {
	char		*name;
	enum {
		FlagNone = 0,
		Flag,
		FlagArgument,
		FlagInput,
		FlagInputFile,
		FlagOutput
	}			type;
	char		*description;

// _private
	int			present;
	const char	*argument;
}	flag_t;

#define FLAG(_name, _type, _description, ...) ((flag_t){ \
	.name = _name, \
	.type = _type, \
	.description = _description, \
	.present = 0, \
	.argument = NULL, \
})
#define FLAG_NOOP FLAG(NULL, FlagNone, NULL)

flag_t *get_flag(flag_t *flags, const char *s) {
	while (flags->type != FlagNone) {
		if (cmp(s, flags->name) == 0) {
			return (flags);
		}
		++flags;
	}
	flags->present = 0;
	flags->argument = NULL;
	return (flags);
}

