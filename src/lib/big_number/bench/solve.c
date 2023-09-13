#include "../big_number.h"
#include <fcntl.h>
#include <unistd.h>

int main() {
	int fd = open("./data.txt", O_RDONLY);
	char buf[10000];

	for (int i = 0 ; i < 10000; ++i) {
		read(fd, buf, 130); // 64 + 1 + 64 + 1
		buf[64] = '\0';
		buf[129] = '\0';
		big_number_t *a = big_number_from_string(buf);
		big_number_t *b = big_number_from_string(buf + 65);
		big_number_t *c = big_number_mult(a, b);
		big_number_print(c);
	}
}