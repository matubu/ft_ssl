#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "lib/malloc.h"
#include "lib/io.h"

int	readfile(const char *filename, string_t *string) {
	int	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		ERROR(filename, "cannot open file", strerror(errno));
		return (-1);
	}

	struct stat	stat;
	if (fstat(fd, &stat) < 0) {
		ERROR(filename, "cannot stat file", strerror(errno));
		return (-1);
	}

	string->ptr = malloc(stat.st_size);
	ssize_t	len = read(fd, string->ptr, stat.st_size);
	if (len < 0) {
		ERROR(filename, "cannot read file", strerror(errno));
		free(string->ptr);
		return (-1);
	}
	string->len = len;

	close(fd);

	return (0);
}

string_t	readall(int fd) {
	int			ret;
	size_t		capacity = 64;
	string_t	str = {
		.ptr = malloc(capacity),
		.len = 0
	};

	do {
		if (str.len >= capacity) {
			capacity *= 2;
			uint8_t	*tmp = malloc(capacity);
			for (uint64_t i = 0; i < str.len; ++i) {
				tmp[i] = str.ptr[i];
			}
			free(str.ptr);
			str.ptr = tmp;
		}
		ret = read(fd, str.ptr + str.len, 1);

		if (ret < 0) {
			DIE("could not read", strerror(errno))
		}
		if (ret) {
			++str.len;
		}
	} while (ret);

	return (str);
}
