#include "woody_woodpacker.h"

#include <stdlib.h>
#include <unistd.h>

/*
 * TODO: Check file with no load phdr
 * TODO: Test for 32 bits files
 */

/*
 * TERMINOLOGY
 * ehdr: An elf header
 * shdr: A section header
 * phdr: A program header
 *
 * To learn more about them, try `man elf`
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
	start_packer(file);
	sclose(file);
	return 0;
}
