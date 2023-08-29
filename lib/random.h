#pragma once

#include "lib/utils.h"
#include "lib/io.h"

uint64_t random_uint64() {
	uint64_t	rand;
	int			fd = open("/dev/urandom", O_RDONLY);

	if (fd < 0) {
		DIE("cannot open /dev/urandom", strerror(errno))
	}

	read(fd, &rand, sizeof(rand));

	close(fd);

	return rand;
}

// TODO unfortunately, this is not uniform maybe need to be fixed
// min inclusive, max exclusive
uint64_t random_range(uint64_t min, uint64_t max) {
	return min + (random_uint64() % (max - min));
}

double random_double() {
	uint64_t	rand;
	int			fd = open("/dev/urandom", O_RDONLY);

	if (fd < 0) {
		DIE("cannot open /dev/urandom", strerror(errno))
	}

	do {
		read(fd, &rand, sizeof(rand));
	} while (rand == 0);

	close(fd);

	return (double)(rand - 1) / (double)(UINT64_MAX);
}

// https://mbed-tls.readthedocs.io/en/latest/kb/cryptography/asn1-key-structures-in-der-and-pem/
// https://www.openssl.org/docs/man1.0.2/man1/genrsa.html