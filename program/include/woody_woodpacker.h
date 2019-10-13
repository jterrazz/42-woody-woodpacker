#ifndef __WOODY_WOODPACKER_H__
# define __WOODY_WOODPACKER_H__

#include "libft.h"

typedef unsigned char      u8;
typedef unsigned int       u32;
typedef unsigned long long u64;

/*
 * elf.c
 * Elf toolkit
 */
int read_elf(u8 *data, size_t len);

/*
 * common.c
 * Basic tools
 */
void perror_and_exit(const char *err);

#ifdef SILENT
#define ft_printf(...) {}
// #define ft_dprintf(...) {}
#endif

#endif
