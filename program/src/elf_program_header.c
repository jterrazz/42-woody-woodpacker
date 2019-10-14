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
    for (u16 i = 0; i < header->e_phnum ; i++) {
        switch (phdr->p_type) {
            case PT_NULL:
                ft_printf("ULL\n");
                break;
            case PT_LOAD:
                ft_printf("LOAD\n");
                break;
            case PT_DYNAMIC:
                ft_printf("DYNAMIC\n");
                break;
            case PT_INTERP:
                ft_printf("INTERP\n");
                break;
            case PT_NOTE:
                ft_printf("NOTE\n");
                break;
            case PT_SHLIB:
                ft_printf("SHLIB\n");
                break;
            case PT_PHDR:
                ft_printf("PHDR\n");
                break;
            case PT_GNU_RELRO:
                ft_printf("GNU_RELRO\n");
                break;
            case PT_LOPROC:
            case PT_HIPROC:
                ft_printf("LOPROC, HIPROC\n");
                break;
            case PT_GNU_STACK:
                ft_printf("GNU_STACK\n");
                break;
        }
        phdr++;
    }
}