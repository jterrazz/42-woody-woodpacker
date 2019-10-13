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

int read_elf(u8 *data, size_t len)
{
	struct e_ident *e_ident = (struct e_ident *)secure_read(data, len, 0, EI_NIDENT);
	if (e_ident == NULL) {
		return -1;
	}

	ft_printf("EI_NIDENT system value is: %u.\n", EI_NIDENT);
	assert(EI_NIDENT == sizeof(struct e_ident));

	char magic[4] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
	if (ft_memcmp(e_ident->magic, magic, 4) != 0) {
		ft_dprintf(STDERR_FILENO, "Bad ELF magic\n");
		return -1;
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
		return -1;
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
		return -1;
	}

	switch (e_ident->version) {
	case EV_CURRENT:
		ft_printf("ELF version is Current version.\n");
		break;
	case EV_NONE:
		ft_dprintf(STDERR_FILENO, "Invalid version.\n");
		return -1;
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
		return -1;
	}

	ft_printf("Private ABI version is %hhu.\n", e_ident->version_abi);
	return 0;
}
