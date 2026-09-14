LIB_DIR = lib
NAME = $(LIB_DIR)/libasm.a

SRC_DIR = src

SOURCE_FILES =	ft_strlen.s \
				ft_strcpy.s \
				ft_strcmp.s \

OBJECTS_DIR = objects

OBJECTS = $(patsubst %.s, $(OBJECTS_DIR)/%.o, $(SOURCE_FILES))

ASM = nasm
ASMFLAGS = -f elf64

AR = ar
ARFLAGS = rcs

all: $(NAME)

$(NAME): $(OBJECTS)
	@mkdir -p $(LIB_DIR)
	$(AR) $(ARFLAGS) $(NAME) $(OBJECTS)

$(OBJECTS_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(OBJECTS_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

test: $(NAME)
	@mkdir -p $(OBJECTS_DIR)
	gcc main.c $(NAME) -o $(OBJECTS_DIR)/test
	$(OBJECTS_DIR)/test

clean:
	rm -rf $(OBJECTS_DIR)

fclean: clean
	rm -rf $(LIB_DIR)

re: fclean all

.PHONY: all test clean fclean re
