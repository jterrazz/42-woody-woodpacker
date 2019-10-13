#include "woody_woodpacker.h"

#include <elf.h>
#include <assert.h>

static u8 *secure_read(u8 *mem,
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

struct e_ident {
	char magic[4];
	u8 class;
	u8 endian;
	u8 version;
	u8 os_abi;
	u8 version_abi;
	u8 pad[7];
} __attribute__((packed));

// ELF header (Ehdr): The ELF header is described by the type Elf32_Ehdr or Elf64_Ehdr
// The 'N' need to be replaced by 32 or 64.
// typedef struct {
// 	unsigned char e_ident[EI_NIDENT];
// 	uint16_t      e_type;
// 	uint16_t      e_machine;
// 	uint32_t      e_version;
// 	ElfN_Addr     e_entry;
// 	ElfN_Off      e_phoff;
// 	ElfN_Off      e_shoff;
// 	uint32_t      e_flags;
// 	uint16_t      e_ehsize;
// 	uint16_t      e_phentsize;
// 	uint16_t      e_phnum;
// 	uint16_t      e_shentsize;
// 	uint16_t      e_shnum;
// 	uint16_t      e_shstrndx;
// } ElfN_Ehdr;

struct e_ident *parse_ident_field(u8 *data, size_t len)
{
	/*
	 * First parsing e_ident field to get globals informations about the ELF file
	 * like Arch or endian
	 */
	struct e_ident *e_ident = (struct e_ident *)secure_read(data, len, 0, EI_NIDENT);
	if (e_ident == NULL) {
		return NULL;
	}

	ft_printf("EI_NIDENT system value is: %u.\n", EI_NIDENT);
	assert(EI_NIDENT == sizeof(struct e_ident));

	char magic[4] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
	if (ft_memcmp(e_ident->magic, magic, 4) != 0) {
		ft_dprintf(STDERR_FILENO, "Bad ELF magic\n");
		return NULL;
	}

	switch (e_ident->class) {
	case ELFCLASS32:
		ft_printf("ELF class 32.\n");
		break;
	case ELFCLASS64:
		ft_printf("ELF class 64.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
		return NULL;
	}

	switch (e_ident->endian) {
	case ELFDATA2LSB:
		ft_printf("Two's complement, little-endian.\n");
		break;
	case ELFDATA2MSB:
		ft_printf("Two's complement, big-endian.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "Unknown data format.\n");
		return NULL;
	}

	switch (e_ident->version) {
	case EV_CURRENT:
		ft_printf("ELF version is Current version.\n");
		break;
	case EV_NONE:
		ft_dprintf(STDERR_FILENO, "Invalid version.\n");
		return NULL;
	default:
		// I don't know how to manage that case
		ft_printf("ELF version is %hhu\n", e_ident->version);
		break;
	}

	switch (e_ident->os_abi) {
	case ELFOSABI_SYSV:
		ft_printf("UNIX System V ABI.\n");
		break;
	case ELFOSABI_HPUX:
		ft_printf("HP-UX ABI.\n");
		break;
	case ELFOSABI_NETBSD:
		ft_printf("NetBSD ABI.\n");
		break;
	case ELFOSABI_LINUX:
		ft_printf("Linux ABI.\n");
		break;
	case ELFOSABI_SOLARIS:
		ft_printf("Solaris ABI.\n");
		break;
	case ELFOSABI_IRIX:
		ft_printf("IRIX ABI.\n");
		break;
	case ELFOSABI_FREEBSD:
		ft_printf("FreeBSD ABI.\n");
		break;
	case ELFOSABI_TRU64:
		ft_printf("TRU64 UNIX ABI.\n");
		break;
	case ELFOSABI_ARM:
		ft_printf("ARM architecture ABI.\n");
		break;
	case ELFOSABI_STANDALONE:
		ft_printf("Stand-alone (embedded) ABI.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "Unknown OS ABI.\n");
		return NULL;
	}

	ft_printf("Private ABI version is %hhu.\n", e_ident->version_abi);
	return e_ident;
}

Elf32_Ehdr *parse_elf_header_32(u8 *data, size_t len);
Elf64_Ehdr *parse_elf_header_64(u8 *data, size_t len);

int read_elf(u8 *data, size_t len)
{
	struct e_ident *ident_field = parse_ident_field(data, len);

	if (ident_field->class == ELFCLASS32) {
		Elf32_Ehdr *header = parse_elf_header_32(data, len);
		(void)header;
	} else if (ident_field->class == ELFCLASS64) {
		Elf64_Ehdr *header = parse_elf_header_64(data, len);
		(void)header;
	} else {
		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
		return -1;
	}

	(void)ident_field;
	return 0;
}
