#include "woody_woodpacker.h"

#include <elf.h>

// TODO Probably need a better way to config the payload
// TODO Do a generic function to get the offset from a payload variable to the file start
int ARCH_PST(config_payload)(STREAM *out, PACKER_CONFIG *config)
{
	void	*payload;
	u32		*payload_end;

#ifndef _64BITS
	unsigned int mul = 1;
#else
	unsigned int mul = 2;
#endif

	if (!(payload = sread(out, config->payload_start_off, ARCH_PST(_payload_size))))
		return -1;
	payload_end = payload + _payload_size_64;

	if (_payload_size_64 < 3 * 4 * mul + 4)
		return -1;

	*(payload_end - 3 * mul - 1) = config->relative_jmp_new_pg;
	*(payload_end - 3 * mul) = 0x000004e8;
	*(payload_end - 2 * mul) = 0x17;
	*(payload_end - 1 * mul) = 0;

	return 0;
}

int ARCH_PST(create_packed)(STREAM *file)
{
	PACKER_CONFIG	config;
	STREAM			*output;

	if (ARCH_PST(config_packer_for_last_load)(file, &config))
		return -1;

	// TODO Will need to transform this to a simple mmap because compression will reduce size
	if (!(output = sopen(OUTPUT_FILENAME, config.output_len, S_RDWR)))
		return -1;

	if (
		!ARCH_PST(p_append_data)(output, file, &config, ARCH_PST(&_payload), ARCH_PST(_payload_size))
		|| ARCH_PST(phdr_update_for_payload)(output, &config)
		|| ARCH_PST(add_shdr)(output, &config)
		|| ARCH_PST(ehdr_update)(output, &config)
		|| ARCH_PST(encrypt_shdrs)(output, &config)
		|| ARCH_PST(config_payload)(output, &config)
		)
		return -1;

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
		if (!parse_ehdr_32(file) || parse_shdr_32(file, NULL, NULL) || create_packed_32(file))
			return -1;
	} else if (ident_field->class == ELFCLASS64) {
		if (!parse_ehdr_64(file) || parse_shdr_64(file, NULL, NULL) || create_packed_64(file))
			return -1;
	} else {
		ft_dprintf(STDERR_FILENO, "Bad ELF class.\n");
		return -1;
	}

	ft_printf("%s created 😱\n", OUTPUT_FILENAME);
	return 0;
}
#endif
