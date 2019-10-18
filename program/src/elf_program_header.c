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

    ft_printf("Last LOAD header is at offset: %llx with size %llx\n", last_load_phdr->p_offset, last_load_phdr->p_memsz);


    u64 before_end_load_size = bin_len - last_load_phdr->p_offset + last_load_phdr->p_memsz;
    u64 after_load_size = bin_len - last_load_phdr->p_offset - last_load_phdr->p_memsz;
    u64 bss_size = last_load_phdr->p_memsz - last_load_phdr->p_filesz;
    u64 new_startpoint = bss_size + before_end_load_size;

    STREAM *new_exec = sopen("./tmpwoody", bin_len + _payload64_size + bss_size);
    if (!new_exec) {
        return -1;
    }

// secure
    int i = swrite(new_exec, bin_start , 0, before_end_load_size);
    int j = swrite(new_exec, bin_start + before_end_load_size, before_end_load_size + _payload64_size + bss_size, after_load_size);

    void *bss_section = sread(new_exec, before_end_load_size, _payload64_size);
    if (!bss_section) {
        return -1;
    }
    ft_bzero(bss_section, _payload64_size);
    ft_printf("Will move the data after last load from %llx %llx = %llx\n", last_load_phdr->p_offset, last_load_phdr->p_memsz, last_load_phdr->p_offset + last_load_phdr->p_memsz);
    int k = swrite(new_exec, &_payload64, new_startpoint, _payload64_size);

    ft_printf("returns: %d %d %d\n", i, j, k);

    ElfN_Ehdr *header_new = (ElfN_Ehdr *)sread(new_exec, 0, sizeof(ElfN_Ehdr));
    if (header_new == NULL) {
        return -1;
    }

    ft_printf("Startpoint: %p\n", header_new->e_entry);
    header_new->e_entry = new_startpoint;
    ft_printf("New startpoint: %p\nPayload size: %llx\n", header_new->e_entry, _payload64_size);


    // Secure
    /*
     * Good to know. Programs like radare2 in visual mode only prints the instructions in dissaembly mode if it's inside the program section (calculated using the start ptr + size)
     */

    ElfN_Phdr *new_last_load_phdr = (ElfN_Phdr *)sread(new_exec, (void *)last_load_phdr - bin_start, sizeof(ElfN_Phdr));
    if (new_last_load_phdr == NULL) {
        return -1;
    }

    new_last_load_phdr->p_flags |= PF_X;
    ft_printf("OLD last load header old size: %p\n", new_last_load_phdr->p_memsz);
    ft_printf("OLD last load header new p_filesize: %p\n", new_last_load_phdr->p_filesz);
    new_last_load_phdr->p_memsz += _payload64_size + bss_size;
    new_last_load_phdr->p_filesz += _payload64_size;
    ft_printf("New last load header new p_memsize: %p\n", new_last_load_phdr->p_memsz);
    ft_printf("New last load header new p_filesize: %p\n", new_last_load_phdr->p_filesz);
    // TODO SHOULD BE THE SAME

//    ((void(*)(void))&_payload64)();

    sclose(new_exec); // check ret
    return 0;
}
