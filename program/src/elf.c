#include "woody_woodpacker.h"

#include <elf.h>
#include <assert.h>

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

	char *str;
	switch (e_ident->os_abi) {
	case ELFOSABI_SYSV:
		str = "UNIX System V ABI.";
		break;
	case ELFOSABI_HPUX:
		str = "HP-UX ABI.";
		break;
	case ELFOSABI_NETBSD:
		str = "NetBSD ABI.";
		break;
	case ELFOSABI_LINUX:
		str = "Linux ABI.";
		break;
	case ELFOSABI_SOLARIS:
		str = "Solaris ABI.";
		break;
	case ELFOSABI_IRIX:
		str = "IRIX ABI.";
		break;
	case ELFOSABI_FREEBSD:
		str = "FreeBSD ABI.";
		break;
	case ELFOSABI_TRU64:
		str = "TRU64 UNIX ABI.";
		break;
	case ELFOSABI_ARM:
		str = "ARM architecture ABI.";
		break;
	case ELFOSABI_STANDALONE:
		str = "Stand-alone (embedded) ABI.";
		break;
	default:
		ft_dprintf(STDERR_FILENO, "Unknown OS ABI.\n");
		return NULL;
	}
	ft_printf("%s\n", str);

	ft_printf("Private ABI version is %hhu.\n", e_ident->version_abi);
	return e_ident;
}

// Generic
size_t get_output_len(STREAM *file)
{
	Elf64_Phdr *phdr = get_last_load_phdr_64(file);

	size_t aligned_payload64_size = (_payload64_size + 63) & ~63;
	size_t last_load_bss_size = phdr->p_memsz - phdr->p_filesz;
	size_t shift_size = aligned_payload64_size + last_load_bss_size;
	size_t len = sfile_len(file);

	return len + shift_size + sizeof(Elf64_Shdr);
}

int add_hdr_entry_64(STREAM *output, size_t entry);
int add_hdr_entry_32(STREAM *output, size_t entry);
int add_shdr_64(STREAM *output, STREAM *original);
int add_shdr_32(STREAM *output, STREAM *original);

int read_elf(STREAM *file)
{
	STREAM *output;
	size_t output_len;

//	struct e_ident *ident_field = parse_ident_field(data, len);
//	if (!ident_field)
//	    return -1;

	output_len = get_output_len(file);

	if (!(output = sopen(OUTPUT_FILENAME, output_len, S_RDWR))) // TODO Will need to transform this to a simple mmap because compression will reduce size
		return -1;

	// TODO Secure calls
	Elf64_Phdr *last_load_phdr = get_last_load_phdr_64(file);
	insert_payload_64(output, file); // TODO Maybe refactor for more generic (set an addr)
	add_hdr_entry_64(output, last_load_phdr->p_memsz + last_load_phdr->p_vaddr);
	add_shdr_64(output, file);
	sclose(output);

//	if (ident_field->class == ELFCLASS32) {
//		Elf32_Ehdr *header = parse_elf_header_32(data, len);
//		dump_section_header_32(data, len);
//        dump_program_header_32(data, len);
//
//        (void)header;
//	} else if (ident_field->class == ELFCLASS64) {
//		Elf64_Ehdr *header = parse_elf_header_64(data, len);
//		dump_section_header_64(data, len);
//        dump_program_header_64(data, len);
//        (void)header;
//	} else {
//		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
//		return -1;
//	}

	return 0;
}
