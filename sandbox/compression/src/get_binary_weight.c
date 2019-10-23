#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	assert(argc == 2);
	int fd = open(argv[1], O_RDONLY);
	assert(fd > 0);
	unsigned long long int weight = 0;
	unsigned char t;
	int r;
	while ((r = read(fd, &t, 1)) > 0) {
		weight += t;
	}
	assert(r >= 0);
	close(fd);
	printf("Total Weght: %llu\n", weight);
	return 0;
}
