SHELL = /bin/bash

LIB_DIR = lib
NAME = $(LIB_DIR)/libasm.a

SRC_DIR = src

SOURCE_FILES =	ft_strlen.s \
				ft_strcpy.s \
				ft_strcmp.s \
				ft_strdup.s \
				ft_write.s \
				ft_read.s \

OBJECTS_DIR = objects

OBJECTS = $(patsubst %.s, $(OBJECTS_DIR)/%.o, $(SOURCE_FILES))

ASM = nasm
ASMFLAGS = -f elf64

AR = ar
ARFLAGS = rcs

FRAMES = ⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏

all: $(NAME)

$(NAME): $(OBJECTS)
	@mkdir -p $(LIB_DIR)
	@$(AR) $(ARFLAGS) $(NAME) $(OBJECTS)
	@printf "  \033[32m✓\033[0m %s\n" "$(NAME)"

$(OBJECTS_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(OBJECTS_DIR)
	@frames="$(FRAMES)"; \
	i=0; \
	while [ $$i -lt 6 ]; do \
		printf "\r  %s %s" "$${frames:$$i:1}" "$<"; \
		sleep 0.02; \
		i=$$((i + 1)); \
	done; \
	if $(ASM) $(ASMFLAGS) $< -o $@ 2>$(OBJECTS_DIR)/.err; then \
		printf "\r  \033[32m✓\033[0m %-30s\n" "$<"; \
	else \
		printf "\r  \033[31m✗\033[0m %-30s\n" "$<"; \
		cat $(OBJECTS_DIR)/.err; \
		rm -f $(OBJECTS_DIR)/.err; \
		exit 1; \
	fi; \
	rm -f $(OBJECTS_DIR)/.err

test: test-errno
	@mkdir -p $(OBJECTS_DIR)
	@gcc main.c $(NAME) -o $(OBJECTS_DIR)/test 2>$(OBJECTS_DIR)/.err; \
	grep -v -e "missing .note.GNU-stack" -e "NOTE: This behaviour" $(OBJECTS_DIR)/.err; \
	rm -f $(OBJECTS_DIR)/.err
	@echo
	@$(OBJECTS_DIR)/test

clean:
	rm -rf $(OBJECTS_DIR)

fclean: clean
	rm -rf $(LIB_DIR)

re: fclean all

.PHONY: all test test-errno clean fclean re
