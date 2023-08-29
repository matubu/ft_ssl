#pragma once

#include "lib/random.h"

#define PRIME_PRECISION 1024

// Fermat's little theorem
// https://www.youtube.com/watch?v=OoQ16YCYksw

// Miller–Rabin primality test
// https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test

// (n ** e) % p
static inline uint64_t power(uint64_t n, uint64_t e, uint64_t p) {
	uint64_t res = 1;
	n %= p;

	while (e > 0) {
		if (e & 1) {
			res = ((uint128_t)res * n) % p;
		}

		n = ((uint128_t)n * n) % p;
		e = e >> 1;
	}

	return res;
}

static inline int128_t mul_inv(int128_t a, int128_t b)
{
	int128_t b0 = b, t, q;
	int128_t x0 = 0, x1 = 1;
	if (b == 1) return 1;
	while (a > 1) {
		q = a / b;
		t = b, b = a % b, a = t;
		t = x0, x0 = x1 - q * x0, x1 = t;
	}
	if (x1 < 0) x1 += b0;
	return x1;
}

int _miller_rabin_is_composite(uint64_t d, uint64_t n) {
	uint64_t a = random_range(2, n-1);

	uint64_t x = power(a, d, n);

	if (x == 1 || x == n - 1) {
		return 0;
	}

	while (d != n-1) {
		x = ((uint128_t)x * x) % n;

		if (x == n - 1) {
			return 0;
		}

		d *= 2;
	}

	return 1;
}

int miller_rabin(uint64_t n, uint64_t k) {
	if (n <= 4) {
		return (n == 2 || n == 3);
	}

	uint64_t d = n - 1;
	while (d % 2 == 0) {
		d /= 2;
	}

	while (k--) {
		if (_miller_rabin_is_composite(d, n)) {
			return 0;
		}
	}

	return 1;
}

uint64_t random_prime(uint64_t bits) {
	while (1) {
		uint64_t n = random_range((uint64_t)1 << (bits - 1), ((uint64_t)1 << bits) - 2) | 1;

		if (miller_rabin(n, PRIME_PRECISION)) {
			return n;
		}
	}
}
