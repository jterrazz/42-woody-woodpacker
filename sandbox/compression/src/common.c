#include "common.h"

#include <stdlib.h>
#include <stdio.h>

void perror_and_exit(const char *err)
{
#ifndef SILENT
	perror(err);
#else
	(void)err;
#endif
	exit(-1);
}

/*
 * Secure read in a bounded chunk
 */
u8 *secure_read(u8 *mem,
		size_t mem_len,
		size_t offset,
		size_t len_to_read)
{
	if (offset + len_to_read > mem_len) {
		return NULL;
	} else {
		return &mem[offset];
	}
}
