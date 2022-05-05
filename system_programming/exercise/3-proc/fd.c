#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char ** argv) {
	if (argc > 1 && strcmp(argv[1], "-i") == 0) {
		int c;
		while ((c = getchar()) != EOF) {
			putchar(c);
		}
	}
	for (int i = 0; i < 3; i++) {
		int fd = open("/dev/null", O_RDONLY);
		printf("%d ", fd);
	}
    printf("\n");
	return 0;
}
