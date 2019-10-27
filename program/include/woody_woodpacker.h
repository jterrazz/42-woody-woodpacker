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

/*
 * ident.c
 * Identify binary files
 */

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
 * packer.c
 * Add a custom payload to binaries that will execute first
 */

#define OUTPUT_FILENAME "./woody"

typedef struct packer_config {
	size_t output_len;
	size_t insert_phdr_off;
	size_t payload_len_aligned;
	size_t bss_len;
	size_t insert_len;
	size_t insert_off;
	size_t payload_mem_off;
	size_t insert_to_end_len;
	size_t new_startpoint_vaddr;
	size_t payload_start_off;
	size_t old_startpoint;
	int relative_jmp_new_pg;
} PACKER_CONFIG;

int start_packer(STREAM *file);
int config_packer_for_last_load_32(STREAM *file, PACKER_CONFIG *packed_file);
int config_packer_for_last_load_64(STREAM *file, PACKER_CONFIG *conf);
int create_packed_32(STREAM *file);
int create_packed_64(STREAM *file);

/*
 * Packer payload
 */

extern u8 _payload_32;
extern u8 _payload_64;
extern u64 _payload_size_32;
extern u64 _payload_size_64;

int config_payload_32(STREAM *out, PACKER_CONFIG *config);
int config_payload_64(STREAM *out, PACKER_CONFIG *config);

/*
 * ehdr.c
 * ELF Header related methods
 */

Elf32_Ehdr *parse_ehdr_32(STREAM *file);
Elf64_Ehdr *parse_ehdr_64(STREAM *file);
int ehdr_update_64(STREAM *output, PACKER_CONFIG *config);
int ehdr_update_32(STREAM *output, PACKER_CONFIG *config);

/*
 * shdr.c
 * ELF Section header related methods
 */

int encrypt_shdrs_32(STREAM *output, PACKER_CONFIG *config);
int encrypt_shdrs_64(STREAM *output, PACKER_CONFIG *config);
int add_shdr_64(STREAM *file, PACKER_CONFIG *conf);
int add_shdr_32(STREAM *file, PACKER_CONFIG *config);
int parse_shdr_32(STREAM *file, void(*ft)(Elf32_Ehdr *ehdr, Elf32_Shdr *shdr, PACKER_CONFIG *config), PACKER_CONFIG *config);
int parse_shdr_64(STREAM *file, void(*ft)(Elf64_Ehdr *ehdr, Elf64_Shdr *shdr, PACKER_CONFIG *config), PACKER_CONFIG *config);

/*
 * phdr.c
 * ELF Program header related methods
 */

int phdr_update_for_payload_32(STREAM *output, PACKER_CONFIG *config);
int phdr_update_for_payload_64(STREAM *output, PACKER_CONFIG *config);
Elf32_Phdr *get_last_load_phdr_32(STREAM *file);
Elf64_Phdr *get_last_load_phdr_64(STREAM *file);
void *p_append_data_64(STREAM *out, STREAM *in, PACKER_CONFIG *conf, void *src, size_t src_len);
void *p_append_data_32(STREAM *out, STREAM *in, PACKER_CONFIG *conf, void *src, size_t src_len);

/*
 * Defines
 */

// Support for 32/64 bits with the same codebase
#ifdef _32BITS
# define ARCH_PST(S) S ## _32
# define ElfN_Ehdr Elf32_Ehdr
# define ElfN_Phdr Elf32_Phdr
# define ElfN_Shdr Elf32_Shdr
# define ElfN_Off Elf32_Off
#else
# define ARCH_PST(S) S ## _64
# define ElfN_Ehdr Elf64_Ehdr
# define ElfN_Phdr Elf64_Phdr
# define ElfN_Shdr Elf64_Shdr
# define ElfN_Off Elf64_Off
#endif

// Silence all outputs
#ifdef SILENT
# define ft_printf(...) {}
# define ft_dprintf(...) {}
# define perror(...) {}
#endif

// Log all operations
#ifdef DEBUG
# define FT_DEBUG(f_, ...) ft_printf((f_), ##__VA_ARGS__)
#else
# define FT_DEBUG(...) {}
#endif

// Removes forbidden functions for the 42 school
#ifdef _42_
# define unlink(...) {}
# define assert(...) {}
#endif

#endif
