#include "woody_woodpacker.h"

#include <elf.h>
#include <stdlib.h>

#ifdef _64BITS // TODO Generic
int set_payload64(void *payload, PACKER_CONFIG *config)
{
	u32 *payload_end = payload + _payload64_size;

	if (_payload64_size < 3 * 8 + 4)
		return -1;

	*(payload_end - 7) = config->relative_jmp_new_pg;
	*(payload_end - 6) = 0x000004e8;
	*(payload_end - 4) = 0x17;
	*(payload_end - 2) = 0;

	return 0;
}
#endif

ElfN_Phdr *get_last_load_phdr_generic(STREAM *file)
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

// TODO Reactor to append in phdr
int insert_payload_generic(STREAM *output, STREAM *original, PACKER_CONFIG *config)
{
	void *ori_start;
	size_t ori_len;

	ori_len = sfile_len(original);
	ori_start = sread(original, 0, ori_len); // TODO secure

	if (swrite(output, ori_start , 0, config->payload_file_off)) // TODO Could do a smove feature
		return -1;
	if (swrite(output, ori_start + config->payload_file_off, config->payload_file_off + config->real_payload_len, config->payload_to_end_len))
		return -1;

	void *bss_section = sread(output, config->payload_file_off, config->bss_to_add);
	if (!bss_section)
		return -1;

	ft_bzero(bss_section, config->bss_to_add);
	if (swrite(output, &_payload64, config->new_startpoint_off, _payload64_size))
		return -1;

	return 0;
}

int update_phdr_generic(STREAM *output, PACKER_CONFIG *config)
{
	ElfN_Phdr *output_last_load_header;

	if (!(output_last_load_header = sread(output, config->phdr_selected_off, sizeof(ElfN_Phdr))))
		return -1;

	output_last_load_header->p_flags |= PF_X;
	output_last_load_header->p_memsz += config->payload_len_aligned;
	output_last_load_header->p_filesz += config->real_payload_len;

	return 0;
}

// TODO Rename using offset and addr
int add_hdr_entry_generic(STREAM *output, PACKER_CONFIG *config)
{
	ElfN_Ehdr *output_header = sread(output, 0, sizeof(ElfN_Ehdr));
	if (output_header == NULL)
		return -1;

	output_header->e_entry = config->new_startpoint_vaddr;
	output_header->e_shnum += 1;
	output_header->e_shstrndx += 1;

	return 0;
}
