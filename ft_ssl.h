#pragma once

#include "commands/md5.h"
#include "commands/sha256.h"
#include "commands/sha512.h"
#include "commands/sm3.h"

#include "commands/base64.h"

#include "io.h"
#include "dump.h"

typedef struct {
	char		*name;
	string_t	(*fn)(const string_t *);
	void		*print_fn;
	// Print(output, currentInput, args)
}	command_t;

typedef struct {
	char	*flag;
	int		with_argument;
	int		required;

// private
	int		present;
	char	**arguments;
}	flag_t;

typedef struct input_s {
	string_t			display_str;
	string_t			str;         // Needs to be free
	struct input_s		*next;
}	input_t;

typedef struct {
	const command_t	*command;
	int				print_stdin;
	int				quiet;
	int				reverse_format;
	input_t	*inputs;        // Needs to be free
}	arguments_t;

void	hash_print(const string_t *hash, const input_t *curr, const arguments_t *args) {
	if (args->reverse_format) {
		hexdump(hash);
		if (!args->quiet) {
			PUTSTR(" ");
			print_escape(&curr->display_str);
		}
	} else {
		if (!args->quiet) {
			putstr(args->command->name);
			PUTSTR("(");
			print_escape(&curr->display_str);
			PUTSTR(") = ");
		}
		hexdump(hash);
	}
	PUTS("");
}

const command_t	commands[] = {
	{ .name = "md5",    .fn = md5_hash,      .print_fn = hash_print },
	{ .name = "sha224", .fn = sha224_hash,   .print_fn = hash_print },
	{ .name = "sha256", .fn = sha256_hash,   .print_fn = hash_print },
	{ .name = "sha384", .fn = sha384_hash,   .print_fn = hash_print },
	{ .name = "sha512", .fn = sha512_hash,   .print_fn = hash_print },
	{ .name = "sm3",    .fn = sm3_hash,      .print_fn = hash_print },

	{ .name = "base64", .fn = base64_encode, .print_fn = textdump },
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