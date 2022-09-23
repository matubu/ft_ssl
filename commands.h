#pragma once

#include "commands/md5.h"
// #include "commands/sha3-512.h"
#include "commands/sha256.h"
// #include "commands/shake256.h"
#include "commands/sm3.h"
// #include "commands/whirlpool.h"

#include "io.h"

typedef struct {
	char		*name;
	string_t	(*fn)(const string_t *);
}	command_t;

const command_t	commands[] = {
	{ .name = "md5", .fn = md5_hash },
	// { .name = "sha3-512", .fn = sha3_512_hash },   // <<< TODO
	{ .name = "sha256", .fn = sha256_hash },
	// { .name = "shake256", .fn = shake256_hash },   // <<< TODO
	{ .name = "sm3", .fn = sm3_hash },             // DONE
	// { .name = "whirlpool", .fn = whirlpool_hash }, // <<< TODO

	// blake2s256 https://www.rfc-editor.org/rfc/rfc7693
	// Tiger      ???

	// rmd160     (deprecated) https://www.reddit.com/r/crypto/comments/sv2y6d/ripemd160_a_disater_in_the_making_thanks_to/
	//                         https://homes.esat.kuleuven.be/~bosselae/ripemd160/pdf/AB-9601/AB-9601.pdf
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