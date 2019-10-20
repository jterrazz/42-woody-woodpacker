#include "woody_woodpacker.h"

#include <elf.h>
#include <stdlib.h>

extern u8 _payload64;
extern u64 _payload64_size;

// TODO Transoform to generic
int dump_program_header_generic(void *bin_start, size_t bin_len)
{
    ElfN_Ehdr *header = (ElfN_Ehdr *)secure_read(bin_start, bin_len, 0, sizeof(ElfN_Ehdr));
    if (header == NULL) {
        return -1;
    }
    ElfN_Off offset = header->e_phoff;
    ft_printf("Offset: %p, %d entries of %d bytes\n", offset, header->e_phnum, header->e_phentsize);

    ElfN_Phdr *phdr = (ElfN_Phdr *)secure_read(bin_start, bin_len, offset, header->e_phnum * header->e_phentsize);
    ft_printf("Program Headers:\n");
    ft_printf("Type    Offset       VirtualAddress     PhysAddr\n");

    ElfN_Phdr *last_load_phdr = NULL;
    for (u16 i = 0; i < header->e_phnum ; i++) {
        switch (phdr->p_type) {
            case PT_NULL:
                ft_printf("%-20s", "ULL");
                break;
            case PT_LOAD:
                ft_printf("%-20s", "LOAD");
                last_load_phdr = phdr;
                break;
            case PT_DYNAMIC:
                ft_printf("%-20s", "DYNAMIC");
                break;
            case PT_INTERP:
                ft_printf("%-20s", "INTERP");
                break;
            case PT_NOTE:
                ft_printf("%-20s", "NOTE");
                break;
            case PT_SHLIB:
                ft_printf("%-20s", "SHLIB");
                break;
            case PT_PHDR:
                ft_printf("%-20s", "PHDR");
                break;
            case PT_GNU_RELRO:
                ft_printf("%-20s", "GNU_RELRO");
                break;
            case PT_GNU_EH_FRAME:
                ft_printf("%-20s", "GNU_EH_FRAME");
                break;
            case PT_LOPROC:
            case PT_HIPROC:
                ft_printf("%-20s", "LOPROC, HIPROC");
                break;
            case PT_GNU_STACK:
                ft_printf("%-20s", "GNU_STACK");
                break;
        }
        ft_printf("%10p %10p %10p\n", phdr->p_offset, phdr->p_vaddr, phdr->p_paddr);
        phdr++;
    }

    ft_printf("Last LOAD is at offset: %llx with a memsize of %llx bytes and filesize of %llx vytes\n", last_load_phdr->p_offset, last_load_phdr->p_memsz, last_load_phdr->p_filesz);

    /*
     * Start the creation of new exec
     */

    u64 before_payload_size = last_load_phdr->p_offset + last_load_phdr->p_memsz;
    u64 last_load_bss_size = last_load_phdr->p_memsz - last_load_phdr->p_filesz;
    u64 after_playload_size = bin_len - before_payload_size;
    u64 new_startpoint_offset = before_payload_size + last_load_bss_size;

    ft_printf("Calculated BSS size to fill %llx (%llx - %llx)\n", last_load_bss_size, last_load_phdr->p_memsz, last_load_phdr->p_filesz);

    /*
     * Creates the new executable
     */

    #define OUTPUT_FILENAME "./woody"

    STREAM *output = sopen(OUTPUT_FILENAME, bin_len + _payload64_size + last_load_bss_size);
    if (!output) {
        return -1;
    }

    /*
     * Moves data to insert the payload
     */

    if (swrite(output, bin_start , 0, before_payload_size))
        return -1;

    if (swrite(output, bin_start + before_payload_size, before_payload_size + _payload64_size + last_load_bss_size, after_playload_size))
        return -1;

    void *bss_section = sread(output, before_payload_size, last_load_bss_size + _payload64_size);
    if (!bss_section)
        return -1;

    ft_bzero(bss_section, last_load_bss_size);
    ft_printf("Will copy the payload of %llx bytes at offset %llx\n", _payload64_size, new_startpoint_offset);
    if (swrite(output, &_payload64, new_startpoint_offset, _payload64_size))
        return -1;

    /*
     * Modify the ELF header
     */

    ElfN_Ehdr *output_header = (ElfN_Ehdr *)sread(output, 0, sizeof(ElfN_Ehdr));
    if (output_header == NULL)
        return -1;

    ft_printf("Will modify the original startpoint %llx to %llx\n", output_header->e_entry, new_startpoint_offset);
    output_header->e_entry = new_startpoint_offset;

    /*
     * Good to know. Programs like radare2 in visual mode only prints the instructions in dissaembly mode
     * if it's inside the program section (calculated using the start ptr + size)
     */

    u64 last_load_header_offset = (void *)last_load_phdr - bin_start;

    ElfN_Phdr *output_last_load_header = (ElfN_Phdr *)sread(output, last_load_header_offset, sizeof(ElfN_Phdr));
    if (output_last_load_header == NULL)
        return -1;

    output_last_load_header->p_flags |= PF_X;
    ft_printf("Output last load header: OLD MEMSIZE: %lld, OLD FILESIZE %lld\n", output_last_load_header->p_memsz, output_last_load_header->p_filesz);
    output_last_load_header->p_memsz += _payload64_size;
    output_last_load_header->p_filesz += _payload64_size + last_load_bss_size;
    ft_printf("Output last load header: NEW MEMSIZE: %lld, NEW FILESIZE %lld\n", output_last_load_header->p_memsz, output_last_load_header->p_filesz);

    /*
     * Modify the ELF section headers to reflect the new offset.
     */

    u64 sh_offset = header->e_shoff;
    if (sh_offset > before_payload_size)
        sh_offset += _payload64_size + last_load_bss_size;
    output_header->e_shoff = sh_offset;

    ft_printf("Will access the first section header at: %llx\n", sh_offset);
    ElfN_Shdr *output_sh = sread(output, sh_offset, sizeof(ElfN_Shdr) * header->e_shnum);
    if (!output_sh)
        return -1;

    ft_printf("Output section headers:\n");
    ft_printf("  %10s %10s %10s\n", "sh_offset", "sh_addr", "sh_addralign");
    for (u16 i = 0; i < header->e_shnum ; i++) {
        ft_printf("  %10p %10p %10p\n", output_sh->sh_offset, output_sh->sh_addr, output_sh->sh_addralign);
        output_sh++;
    }



        // TODO Move header->e_shoff too if was move



    // TODO Align Moved data (take last number in hex)
//    ((void(*)(void))&_payload64)();

    sclose(output); // check ret
    return 0;
}
