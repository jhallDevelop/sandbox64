all: sandbox64.z64
.PHONY: all
NAME = sandbox64
BUILD_DIR = build
SOURCE_DIR = src
SRC = $(wildcard $(SOURCE_DIR)/*.c)

include $(N64_INST)/include/n64.mk

OBJS = $(SRC:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)

$(NAME).z64: N64_ROM_TITLE = $(NAME) 
$(BUILD_DIR)/$(NAME).elf: $(OBJS) 


clean:
	rm -rf $(BUILD_DIR) *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
