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

typedef struct input_list_s {
	const char			*display_str;
	string_t			str;         // Needs to be free
	struct input_list_s	*next;
}	input_list_t;

typedef struct {
	const command_t	*command;
	int				print_stdout;
	int				quiet;
	int				reverse_format;
	input_list_t	*inputs;        // Needs to be free
}	arguments_t;

const command_t	commands[] = {
	{ .name = "md5", .fn = md5_hash },
	{ .name = "sha256", .fn = sha256_hash },
};

const command_t	*get_command(const char *command) {
	size_t	commands_count = sizeof(commands) / sizeof(commands[0]);

	for (size_t i = 0; i < commands_count; ++i) {
		if (cmp(command, commands[i].name) == 0) {
			return (&commands[i]);
		}
	}
	HELP_AND_DIE("no such command");
	return (NULL);
}

void	push_input(input_list_t **lst, const char *display_str, string_t str) {
	input_list_t	*new = malloc(sizeof(*new));
	new->display_str = display_str;
	new->str = str;
	new->next = *lst;
	*lst = new;
}

arguments_t	parse_arguments(const char **av) {
	if (av == NULL || *av == NULL) {
		usage();
	}

	arguments_t args = {
		.command = get_command(*av++),
		.print_stdout = 0,
		.quiet = 0,
		.reverse_format = 0,
		.inputs = NULL
	};

	while (*av && **av == '-') {
		if ((*av)[1] == '\0' || (*av)[2] != '\0') {
			HELP_AND_DIE("not supported flag");
		}

		switch ((*av)[1]) {
			case 'p': args.print_stdout = 1;   break ;
			case 'q': args.quiet = 1;          break ;
			case 'r': args.reverse_format = 1; break ;
			case 's':
				if (*++av == NULL) {
					HELP_AND_DIE("expected a string after -s");
				}

				push_input(&args.inputs, *av, stringdup(*av));
			break ;
			default:
				HELP_AND_DIE("not supported flag");
		}

		++av;
	}

	while (*av) {
		push_input(&args.inputs, *av, readfile(*av));
		++av;
	}

	if (args.inputs == NULL) {
		HELP_AND_DIE("missing input file/string");
	}

	return (args);
}

#include <stdio.h>

int	main(int ac, const char **av) {
	(void)ac;
	arguments_t	opt = parse_arguments(av + 1);

	input_list_t	*ptr = opt.inputs;

	while (ptr) {
		input_list_t	*next = ptr->next;

		string_t	hash = opt.command->fn(&ptr->str);

		printf("%s (%s) = ", opt.command->name, ptr->display_str); fflush(stdout);
		hexdump(hash);

		free(ptr->str.ptr);
		free(ptr);
		free(hash.ptr);
		ptr = next;
	}
}