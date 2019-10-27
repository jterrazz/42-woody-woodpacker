#include "woody_woodpacker.h"

#include <elf.h>

/*
 * STATIC METHODS
 */

// TODO Generic
static size_t get_packed_len(STREAM *file, size_t payload_size, size_t *ret_len)
{
	Elf64_Phdr *phdr;

	if (!(phdr = get_last_load_phdr_64(file)))
		return -1;

	size_t aligned_payload64_len = (payload_size + 63) & ~63;
	size_t last_load_bss_size = phdr->p_memsz - phdr->p_filesz;
	size_t shift_size = aligned_payload64_len + last_load_bss_size;
	*ret_len = sfile_len(file) + shift_size + sizeof(Elf64_Shdr);

	return 0;
}

// TODO Generic
int config_packer_for_last_load(STREAM *file, u8 elf_class, PACKER_CONFIG *packed_file)
{
	Elf64_Phdr *phdr;
	Elf64_Ehdr *ehdr;
	void *start;

	if (!(ehdr = sread(file, 0, sizeof(Elf64_Ehdr))))
		return -1;
	if (!(phdr = get_last_load_phdr_64(file)))
		return -1;
	if (!(start = sread(file, 0, sfile_len(file))))
		return -1;

	// TODO Maybe do a generic function in s... that copy data giving the origin ptr, and comparing if data was moved in output
	packed_file->phdr_selected_off = (void *)phdr - start;
	packed_file->bss_to_add = phdr->p_memsz - phdr->p_filesz;
	packed_file->payload_len_aligned = (_payload_size_64 + 63) & ~63;
	packed_file->real_payload_len = packed_file->payload_len_aligned + packed_file->bss_to_add; // TODO Rename this bc its not the payload
	packed_file->payload_file_off = phdr->p_offset + phdr->p_filesz;
	packed_file->payload_mem_off = phdr->p_offset + phdr->p_memsz;
	packed_file->payload_to_end_len = sfile_len(file) - packed_file->payload_file_off; // TODO Maybe do mem/file too
	packed_file->new_startpoint_vaddr = phdr->p_memsz + phdr->p_vaddr;
	packed_file->new_startpoint_off = packed_file->payload_file_off + packed_file->bss_to_add; // TODO Maybe do mem/file too
	packed_file->old_startpoint = ehdr->e_entry;

	// TODO Probably better config this var
	packed_file->relative_jmp_new_pg = packed_file->old_startpoint - phdr->p_vaddr - phdr->p_memsz - _payload_size_64 + 24;


	if (get_packed_len(file, elf_class == ELFCLASS32 ? _payload_size_64 : _payload_size_64, &(packed_file->output_len)))
		return -1;

	return 0;
}
