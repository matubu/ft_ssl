#pragma once

#include "lib/malloc.h"
#include "lib/io.h"
#include "lib/dump.h"
#include "lib/readfile.h"

typedef struct {
	enum {
		FlagNone = 0,
		Flag,
		FlagArgument,
		FlagInput,
		FlagInputFile,
		FlagOutput
	}			type;
	char		*description;

// private
	int			present;
	const char	*argument;
}	flag_t;

#define newFlag(_type, _description, ...) ((flag_t){ \
	.type = _type, \
	.description = _description, \
	__VA_ARGS__ \
})

typedef struct {
	void	*fn;
	void	*print_fn;
}	t_func;

typedef struct {
	char			*name;
	const flag_t	*flags;
	enum {
		OneWayFn,
		TwoWayFn,
	}	fn_type;
	union
	{
		struct
		{
			t_func	e;
			t_func	d;
		}	twoway;
		struct
		{
			t_func	fn;
		}	oneway;
		// Fn(input: string_t, args: arguments_t)
	} u;
	// Print(output: string_t, currentInput: input_t, args: arguments_t)
}	command_t;

typedef struct input_s {
	string_t			filename;
	enum {
		InputFile,
		InputStdin,
		InputArgument
	}					origin;
	string_t			str;         // Needs to be free
	struct input_s		*next;
}	input_t;

typedef struct {
	const command_t	*command;
	flag_t			flags[256];
	input_t			*inputs;        // Needs to be free
	int				out_fd;
}	arguments_t;

void	print_origin(int fd, const input_t *curr, const arguments_t *args) {
	if (curr->origin == InputStdin && !args->flags['p'].present) {
		PUTSTR(fd, "\x1b[94mStdin\x1b[0m");
	} else if (curr->filename.ptr) {
		PUTSTR(fd, "\x1b[94mFile\x1b[0m<");
		print_escape(fd, &curr->filename);
		PUTSTR(fd, ">");
	} else {
		PUTSTR(fd, "\x1b[94m\"\x1b[0m");
		print_escape(fd, &curr->str);
		PUTSTR(fd, "\x1b[94m\"\x1b[0m");
	}
}

void	hash_print(int fd, const string_t *oneway, const input_t *curr, const arguments_t *args) {
	if (args->flags['r'].present) {
		hexdump(fd, oneway);
		if (!args->flags['q'].present) {
			PUTSTR(fd, " ");
			print_origin(fd, curr, args);
		}
	} else {
		if (!args->flags['q'].present) {
			putstr(fd, args->command->name);
			PUTSTR(fd, "(");
			print_origin(fd, curr, args);
			PUTSTR(fd, ") = ");
		}
		hexdump(fd, oneway);
	}
	write(fd, "\n", 1);
}
