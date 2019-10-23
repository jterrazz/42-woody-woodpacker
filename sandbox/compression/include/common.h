#ifndef __COMMON_H__
# define __COMMON_H__

#include <string.h>

typedef unsigned char u8;

typedef struct stream STREAM;

STREAM *sopen(const char *filename, size_t file_len);
int sclose(STREAM *ctx);
void *sread(STREAM *ctx, size_t offset, size_t len);
int swrite(STREAM *ctx, void *content, size_t offset, size_t len);

void perror_and_exit(const char *err);
u8 *secure_read(u8 *mem,
		size_t mem_len,
		size_t offset,
		size_t len_to_read);

#endif
