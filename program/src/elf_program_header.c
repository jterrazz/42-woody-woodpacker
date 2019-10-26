#include "woody_woodpacker.h"

#include <elf.h>
#include <stdlib.h>

extern u8 _payload64;
extern u64 _payload64_size;

#ifdef _64BITS //tmp
int set_payload64(void *payload, size_t payload_size, u64 jump_addr, u64 encrypted_data_start, size_t encrypted_data_len, u64 encryption_key, u32 old_start)
{
	u32 *payload_end = payload + payload_size;

	if (payload_size < 3 * 8 + 4)
		return 1;

	*(payload_end - 7) = old_start;
	*(payload_end - 6) = encrypted_data_start;
	*(payload_end - 4) = encrypted_data_len;
	*(payload_end - 2) = encryption_key;

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
int insert_payload_generic(STREAM *output, STREAM *original)
{
	void *ori_start;

	Elf64_Phdr *phdr = get_last_load_phdr_64(original); // TODO Generic
	size_t ori_len = sfile_len(original);
	ori_start = sread(original, 0, ori_len);

	size_t aligned_payload64_size = (_payload64_size + 63) & ~63;
	size_t last_load_bss_size = phdr->p_memsz - phdr->p_filesz;
	size_t shift_size = aligned_payload64_size + last_load_bss_size;
	u64 before_payload_size = phdr->p_offset + phdr->p_filesz;
	u64 after_payload_size = ori_len - before_payload_size;
	u64 new_startpoint_offset = before_payload_size + last_load_bss_size;

	if (swrite(output, ori_start , 0, before_payload_size)) // TODO Could do a smove feature
		return -1;

	if (swrite(output, ori_start + before_payload_size, before_payload_size + shift_size, after_payload_size))
		return -1;

	void *bss_section = sread(output, before_payload_size, last_load_bss_size + aligned_payload64_size);
	if (!bss_section)
		return -1;

	ft_bzero(bss_section, last_load_bss_size);

	ft_printf("Will copy the payload of %llx bytes at offset %llx\n", _payload64_size, new_startpoint_offset);
	if (swrite(output, &_payload64, new_startpoint_offset, _payload64_size))
		return -1;

	ElfN_Ehdr *elf_hdr;
	if (!(elf_hdr = sread(original, 0, sizeof(ElfN_Ehdr)))) // TODO Maybe use output for this
		return NULL;

	void *payload = sread(output, new_startpoint_offset, _payload64_size); // TODO secure
	u32 old_start_off = elf_hdr->e_entry - phdr->p_vaddr - phdr->p_memsz - _payload64_size + 24;  // (u32) -((u64)&_payload_end - old_start);
	set_payload64(payload, _payload64_size, elf_hdr->e_entry, 42, 42, 42, old_start_off);

	u64 last_load_header_offset = (void *)phdr - ori_start;

	ElfN_Phdr *output_last_load_header = (ElfN_Phdr *)sread(output, last_load_header_offset, sizeof(ElfN_Phdr));
	if (output_last_load_header == NULL)
		return -1;

	output_last_load_header->p_flags |= PF_X;
	ft_printf("Output last load header: OLD MEMSIZE: %lld, OLD FILESIZE %lld\n", output_last_load_header->p_memsz, output_last_load_header->p_filesz);
	output_last_load_header->p_memsz += aligned_payload64_size;
	output_last_load_header->p_filesz += shift_size;
	ft_printf("Output last load header: NEW MEMSIZE: %lld, NEW FILESIZE %lld\n", output_last_load_header->p_memsz, output_last_load_header->p_filesz);

}

// TODO Rename using offset and addr
int add_hdr_entry_generic(STREAM *output, size_t entry)
{
	ElfN_Ehdr *output_header = sread(output, 0, sizeof(ElfN_Ehdr));
	if (output_header == NULL)
		return -1;

	output_header->e_entry = entry;
	output_header->e_shnum += 1;
	output_header->e_shstrndx += 1;
}

int add_shdr_generic(STREAM *output, STREAM *original)
{
	ElfN_Ehdr *elf_hdr;
	if (!(elf_hdr = sread(original, 0, sizeof(ElfN_Ehdr)))) // TODO Maybe use output for this
		return NULL;

	ElfN_Ehdr *output_header;
	if (!(output_header = sread(output, 0, sizeof(ElfN_Ehdr)))) // TODO Rename
		return NULL;

	u64 sh_offset = elf_hdr->e_shoff;


	Elf64_Phdr *phdr = get_last_load_phdr_64(original); // TODO Generic
//	size_t ori_len = sfile_len(original);
//	ori_start = sread(original, 0, ori_len);

	size_t aligned_payload64_size = (_payload64_size + 63) & ~63;
	size_t last_load_bss_size = phdr->p_memsz - phdr->p_filesz;
	size_t shift_size = aligned_payload64_size + last_load_bss_size; // TODO Maybe use in global
	u64 before_payload_size = phdr->p_offset + phdr->p_filesz;
	u64 new_startpoint_offset = before_payload_size + last_load_bss_size;

	if (sh_offset > before_payload_size) // TODO Use insted at returned in main
		sh_offset += shift_size;
	output_header->e_shoff = sh_offset;

	ft_printf("Will access the first section header at: %llx\n", sh_offset);
	ElfN_Shdr *output_sh = sread(output, sh_offset, sizeof(ElfN_Shdr) * output_header->e_shnum);
	if (!output_sh)
		return -1;

	ft_printf("New section headers:\n");
	ft_printf("sections will be moved by %lld bytes\n", shift_size);
	ft_printf("  %10s %10s %15s %10s\n", "sh_offset", "sh_addr", "sh_addralign", "was shifted");

	void *new_sh_addr = 0;
	for (u16 i = 0; i < elf_hdr->e_shnum ; i++) {
		u8 was_moved = 0;
		if (output_sh->sh_offset > before_payload_size) {
			if (!new_sh_addr) {
				new_sh_addr = output_sh;
			}
			output_sh->sh_offset += shift_size;
			was_moved = 1;
		}

		ft_printf("  %10llx %10llx %15llx %15d\n", output_sh->sh_offset, output_sh->sh_addr, output_sh->sh_addralign, was_moved);
		output_sh++;
	}

	/*
	 * Add a new section header
	 */
	new_sh_addr -= sizeof(ElfN_Shdr); // TODO Only do that if the last segment was a BSS and had empty space
	ElfN_Shdr *prev_sh = new_sh_addr - sizeof(ElfN_Shdr) ;

	((ElfN_Shdr *)new_sh_addr)->sh_offset += shift_size;

	size_t output_len = sfile_len(output);

	u64 new_sh_offset = new_sh_addr - (void *)output_header;
	u64 end_output_to_move_size = output_len - new_sh_offset - sizeof(ElfN_Shdr);
	if (swrite(output, new_sh_addr, new_sh_offset + sizeof(ElfN_Shdr), end_output_to_move_size))
		return -1;

	ElfN_Shdr *new_sh = new_sh_addr;
	new_sh->sh_name = 0;
	new_sh->sh_type = SHT_PROGBITS;
	new_sh->sh_flags = SHF_EXECINSTR | SHF_ALLOC;
	new_sh->sh_offset = new_startpoint_offset;
	new_sh->sh_addr = prev_sh->sh_addr + (new_startpoint_offset - prev_sh->sh_offset); // TODO This should also works when addr is not 0
	new_sh->sh_size = aligned_payload64_size;
	new_sh->sh_link = 0;
	new_sh->sh_info =  0;
	new_sh->sh_addralign = 16;
	new_sh->sh_entsize = 0;
}
