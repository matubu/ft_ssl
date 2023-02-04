#pragma once

#include "lib/malloc.h"
#include "lib/io.h"
#include "lib/dump.h"
#include "lib/readfile.h"

#include "commands/md5.h"
#include "commands/sha256.h"
#include "commands/sha512.h"
#include "commands/sm3.h"

#include "commands/base64.h"

typedef struct {
	enum {
		FlagNone = 0,
		Flag,
		FlagArgument,
		FlagInput,
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

const flag_t	hash_flags[256] = {
	['p'] = newFlag(Flag,      "echo STDIN to STDOUT and append the checksum to STDOUT"),
	['q'] = newFlag(Flag,      "quiet mode"),
	['r'] = newFlag(Flag,      "reverse the format of the output"),
	['s'] = newFlag(FlagInput, "print the sum of the given string"),
};
const flag_t	base64_flags[256] = {
	['d'] = newFlag(Flag,       "decode mode"),
	['e'] = newFlag(Flag,       "encode mode (default)"),
	['i'] = newFlag(FlagInput,  "input file"),
	['o'] = newFlag(FlagOutput, "output file")
};
const flag_t	cipher_flags[256] = {
	['a'] = newFlag(Flag,         "decode/encode the input/output in base64, depending on the encrypt mode"),
	['d'] = newFlag(Flag,         "decrypt mode"),
	['e'] = newFlag(Flag,         "encrypt mode (default)"),
	['i'] = newFlag(FlagInput,    "input file for message"),
	['o'] = newFlag(FlagOutput,   "key in hex is the next argument"),
	['k'] = newFlag(FlagArgument, "output file for message"),
	['p'] = newFlag(FlagArgument, "password in ascii is the next argument"),
	['s'] = newFlag(FlagArgument, "the salt in hex is the next argument"),
	['v'] = newFlag(FlagArgument, "initialization vector in hex is the next argument")
};

const command_t	commands[] = {
	{ .name = "md5",    .fn_type = OneWayFn, .u.oneway.fn = {md5_hash, hash_print},
		.flags = hash_flags },
	{ .name = "sha224", .fn_type = OneWayFn, .u.oneway.fn = {sha224_hash, hash_print},
		.flags = hash_flags },
	{ .name = "sha256", .fn_type = OneWayFn, .u.oneway.fn = {sha256_hash, hash_print},
		.flags = hash_flags },
	{ .name = "sha384", .fn_type = OneWayFn, .u.oneway.fn = {sha384_hash, hash_print},
		.flags = hash_flags },
	{ .name = "sha512", .fn_type = OneWayFn, .u.oneway.fn = {sha512_hash, hash_print},
		.flags = hash_flags },
	{ .name = "sm3",    .fn_type = OneWayFn, .u.oneway.fn = {sm3_hash, hash_print},
		.flags = hash_flags },

	{ .name = "base64", .fn_type = TwoWayFn, .u.twoway = {
		.e = {base64_encode, textdump},
		.d = {base64_decode, stringdump}
	}, .flags = base64_flags },
	// { .name = "des", .fn = des_command, .flags = cipher_flags, .print_fn = textdump },
};
size_t	commands_count = sizeof(commands) / sizeof(commands[0]);

const command_t	*get_command(const char *command) {
	for (size_t i = 0; i < commands_count; ++i) {
		if (cmp(command, commands[i].name) == 0) {
			return (&commands[i]);
		}
	}
	return (NULL);
}