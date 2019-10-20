TARGET = jbe_encode jbe_decode mtf

CC = gcc
AS = nasm

ifeq ($(DEBUG),yes)
CFLAGS = -m64 -g -O0 -fsanitize=address -std=gnu11 -Wall -Wextra
else
CFLAGS = -m64 -Ofast -fno-omit-frame-pointer -march=native -std=gnu11 -Wall -Wextra
endif
IFLAGS = -I./include
ASFLAGS = -f elf64

VPATH = src
OBJDIR	= obj

SRC_C = common file jbe
SRC_ASM =
HEADERS = jbe.h common.h
VPATH += include

OBJ_C = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC_C)))))
OBJ_ASM = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC_ASM)))))

.PHONY: all re clean fclean help mrproper

all: $(TARGET)
link = $(CC) $(CFLAGS) $^ -o $@
common_objects = $(OBJ_C) $(OBJ_ASM)

jbe_encode: $(OBJDIR)/jbe_encode.o $(common_objects)
	$(link)

jbe_decode: $(OBJDIR)/jbe_decode.o $(common_objects)
	$(link)

mtf: $(OBJDIR)/mtf_main.o $(common_objects)
	$(link)

$(OBJDIR)/%.o: %.c $(HEADERS) Makefile
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $<

$(OBJDIR)/%.o: %.asm Makefile
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	find $(OBJDIR) -type f -name '*.o' -delete

fclean: clean
	@rm -vf $(TARGET)

re: fclean all

mrproper: fclean
	find . -name "*~" -exec rm -v {} \;
	find . -name "*#" -exec rm -v {} \;
	find . -name "*.orig" -exec rm -v {} \;

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
