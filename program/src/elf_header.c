#include "woody_woodpacker.h"

#include <elf.h>

ElfN_Ehdr *parse_elf_header_generic(STREAM *file)
{
	ElfN_Ehdr *header;

	if (!(header = sread(file, 0, sizeof(ElfN_Ehdr))))
		return NULL;

	// I consider only executable ELF type or shared object as valid here
	switch (header->e_type) {
	case ET_NONE:
		ft_dprintf(STDERR_FILENO, "Unknown ELF type.\n");
		return NULL;
	case ET_REL:
		ft_dprintf(STDERR_FILENO, "ELF type is a relocatable file.\n");
		return NULL;
	case ET_DYN:
		FT_DEBUG("ELF type is a shared object.\n");
		break;
	case ET_CORE:
		ft_dprintf(STDERR_FILENO, "ELF type is a core file.\n");
		return NULL;
	case ET_EXEC:
		FT_DEBUG("ELF type is an executable file.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "ELF type is a core file.\n");
		return NULL;
	}

	// We have only payloads for EM_386 or EM_X86_64 machine
	switch (header->e_machine) {
	case EM_386:
		FT_DEBUG("intel 80386.\n");
		break;
	case EM_X86_64:
		FT_DEBUG("amd x86-64.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "this type of machine is not managed: %hu.\n", header->e_machine);
		return NULL;
	}

	// Only invalid version can interrupt the program
	switch (header->e_version) {
	case EV_CURRENT:
		FT_DEBUG("The machine version is current.\n");
		break;
	case EV_NONE:
		ft_dprintf(STDERR_FILENO, "this machine version is invalid.\n");
		return NULL;
	default:
		FT_DEBUG("this machine version is %hu.\n", header->e_version);
		break;
	}

	FT_DEBUG("start_point address: %p\n", header->e_entry);
	FT_DEBUG("program header table offset: %p\n", header->e_phoff);
	FT_DEBUG("section header table offset: %p\n", header->e_shoff);
	FT_DEBUG("processor specific flags: %u\n", header->e_flags);
	FT_DEBUG("elf header size: %hu\n", header->e_ehsize);
	FT_DEBUG("size in bytes of one entry in the file's program header table: %hu\n", header->e_phentsize);
	FT_DEBUG("number of entries in the program header table: %hu\n", header->e_phnum);
	FT_DEBUG("sections header's size in bytes: %hu\n", header->e_shentsize);
	FT_DEBUG("number of entries in the section header table: %hu\n", header->e_shnum);
	FT_DEBUG("section header table index of the entry associated with the section name string table: %hu\n", header->e_shstrndx);

	return header;
}
