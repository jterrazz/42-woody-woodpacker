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

struct e_ident *parse_ident(STREAM *file)
{
	struct e_ident *e_ident = sread(file, 0, EI_NIDENT);
	if (e_ident == NULL) {
		return NULL;
	}

	FT_DEBUG("EI_NIDENT system value is: %u.\n", EI_NIDENT);
	assert(EI_NIDENT == sizeof(struct e_ident));

	char magic[4] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
	if (ft_memcmp(e_ident->magic, magic, 4) != 0) {
		ft_dprintf(STDERR_FILENO, "Bad ELF magic\n");
		return NULL;
	}

	switch (e_ident->class) {
	case ELFCLASS32:
		FT_DEBUG("ELF class 32.\n");
		break;
	case ELFCLASS64:
		FT_DEBUG("ELF class 64.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
		return NULL;
	}

	switch (e_ident->endian) {
	case ELFDATA2LSB:
		FT_DEBUG("Two's complement, little-endian.\n");
		break;
	case ELFDATA2MSB:
		FT_DEBUG("Two's complement, big-endian.\n");
		break;
	default:
		ft_dprintf(STDERR_FILENO, "Unknown data format.\n");
		return NULL;
	}

	switch (e_ident->version) {
	case EV_CURRENT:
		FT_DEBUG("ELF version is Current version.\n");
		break;
	case EV_NONE:
		ft_dprintf(STDERR_FILENO, "Invalid version.\n");
		return NULL;
	default:
		// TODO I don't know how to manage that case
		FT_DEBUG("ELF version is %hhu\n", e_ident->version);
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
	FT_DEBUG("%s\n", str);
	(void)str;
	FT_DEBUG("Private ABI version is %hhu.\n", e_ident->version_abi);

	return e_ident;
}

// TODO Generic
static size_t get_output_len(STREAM *file, size_t payload_size)
{
	Elf64_Phdr *phdr = get_last_load_phdr_64(file); // TODO Secure

	size_t aligned_payload64_len = (payload_size + 63) & ~63;
	size_t last_load_bss_size = phdr->p_memsz - phdr->p_filesz;
	size_t shift_size = aligned_payload64_len + last_load_bss_size;
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
	packed_file->new_startpoint_vaddr = phdr->p_memsz + phdr->p_vaddr;
	packed_file->new_startpoint_off = packed_file->payload_file_off + packed_file->bss_to_add; // TODO Maybe do mem/file too
	packed_file->old_startpoint = ehdr->e_entry;

	// TODO Probably better config this var
	packed_file->relative_jmp_new_pg = packed_file->old_startpoint - phdr->p_vaddr - phdr->p_memsz - _payload64_size + 24;

	return 0;
}

int start_packer(STREAM *file, u8 class)
{
	PACKER_CONFIG	config;
	STREAM			*output;
	size_t			output_len;

	output_len = get_output_len(file, class == ELFCLASS32 ? _payload64_size : _payload64_size);

	if (config_packer_for_last_load(file, &config))
		return -1;
	if (!(output = sopen(OUTPUT_FILENAME, output_len, S_RDWR))) // TODO Will need to transform this to a simple mmap because compression will reduce size
		return -1;
	if (
		insert_payload_64(output, file, &config)
		|| add_hdr_entry_64(output, &config)
		|| update_phdr_64(output, &config)
		|| add_shdr_64(output, file, &config)
		)
		return -1;

	void *payload = sread(output, config.new_startpoint_off, _payload64_size); // TODO secure
	set_payload64(payload, &config);
	sclose(output);
	ft_printf("%s created 😱\n", OUTPUT_FILENAME);

	return 0;
}

int read_elf(STREAM *file)
{
	struct e_ident	*ident_field;
	void			*ehdr;

	if (!(ident_field = parse_ident(file)))
	    return -1;

	if (ident_field->class == ELFCLASS32) {
		if (!(ehdr = parse_elf_header_32(file)))
			return -1;
		if (parse_shdr_32(file))
			return -1;
		if (start_packer(file, ELFCLASS32))
			return -1;
	} else if (ident_field->class == ELFCLASS64) {
		if (!(ehdr = parse_elf_header_32(file)))
			return -1;
		if (parse_shdr_64(file))
			return -1;
		if (start_packer(file, ELFCLASS64))
			return -1;
	} else {
		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
		return -1;
	}

	return 0;
}
