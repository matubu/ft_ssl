Name = ft_ssl
Srcs = main.c
Deps = $(shell find . -type f -name '*.h') Makefile
Flag = -Wall -Wextra -Werror -I. -g -fsanitize=address

all: $(Name)

$(Name): $(Srcs) $(Deps)
	gcc $(Flag) -o $@ $(Srcs)

clean:

fclean:
	rm -rf $(Name)

re: fclean all

.PHONY = all clean fclean re