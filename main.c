#include "malloc.h"
#include "io.h"
#include "readfile.h"
#include "hexdump.h"

#include "commands/md5.h"
#include "commands/sha.h"

// TODO add support for sha224, sha384, sha512, whirlpool
// BUG if none of the files could be open, the program read stdin, but should not

typedef struct {
	char		*name;
	string_t	(*fn)(const string_t *);
}	command_t;

typedef struct input_list_s {
	string_t			display_str;
	string_t			str;         // Needs to be free
	struct input_list_s	*next;
}	input_list_t;

typedef struct {
	const command_t	*command;
	int				print_stdin;
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

void	push_input(input_list_t **lst, string_t display_str, string_t str) {
	input_list_t	*new = malloc(sizeof(*new));
	new->display_str = display_str;
	new->str = str;
	new->next = *lst;
	*lst = new;
}

arguments_t	parse_arguments(const char **av) {
	if (av == NULL || *av == NULL) {
		HELP_AND_DIE("missing command");
	}

	arguments_t args = {
		.command = get_command(*av++),
		.print_stdin = 0,
		.quiet = 0,
		.reverse_format = 0,
		.inputs = NULL
	};

	while (*av && **av == '-') {
		if ((*av)[1] == '\0' || (*av)[2] != '\0') {
			HELP_AND_DIE("invalid flag", *av);
		}

		switch ((*av)[1]) {
			case 'p': args.print_stdin  = 1;   break ;
			case 'q': args.quiet = 1;          break ;
			case 'r': args.reverse_format = 1; break ;
			case 's':
				if (*++av == NULL) {
					HELP_AND_DIE("expected a string after -s");
				}

				push_input(&args.inputs, string((uint8_t *)*av), stringdup(*av));
			break ;
			default:
				HELP_AND_DIE("invalid flag", *av);
		}

		++av;
	}

	while (*av) {
		string_t	file;

		if (readfile(*av, &file) == 0) {
			push_input(&args.inputs, string((uint8_t *)*av), file);
		}
		++av;
	}

	if (args.inputs == NULL) {
		string_t	stdin_input = readall(0);

		push_input(&args.inputs, args.print_stdin ? stdin_input : string((uint8_t *)"stdin"), stdin_input);
	}

	return (args);
}

int	main(int ac, const char **av) {
	(void)ac;
	arguments_t	args = parse_arguments(av + 1);

	input_list_t	*ptr = args.inputs;

	while (ptr) {
		input_list_t	*next = ptr->next;

		string_t	hash = args.command->fn(&ptr->str);

		if (args.reverse_format) {
			hexdump(hash);
			if (!args.quiet) {
				PUTSTR(" ");
				print_escape(&ptr->display_str);
			}
		} else {
			if (!args.quiet) {
				putstr(args.command->name);
				PUTSTR("(");
				print_escape(&ptr->display_str);
				PUTSTR(") = ");
			}
			hexdump(hash);
		}
		PUTS("");

		free(ptr->str.ptr);
		free(ptr);
		free(hash.ptr);
		ptr = next;
	}
}