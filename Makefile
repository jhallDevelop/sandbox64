all: sandbox64.z64
.PHONY: all
NAME = sandbox64
BUILD_DIR = build
SOURCE_DIR = src

assets = $(wildcard assets/*.wav)
assets_ttf = $(wildcard assets/*.ttf)
assets_png = $(wildcard assets/*.png)
assets_conv = $(addprefix filesystem/,$(notdir $(assets:%.wav=%.wav64))) $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
              $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite)))



MKSPRITE_FLAGS ?=
MKFONT_FLAGS ?=
WARNING_FLAGS = -Wno-unused-variables -Wno-unsued-function

# UNFLoader files
UNFLLOADER_DIR = libs/UNFLoader/
DEBUGFILES = $(UNFLLOADER_DIR)debug.c $(UNFLLOADER_DIR)usb.c
SRC = $(wildcard $(SOURCE_DIR)/*.c) $(DEBUGFILES:.c=.o)
#Includes
CFLAGS = -Ilibs/AF_Math/include -Ilibs/AF_Lib/include -Ilibs/UNFLoader

include $(N64_INST)/include/n64.mk

OBJS = $(SRC:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)


# Run audioconv on all WAV files under assets/
# We do this file by file, but we could even do it just once for the whole
# directory, because audioconv64 supports directory walking.
filesystem/%.wav64: assets/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	@$(N64_AUDIOCONV) -o filesystem $<

# Make sprites to be used for n64
filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	@$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"



$(BUILD_DIR)/$(NAME).dfs: $(assets_conv)
$(BUILD_DIR)/$(NAME).elf: $(OBJS) 

$(NAME).z64: N64_ROM_TITLE = $(NAME) 
$(NAME).z64: $(BUILD_DIR)/$(NAME).dfs


clean:
	rm -rf $(BUILD_DIR) *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
