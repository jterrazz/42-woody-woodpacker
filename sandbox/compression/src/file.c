#include "common.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef SILENT
#define perror(...) {}
#endif

struct stream {
    int fd;
    u8 *data;
    size_t len;
};

#define WRITE_BUF_LEN (4096 * 4)

static u8 write_buf[WRITE_BUF_LEN] = {0};

/*
 * Open a file and map it
 */
STREAM *sopen(const char *filename, size_t file_len)
{
	if (filename == NULL) {
		return NULL;
	}

	int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0755);
	if (fd < 0) {
		perror("open");
		return NULL;
	}

	size_t i = file_len;
	while (true) {
		int ret = write(fd, &write_buf, i < WRITE_BUF_LEN ? i : WRITE_BUF_LEN);
		if (ret < 0) {
			perror("write");
			unlink(filename);
			return NULL;
		}
		if (i < WRITE_BUF_LEN) {
			break;
		} else {
			i -= WRITE_BUF_LEN;
		}
	}

	off_t off = lseek(fd, 0, SEEK_SET);
	if (off == -1) {
		perror("lseek");
		unlink(filename);
		return NULL;
	}
	u8 *data = mmap(0, file_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
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
	return ctx;
}

/*
 * Close the file
 */
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
 * Read data from the file
 */
void *sread(STREAM *ctx, size_t offset, size_t len)
{
	if (ctx == NULL || offset >= ctx->len || len > ctx->len) {
		return NULL;
	}
	return &ctx->data[offset];
}

/*
 * Write data into the file
 */
int swrite(STREAM *ctx, void *content, size_t offset, size_t len)
{
	if (ctx == NULL || content == NULL || offset >= ctx->len || len > ctx->len) {
		return -1;
	}
	memcpy(ctx->data + offset, content, len);
	return 0;
}
