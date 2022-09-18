#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "malloc.h"
#include "string.h"

int	readfile(const char *filename, string_t *string) {
	int	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		ERROR("cannot open file");
		return (-1);
	}

	struct stat	stat;
	if (fstat(fd, &stat) < 0) {
		ERROR("cannot stat file");
		return (-1);
	}

	string->ptr = malloc(stat.st_size);
	ssize_t	len = read(fd, string->ptr, stat.st_size);
	if (len < 0) {
		ERROR("cannot read file");
		free(string->ptr);
		return (-1);
	}
	string->len = len;

	close(fd);

	return (0);
}