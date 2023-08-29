#pragma once

#include "ft_ssl.h"

#include "commands/md5.h"
#include "commands/sha256.h"
#include "commands/sha512.h"
#include "commands/sm3.h"

#include "commands/base64.h"

#include "commands/des.h"

#include "commands/rsa/genrsa.h"
#include "commands/rsa/rsa.h"

flag_t	hash_flags[] = {
	FLAG("-p", Flag,      "echo STDIN to STDOUT and append the checksum to STDOUT"),
	FLAG("-q", Flag,      "quiet mode"),
	FLAG("-r", Flag,      "reverse the format of the output"),
	FLAG("-s", FlagInput, "print the sum of the given string"),
	FLAG_NOOP
};
flag_t	base64_flags[] = {
	FLAG("-d", Flag,          "decode mode"),
	FLAG("-e", Flag,          "encode mode (default)"),
	FLAG("-i" ,FlagInputFile, "input file"),
	FLAG("-o", FlagOutput,    "output file"),
	FLAG_NOOP
};
flag_t	cipher_flags[] = {
	FLAG("-a", Flag,          "decode/encode the input/output in base64, depending on the encrypt mode"),
	FLAG("-d", Flag,          "decrypt mode"),
	FLAG("-e", Flag,          "encrypt mode (default)"),
	FLAG("-i", FlagInputFile, "input file for message"),
	FLAG("-k", FlagArgument,  "key in hex is the next argument"),
	FLAG("-o", FlagOutput,    "output file for message"),
	FLAG("-p", FlagArgument,  "password in ascii is the next argument"),
	FLAG("-s", FlagArgument,  "the salt in hex is the next argument"),
	FLAG("-v", FlagArgument,  "initialization vector in hex is the next argument"),
	FLAG_NOOP
};
flag_t	genrsa_flags[] = {
	FLAG("-out", FlagOutput,  "output the key to the specified file. If this argument is not specified then standard output is used"),
	FLAG_NOOP
};
flag_t	rsa_flags[] = {
	FLAG("-inform",  FlagArgument, "input format (PEM)"),
	FLAG("-outform", FlagArgument, "output format (PEM)"),
	/*TODO*/FLAG("-in",      FlagInputFile, "input file"),
	/*TODO*/FLAG("-passin",  FlagArgument, "input file password source"),
	/*TODO*/FLAG("-out",     FlagOutput,   "output file"),
	/*TODO*/FLAG("-passout", FlagArgument, "output file password source"),
	/*TODO*/FLAG("-des",     Flag,         "encrypt the generated key with DES in cbc mode"),
	/*TODO*/FLAG("-text",    Flag,         "print the key in text form"),
	/*TODO*/FLAG("-noout",   Flag,         "do not print key out"),
	/*TODO*/FLAG("-modulus", Flag,         "print the RSA key modulus"),
	/*TODO*/FLAG("-check",   Flag,         "verify key consistency"),
	/*TODO*/FLAG("-pubin",   Flag,         "expect a public key in input file"),
	/*TODO*/FLAG("-pubout",  Flag,         "output a public key"),
};
flag_t	rsautl[] = {
	/*TODO*/FLAG("-in",      FlagInputFile, "input file"),
	/*TODO*/FLAG("-out",     FlagOutput,   "output file"),
	/*TODO*/FLAG("-inkey",   FlagArgument, "input key"),
	/*TODO*/FLAG("-pubin",   Flag,         "expect a public key in input file"),
	/*TODO*/FLAG("-encrypt", Flag,         "encrypt with public key"),
	/*TODO*/FLAG("-decrypt", Flag,         "decrypt with private key (default)"),
	/*TODO*/FLAG("-hexdump", Flag,         "hex dump output"),
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

	{ .name = "des"    , .fn_type = OneWayFn, .u.oneway.fn = {des_cbc_cipher, des_print}, .flags = cipher_flags },
	{ .name = "des-ecb", .fn_type = OneWayFn, .u.oneway.fn = {des_ecb_cipher, des_print}, .flags = cipher_flags },
	{ .name = "des-cbc", .fn_type = OneWayFn, .u.oneway.fn = {des_cbc_cipher, des_print}, .flags = cipher_flags },
	{ .name = "des-ede", .fn_type = OneWayFn, .u.oneway.fn = {des_ede_cipher, des_print}, .flags = cipher_flags },

	{ .name = "genrsa" , .fn_type = GeneratorFn, .u.generator.fn = {genrsa, rawtextdump}, .flags = genrsa_flags },
	{ .name = "rsa",     .fn_type = OneWayFn,    .u.oneway.fn = {rsa, rawtextdump},    .flags = rsa_flags },
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