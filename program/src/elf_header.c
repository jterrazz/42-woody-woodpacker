#include "woody_woodpacker.h"

#include <elf.h>

ElfN_Ehdr *parse_elf_header_generic(u8 *data, size_t len)
{
	/*
	 * Then parse the header field to get more informations about the ELF file
	 */
	ElfN_Ehdr *header = (ElfN_Ehdr *)secure_read(data, len, 0, sizeof(ElfN_Ehdr));
	if (header == NULL) {
		return NULL;
	}

	// I consider only executable ELF type or shared object as valid here
	switch (header->e_type) {
	case ET_NONE:
		ft_dprintf(STDERR_FILENO, "Unknown ELF type.\n");
		return NULL;
	case ET_REL:
		ft_dprintf(STDERR_FILENO, "ELF type is a relocatable file.\n");
		return NULL;
	case ET_DYN:
		ft_printf("ELF type is a shared object.\n");
		break;
	case ET_CORE:
		ft_dprintf(STDERR_FILENO, "ELF type is a core file.\n");
		return NULL;
	case ET_EXEC:
		ft_printf("ELF type is an executable file.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "ELF type is a core file.\n");
		return NULL;
	}

	// We have only payloads for EM_386 or EM_X86_64 machine
	switch (header->e_machine) {
	case EM_386:
		ft_printf("intel 80386.\n");
		break;
	case EM_X86_64:
		ft_printf("amd x86-64.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "this type of machine is not managed: %hu.\n", header->e_machine);
		return NULL;
	}

	// Only invalid version can interrupt the program
	switch (header->e_version) {
	case EV_CURRENT:
		ft_printf("The machine version is current.\n");
		break;
	case EV_NONE:
		ft_dprintf(STDERR_FILENO, "this machine version is invalid.\n");
		return NULL;
	default:
		ft_printf("this machine version is %hu.\n", header->e_version);
		break;
	}

	// Dump others important informations from the header
	ft_printf("start_point address: %p\n", header->e_entry);
	ft_printf("program header table offset: %p\n", header->e_phoff);
	ft_printf("section header table offset: %p\n", header->e_shoff);
	ft_printf("processor specific flags: %u\n", header->e_flags);
	ft_printf("elf header size: %hu\n", header->e_ehsize);
	ft_printf("size in bytes of one entry in the file's program header table: %hu\n", header->e_phentsize);
	ft_printf("number of entries in the program header table: %hu\n", header->e_phnum);
	ft_printf("sections header's size in bytes: %hu\n", header->e_shentsize);
	ft_printf("number of entries in the section header table: %hu\n", header->e_shnum);
	ft_printf("section header table index of the entry associated with the section name string table: %hu\n", header->e_shstrndx);
	return header;
}
