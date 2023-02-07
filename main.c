#include "ft_ssl.h"
#include "commands.h"

void	usage(flag_t *flags) {
	PUTS(2, "usage: ft_ssl command [flags] [file/string]");
	PUTS(2, "");
	if (flags) {
		PUTS(2, "Flags:");
		for (size_t i = 0; i < 256; ++i) {
			if (flags[i].type != FlagNone) {
				PUTSTR(2, "  -");
				write(2, &i, 1);
				PUTSTR(2, "  ");
				putstr(2, flags[i].description);
				PUTS(2, "");
			}
		}
		PUTS(2, "");
	}
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

#define HELP_AND_DIE(flags, ...) { ERROR(__VA_ARGS__); PUTS(2, ""); usage(flags); }

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
		HELP_AND_DIE(NULL, "missing command");
	}

	arguments_t args = {
		.command = get_command(*av++),
		.out_fd = 1
	};
	if (args.command == NULL) {
		HELP_AND_DIE(NULL, "no such command");
	}
	for (size_t i = 0; i < 256; ++i) {
		args.flags[i] = args.command->flags[i];
	}

	int			read_stdin = 1;

	while (*av && **av == '-') {
		if ((*av)[1] == '\0' || (*av)[2] != '\0') {
			HELP_AND_DIE(args.flags, "invalid flag", *av);
		}

		flag_t	*flag = &args.flags[(uint8_t)(*av)[1]];

		if (flag->type == FlagNone) {
			HELP_AND_DIE(args.flags, "invalid flag", *av);
		}

		flag->present = 1;
		if (flag->type == FlagArgument || flag->type == FlagInput || flag->type == FlagInputFile || flag->type == FlagOutput) {
			if (*++av == NULL) {
				HELP_AND_DIE(args.flags, "expected a string after flag", av[-1]);
			}
			
			flag->argument = *av;

			if (flag->type == FlagInput) {
				read_stdin = 0;
				push_input(&args.inputs, (string_t){ .ptr = NULL, .len = 0 }, InputArgument, string_from_chars(*av));
			}
			if (flag->type == FlagInputFile) {
				string_t	file;

				read_stdin = 0;
				if (readfile(*av, &file) == 0) {
					push_input(&args.inputs, string_from_ptr((uint8_t *)*av), InputFile, file);
				}
			}
			if (flag->type == FlagOutput) {
				if (args.out_fd != 1)
					close(args.out_fd);
				args.out_fd = open(*av, O_WRONLY | O_TRUNC | O_CREAT, 0644);
				if (args.out_fd < 0)
					DIE(NULL, *av, "cannot open out file", strerror(errno));
			} 
		}

		++av;
	}

	while (*av) {
		string_t	file;

		read_stdin = 0;
		if (readfile(*av, &file) == 0) {
			push_input(&args.inputs, string_from_ptr((uint8_t *)*av), InputFile, file);
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

		const t_func	*fn;
		if (args.command->fn_type == OneWayFn) {
			fn = &args.command->u.oneway.fn;
		} else {
			if (args.flags['d'].present)
				fn = &args.command->u.twoway.d;
			else
				fn = &args.command->u.twoway.e;
		}
		string_t	output = ((string_t (*)(const string_t *, const arguments_t *))fn->fn)
			(&ptr->str, &args);

		((void (*)(int, const string_t *, const input_t *, const arguments_t *))
			fn->print_fn)(args.out_fd, &output, ptr, &args);

		free(ptr->str.ptr);
		free(ptr);
		free(output.ptr);
		ptr = next;
	}

	if (args.out_fd != 1)
		close(args.out_fd);
}