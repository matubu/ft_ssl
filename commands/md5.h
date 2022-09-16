#pragma once

#include "malloc.h"
#include "string.h"

#include "hexdump.h"
#include "bindump.h"

#include <stdio.h>

// nice links:
// https://fthb321.github.io/MD5-Hash/MD5OurVersion2.html
// https://en.wikipedia.org/wiki/MD5
// https://www.herongyang.com/Cryptography/MD5-Message-Digest-Algorithm-Overview.html

string_t	md5_pad_message(const string_t s) {
	string_t	buf;

	buf.len = s.len + 1 + 4;
	if (buf.len % 64) {
		buf.len += 64 - (buf.len % 64);
	}
	buf.ptr = malloc(buf.len);

	size_t	i = 0;

	while (i < s.len) {
		buf.ptr[i] = s.ptr[i];
		++i;
	}
	buf.ptr[i++] = 0x80;

	while (i % 64 != 56) {
		buf.ptr[i++] = 0;
	}

	uint64_t	message_size = s.len * 8;
	for (uint8_t j = 0; j < 8; ++j) {
		buf.ptr[i + j] = ((uint8_t *)&message_size)[7 - j];
	}

	hexdump_with_preview(buf);
	bindump(buf);

	return (buf);
}

uint32_t	leftrotate(uint32_t n, uint32_t offset) {
	return ((n << offset) | (n >> (32 - offset)));
}

string_t	md5_hash(const string_t str) {
	string_t	buf = md5_pad_message(str);

	uint32_t	s[64] = {
		7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
		5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
	};
	uint32_t	K[64] = {
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
	};

	// Maybe problems due to endianess when later converted to uint8_t
	uint32_t	*digest = malloc(4 * sizeof(uint32_t));
	digest[0] = 0x67452301;
	digest[1] = 0xefcdab89;
	digest[2] = 0x98badcfe;
	digest[3] = 0x10325476;

	for (size_t j = 0; j < buf.len; j += 64) {
		printf("chunk %zu\n", j);

		uint32_t	*M = (uint32_t *)(buf.ptr + j);

		uint32_t	a = digest[0];
		uint32_t	b = digest[1];
		uint32_t	c = digest[2];
		uint32_t	d = digest[3];

		for (int i = 0; i < 64; ++i) {
			uint32_t	F, g;
			if (i < 16) {
				F = d ^ (b & (c ^ d));
				g = i;
			}
			else if (i < 32) {
				F = c ^ (d & (b ^ c));
				g = (5 * i + 1) % 16;
			}
			else if (i < 48) {
				F = b ^ c ^ d;
				g = (3 * i + 5) % 16;
			}
			else {
				F = c ^ (b | (~d));
				g = (7 * i) % 16;
			}
			F += a + K[i] + M[g];
			a = d;
			d = c;
			c = b;
			b = b + leftrotate(F, s[i]);
		}

		digest[0] += a;
		digest[1] += b;
		digest[2] += c;
		digest[3] += d;
	}

	return ((string_t){ .ptr = (uint8_t *)digest, .len = 16 });
}