#ifndef __WOODY_WOODPACKER_H__
# define __WOODY_WOODPACKER_H__

#include "libft.h"

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef struct stream STREAM;

STREAM *sopen(const char *filename, size_t file_len);
int sclose(STREAM *ctx);
void *sread(STREAM *ctx, size_t offset, size_t len);
int swrite(STREAM *ctx, void *content, size_t offset, size_t len);

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

u8 *secure_read(u8 *mem,
		size_t mem_len,
		size_t offset,
		size_t len_to_read);

#ifdef SILENT
#define ft_printf(...) {}
#define ft_dprintf(...) {}
#endif

/*
 * Avoid linking of forbidden functions in 42
 */
#ifdef _42_
#define unlink(...) {}
#endif

#endif
