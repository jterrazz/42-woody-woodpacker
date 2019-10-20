/*
 * Jbit encoding (JBE)
 * see https://arxiv.org/pdf/1209.1045.pdf
 */
#include "jbe.h"
#include "common.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	if (argc != 3) {
		dprintf(STDERR_FILENO, "usage: %s input_file output_file\n", argv[0]);
		exit(1);
	}
	int ret;
	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror_and_exit("open");
	}

	off_t off = lseek(fd, 0, SEEK_END);
	if (off == -1) {
		perror_and_exit("lseek end");
	}
	size_t file_len = (size_t)off;

	printf("Mapping a file of len: %zu\n", file_len);

	off = lseek(fd, 0, SEEK_SET);
	if (off == -1) {
		perror_and_exit("lseek set");
	}
	u8 *data = mmap(0, file_len, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data == MAP_FAILED) {
		perror_and_exit("mmap");
	}

	size_t new_len = file_len;
	char *new_data = (char *)jbe_decode(data, &new_len);
	if (new_data == NULL) {
		dprintf(STDERR_FILENO, "JBE encore failed\n");
		exit(1);
	}
	// Do something

	ret = munmap(data, file_len);
	if (ret < 0) {
		perror_and_exit("munmap");
	}
	ret = close(fd);
	if (ret < 0) {
		perror_and_exit("close");
	}
	return 0;
}
