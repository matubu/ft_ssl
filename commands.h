#pragma once

#include "commands/md5.h"
#include "commands/sha256.h"
#include "commands/sha512.h"
#include "commands/sm3.h"

#include "io.h"

typedef struct {
	char		*name;
	string_t	(*fn)(const string_t *);
}	command_t;

const command_t	commands[] = {
	{ .name = "md5", .fn = md5_hash },
	{ .name = "sha224", .fn = sha224_hash },
	{ .name = "sha256", .fn = sha256_hash },
	{ .name = "sha384", .fn = sha384_hash },
	{ .name = "sha512", .fn = sha512_hash },
	{ .name = "sm3", .fn = sm3_hash },
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