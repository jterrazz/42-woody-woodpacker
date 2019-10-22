NAME = sandbox
CC = gcc
AS = nasm

ifeq ($(DEBUG),yes)
CFLAGS = -g -O0 -fsanitize=address -std=gnu11 -Wall -Wextra
else
CFLAGS = -Ofast -fno-omit-frame-pointer -march=native -std=gnu11 -Wall -Wextra
endif
IFLAGS = -I./include
ASFLAGS = -f elf64

VPATH = src
OBJDIR	= obj

SRC_C = main
SRC_ASM = payloads
HEADERS = include/woody_woodpacker.h

OBJ_C = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC_C)))))
OBJ_ASM = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC_ASM)))))

.PHONY: all re clean fclean help mrproper exec

all: $(NAME)

$(NAME): $(OBJ_C) $(OBJ_ASM)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $<

$(OBJDIR)/%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	rm -vf $(OBJ_C) $(OBJ_ASM)

fclean:
	rm -vf $(OBJ_C) $(OBJ_ASM)
	rm -vf $(NAME)

re: fclean all

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
