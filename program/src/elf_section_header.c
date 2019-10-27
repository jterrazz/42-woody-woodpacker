#include "woody_woodpacker.h"

#include <elf.h>

#define SECTION_HEADER_TYPE_N 12

struct section_header_type {
	u32 type;
	char *s;
};

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

/*
 * Packer
 */

/*
 * Return the first to be updated
 */
static void *ARCH_PST(update_shdrs_off)(ElfN_Shdr *shdr, size_t shnum, size_t offset, size_t insert_len)
{
	void *first_updated = 0;

	for (u16 i = 0; i < shnum ; i++) {
		if (shdr->sh_offset > offset) {
			if (!first_updated)
				first_updated = shdr;
			shdr->sh_offset += insert_len;
		}
		shdr++;
	}
	return first_updated;
}

int ARCH_PST(add_shdr)(STREAM *file, PACKER_CONFIG *conf)
{
	ElfN_Ehdr	*ehdr;
	ElfN_Shdr	*shdr;

	if (!(ehdr = sread(file, 0, sizeof(ElfN_Ehdr))))
		return -1;

	if (ehdr->e_shoff > conf->insert_off)
		ehdr->e_shoff += conf->insert_len;

	if (!(shdr = sread(file, ehdr->e_shoff, sizeof(ElfN_Shdr) * ehdr->e_shnum)))
		return -1;

	ElfN_Shdr *new_shdr = ARCH_PST(update_shdrs_off)(shdr, ehdr->e_shnum, conf->insert_off, conf->insert_len);
	ElfN_Shdr *prev_shdr = new_shdr - 1; // TODO This is not secure

	new_shdr->sh_offset += conf->insert_len;
	size_t new_shdr_off = (void *)new_shdr - (void *)ehdr;
	size_t shift_len = sfile_len(file) - new_shdr_off - sizeof(ElfN_Shdr);

	if (swrite(file, new_shdr, new_shdr_off + sizeof(ElfN_Shdr), shift_len))
		return -1;

	new_shdr->sh_name = 0;
	new_shdr->sh_type = SHT_PROGBITS;
	new_shdr->sh_flags = SHF_EXECINSTR | SHF_ALLOC;
	new_shdr->sh_offset = conf->payload_start_off;
	new_shdr->sh_addr = prev_shdr->sh_addr + (conf->payload_start_off - prev_shdr->sh_offset);
	new_shdr->sh_size = conf->payload_len_aligned;
	new_shdr->sh_link = 0;
	new_shdr->sh_info =  0;
	new_shdr->sh_addralign = 16;
	new_shdr->sh_entsize = 0;

	return 0;
}

#ifndef _64BITS // TODO Generic
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
			ft_printf("Encrypt %2hu: %20s %c%c%c %.8x %.8x %.8x\n", i, section_header_type[j].s,
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

			size_t safe_zone_start = (char *)elf_hdr + config->insert_off;
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

/*
 * Parsing
 */

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
