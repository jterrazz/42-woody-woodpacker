#ifndef __WOODY_WOODPACKER_H__
# define __WOODY_WOODPACKER_H__

#include "libft.h"
#include <elf.h>

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

/*
 * file.c
 * File helpers
 */

#define S_RDONLY        0x0001          /* open for reading only */
#define S_WRONLY        0x0002          /* open for writing only */
#define S_RDWR          0x0003          /* open for reading and writing */

typedef struct stream STREAM;

STREAM	*sopen(const char *filename, size_t file_len, int oflag);
int		sclose(STREAM *ctx);
void	*sread(STREAM *ctx, size_t offset, size_t len);
int		swrite(STREAM *ctx, void *content, size_t offset, size_t len);
size_t	sfile_len(STREAM *ctx);

/*
 * elf.c
 * Elf toolkit
 */
#define OUTPUT_FILENAME "./woody"

extern u8 _payload64;
extern u64 _payload64_size;

int read_elf(STREAM *file);

Elf32_Ehdr *parse_elf_header_32(u8 *data, size_t len);
Elf64_Ehdr *parse_elf_header_64(u8 *data, size_t len);
Elf32_Phdr *get_last_load_phdr_32(STREAM *file);
Elf64_Phdr *get_last_load_phdr_64(STREAM *file);
int insert_payload_64(STREAM *output, STREAM *original);
int insert_payload_32(STREAM *output, STREAM *original);
int dump_section_header_32(u8 *data, size_t len);
int dump_section_header_64(u8 *data, size_t len);
int dump_program_header_32(u8 *data, size_t len);
int dump_program_header_64(u8 *data, size_t len);

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
#define perror(...) {}
#endif

/*
 * Avoid linking of forbidden functions in 42
 */
#ifdef _42_
#define unlink(...) {}
#endif

#endif
