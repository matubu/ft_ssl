#pragma once

#include "lib/malloc.h"
#include "lib/io.h"
#include "lib/dump.h"
#include "lib/readfile.h"
#include "flag.h"

typedef struct {
	void	*fn;
	// Fn(input: string_t *, args: arguments_t *) -> string_t

	void	*print_fn;
	// Print(fd: int, output: string_t *, currentInput: input_t *, args: arguments_t *)
}	t_func;

typedef struct {
	const char		*name;
	flag_t			*flags;
	enum {
		OneWayFn,
		TwoWayFn,
		GeneratorFn
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
		struct
		{
			t_func	fn;
		}	generator;
	} u;
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
	input_t			*inputs;        // Needs to be free
	int				out_fd;
	flag_t			*flags;
}	arguments_t;


void	print_origin(int fd, const input_t *curr) {
	if (curr->origin == InputStdin) {
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
	if (get_flag(args->flags, "-r")->present) {
		hexdump(fd, oneway);
		if (!get_flag(args->flags, "-q")->present) {
			PUTSTR(fd, " ");
			print_origin(fd, curr);
		}
	} else {
		if (!get_flag(args->flags, "-q")->present) {
			putstr(fd, args->command->name);
			PUTSTR(fd, "(");
			print_origin(fd, curr);
			PUTSTR(fd, ") = ");
		}
		hexdump(fd, oneway);
	}
	write(fd, "\n", 1);
}
