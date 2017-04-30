# Tiva Makefile
# #####################################
#
# Part of the uCtools project
# uctools.github.com
#
#######################################
# user configuration:
#######################################
# TARGET: name of the output file
TARGET = main
# MCU: part number to build for
MCU = TM4C123GH6PM
# SOURCES: list of input source sources
SOURCES = main.c startup_gcc.c display/ST7735.c display/drawing.c display/menu.c game/game.c game/queue.c game/stack.c input/ADC.c input/portf.c math/physics.c sound/dac.c sound/sound.c sound/sound_lookup.c timer/systick.c timer/Timer0.c timer/Timer1.c TExaS.c display/buffer.c math/random_squares.c
ASM_SOURCES = display/LCD.gcc.s display/print.gcc.s math/random.gcc.s critical.gcc.s
SOURCE_DIRS = display game input math sound timer
# INCLUDES: list of includes, by default, use Includes directory
INCLUDES = -IInclude
# OUTDIR: directory to use for output
OUTDIR = build
# TIVAWARE_PATH: path to tivaware folder
TIVAWARE_PATH = $(HOME)/school/ee319k/tiva/tivaware

# LD_SCRIPT: linker script
LD_SCRIPT = $(MCU).ld

# define flags
CFLAGS = -g -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
CFLAGS +=-Os -ffunction-sections -fdata-sections -MD -std=c99 -Wall -Wextra
CFLAGS += -pedantic -DPART_$(MCU) -c -I$(TIVAWARE_PATH)
CFLAGS += -DTARGET_IS_BLIZZARD_RA1
LDFLAGS = -T $(LD_SCRIPT) --entry ResetISR --gc-sections

#######################################
# end of user configuration
#######################################
#
#######################################
# binaries
#######################################
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
AS = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
RM      = rm -f
MKDIR	= mkdir -p
#######################################

# list of object files, placed in the build directory regardless of source path
ASM_OBJECTS = $(addprefix $(OUTDIR)/,$(notdir $(ASM_SOURCES:.gcc.s=.gcc.o)))
OBJECTS = $(addprefix $(OUTDIR)/,$(notdir $(SOURCES:.c=.o))) 

# default: build bin
all: $(OUTDIR)/$(TARGET).bin

vpath %.gcc.s $(SOURCE_DIRS)
$(OUTDIR)/%.gcc.o: %.gcc.s | $(OUTDIR)
	$(AS) -o $@ $< $(CFLAGS)

vpath %.c $(SOURCE_DIRS) 
$(OUTDIR)/%.o: %.c | $(OUTDIR) $(ASM_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

$(OUTDIR)/a.out: $(ASM_OBJECTS) $(OBJECTS)
	$(LD) -o $@ $^ $(LDFLAGS)

$(OUTDIR)/main.bin: $(OUTDIR)/a.out
	$(OBJCOPY) -O binary $< $@

# create the output directory
$(OUTDIR):
	$(MKDIR) $(OUTDIR)

clean:
	-$(RM) $(OUTDIR)/*

flash:
	lm4flash -v $(OUTDIR)/$(TARGET).bin

#
# The rule to rebuild the project and debug it with Nemiver.
#
debug: clean
debug: CFLAGS+=-g -D DEBUG
debug: all
debug:
	./debug_nemiver.sh

.PHONY: all clean
