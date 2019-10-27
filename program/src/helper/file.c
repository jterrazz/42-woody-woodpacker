#include "woody_woodpacker.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>

struct stream {
    int fd;
    u8 *data;
    size_t len;
    int oflag;
};

/*
 * Open a file and map it
 * Flag can be one of S_RDONLY, S_WRONLY, S_RDWR
 */
STREAM *sopen(const char *filename, size_t file_len, int oflag) // TODO Put len as facultative
{
	int fd;
	off_t off;
	u8 *data;

	if (filename == NULL)
		return NULL;

	if (oflag & S_WRONLY)
		fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0755);
	else
		fd = open(filename, O_RDONLY);

	if (fd < 0) {
		perror("open");
		return NULL;
	}

	if (oflag & S_WRONLY) {
		for (size_t i = 0; i < file_len; i++) {
			int ret = write(fd, "\0", 1);
			if (ret < 0) {
				perror("write");
				unlink(filename);
				return NULL;
			}
		}
	}

	off = lseek(fd, 0, oflag & S_WRONLY ? SEEK_SET : SEEK_END);
	if (off == -1) {
		perror("lseek");
		unlink(filename);
		return NULL;
	}

	if (!(oflag & S_WRONLY))
		file_len = (size_t)off;

	data = mmap(0, file_len, PROT_READ | PROT_WRITE, oflag & S_WRONLY ? MAP_SHARED : MAP_PRIVATE, fd, 0);
	if (data == MAP_FAILED) {
		perror("mmap");
		unlink(filename);
		return NULL;
	}

#ifndef _42_
	STREAM *ctx = (STREAM *)calloc(1, sizeof(STREAM));
#else
	STREAM *ctx = (STREAM *)malloc(sizeof(STREAM));
#endif
	if (ctx == NULL) {
		perror("open");
		munmap(data, file_len);
		unlink(filename);
		return NULL;
	}
#ifdef _42_
	ft_bzero(ctx, sizeof(STREAM));
#endif
	ctx->fd = fd;
	ctx->data = data;
	ctx->len = file_len;
	ctx->oflag = oflag;
	return ctx;
}

int sclose(STREAM *ctx)
{
	int ret;

	if (ctx == NULL) {
		return -1;
	}
	ret = munmap(ctx->data, ctx->len);
	if (ret < 0) {
		perror("munmap");
		return -1;
	}
	ret = close(ctx->fd);
	if (ret < 0) {
		perror("close");
		return -1;
	}
	free(ctx);
	return 0;
}

/*
 * Securely return a pointer to the given offset of a file
 */
void *sread(STREAM *ctx, size_t offset, size_t len)
{
	if (!(ctx->oflag & S_RDONLY) || ctx == NULL || offset >= ctx->len || len > ctx->len) {
		return NULL;
	}
	return &ctx->data[offset];
}

/*
 * Securely write data in a file
 */
int swrite(STREAM *ctx, void *content, size_t offset, size_t len)
{
	if (!(ctx->oflag & S_WRONLY) || ctx == NULL || content == NULL || offset >= ctx->len || len > ctx->len) {
		return -1;
	}
	ft_memmove(ctx->data + offset, content, len);
	return 0;
}

size_t sfile_len(STREAM *ctx)
{
	if (ctx == NULL)
		return -1;
	return ctx->len;
}
