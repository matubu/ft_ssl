#include "ft_ssl.h"
#include "commands.h"

#include "lib/aflplusplus/argv-fuzz-inl.h"

void	usage(flag_t *flags) {
	PUTS(2, "usage: ft_ssl command [flags] [file/string]");
	PUTS(2, "");
	if (flags) {
		PUTS(2, "Flags:");
		for (size_t i = 0; flags[i].type != FlagNone; ++i) {
			PUTSTR(2, "  ");
			putstr(2, flags[i].name);
			PUTSTR(2, "  ");
			putstr(2, flags[i].description);
			PUTS(2, "");
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

arguments_t	parsearguments(const char **av) {
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

	args.flags = args.command->flags;

	int			read_stdin = 1;

	while (*av && **av == '-') {
		if ((*av)[1] == '\0') {
			HELP_AND_DIE(args.flags, "invalid flag", *av);
		}

		flag_t	*flag = get_flag(args.flags, *av);

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
					push_input(&args.inputs, string_from_ptr(*av), InputFile, file);
				}
			}
			if (flag->type == FlagOutput) {
				if (args.out_fd != 1)
					close(args.out_fd);
				args.out_fd = open(*av, O_WRONLY | O_TRUNC | O_CREAT, 0644);
				if (args.out_fd < 0) {
					DIE((*av), "cannot open out file", strerror(errno));
				}
			} 
		}

		++av;
	}

	while (*av) {
		string_t	file;

		read_stdin = 0;
		if (readfile(*av, &file) == 0) {
			push_input(&args.inputs, string_from_ptr(*av), InputFile, file);
		}
		++av;
	}

	if (args.command->fn_type != GeneratorFn && read_stdin) {
		string_t	stdin_input = readall(0);

		push_input(&args.inputs, (string_t){ .ptr = NULL, .len = 0 }, InputStdin, stdin_input);
	}

	return (args);
}

void	exec_command(const t_func *fn, const input_t *input, const arguments_t *args) {
	const string_t *input_str = NULL;

	if (input) {
		input_str = &input->str;
	}

	string_t	output = ((string_t (*)(const string_t *, const arguments_t *))fn->fn)(input_str, args);

	((void (*)(int, const string_t *, const input_t *, const arguments_t *))
		fn->print_fn)(args->out_fd, &output, input, args);

	free(output.ptr);
}

int	main(int argc, char **argv) {
	(void)argc;
	// AFL_INIT_ARGV();

	arguments_t	args = parsearguments((const char **)argv + 1);

	if (args.command->fn_type == GeneratorFn) {

		exec_command(&args.command->u.generator.fn, NULL, &args);

	}
	else {

		input_t	*ptr = args.inputs;

		while (ptr) {

			const t_func	*fn;
			if (args.command->fn_type == OneWayFn) {
				fn = &args.command->u.oneway.fn;
			} else {
				if (get_flag(args.flags, "-d")->present)
					fn = &args.command->u.twoway.d;
				else
					fn = &args.command->u.twoway.e;
			}
			
			exec_command(fn, ptr, &args);

			input_t	*next = ptr->next;
			free(ptr->str.ptr);
			free(ptr);
			ptr = next;

		}

	}

	if (args.out_fd != 1)
		close(args.out_fd);
}