#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "ft_ssl.h"
#include "malloc.h"
#include "string.h"

string_t	readfile(const char *filename) {
	int	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		PUTS("ft_ssl: Error: cannot open file");
		usage();
	}

	struct stat	stat;
	if (fstat(fd, &stat) < 0) {
		PUTS("ft_ssl: Error: cannot stat file");
		usage();
	}

	uint8_t		*ptr = malloc(stat.st_size);
	string_t	string = {
		.ptr = ptr,
		.len = read(fd, ptr, stat.st_size)
	};

	close(fd);

	return (string);
}