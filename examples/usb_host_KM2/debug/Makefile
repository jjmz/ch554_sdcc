TARGET = debug

FREQ_SYS ?= 24000000

C_FILES = putchar.c getchar.c \
          mdelay.c udelay.c \
          cfgfsys.c

include ../../Makefile.include

$(TARGET).lib: $(RELS)
	sdar r debug.lib $(notdir $(RELS))

all:	$(TARGET).lib
