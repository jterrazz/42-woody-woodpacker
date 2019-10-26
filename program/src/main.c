#include "woody_woodpacker.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	STREAM *file;
	size_t file_len;
	void *file_start;

	if (argc != 2) {
		ft_dprintf(STDERR_FILENO, "usage: %s executable_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!(file = sopen(argv[1], 0, S_RDONLY))) {
		return EXIT_FAILURE;
	}
	file_len = sfile_len(file);
	if (!(file_start = sread(file, 0, file_len))) {
		return EXIT_FAILURE;
	}
	read_elf(file_start, file_len);
	sclose(file);
	return 0;
}
