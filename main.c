#include "ft_ssl.h"

// TODO change usage to print flags dynamically
void	usage(void) {
	PUTS(2, "usage: ft_ssl command [flags] [file/string]");
	PUTS(2, "");
	PUTS(2, "Flags:");
	PUTS(2, "  -p          echo STDIN to STDOUT and append the checksum to STDOUT");
	PUTS(2, "  -q          quiet mode");
	PUTS(2, "  -r          reverse the format of the output");
	PUTS(2, "  -s STRING   print the sum of the given string");
	PUTS(2, "");
	PUTSTR(2, "Commands: ");
	for (size_t i = 0; i < commands_count; ++i) {
		if (i) {
			PUTSTR(2, ", ");
		}
		putstr(2, commands[i].name);
	}
	PUTS(2, "");
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

// TODO fix if their is an error the out file will still be open
arguments_t	parse_arguments(const char **av) {
	if (av == NULL || *av == NULL) {
		HELP_AND_DIE("missing command");
	}

	arguments_t args = {
		.command = get_command(*av++),
		.out_fd = 1
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
		if (flag->type == FlagArgument || flag->type == FlagInput || flag->type == FlagOutput) {
			if (*++av == NULL) {
				HELP_AND_DIE("expected a string after -s");
			}

			if (flag->type == FlagInput) {
				read_stdin = 0;
				push_input(&args.inputs, (string_t){ .ptr = NULL, .len = 0 }, InputArgument, string_dup(*av));
			} if (flag->type == FlagOutput) {
				if (args.out_fd != 1)
					close(args.out_fd);
				args.out_fd = open(*av, O_WRONLY | O_TRUNC | O_CREAT, 0644);
				if (args.out_fd < 0)
					DIE(*av, "cannot open out file", strerror(errno));
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

		void	*fn;
		if (args.command->fn_type == OneWayFn) {
			fn = args.command->u.oneway.fn;
		} else {
			if (args.flags['d'].present)
				fn = args.command->u.twoway.d;
			else
				fn = args.command->u.twoway.e;
		}
		string_t	output = ((string_t (*)(const string_t *, const arguments_t *))fn)
			(&ptr->str, &args);

		((void (*)(int, const string_t *, const input_t *, const arguments_t *))
			args.command->print_fn)(args.out_fd, &output, ptr, &args);

		free(ptr->str.ptr);
		free(ptr);
		free(output.ptr);
		ptr = next;
	}

	if (args.out_fd != 1)
		close(args.out_fd);
}