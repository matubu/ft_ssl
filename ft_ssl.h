#pragma once

#include "commands/md5.h"
#include "commands/sha256.h"
#include "commands/sha512.h"
#include "commands/sm3.h"

#include "commands/base64.h"

#include "io.h"
#include "dump.h"

typedef struct {
	enum {
		FlagNone = 0,
		Flag,
		FlagArgument,
		FlagInput
	}			type;

// private
	int			present;
	const char	*argument;
}	flag_t;

#define newFlag(_type, ...) ((flag_t){ \
	.type = _type, \
	__VA_ARGS__ \
})

typedef struct {
	char			*name;
	const flag_t	*flags;
	string_t		(*fn)(const string_t *);
	// Fn(input, args)
	void			*print_fn;
	// Print(output, currentInput, args)
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
}	arguments_t;

void	print_origin(const input_t *curr, const arguments_t *args) {
	if (curr->origin == InputStdin && !args->flags['p'].present) {
		PUTSTR("\x1b[94mStdin\x1b[0m");
	} else if (curr->filename.ptr) {
		PUTSTR("\x1b[94mFile\x1b[0m<");
		print_escape(&curr->filename);
		PUTSTR(">");
	} else {
		PUTSTR("\x1b[94m\"\x1b[0m");
		print_escape(&curr->str);
		PUTSTR("\x1b[94m\"\x1b[0m");
	}
}

void	hash_print(const string_t *hash, const input_t *curr, const arguments_t *args) {
	if (args->flags['r'].present) {
		hexdump(hash);
		if (!args->flags['q'].present) {
			PUTSTR(" ");
			print_origin(curr, args);
		}
	} else {
		if (!args->flags['q'].present) {
			putstr(args->command->name);
			PUTSTR("(");
			print_origin(curr, args);
			PUTSTR(") = ");
		}
		hexdump(hash);
	}
	PUTS("");
}

const flag_t	hash_flags[256] = {
	['p'] = newFlag(Flag),
	['q'] = newFlag(Flag),
	['r'] = newFlag(Flag),
	['s'] = newFlag(FlagInput),
};
const flag_t	base64_flags[256] = {
	['d'] = newFlag(Flag),
	['e'] = newFlag(Flag),
	['i'] = newFlag(FlagArgument),
	['o'] = newFlag(FlagArgument)
};
const flag_t	cipher_flags[256] = {
	['a'] = newFlag(Flag),
	['d'] = newFlag(Flag),
	['e'] = newFlag(Flag),
	['i'] = newFlag(FlagArgument),
	['o'] = newFlag(FlagArgument),
	['k'] = newFlag(FlagArgument),
	['p'] = newFlag(FlagArgument),
	['s'] = newFlag(FlagArgument),
	['v'] = newFlag(FlagArgument)
};

const command_t	commands[] = {
	{ .name = "md5",    .fn = md5_hash,      .flags = hash_flags, .print_fn = hash_print },
	{ .name = "sha224", .fn = sha224_hash,   .flags = hash_flags, .print_fn = hash_print },
	{ .name = "sha256", .fn = sha256_hash,   .flags = hash_flags, .print_fn = hash_print },
	{ .name = "sha384", .fn = sha384_hash,   .flags = hash_flags, .print_fn = hash_print },
	{ .name = "sha512", .fn = sha512_hash,   .flags = hash_flags, .print_fn = hash_print },
	{ .name = "sm3",    .fn = sm3_hash,      .flags = hash_flags, .print_fn = hash_print },

	// { .name = "base64", .fn = base64_command, .flags = base64_flags, .print_fn = textdump },
	// { .name = "des", .fn = des_command, .flags = cipher_flags, .print_fn = textdump },
};
size_t	commands_count = sizeof(commands) / sizeof(commands[0]);

const command_t	*get_command(const char *command) {
	for (size_t i = 0; i < commands_count; ++i) {
		if (cmp(command, commands[i].name) == 0) {
			return (&commands[i]);
		}
	}
	HELP_AND_DIE("no such command");
	return (NULL);
}