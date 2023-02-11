#pragma once

#include "ft_ssl.h"

#include "commands/md5.h"
#include "commands/sha256.h"
#include "commands/sha512.h"
#include "commands/sm3.h"

#include "commands/base64.h"

#include "commands/des.h"

const flag_t	hash_flags[256] = {
	['p'] = newFlag(Flag,      "echo STDIN to STDOUT and append the checksum to STDOUT"),
	['q'] = newFlag(Flag,      "quiet mode"),
	['r'] = newFlag(Flag,      "reverse the format of the output"),
	['s'] = newFlag(FlagInput, "print the sum of the given string"),
};
const flag_t	base64_flags[256] = {
	['d'] = newFlag(Flag,          "decode mode"),
	['e'] = newFlag(Flag,          "encode mode (default)"),
	['i'] = newFlag(FlagInputFile, "input file"),
	['o'] = newFlag(FlagOutput,    "output file")
};
const flag_t	cipher_flags[256] = {
	['a'] = newFlag(Flag,          "decode/encode the input/output in base64, depending on the encrypt mode"),
	['d'] = newFlag(Flag,          "decrypt mode"),
	['e'] = newFlag(Flag,          "encrypt mode (default)"),
	['i'] = newFlag(FlagInputFile, "input file for message"),
	['k'] = newFlag(FlagArgument,  "key in hex is the next argument"),
	['o'] = newFlag(FlagOutput,    "output file for message"),
	['p'] = newFlag(FlagArgument,  "password in ascii is the next argument"),
	['s'] = newFlag(FlagArgument,  "the salt in hex is the next argument"),
	['v'] = newFlag(FlagArgument,  "initialization vector in hex is the next argument")
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
		.d = {base64_decode, rawtextdump}
	}, .flags = base64_flags },

	{ .name = "des"    , .fn_type = OneWayFn, .u.oneway.fn = {des_cbc_cipher, rawtextdump}, .flags = cipher_flags },
	{ .name = "des-ecb", .fn_type = OneWayFn, .u.oneway.fn = {des_ecb_cipher, rawtextdump}, .flags = cipher_flags },
	{ .name = "des-cbc", .fn_type = OneWayFn, .u.oneway.fn = {des_cbc_cipher, rawtextdump}, .flags = cipher_flags },
	{ .name = "des-ede", .fn_type = OneWayFn, .u.oneway.fn = {des_ede_cipher, rawtextdump}, .flags = cipher_flags },
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