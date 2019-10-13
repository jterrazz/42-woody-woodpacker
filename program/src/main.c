#include "woody_woodpacker.h"

#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc != 2) {
		ft_dprintf(STDERR_FILENO, "usage: %s executable_file\n", argv[0]);
		return 1;
	}
	return 0;
}
