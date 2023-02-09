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

test:
	echo "$(curl 'https://google.com')" > original.html
	echo "password" > password_file
	openssl des-ecb -p -in original.html -out ciphertext.html -pass "pass::$(cat password_file)"
	./ft_ssl des-ecb -d -i ciphertext.html -o decrypted.html -p "$(cat password_file)"
	open decrypted.html

.PHONY = all clean fclean re test