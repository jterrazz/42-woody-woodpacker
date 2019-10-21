/*
 * Move To Front decoding (MTF)
 * see https://en.wikipedia.org/wiki/Move-to-front_transform
 */
#include "mtf.h"
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

#ifndef SILENT
	printf("Mapping a file of len: %zu\n", file_len);
#endif

	off = lseek(fd, 0, SEEK_SET);
	if (off == -1) {
		perror_and_exit("lseek set");
	}
	u8 *data = mmap(0, file_len, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data == MAP_FAILED) {
		perror_and_exit("mmap");
	}

	size_t new_len = file_len;
	char *new_data = (char *)mtf_encode(data, &new_len);
	if (new_data == NULL) {
		dprintf(STDERR_FILENO, "MTF encode failed\n");
		exit(1);
	}

#ifndef SILENT
	printf("Output file len: %zu\n", new_len);
#endif

	STREAM *out = sopen(argv[2], new_len);
	if (out == NULL) {
		dprintf(STDERR_FILENO, "Cannot create new file\n");
		exit(1);
	}
	ret = swrite(out, new_data, 0, new_len);
	if (ret < 0) {
		dprintf(STDERR_FILENO, "Write error on new file\n");
		exit(1);
	}
	ret = sclose(out);
	if (ret < 0) {
		dprintf(STDERR_FILENO, "Cannot close new file\n");
		exit(1);
	}

	free(new_data);

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
