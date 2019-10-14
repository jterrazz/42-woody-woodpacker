#include "woody_woodpacker.h"

#include <elf.h>

// TODO Transoform to generic
int dump_program_header_64(void *bin_start, size_t bin_len)
{
    Elf64_Ehdr *header = (Elf64_Ehdr *)secure_read(bin_start, bin_len, 0, sizeof(Elf64_Ehdr));
    if (header == NULL) {
        return -1;
    }
    Elf64_Off offset = header->e_phoff;
    ft_printf("Offset: %p, %d entries of %d bytes\n", offset, header->e_phnum, header->e_phentsize);

    Elf64_Phdr *phdr = secure_read(bin_start, bin_len, offset, header->e_phnum * header->e_phentsize);
    ft_printf("Program Headers:\n");
    ft_printf("Type    Offset       VirtualAddress     PhysAddr\n");

    Elf64_Phdr *last_load_phdr = NULL;
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

    ft_printf("Last LOAD header is at offset: %p with size %lld", last_load_phdr->p_offset, last_load_phdr->p_memsz);
}