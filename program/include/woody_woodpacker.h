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
void	*sread(STREAM *ctx, size_t offset, size_t len);
int		swrite(STREAM *ctx, void *content, size_t offset, size_t len);
size_t	sfile_len(STREAM *ctx);
int		sclose(STREAM *ctx);




struct e_ident {
	char magic[4];
	u8 class;
	u8 endian;
	u8 version;
	u8 os_abi;
	u8 version_abi;
	u8 pad[7];
} __attribute__((packed));

struct e_ident *parse_ident(STREAM *file);

/*
 * elf.c
 * Elf toolkit
 */

#define OUTPUT_FILENAME "./woody"

typedef struct packer_config {
	size_t output_len;
	size_t phdr_selected_off;
	size_t payload_len_aligned;
	size_t bss_to_add;
	size_t real_payload_len;
	size_t payload_file_off;
	size_t payload_mem_off;
	size_t payload_to_end_len;
	size_t new_startpoint_vaddr;
	size_t new_startpoint_off;
	size_t old_startpoint;
	int relative_jmp_new_pg;
} PACKER_CONFIG;

extern u8 _payload64;
extern u64 _payload64_size;

int start_packer(STREAM *file);

int encrypt_old_phdrs(STREAM *output, PACKER_CONFIG *config);
int config_packer_for_last_load(STREAM *file, u8 elf_class, PACKER_CONFIG *packed_file);

int add_hdr_entry_64(STREAM *output, PACKER_CONFIG *config);
int add_hdr_entry_32(STREAM *output, PACKER_CONFIG *config);
int add_shdr_64(STREAM *output, STREAM *original, PACKER_CONFIG *config);
int add_shdr_32(STREAM *output, STREAM *original, PACKER_CONFIG *config);
Elf32_Ehdr *parse_elf_header_32(STREAM *file);
Elf64_Ehdr *parse_elf_header_64(STREAM *file);
Elf32_Phdr *get_last_load_phdr_32(STREAM *file);
Elf64_Phdr *get_last_load_phdr_64(STREAM *file);
int insert_payload_64(STREAM *output, STREAM *original, PACKER_CONFIG *config);
int insert_payload_32(STREAM *output, STREAM *original, PACKER_CONFIG *config);
int parse_shdr_32(STREAM *file);
int parse_shdr_64(STREAM *file);
int update_phdr_32(STREAM *output, PACKER_CONFIG *config);
int update_phdr_64(STREAM *output, PACKER_CONFIG *config);
int set_payload64(void *payload, PACKER_CONFIG *config);

#ifdef _32BITS
#define ARCH_PST(S) S ## _32
#else
#define ARCH_PST(S) S ## _64
#endif

#ifdef SILENT
# define ft_printf(...) {}
# define ft_dprintf(...) {}
# define perror(...) {}
#endif

#ifdef DEBUG
# define FT_DEBUG(f_, ...) ft_printf((f_), ##__VA_ARGS__)
#else
# define FT_DEBUG(...) {}
#endif

/*
 * Removes forbidden functions for the 42 school
 */
#ifdef _42_
#define unlink(...) {}
#define assert(...) {}
#endif

#endif
