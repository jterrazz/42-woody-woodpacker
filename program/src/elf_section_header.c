#include "woody_woodpacker.h"

#include <elf.h>

struct section_header_type {
	u32 type;
	char *s;
};

#define SECTION_HEADER_TYPE_N 12

static struct section_header_type section_header_type[SECTION_HEADER_TYPE_N] = {
	{ SHT_NULL, "NULL" },
	{ SHT_PROGBITS, "PROGBITS" },
	{ SHT_SYMTAB, "SYMTAB" },
	{ SHT_STRTAB, "STRTAB" },
	{ SHT_RELA, "RELA" },
	{ SHT_HASH, "HASH" },
	{ SHT_DYNAMIC, "DYNAMIC" },
	{ SHT_NOTE, "NOTE" },
	{ SHT_NOBITS, "NOBITS" },
	{ SHT_REL, "REL" },
	{ SHT_SHLIB, "SHLIB" },
	{ SHT_DYNSYM, "DYNSYM" },
};

// typedef struct {
// 	uint32_t   sh_name;
// 	uint32_t   sh_type;
// 	uint32_t   sh_flags;
// 	Elf32_Addr sh_addr;
// 	Elf32_Off  sh_offset;
// 	uint32_t   sh_size;
// 	uint32_t   sh_link;
// 	uint32_t   sh_info;
// 	uint32_t   sh_addralign;
// 	uint32_t   sh_entsize;
// } Elf32_Shdr;

int dump_section_header_generic(u8 *data, size_t len)
{
	ElfN_Ehdr *header = (ElfN_Ehdr *)secure_read(data, len, 0, sizeof(ElfN_Ehdr));
	if (header == NULL) {
		return -1;
	}
	size_t section_header_array_location = header->e_shoff;
	if (section_header_array_location == 0) {
		ft_printf("No section header found !");
		return 0;
	}
	size_t section_header_length = header->e_shentsize * header->e_shnum;
	ElfN_Shdr *section_header_array = (ElfN_Shdr *)secure_read(data, len, header->e_shoff, section_header_length);
	if (section_header_array == NULL) {
		ft_dprintf(STDERR_FILENO, "Corrupted file while parsing section header table\n");
		return -1;
	}

	for (u16 i = 0; i < header->e_shnum; i++) {
		int j = 0;
		for (j = 0; j < SECTION_HEADER_TYPE_N; j++) {
			if (section_header_type[j].type == section_header_array[i].sh_type) {
				break;
			}
		}
		ElfN_Shdr *s = &section_header_array[i];
		if (j != SECTION_HEADER_TYPE_N) {
			ft_printf("%2hu: %20s %c%c%c %.8x %.8x %.8x\n", i, section_header_type[j].s,
				  s->sh_flags & SHF_WRITE ? 'W' : ' ',
				  s->sh_flags & SHF_ALLOC ? 'A' : ' ',
				  s->sh_flags & SHF_EXECINSTR ? 'X' : ' ',
				  s->sh_addr,
				  s->sh_offset,
				  s->sh_size
			);
		} else {
			ft_printf("%2hu: %20s\n", i, "Unknown section");
		}
	}
	return 0;
}
