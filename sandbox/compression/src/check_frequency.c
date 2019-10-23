#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

static size_t result[256] = {0};

int main(int argc, char *argv[])
{
	assert(argc == 2);
	int fd = open(argv[1], O_RDONLY);
	assert(fd > 0);
	unsigned char t;
	int r;
	while ((r = read(fd, &t, 1)) > 0) {
		result[t] += 1;
	}
	assert(r >= 0);
	close(fd);
	for (size_t i = 0; i < 256; i++) {
		printf("%3zu: %zu\n", i, result[i]);
	}
	return 0;
}
