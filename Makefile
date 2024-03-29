Name = ft_ssl
Srcs = src/ft_ssl.c
Deps = $(shell find . -type f -name '*.h') Makefile
Flag = -Wall -Wextra -Werror -Isrc -g -fsanitize=address -Ofast -lm

CC = gcc

all: $(Name)

$(Name): $(Srcs) $(Deps)
	$(CC) $(Flag) -o $@ $(Srcs)

clean:

fclean:
	rm -rf $(Name)

re: fclean all

fuzz_build:
	make re CC=afl-clang-fast

fuzz_run:
	# check if ID variable is set
ifndef ID
	$(error ID is not set)
endif
	afl-fuzz -M $(ID) -i seeds -o out -D -- ./ft_ssl

fuzz_run_secondary:
	# check if ID variable is set
ifndef ID
	$(error ID is not set)
endif
	afl-fuzz -S $(ID) -i seeds -o out -D -- ./ft_ssl

fuzz_secondary:
	# check if ID variable is set
ifndef ID
	$(error ID is not set)
endif
ifndef CPUID
	$(error CPUID is not set)
endif
	docker run --cpuset-cpus=$(CPUID) -w=/src -ti -v $(shell pwd)/fuzz/env:/src aflplusplus/aflplusplus make fuzz_run_secondary ID=$(ID)

fuzz_retrieve:
	doas cp fuzz/env/out/*/hangs/* hangs
	doas cp fuzz/env/out/*/crashes/* crashes/
	doas chown -R $(shell id -u):$(shell id -g) fuzz hangs crashes

# doas watch make fuzz_cleanup
fuzz_cleanup:
	find fuzz/env/ -type f -maxdepth 1 -not -name 'out' -not -name 'seeds' -not -name 'ft_ssl' -not -name 'Makefile' -delete
	cp -r fuzz/files/* fuzz/env

fuzz:
	# check if ID variable is set
ifndef ID
	$(error ID is not set)
endif
	docker pull aflplusplus/aflplusplus
	docker run -w=/src -ti -v $(shell pwd):/src aflplusplus/aflplusplus make fuzz_build

	doas chown -R $(shell id -u):$(shell id -g) fuzz
	mkdir -p fuzz/env
	cp -r fuzz/seeds fuzz/env
	cp -r fuzz/files/* fuzz/env
	cp ft_ssl fuzz/env
	cp Makefile fuzz/env

	docker run -w=/src -ti -v $(shell pwd)/fuzz/env:/src aflplusplus/aflplusplus make fuzz_run ID=$(ID)

# 96 edges

test: all
	echo "$$(curl 'https://google.com')" > original.html
	echo -n "password" > password_file

	# openssl des-ecb -pbkdf2 -S 4242 -p -in original.html -out ciphertext.html -pass "pass:$$(cat password_file)"
	# ./ft_ssl des-ecb -d -i ciphertext.html -o decrypted.html -p "$$(cat password_file)"
	# ./ft_ssl des-ecb -s 4242 -d -i ciphertext.html -o decrypted.html -p "$$(cat password_file)"

	./ft_ssl des-ecb -s 4242 -i original.html -o ciphertext.html -p "$$(cat password_file)"
	openssl des-ecb -pbkdf2 -d -p -in ciphertext.html -out decrypted.html -pass "pass:$$(cat password_file)"

	# open decrypted.html


	./ft_ssl des -i main.c -v 0 -p "des" | openssl des -pbkdf2 -d -iv 0 -p -pass "pass:des"


	openssl des-ede-cbc -pbkdf2 -pass "pass:password" -in main.c | openssl des-ede-cbc -pbkdf2 -pass "pass:password" -d
	./ft_ssl des-ede -i main.c -p "test" -o out; openssl des-ede -in out -pbkdf2 -pass "pass:test" -d
	openssl des-ede -in main.c -pbkdf2 -pass "pass:test" -out out; ./ft_ssl des-ede -p "test" -i out -d
	openssl des-ede -in main.c -K 12345678901234567890 -out out -p; ./ft_ssl des-ede -k 12345678901234567890 -i out -d

clean_test:
	rm original.html ciphertext.html decrypted.html password_file out


.PHONY: all clean fclean re fuzz test clean_test
