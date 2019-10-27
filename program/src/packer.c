#include "woody_woodpacker.h"

#include <elf.h>

int ARCH_PST(create_packed_output)(STREAM *file, u8 elf_class) // TODO Delete class
{
	PACKER_CONFIG	config;
	STREAM			*output;

	if (config_packer_for_last_load(file, elf_class, &config))
		return -1;

	// TODO Will need to transform this to a simple mmap because compression will reduce size
	if (!(output = sopen(OUTPUT_FILENAME, config.output_len, S_RDWR)))
		return -1;

	if (
		ARCH_PST(insert_payload)(output, file, &config)
		|| ARCH_PST(add_hdr_entry)(output, &config)
		|| ARCH_PST(update_phdr)(output, &config)
		|| add_shdr_64(output, file, &config)
		)
		return -1;

//	if (encrypt_old_phdrs(output, &config))
//		return -1;

	void *payload = sread(output, config.new_startpoint_off, _payload_size_64); // TODO secure
	set_payload64(payload, &config);
	sclose(output);
	ft_printf("%s created 😱\n", OUTPUT_FILENAME);
	return 0;
}

#ifndef _32BITS
int start_packer(STREAM *file)
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
		if (create_packed_output_32(file, ELFCLASS32))
			return -1;
	} else if (ident_field->class == ELFCLASS64) {
		if (!(ehdr = parse_elf_header_32(file)))
			return -1;
		if (parse_shdr_64(file))
			return -1;
		if (create_packed_output_64(file, ELFCLASS64))
			return -1;
	} else {
		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
		return -1;
	}

	return 0;
}
#endif
