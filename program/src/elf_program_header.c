#include "woody_woodpacker.h"

#include <elf.h>
#include <stdlib.h>

// TODO Maybe use unique function for parse phdr
ElfN_Phdr *ARCH_PST(get_last_load_phdr)(STREAM *file)
{
	ElfN_Ehdr *elf_hdr;
	ElfN_Phdr *phdr;
	ElfN_Phdr *last_load_phdr = NULL;

	if (!(elf_hdr = sread(file, 0, sizeof(ElfN_Ehdr))))
		return NULL;
	if (!(phdr = sread(file, elf_hdr->e_phoff, elf_hdr->e_phnum * elf_hdr->e_phentsize))) {
		ft_dprintf(STDERR_FILENO, "Corrupted file while parsing program header table\n");
		return NULL;
	}
	for (u16 i = 0; i < elf_hdr->e_phnum ; i++) {
		if (phdr->p_type == PT_LOAD)
			last_load_phdr = phdr;
		phdr++;
	}
	return last_load_phdr;
}

/*
 * - insert data at conf->insert_off for conf->insert_len bytes
 * - data starts with a forced bss data set to zero
 */
void *ARCH_PST(p_append_data)(STREAM *out, STREAM *in, PACKER_CONFIG *conf, void *src, size_t src_len)
{
	void	*in_start;
	void	*out_bss_addr;

	if (!(in_start = sread(in, 0, sfile_len(in))))
		return NULL;
	if (swrite(out, in_start , 0, conf->insert_off))
		return NULL;
	if (swrite(out, in_start + conf->insert_off, conf->insert_off + conf->insert_len, conf->insert_to_end_len))
		return NULL;
	if (!(out_bss_addr = sread(out, conf->insert_off, conf->bss_len)))
		return NULL;
	ft_bzero(out_bss_addr, conf->bss_len);

	if (swrite(out, src, conf->payload_start_off, src_len))
		return NULL;

	return sread(out, conf->payload_start_off, src_len);
}

int ARCH_PST(phdr_append_data)(STREAM *output, PACKER_CONFIG *config)
{
	ElfN_Phdr *phdr;

	if (!(phdr = sread(output, config->insert_phdr_off, sizeof(ElfN_Phdr))))
		return -1;

	phdr->p_flags |= PF_X;
	phdr->p_memsz += config->payload_len_aligned;
	phdr->p_filesz += config->insert_len;

	return 0;
}
