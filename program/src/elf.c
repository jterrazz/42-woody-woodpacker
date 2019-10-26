#include "woody_woodpacker.h"

#include <elf.h>
#include <assert.h>

struct e_ident {
	char magic[4];
	u8 class;
	u8 endian;
	u8 version;
	u8 os_abi;
	u8 version_abi;
	u8 pad[7];
} __attribute__((packed));

struct e_ident *parse_ident_field(STREAM *file)
{
	size_t len = sfile_len(file);
	struct e_ident *e_ident = sread(file, 0, EI_NIDENT);
	if (e_ident == NULL) {
		return NULL;
	}

	ft_printf("EI_NIDENT system value is: %u.\n", EI_NIDENT);
	assert(EI_NIDENT == sizeof(struct e_ident));

	char magic[4] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
	if (ft_memcmp(e_ident->magic, magic, 4) != 0) {
		ft_dprintf(STDERR_FILENO, "Bad ELF magic\n");
		return NULL;
	}

	switch (e_ident->class) {
	case ELFCLASS32:
		ft_printf("ELF class 32.\n");
		break;
	case ELFCLASS64:
		ft_printf("ELF class 64.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
		return NULL;
	}

	switch (e_ident->endian) {
	case ELFDATA2LSB:
		ft_printf("Two's complement, little-endian.\n");
		break;
	case ELFDATA2MSB:
		ft_printf("Two's complement, big-endian.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "Unknown data format.\n");
		return NULL;
	}

	switch (e_ident->version) {
	case EV_CURRENT:
		ft_printf("ELF version is Current version.\n");
		break;
	case EV_NONE:
		ft_dprintf(STDERR_FILENO, "Invalid version.\n");
		return NULL;
	default:
		// I don't know how to manage that case
		ft_printf("ELF version is %hhu\n", e_ident->version);
		break;
	}

	char *str;
	switch (e_ident->os_abi) {
	case ELFOSABI_SYSV:
		str = "UNIX System V ABI.";
		break;
	case ELFOSABI_HPUX:
		str = "HP-UX ABI.";
		break;
	case ELFOSABI_NETBSD:
		str = "NetBSD ABI.";
		break;
	case ELFOSABI_LINUX:
		str = "Linux ABI.";
		break;
	case ELFOSABI_SOLARIS:
		str = "Solaris ABI.";
		break;
	case ELFOSABI_IRIX:
		str = "IRIX ABI.";
		break;
	case ELFOSABI_FREEBSD:
		str = "FreeBSD ABI.";
		break;
	case ELFOSABI_TRU64:
		str = "TRU64 UNIX ABI.";
		break;
	case ELFOSABI_ARM:
		str = "ARM architecture ABI.";
		break;
	case ELFOSABI_STANDALONE:
		str = "Stand-alone (embedded) ABI.";
		break;
	default:
		ft_dprintf(STDERR_FILENO, "Unknown OS ABI.\n");
		return NULL;
	}
	ft_printf("%s\n", str);

	ft_printf("Private ABI version is %hhu.\n", e_ident->version_abi);
	return e_ident;
}

// TODO Generic
size_t get_output_len(STREAM *file)
{
	Elf64_Phdr *phdr = get_last_load_phdr_64(file); // TODO Secure

	size_t aligned_payload64_size = (_payload64_size + 63) & ~63;
	size_t last_load_bss_size = phdr->p_memsz - phdr->p_filesz;
	size_t shift_size = aligned_payload64_size + last_load_bss_size;
	size_t len = sfile_len(file);

	return len + shift_size + sizeof(Elf64_Shdr);
}

// TODO Generic
int config_packer_for_last_load(STREAM *file, PACKER_CONFIG *packed_file)
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
	packed_file->payload_len_aligned = (_payload64_size + 63) & ~63;
	packed_file->real_payload_len = packed_file->payload_len_aligned + packed_file->bss_to_add;
	packed_file->payload_file_off = phdr->p_offset + phdr->p_filesz;
	packed_file->payload_mem_off = phdr->p_offset + phdr->p_memsz;
	packed_file->payload_to_end_len = sfile_len(file) - packed_file->payload_file_off; // TODO Maybe do mem/file too
	packed_file->new_startpoint = packed_file->payload_file_off + packed_file->bss_to_add; // TODO Maybe do mem/file too
	packed_file->old_startpoint = ehdr->e_entry;

	// TODO Probably better config this var
	packed_file->relative_jmp_new_pg = packed_file->old_startpoint - phdr->p_vaddr - phdr->p_memsz - _payload64_size + 24;

	return 0;
}

int read_elf(STREAM *file)
{
	PACKER_CONFIG config;
	STREAM *output;
	size_t output_len;
	struct e_ident *ident_field;

	if (!(ident_field = parse_ident_field(file)))
	    return -1;

	output_len = get_output_len(file);
	if (config_packer_for_last_load(file, &config))
		return -1;
	if (!(output = sopen(OUTPUT_FILENAME, output_len, S_RDWR))) // TODO Will need to transform this to a simple mmap because compression will reduce size
		return -1;

	// TODO Secure calls
	Elf64_Phdr *last_load_phdr = get_last_load_phdr_64(file);
	insert_payload_64(output, file, &config); // TODO Maybe refactor for more generic (set an addr)
	update_phdr_64(output, &config);
	add_hdr_entry_64(output, last_load_phdr->p_memsz + last_load_phdr->p_vaddr);
	add_shdr_64(output, file, &config);

	void *payload = sread(output, config.new_startpoint, _payload64_size); // TODO secure
	set_payload64(payload, &config);


	sclose(output);

	if (ident_field->class == ELFCLASS32) {
//		Elf32_Ehdr *header = parse_elf_header_32(data, len);
//		dump_section_header_32(data, len);
//        dump_program_header_32(data, len);
//
//        (void)header;
	} else if (ident_field->class == ELFCLASS64) {
//		Elf64_Ehdr *header = parse_elf_header_64(data, len);
//		dump_section_header_64(data, len);
//        dump_program_header_64(data, len);
//        (void)header;
	} else {
		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
		return -1;
	}

	return 0;
}
