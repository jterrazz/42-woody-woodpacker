#include "woody_woodpacker.h"

#include <elf.h>

int ARCH_PST(config_packer_for_last_load)(STREAM *file, PACKER_CONFIG *conf)
{
	ElfN_Phdr *phdr;
	ElfN_Ehdr *ehdr;
	void *start;

	if (!(ehdr = sread(file, 0, sizeof(ElfN_Ehdr))))
		return -1;
	if (!(phdr = ARCH_PST(get_last_load_phdr)(file)))
		return -1;
	if (!(start = sread(file, 0, sfile_len(file))))
		return -1;

	conf->insert_phdr_off = (void *)phdr - start;
	conf->bss_len = phdr->p_memsz - phdr->p_filesz;
	conf->payload_len_aligned = (ARCH_PST(_payload_size) + 63) & ~63;
	conf->insert_len = conf->payload_len_aligned + conf->bss_len;
	conf->insert_off = phdr->p_offset + phdr->p_filesz;
	conf->payload_mem_off = phdr->p_offset + phdr->p_memsz;
	conf->insert_to_end_len = sfile_len(file) - conf->insert_off;
	conf->new_startpoint_vaddr = phdr->p_memsz + phdr->p_vaddr;
	conf->payload_start_off = conf->insert_off + conf->bss_len;
	conf->old_startpoint = ehdr->e_entry;

	// TODO Probably do a generic function to get the offset from a payload variable to the file start
	conf->relative_jmp_new_pg = conf->old_startpoint - phdr->p_vaddr - phdr->p_memsz - ARCH_PST(_payload_size) + 24;

	conf->output_len = sfile_len(file) + conf->insert_len + sizeof(ElfN_Shdr);

	return 0;
}
