TARGET = print

C_FILES = printstr.c printlf.c printhex2.c printhex4.c

include ../../Makefile.include

$(TARGET).lib: $(RELS)
	sdar r print.lib $(notdir $(RELS))

all:	$(TARGET).lib
