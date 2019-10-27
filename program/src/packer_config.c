#include "woody_woodpacker.h"

#include <elf.h>

/*
 * STATIC METHODS
 */

static size_t ARCH_PST(get_packed_len)(STREAM *file, size_t *ret_len)
{
	Elf64_Phdr *phdr;

	if (!(phdr = ARCH_PST(get_last_load_phdr)(file)))
		return -1;

	size_t aligned_payload64_len = (ARCH_PST(_payload_size) + 63) & ~63;
	size_t last_load_bss_size = phdr->p_memsz - phdr->p_filesz;
	size_t shift_size = aligned_payload64_len + last_load_bss_size;
	*ret_len = sfile_len(file) + shift_size + sizeof(Elf64_Shdr);

	return 0;
}

/*
 * PUBLIC METHODS
 */

int ARCH_PST(config_packer_for_last_load)(STREAM *file, PACKER_CONFIG *packed_file)
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

	// TODO Maybe do a generic function in s... that copy data giving the origin ptr, and comparing if data was moved in output
	packed_file->insert_phdr_off = (void *)phdr - start;
	packed_file->bss_len = phdr->p_memsz - phdr->p_filesz;
	packed_file->payload_len_aligned = (ARCH_PST(_payload_size) + 63) & ~63;
	packed_file->insert_len = packed_file->payload_len_aligned + packed_file->bss_len; // TODO Rename this bc its not the payload
	packed_file->insert_off = phdr->p_offset + phdr->p_filesz;
	packed_file->payload_mem_off = phdr->p_offset + phdr->p_memsz;
	packed_file->insert_to_end_len = sfile_len(file) - packed_file->insert_off;
	packed_file->new_startpoint_vaddr = phdr->p_memsz + phdr->p_vaddr;
	packed_file->payload_start_off = packed_file->insert_off + packed_file->bss_len;
	packed_file->old_startpoint = ehdr->e_entry;

	// TODO Probably better config this var
	packed_file->relative_jmp_new_pg = packed_file->old_startpoint - phdr->p_vaddr - phdr->p_memsz - ARCH_PST(_payload_size) + 24;


	if (ARCH_PST(get_packed_len)(file, &(packed_file->output_len)))
		return -1;

	return 0;
}
