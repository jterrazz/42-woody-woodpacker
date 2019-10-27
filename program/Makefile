NAME = woody_woodpacker
CC = gcc
AS = nasm

ifeq ($(DEBUG),yes)
CFLAGS = -g -O0 -fsanitize=address -std=gnu11 -Wall -Wextra -D DEBUG
else
CFLAGS = -Ofast -fno-omit-frame-pointer -march=native -std=gnu11 -Wall -Wextra
endif
IFLAGS = -I./include -I./libft_ng/includes
ASFLAGS = -f elf64
LDFLAGS = -L./libft_ng -lft

ifeq ($(SILENT),yes)
CFLAGS += -D SILENT
endif

ifeq ($(_42_),yes)
CFLAGS += -D _42_
endif

OBJDIR = obj

VPATH += src
# common C files
SRC_C += main common packer file ident packer_config
# generic C files
SRC_C += elf_header_32 elf_header_64 elf_section_header_32 elf_section_header_64 elf_program_header_32 elf_program_header_64
SRC_ASM += payload_64 payload_32

HEADERS += include/woody_woodpacker.h
LIBS += libft_ng/libft.a

OBJ_C = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC_C)))))
OBJ_ASM = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC_ASM)))))

.PHONY: all re clean fclean help mrproper exec

all: libft $(NAME)

# link

$(NAME): $(OBJ_C) $(OBJ_ASM)
	$(CC) -Wl,--gc-sections $(CFLAGS) $^ $(LDFLAGS) -o $@

# C files

$(OBJDIR)/%.o: %.c $(HEADERS) $(LIBS) Makefile
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $<

# generic C files section:

$(OBJDIR)/elf_header_32.o: elf_header.c $(HEADERS) $(LIBS) Makefile
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $< \
	-D ElfN_Ehdr=Elf32_Ehdr \
	-D parse_elf_header_generic=parse_elf_header_32 \
	-D ElfN_Addr=Elf32_Addr \
	-D ElfN_Off=Elf32_Off \
	-D _32BITS

$(OBJDIR)/elf_header_64.o: elf_header.c $(HEADERS) $(LIBS) Makefile
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $< \
	-D ElfN_Ehdr=Elf64_Ehdr \
	-D parse_elf_header_generic=parse_elf_header_64 \
	-D ElfN_Addr=Elf64_Addr \
	-D ElfN_Off=Elf64_Off \
	-D _64BITS

$(OBJDIR)/elf_section_header_32.o: elf_section_header.c $(HEADERS) $(LIBS) Makefile
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $< \
	-D ElfN_Ehdr=Elf32_Ehdr \
	-D parse_shdr_generic=parse_shdr_32 \
	-D ElfN_Addr=Elf32_Addr \
	-D ElfN_Off=Elf32_Off \
	-D ElfN_Shdr=Elf32_Shdr \
	-D _32BITS

$(OBJDIR)/elf_section_header_64.o: elf_section_header.c $(HEADERS) $(LIBS) Makefile
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $< \
	-D ElfN_Ehdr=Elf64_Ehdr \
	-D parse_shdr_generic=parse_shdr_64 \
	-D add_shdr_generic=add_shdr_64 \
	-D ElfN_Addr=Elf64_Addr \
	-D ElfN_Off=Elf64_Off \
	-D ElfN_Shdr=Elf64_Shdr \
	-D _64BITS

$(OBJDIR)/elf_program_header_32.o: elf_program_header.c $(HEADERS) $(LIBS) Makefile # TODO Update from 64
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $< \
	-D dump_program_header_generic=dump_program_header_32 \
	-D ElfN_Ehdr=Elf32_Ehdr \
	-D ElfN_Off=Elf32_Off \
	-D ElfN_Phdr=Elf32_Phdr \
	-D ElfN_Shdr=Elf32_Shdr \
	-D _32BITS

$(OBJDIR)/elf_program_header_64.o: elf_program_header.c $(HEADERS) $(LIBS) Makefile # TODO Try finding more generic opt
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $< \
	-D dump_program_header_generic=dump_program_header_64 \
	-D get_last_load_phdr_generic=get_last_load_phdr_64 \
	-D __N__=64 \
	-D add_hdr_entry_generic=add_hdr_entry_64 \
	-D update_phdr_generic=update_phdr_64 \
	-D ElfN_Ehdr=Elf64_Ehdr \
	-D ElfN_Off=Elf64_Off \
	-D ElfN_Phdr=Elf64_Phdr \
	-D ElfN_Shdr=Elf64_Shdr \
	-D _64BITS
#	-D insert_payload_generic=insert_payload_64 \

# asm files

$(OBJDIR)/%.o: %.asm $(LIBS) Makefile
	$(AS) $(ASFLAGS) -o $@ $<

# other targets

clean:
	make -C libft_ng clean
	rm -vf $(OBJ_C) $(OBJ_ASM)

fclean:
	make -C libft_ng fclean
	rm -vf $(OBJ_C) $(OBJ_ASM)
	rm -vf $(NAME)
	rm -f tmpwoody

re: fclean all

libft:
	make -C libft_ng

mrproper: fclean
	find . -name "*~" -exec rm -v {} \;
	find . -name "*#" -exec rm -v {} \;
	find . -name "*.orig" -exec rm -v {} \;

exec:
	./$(NAME) woot

help:
	@echo
	@echo "Program $(NAME)"
	@echo
	@echo "--------------------------------------------------------------------------"
	@echo " Disp rules."
	@echo
	@echo " all     : Compile the program if a file has changed."
	@echo " re      : Recompile all objets of the librairy."
	@echo " clean   : Remove objects."
	@echo " fclean  : Remove objects and executable."
	@echo " help    : Display this."
	@echo "--------------------------------------------------------------------------"
	@echo
