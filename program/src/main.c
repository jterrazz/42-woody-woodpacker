#include "woody_woodpacker.h"

#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		dprintf(STDERR_FILENO, "usage: %s executable_file\n", argv[0]);
		return 1;
	}
	return 0;
}
