#include "malloc.h"
#include "io.h"
#include "readfile.h"
#include "ft_ssl.h"

// TODO change usage to print flags dynamically
void	usage(void) {
	PUTS("usage: ft_ssl command [flags] [file/string]");
	PUTS("");
	PUTS("Flags:");
	PUTS("  -p          echo STDIN to STDOUT and append the checksum to STDOUT");
	PUTS("  -q          quiet mode");
	PUTS("  -r          reverse the format of the output");
	PUTS("  -s STRING   print the sum of the given string");
	PUTS("");
	PUTSTR("Commands: ");
	for (size_t i = 0; i < commands_count; ++i) {
		if (i) {
			PUTSTR(", ");
		}
		putstr(commands[i].name);
	}
	PUTS("");
	exit(1);
}

void	push_input(input_t **lst, string_t filename, int origin, string_t str) {
	input_t	*new = malloc(sizeof(*new));
	new->filename = filename;
	new->origin = origin;
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
		.inputs = NULL
	};
	for (size_t i = 0; i < 256; ++i)
		args.flags[i] = args.command->flags[i];

	int			read_stdin = 1;

	while (*av && **av == '-') {
		if ((*av)[1] == '\0' || (*av)[2] != '\0') {
			HELP_AND_DIE("invalid flag", *av);
		}

		flag_t	*flag = &args.flags[(uint8_t)(*av)[1]];

		if (flag->type == FlagNone) {
			HELP_AND_DIE("invalid flag", *av);
		}

		flag->present = 1;
		if (flag->type == FlagArgument || flag->type == FlagInput) {
			if (*++av == NULL) {
				HELP_AND_DIE("expected a string after -s");
			}

			if (flag->type == FlagInput) {
				read_stdin = 0;
				push_input(&args.inputs, (string_t){ .ptr = NULL, .len = 0 }, InputArgument, string_dup(*av));
			} else {
				flag->argument = *av;
			}
		}

		++av;
	}

	while (*av) {
		string_t	file;

		read_stdin = 0;
		if (readfile(*av, &file) == 0) {
			push_input(&args.inputs, string_ptr((uint8_t *)*av), InputFile, file);
		}
		++av;
	}

	if (read_stdin) {
		string_t	stdin_input = readall(0);

		push_input(&args.inputs, (string_t){ .ptr = NULL, .len = 0 }, InputStdin, stdin_input);
	}

	return (args);
}

int	main(int ac, const char **av) {
	(void)ac;
	arguments_t	args = parse_arguments(av + 1);

	input_t	*ptr = args.inputs;

	while (ptr) {
		input_t	*next = ptr->next;

		string_t	hash = args.command->fn(&ptr->str);
		((void (*)(const string_t *, const input_t *, const arguments_t *))
			args.command->print_fn)(&hash, ptr, &args);

		free(ptr->str.ptr);
		free(ptr);
		free(hash.ptr);
		ptr = next;
	}
}