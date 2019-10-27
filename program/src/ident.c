#include "woody_woodpacker.h"

#include <elf.h>
#include <assert.h>

struct e_ident *parse_ident(STREAM *file)
{
	struct e_ident *e_ident = sread(file, 0, EI_NIDENT);
	if (e_ident == NULL)
		return NULL;
	FT_DEBUG("EI_NIDENT system value is: %u.\n", EI_NIDENT);
	assert(EI_NIDENT == sizeof(struct e_ident));

	char magic[4] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
	if (ft_memcmp(e_ident->magic, magic, 4) != 0) {
		ft_dprintf(STDERR_FILENO, "Bad ELF magic\n");
		return NULL;
	}

	switch (e_ident->class) {
		case ELFCLASS32:
			FT_DEBUG("ELF class 32.\n");
			break;
		case ELFCLASS64:
			FT_DEBUG("ELF class 64.\n");
			break;
		default:
			ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
			return NULL;
	}

	switch (e_ident->endian) {
		case ELFDATA2LSB:
			FT_DEBUG("Two's complement, little-endian.\n");
			break;
		case ELFDATA2MSB:
			FT_DEBUG("Two's complement, big-endian.\n");
			break;
		default:
			ft_dprintf(STDERR_FILENO, "Unknown data format.\n");
			return NULL;
	}

	switch (e_ident->version) {
		case EV_CURRENT:
			FT_DEBUG("ELF version is Current version.\n");
			break;
		case EV_NONE:
			ft_dprintf(STDERR_FILENO, "Invalid version.\n");
			return NULL;
		default:
			// TODO I don't know how to manage that case
			FT_DEBUG("ELF version is %hhu\n", e_ident->version);
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
	FT_DEBUG("%s\n", str);
	(void)str;
	FT_DEBUG("Private ABI version is %hhu.\n", e_ident->version_abi);

	return e_ident;
}
