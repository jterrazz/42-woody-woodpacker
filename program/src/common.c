#include "woody_woodpacker.h"

#include <stdlib.h>
#include <stdio.h>

void perror_and_exit(const char *err)
{
	perror(err);
	exit(-1);
}
