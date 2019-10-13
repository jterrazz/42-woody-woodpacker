NAME = woody_woodpacker
CC = gcc
AS = nasm

ifeq ($(DEBUG),yes)
CFLAGS = -g -O0 -fsanitize=address -std=gnu11 -Wall -Wextra
else
CFLAGS = -Ofast -fno-omit-frame-pointer -march=native -std=gnu11 -Wall -Wextra
endif
IFLAGS = -I./include -I./libft_ng/includes
ASFLAGS = -f elf64
LDFLAGS = -L./libft_ng -lft

OBJDIR = obj

VPATH += src
SRC_C += main common elf
SRC_ASM +=
HEADERS += include/woody_woodpacker.h
LIBS += libft_ng/libft.a

OBJ_C = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC_C)))))
OBJ_ASM = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC_ASM)))))

.PHONY: all re clean fclean help mrproper exec

all: libft $(NAME)

$(NAME): $(OBJ_C) $(OBJ_ASM)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.c $(HEADERS) $(LIBS) Makefile
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $<

$(OBJDIR)/%.o: %.asm $(LIBS) Makefile
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	make -C libft_ng clean
	rm -vf $(OBJ_C) $(OBJ_ASM)

fclean:
	make -C libft_ng fclean
	rm -vf $(OBJ_C) $(OBJ_ASM)
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
