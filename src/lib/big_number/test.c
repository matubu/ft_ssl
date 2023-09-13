#include "big_number.h"

int main(int ac, char **av) {
	if (ac != 3) {
		return 1;
	}

	big_number_t *a = big_number_from_string(av[1]);
	big_number_t *b = big_number_from_string(av[2]);

	big_number_t *c = big_number_add(a, b);
	big_number_t *d = big_number_mult(a, b);

	big_number_print(a);
	big_number_print(b);
	big_number_print(c);
	big_number_print(d);
}