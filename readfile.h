#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "malloc.h"
#include "string.h"

string_t	readfile(const char *filename) {
	int	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		DIE("cannot open file");
	}

	struct stat	stat;
	if (fstat(fd, &stat) < 0) {
		DIE("cannot stat file");
	}

	uint8_t		*ptr = malloc(stat.st_size);
	string_t	string = {
		.ptr = ptr,
		.len = read(fd, ptr, stat.st_size)
	};

	close(fd);

	return (string);
}