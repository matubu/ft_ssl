#pragma once

#include <stdint.h>

#include "endianess.h"
#include "string.h"

uint32_t	leftrotate(uint32_t n, uint8_t offset) {
	return ((n << offset) | (n >> (32 - offset)));
}

uint32_t	rightrotate(uint32_t n, uint8_t offset) {
	return ((n >> offset) | (n << (32 - offset)));
}

typedef struct {
	size_t	chunk_byte_count;
	int		length_byte_order;
}	padding_opt;

void		init_chunk_buffer(uint8_t *buffer, const string_t *input, size_t i, const padding_opt *opt) {
	size_t j = 0;
	
	while (j < opt->chunk_byte_count && i < input->len) {
		// need to change depending on endianess
		buffer[j] = input->ptr[i];
		++j;
		++i;
	}

	if (j >= opt->chunk_byte_count)
		return ;

	buffer[j++] = 0x80;

	while (j < opt->chunk_byte_count && j != opt->chunk_byte_count - sizeof(uint64_t)) {
		buffer[j++] = 0;
	}

	if (j >= opt->chunk_byte_count)
		return ;

	*(uint64_t *)&buffer[j] = uint64_endianess(input->len * 8, opt->length_byte_order);
}