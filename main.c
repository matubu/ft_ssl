#include "malloc.h"
#include "string.h"
#include "readfile.h"
#include "hexdump.h"

#include "commands/md5.h"
#include "commands/sha.h"

typedef struct {
	char		*name;
	string_t	(*fn)(const string_t *);
}	command_t;

int	main(int ac, const char **av) {
	command_t	commands[] = {
		{ .name = "md5", .fn = md5_hash },
		{ .name = "sha256", .fn = sha256_hash },
	};

	// TODO parse flags
	if (ac != 3) {
		usage();
	}

	const char	*command = av[1];
	string_t	file = readfile(av[2]);

	int	commands_count = sizeof(commands) / sizeof(commands[0]);

	for (int i = 0; i < commands_count; ++i) {
		if (cmp(command, commands[i].name) == 0) {
			string_t	hash = commands[i].fn(&file);

			hexdump(hash);

			free(file.ptr);
			free(hash.ptr);

			exit(0);
		}
	}

	free(file.ptr);

	PUTS("ft_ssl: Error: invalid command");
	usage();
}