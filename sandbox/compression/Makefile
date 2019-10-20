TARGET = jbe_encode jbe_decode mtf_encode mtf_decode rle_encode rle_decode get_binary_weight

CC = gcc
AS = nasm

ifeq ($(DEBUG),yes)
CFLAGS = -Wl,--gc-sections -m64 -g -O0 -fsanitize=address -std=gnu11 -Wall -Wextra
else
CFLAGS = -Wl,--gc-sections -m64 -Ofast -fno-omit-frame-pointer -march=native -std=gnu11 -Wall -Wextra
endif
IFLAGS = -I./include
ASFLAGS = -f elf64

VPATH = src
OBJDIR	= obj

C_FILES = common file
SRC_ASM =
VPATH += include

C_FILES := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(C_FILES)))))
OBJ_ASM = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC_ASM)))))

DEPENDS = $(OBJ_C:%.o=%.d) $(OBJDIR)/jbe.d $(OBJDIR)/mtf.d

.PHONY: all re clean fclean help mrproper

all: $(TARGET)
link = $(CC) $(CFLAGS) $^ -o $@
common_objects = $(C_FILES) $(OBJ_ASM)

jbe_encode: $(OBJDIR)/jbe_encode.o $(OBJDIR)/jbe.o $(common_objects)
	$(link)

jbe_decode: $(OBJDIR)/jbe_decode.o $(OBJDIR)/jbe.o $(common_objects)
	$(link)

mtf_encode: $(OBJDIR)/mtf_encode.o $(OBJDIR)/mtf.o $(common_objects)
	$(link)

mtf_decode: $(OBJDIR)/mtf_decode.o $(OBJDIR)/mtf.o $(common_objects)
	$(link)

rle_encode: $(OBJDIR)/rle_encode.o $(OBJDIR)/rle.o $(common_objects)
	$(link)

rle_decode: $(OBJDIR)/rle_decode.o $(OBJDIR)/rle.o $(common_objects)
	$(link)

get_binary_weight: $(OBJDIR)/get_binary_weight.o
	$(link)

-include $(DEPENDS)
$(OBJDIR)/%.o: %.c $(HEADERS) Makefile
	$(CC) -MD -c $(CFLAGS) $(IFLAGS) -o $@ $<

$(OBJDIR)/%.o: %.asm Makefile
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	find $(OBJDIR) -type f -name '*.o' -delete
	find $(OBJDIR) -type f -name '*.d' -delete

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
