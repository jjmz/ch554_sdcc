TARGET = UsbHostHub_KM

WCHISP = python3 ../blink/ch55xtool.py -r -f
FREQ_SYS=16000000

EXTRA_FLAGS = -DMAX_PACKET_SIZE=64 -I ./print/ -I ./usbhost/

XRAM_SIZE = 0x380
XRAM_LOC = 0x080

C_FILES = \
	UsbHostHub_KM.c

pre-flash:

LIBS = debug/debug.lib \
	print/print.lib \
	 usbhost/usbhost.lib

debug/debug.lib:
	make -C debug FREQ_SYS=$(FREQ_SYS) debug.lib

print/print.lib:
	make -C print print.lib

usbhost/usbhost.lib:
	make -C usbhost usbhost.lib

cleanlib:
	make -C print clean
	make -C debug clean
	make -C usbhost clean
	rm $(LIBS)

include ../Makefile.include

$(TARGET).ihx: $(RELS) $(LIBS)
	$(CC) $(notdir $(RELS)) -l debug/debug.lib -l print/print.lib -l usbhost/usbhost.lib $(LFLAGS) -o $(TARGET).ihx

