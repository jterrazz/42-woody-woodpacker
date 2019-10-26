#include "woody_woodpacker.h"

#include <stdlib.h>
#include <unistd.h>

/*
 * TODO: Check file with no load phdr
 * TODO: Test for 32 bits files
 */

int main(int argc, char *argv[])
{
	STREAM *file;

	if (argc != 2) {
		ft_dprintf(STDERR_FILENO, "usage: %s executable_file\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!(file = sopen(argv[1], 0, S_RDONLY))) {
		return EXIT_FAILURE;
	}
	read_elf(file);
	sclose(file);
	return 0;
}
