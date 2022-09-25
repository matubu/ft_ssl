#include <stdio.h>

int	main() {
	for (int a = 0; a < 256; ++a)
		for (int b = 0; b < 256; ++b)
			printf("%c%c0", a, b);
	for (int a = 0; a < 256; ++a)
		for (int b = 0; b < 256; ++b)
			printf("0%c%c", a, b);
}