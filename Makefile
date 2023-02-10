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

test: all
	echo "$$(curl 'https://google.com')" > original.html
	echo -n "password" > password_file

	# openssl des-ecb -pbkdf2 -S 4242 -p -in original.html -out ciphertext.html -pass "pass:$$(cat password_file)"
	# ./ft_ssl des-ecb -d -i ciphertext.html -o decrypted.html -p "$$(cat password_file)"
	# ./ft_ssl des-ecb -s 4242 -d -i ciphertext.html -o decrypted.html -p "$$(cat password_file)"

	./ft_ssl des-ecb -s 4242 -i original.html -o ciphertext.html -p "$$(cat password_file)"
	openssl des-ecb -pbkdf2 -d -p -in ciphertext.html -out decrypted.html -pass "pass:$$(cat password_file)"

	open decrypted.html


	 ./ft_ssl des -i main.c -v 0 -p "des" | openssl des -pbkdf2 -d -iv 0 -p -pass "pass:des"

clean_test:
	rm original.html ciphertext.html decrypted.html password_file

.PHONY = all clean fclean re test clean_test