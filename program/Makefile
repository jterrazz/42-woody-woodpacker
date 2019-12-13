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
#      SOURCES / OBJ      #
###########################

OBJDIR = obj
VPATH += src

# SOURCES
# support 32/64 bits parsing functions

SRC_C += main helper/file ident
ARCH_SRC_C = elf/ehdr elf/shdr elf/phdr packer packer_config

HEADERS += include/woody_woodpacker.h
LIBS += libft_ng/libft.a

SRC_ASM += payload/entry_64 payload/entry_32

# OBJECTS

SRC_C += $(addsuffix __32, $(ARCH_SRC_C)) $(addsuffix __64, $(ARCH_SRC_C))
OBJ_C = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(SRC_C))))
OBJ_ASM = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(SRC_ASM))))

###########################
#          RULES          #
###########################

.PHONY: all re clean fclean help mrproper exec

all: libft $(NAME)

# Link objects and create executable

$(NAME): $(OBJ_C) $(OBJ_ASM)
	$(CC) -Wl,--gc-sections $(CFLAGS) $^ $(LDFLAGS) -o $@

# Compile C files

$(OBJDIR)/%.o: %.c $(HEADERS) $(LIBS) # TODO Doesnt update when .c changes
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $<

# Support for 32/64 bits

$(OBJDIR)/%__32.o: $(subst $(OBJDIR),$(VPATH),$(@:__32.o=.c))
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $(subst $(OBJDIR),$(VPATH),$(@:__32.o=.c)) -D _32BITS

$(OBJDIR)/%__64.o: $(subst $(OBJDIR),$(VPATH),$(@:__64.o=.c))
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $(subst $(OBJDIR),$(VPATH),$(@:__64.o=.c)) -D _64BITS

# Compile ASM files

$(OBJDIR)/%.o: %.asm $(LIBS)
	@mkdir -p $(dir $@)
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
