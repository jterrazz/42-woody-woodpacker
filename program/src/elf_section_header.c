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

int ARCH_PST(parse_shdr)(STREAM *file)
{
	ElfN_Ehdr *ehdr;
	ElfN_Shdr *shdr;

	if (!(ehdr = sread(file, 0, sizeof(ElfN_Ehdr))))
		return -1;

	if (ehdr->e_shoff == 0) {
		ft_printf("No section header found !");
		return 0;
	}

	if (!(shdr = sread(file, ehdr->e_shoff, ehdr->e_shentsize * ehdr->e_shnum))) {
		ft_dprintf(STDERR_FILENO, "Corrupted file while parsing section header table\n");
		return -1;
	}

	for (u16 i = 0; i < ehdr->e_shnum; i++) {
		int j = 0;
		for (j = 0; j < SECTION_HEADER_TYPE_N; j++) {
			if (section_header_type[j].type == shdr[i].sh_type) {
				break;
			}
		}
		ElfN_Shdr *s = &shdr[i];
		if (j != SECTION_HEADER_TYPE_N) {
			FT_DEBUG("%2hu: %20s %c%c%c %.8x %.8x %.8x\n", i, section_header_type[j].s,
				  s->sh_flags & SHF_WRITE ? 'W' : ' ',
				  s->sh_flags & SHF_ALLOC ? 'A' : ' ',
				  s->sh_flags & SHF_EXECINSTR ? 'X' : ' ',
				  s->sh_addr,
				  s->sh_offset,
				  s->sh_size
			);
			(void)s; // TODO Find a way to use only when SILENT and not DEBUG
		} else {
			FT_DEBUG("%2hu: %20s\n", i, "Unknown section");
		}
	}
	return 0;
}

int ARCH_PST(add_shdr)(STREAM *output, STREAM *original, PACKER_CONFIG *config)
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

#ifdef _64BITS // TODO Generic
// TODO Refactor: Maybe share a common functon to parse all phdr with get_last_load_phdr (by passing a ft ptr)
int encrypt_old_phdrs(STREAM *output, PACKER_CONFIG *config)
{
	ElfN_Ehdr *elf_hdr;
	ElfN_Shdr *shdr;

	// If not in the payload encrypt
	if (!(elf_hdr = sread(output, 0, sizeof(ElfN_Ehdr))))
		return -1;
	if (!(shdr = sread(output, elf_hdr->e_shoff, elf_hdr->e_shnum * elf_hdr->e_shentsize))) {
		ft_dprintf(STDERR_FILENO, "Corrupted file while parsing program header table\n");
		return -1;
	}
	for (u16 i = 0; i < elf_hdr->e_shnum ; i++) {
		if (shdr->sh_type == SHT_PROGBITS && shdr->sh_flags & SHF_ALLOC && shdr->sh_flags & SHF_EXECINSTR) { // TODO Maybe more sections ?

//#ifdef DEBUG
			ElfN_Shdr *s = shdr;
			int j = 0;
			for (j = 0; j < SECTION_HEADER_TYPE_N; j++) {
				if (section_header_type[j].type == shdr->sh_type)
					break;
			}
			ft_printf("%2hu: %20s %c%c%c %.8x %.8x %.8x\n", i, section_header_type[j].s,
				  s->sh_flags & SHF_WRITE ? 'W' : ' ',
				  s->sh_flags & SHF_ALLOC ? 'A' : ' ',
				  s->sh_flags & SHF_EXECINSTR ? 'X' : ' ',
				  s->sh_addr,
				  s->sh_offset,
				  s->sh_size
			);
//#endif

			// TODO Only if sh_addr exist
			char *ptr = (char *)elf_hdr + shdr->sh_addr;
			u64 end = (char *)elf_hdr + shdr->sh_addr + shdr->sh_size;

			size_t safe_zone_start = (char *)elf_hdr + config->payload_file_off;
			size_t safe_zone_end = safe_zone_start + config->payload_len_aligned;

			// TODO Secure
			while (ptr < end)
			{
				if (ptr < safe_zone_start || ptr > safe_zone_end) {
					*ptr += 1;
				}
				ptr++;
			}
			break;
		}
		shdr++;
	}
	return 0;
}
#endif
