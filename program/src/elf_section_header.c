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

int add_shdr_generic(STREAM *output, STREAM *original, PACKER_CONFIG *config)
{
	ElfN_Ehdr *elf_hdr;
	ElfN_Ehdr *output_header;

	if (!(elf_hdr = sread(original, 0, sizeof(ElfN_Ehdr)))) // TODO Maybe use output for this
		return -1;
	if (!(output_header = sread(output, 0, sizeof(ElfN_Ehdr)))) // TODO Rename
		return -1;

	u64 sh_offset = elf_hdr->e_shoff;

	if (sh_offset > config->payload_file_off) // TODO Use insted at returned in main
		sh_offset += config->real_payload_len;
	output_header->e_shoff = sh_offset;

	ElfN_Shdr *output_sh = sread(output, sh_offset, sizeof(ElfN_Shdr) * output_header->e_shnum);
	if (!output_sh)
		return -1;

	/*
	 * Update old segments with their new offsets
	 */

	void *new_shdr_addr = 0;
	for (u16 i = 0; i < elf_hdr->e_shnum ; i++) {
		if (output_sh->sh_offset > config->payload_file_off) {
			if (!new_shdr_addr)
				new_shdr_addr = output_sh;
			output_sh->sh_offset += config->real_payload_len;
		}
		output_sh++;
	}

	/*
	 * Add and setup a new segment
	 */

	new_shdr_addr -= sizeof(ElfN_Shdr); // TODO Only do that if the last segment was a BSS and had empty space
	ElfN_Shdr *prev_sh = new_shdr_addr - sizeof(ElfN_Shdr) ;

	((ElfN_Shdr *)new_shdr_addr)->sh_offset += config->real_payload_len;

	u64 new_sh_offset = new_shdr_addr - (void *)output_header;
	u64 end_output_to_move_size = sfile_len(output) - new_sh_offset - sizeof(ElfN_Shdr);
	if (swrite(output, new_shdr_addr, new_sh_offset + sizeof(ElfN_Shdr), end_output_to_move_size))
		return -1;

	ElfN_Shdr *new_sh = new_shdr_addr;
	new_sh->sh_name = 0;
	new_sh->sh_type = SHT_PROGBITS;
	new_sh->sh_flags = SHF_EXECINSTR | SHF_ALLOC;
	new_sh->sh_offset = config->new_startpoint_off;
	new_sh->sh_addr = prev_sh->sh_addr + (config->new_startpoint_off - prev_sh->sh_offset);
	new_sh->sh_size = config->payload_len_aligned;
	new_sh->sh_link = 0;
	new_sh->sh_info =  0;
	new_sh->sh_addralign = 16;
	new_sh->sh_entsize = 0;

	return 0;
}

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
#ifdef SILENT
			(void)s;
#endif
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
