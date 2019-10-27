NAME = woody_woodpacker
CC = gcc
AS = nasm

###########################
#          FLAGS          #
###########################

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

###########################
#         SOURCES         #
###########################

OBJDIR = obj
VPATH += src

# common C files
SRC_C += main helper/file ident
# generic C files
SRC_C += elf/ehdr_32 elf/ehdr_64 elf/shdr_32 elf/shdr_64 elf/phdr_32 elf/phdr_64 packer_32 packer_64 packer_config_32 packer_config_64
SRC_ASM += payload/entry_64 payload/entry_32

HEADERS += include/woody_woodpacker.h
LIBS += libft_ng/libft.a

OBJ_C = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(SRC_C))))
OBJ_ASM = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(SRC_ASM))))

###########################
#          RULES          #
###########################

.PHONY: all re clean fclean help mrproper exec

all: libft $(NAME)

# Link objects and creates executable
$(NAME): $(OBJ_C) $(OBJ_ASM)
	$(CC) -Wl,--gc-sections $(CFLAGS) $^ $(LDFLAGS) -o $@

# Compile C files
$(OBJDIR)/%.o: %.c $(HEADERS) $(LIBS)
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $<

# generic C files section:
define CC-32
mkdir -p $(dir $@)
$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $< -D _32BITS
endef

define CC-64
mkdir -p $(dir $@)
$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $< -D _64BITS
endef

$(OBJDIR)/elf/ehdr_32.o: elf/ehdr.c $(HEADERS) $(LIBS)
	$(CC-32)
$(OBJDIR)/elf/ehdr_64.o: elf/ehdr.c $(HEADERS) $(LIBS)
	$(CC-64)
$(OBJDIR)/elf/shdr_32.o: elf/shdr.c $(HEADERS) $(LIBS)
	$(CC-32)
$(OBJDIR)/elf/shdr_64.o: elf/shdr.c $(HEADERS) $(LIBS)
	$(CC-64)
$(OBJDIR)/elf/phdr_32.o: elf/phdr.c $(HEADERS) $(LIBS)
	$(CC-32)
$(OBJDIR)/elf/phdr_64.o: elf/phdr.c $(HEADERS) $(LIBS)
	$(CC-64)
$(OBJDIR)/packer_32.o: packer.c $(HEADERS) $(LIBS)
	$(CC-32)
$(OBJDIR)/packer_64.o: packer.c $(HEADERS) $(LIBS)
	$(CC-64)
$(OBJDIR)/packer_config_32.o: packer_config.c $(HEADERS) $(LIBS)
	$(CC-32)
$(OBJDIR)/packer_config_64.o: packer_config.c $(HEADERS) $(LIBS)
	$(CC-64)

# Compile ASM files
$(OBJDIR)/%.o: %.asm $(LIBS) Makefile
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

# Other targets

clean:
	make -C libft_ng clean
	rm -vrf $(OBJDIR)

fclean:
	make -C libft_ng fclean
	rm -vrf $(OBJDIR)
	rm -vf $(NAME)

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
