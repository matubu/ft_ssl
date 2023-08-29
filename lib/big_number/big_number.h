// #pragma once

// #include <stdint.h>
// #include <stdlib.h>
// #include <stdio.h>


// // TODO remove printf



// #undef MAX
// #define MAX(a, b) ({ \
// 	typeof(a) _a = (a); \
// 	typeof(b) _b = (b); \
// 	_a > _b ? _a : _b; \
// })

// #undef MIN
// #define MIN(a, b) ({ \
// 	typeof(a) _a = (a); \
// 	typeof(b) _b = (b); \
// 	_a < _b ? _a : _b; \
// })

// #undef SWAP
// #define SWAP(a, b) ({ \
// 	typeof(a) _a = (a); \
// 	typeof(b) _b = (b); \
// 	typeof(a) _tmp = _a; \
// 	_a = _b; \
// 	_b = _tmp; \
// })

// uint64_t len(const char *s) {
// 	uint64_t i = 0;
// 	while (s[i] != '\0') {
// 		++i;
// 	}
// 	return i;
// }




// #define MASK_32BIT (((uint64_t)1 << 32) - 1)





// typedef struct {
// 	uint32_t *data; // little endian
// 	uint64_t size;
// } big_number_t;


// big_number_t *big_number_allocate(uint64_t size) {
// 	big_number_t *number = malloc(sizeof(big_number_t));
// 	if (number == NULL) {
// 		goto allocation_error;
// 	}

// 	number->data = malloc(size * sizeof(uint64_t));
// 	if (number->data == NULL) {
// 		goto allocation_error;
// 	}
// 	for (uint64_t i = 0; i < size; ++i) {
// 		number->data[i] = 0;
// 	}
// 	number->size = size;

// 	return number;

// allocation_error:
// 	fprintf(stderr, "\x1b[91mError\x1b[0m: could not allocate memory for big number\n");
// 	exit(1);
// }


// big_number_t *big_number_create(uint64_t value) {
// 	big_number_t *number = big_number_allocate(2);

// 	number->data[0] = value & (((uint64_t)1 << 32) - 1);
// 	number->data[1] = value >> 32;

// 	return number;
// }


// big_number_t *big_number_copy(big_number_t *number) {
// 	big_number_t *copy = big_number_allocate(number->size);

// 	for (uint64_t i = 0; i < number->size; ++i) {
// 		copy->data[i] = number->data[i];
// 	}

// 	return copy;
// }


// big_number_t *big_number_from_string(const char *s) {
// 	int8_t base16_map[256];
// 	for (uint64_t i = 0; i < 256; ++i) {
// 		base16_map[i] = -1;
// 	}
// 	for (uint64_t i = 0; i < 10; ++i) {
// 		base16_map['0' + i] = i;
// 	}
// 	for (uint64_t i = 0; i < 6; ++i) {
// 		base16_map['a' + i] = 10 + i;
// 		base16_map['A' + i] = 10 + i;
// 	}

// 	uint64_t length = len(s);
// 	big_number_t *number = big_number_allocate((length + 7) / 8);

// 	for (uint64_t i = 0; i < length; ++i) {
// 		number->data[i / 8] += base16_map[(uint8_t)s[length - i - 1]] << ((i % 8) * 4);
// 	}

// 	return number;
// }


// void big_number_print(big_number_t *number) {
// 	for (uint64_t i = number->size; i-- > 0;) {
// 		printf("%08x", number->data[i]);
// 	}
// 	printf("\n");
// }


// void big_number_destroy(big_number_t **number) {
// 	free((*number)->data);
// 	free((*number));
// 	*number = NULL;
// }


// typedef struct {
// 	uint64_t chunk_index;
// 	uint64_t index_in_chunk;
// }	bit_position_t;

// bit_position_t big_number_left_most_bit(big_number_t *n) {
// 	for (uint64_t i = n->size; i-- > 0;) {
// 		if (n->data[i] == 0) {
// 			continue;
// 		}

// 		uint64_t mask_position = 16;
// 		uint64_t mask_size = 16;

// 		while (mask_size > 1) {
// 			if ((n->data[i] & (((1 << mask_size) - 1) << mask_position)) == 0) {
// 				mask_position -= mask_size / 2;
// 			}

// 			mask_size /= 2;
// 			mask_position += mask_size;
// 		}

// 		return (bit_position_t){i, mask_position};
// 	}

// 	return (bit_position_t){0, 0};
// }


// static inline uint64_t big_number_get_chunk(big_number_t *n, uint64_t chunk_index) {
// 	if (chunk_index >= n->size) {
// 		return 0;
// 	}

// 	return n->data[chunk_index];
// }

// uint64_t big_number_get_real_size(big_number_t *n) {
// 	bit_position_t left_most_bit = big_number_left_most_bit(n);
// 	return left_most_bit.chunk_index + 1;
// }


// big_number_t *big_number_add(big_number_t *a, big_number_t *b) {
// 	uint64_t new_size = MAX(
// 		big_number_get_real_size(a),
// 		big_number_get_real_size(b)) + 1;
// 	big_number_t *result = big_number_allocate(new_size);

// 	uint64_t carry = 0;
// 	for (uint64_t i = 0; i < (result->size - 1); ++i) {
// 		uint64_t sum = (uint64_t)big_number_get_chunk(a, i) + big_number_get_chunk(b, i) + carry;
// 		result->data[i] = sum;
// 		carry = sum >> 32;
// 	}

// 	result->data[result->size - 1] = carry;

// 	return result;
// }


// big_number_t *big_number_mult(big_number_t *a, big_number_t *b) {
// 	uint64_t a_size = big_number_get_real_size(a);
// 	uint64_t b_size = big_number_get_real_size(b);

// 	uint64_t new_size = a_size + b_size;
// 	big_number_t *result = big_number_allocate(new_size);

// 	// c = [i] * [j]
// 	// [i + j] = c % (2^32-1)
// 	// move overflow to [i + j + 1]...

// 	for (uint64_t a_idx = 0; a_idx < a_size; ++a_idx) {
// 		for (uint64_t b_idx = 0; b_idx < b_size; ++b_idx) {
// 			uint64_t chunk = big_number_get_chunk(a, a_idx) * big_number_get_chunk(b, b_idx);
// 			uint64_t carry = chunk >> 32;

// 			chunk = result->data[a_idx + b_idx] + (chunk & MASK_32BIT);
// 			result->data[a_idx + b_idx] = chunk & MASK_32BIT;

// 			carry += (chunk >> 32);

// 			for (uint64_t i = a_idx + b_idx + 1; i < new_size; ++i) {

// 				if (carry == 0) {
// 					break;
// 				}

// 				chunk = (uint64_t)result->data[i] + carry;
// 				result->data[i] = chunk & MASK_32BIT;

// 				carry = chunk >> 32;

// 			}
// 		}
// 	}

// 	return result;
// }


// // prime probabilty


// // modulor


// // big_number_t *big_number_pow(big_number_t *n, big_number_t *e, big_number_t *m) {

// // }

