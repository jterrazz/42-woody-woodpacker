#include "woody_woodpacker.h"

#include <elf.h>

int ARCH_PST(set_payload)(void *payload, PACKER_CONFIG *config) // TODO Generic working version
{
	u32 *payload_end = payload + _payload_size_64;

	if (_payload_size_64 < 3 * 8 + 4)
		return -1;

	*(payload_end - 7) = config->relative_jmp_new_pg;
	*(payload_end - 6) = 0x000004e8;
	*(payload_end - 4) = 0x17;
	*(payload_end - 2) = 0;

	return 0;
}

int ARCH_PST(create_packed_output)(STREAM *file)
{
	PACKER_CONFIG	config;
	STREAM			*output;

	if (ARCH_PST(config_packer_for_last_load)(file, &config))
		return -1;

	// TODO Will need to transform this to a simple mmap because compression will reduce size
	if (!(output = sopen(OUTPUT_FILENAME, config.output_len, S_RDWR)))
		return -1;

	if (
		!ARCH_PST(phdr_append_data)(output, file, &config)
		|| ARCH_PST(add_hdr_entry)(output, &config)
		|| ARCH_PST(update_phdr)(output, &config)
		|| ARCH_PST(add_shdr)(output, file, &config)
		)
		return -1;

//	if (encrypt_old_phdrs(output, &config))
//		return -1;

	void *payload = sread(output, config.new_startpoint_off, _payload_size_64); // TODO secure + generic
	ARCH_PST(set_payload)(payload, &config);
	sclose(output);
	return 0;
}

#ifndef _64BITS
int start_packer(STREAM *file)
{
	struct e_ident	*ident_field;

	if (!(ident_field = parse_ident(file)))
	    return -1;

	if (ident_field->class == ELFCLASS32) {
		if (!parse_elf_header_32(file) || parse_shdr_32(file))
			return -1;
		if (create_packed_output_32(file))
			return -1;
	} else if (ident_field->class == ELFCLASS64) {
		if (!parse_elf_header_64(file) || parse_shdr_64(file))
			return -1;
		if (create_packed_output_64(file))
			return -1;
	} else {
		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
		return -1;
	}

	ft_printf("%s created 😱\n", OUTPUT_FILENAME);
	return 0;
}
#endif
