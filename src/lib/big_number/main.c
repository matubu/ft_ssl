#include "big_number.h"

int main() {
	// {
	// 	big_number_t *a = big_number_create(1337);
	// 	big_number_t *b = big_number_create(42);
	// 	big_number_t *c = big_number_add(a, b);
	// 	big_number_t *d = big_number_create(0);
	// 	big_number_t *e = big_number_add(c, b);

	// 	printf("a=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(a).chunk_index);
	// 	printf("b=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(b).chunk_index);
	// 	printf("c=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(c).chunk_index);
	// 	printf("d=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(d).chunk_index);
	// 	printf("e=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(e).chunk_index);

	// 	big_number_print(a);
	// 	big_number_print(b);
	// 	big_number_print(c);
	// 	big_number_print(d);
	// 	big_number_print(e);
	// }

	// {
	// 	big_number_t *a = big_number_create((uint64_t)1 << 63);
	// 	big_number_t *b = big_number_create((uint64_t)1 << 63);
	// 	big_number_t *c = big_number_add(a, b);
	// 	big_number_t *d = big_number_add(c, c);
	// 	big_number_t *e = big_number_add(d, a);

	// 	printf("a=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(a).chunk_index);
	// 	printf("b=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(b).chunk_index);
	// 	printf("c=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(c).chunk_index);
	// 	printf("d=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(d).chunk_index);
	// 	printf("e=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(e).chunk_index);

	// 	big_number_print(a);
	// 	big_number_print(b);
	// 	big_number_print(c);
	// 	big_number_print(d);
	// 	big_number_print(e);
	// }

	{
		big_number_t *a = big_number_create((uint64_t)1 << 63);
		big_number_t *b = big_number_create((uint64_t)1 << 63);
		big_number_t *c = big_number_mult(a, b);

		// printf("a=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(a).chunk_index);
		// printf("b=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(b).chunk_index);
		// printf("c=\x1b[92m%llu\x1b[0m\n", big_number_left_most_bit(c).chunk_index);

		big_number_print(a);
		big_number_print(b);
		big_number_print(c);
	}
}