#include "malloc.h"
#include "io.h"
#include "readfile.h"
#include "ft_ssl.h"

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

void	push_input(input_t **lst, string_t display_str, string_t str) {
	input_t	*new = malloc(sizeof(*new));
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
	int			read_stdin = 1;

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

				read_stdin = 0;
				push_input(&args.inputs, string_ptr((uint8_t *)*av), string_dup(*av));
			break ;
			default:
				HELP_AND_DIE("invalid flag", *av);
		}

		++av;
	}

	while (*av) {
		string_t	file;

		read_stdin = 0;
		if (readfile(*av, &file) == 0) {
			push_input(&args.inputs, string_ptr((uint8_t *)*av), file);
		}
		++av;
	}

	if (read_stdin) {
		string_t	stdin_input = readall(0);

		push_input(&args.inputs, args.print_stdin ? stdin_input : string_ptr((uint8_t *)"stdin"), stdin_input);
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