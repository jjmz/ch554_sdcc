TARGET = usbhost

EXTRA_FLAGS = -DMAX_PACKET_SIZE=64 -DDE_PRINTF

XRAM_SIZE = 0x380
XRAM_LOC = 0x080

C_FILES = usbh_init.c \
	  usbh_numlock.c \
	  usbh_searchtypedev.c \
	  usbh_enumallhubport.c usbh_enumhubport.c \
	  usbh_initdevonhub.c \
	  usbh_enumallrootdev.c \
	  usbh_initrootdev.c \
	  usbh_touchstartaoa.c \
	  usbh_analyzebulkendp.c usbh_analyzehidintendp.c \
	  usbh_ctrlgetxprinterreport1.c \
	  usbh_hubclearportfeature.c usbh_hubsetportfeature.c usbh_hubgetportstatus.c \
	  usbh_ctrlgethubdescr.c usbh_ctrlgethiddevicereport.c \
	  usbh_usbsetusbint.c \
	  usbh_ctrlclearendpstall.c \
	  usbh_ctrlsetusbaddr.c usbh_ctrlsetusbconfig.c \
	  usbh_ctrlgetdevdescr.c usbh_ctrlgetconfigdescr.c \
	  usbh_copysetupreqpkg.c \
	  usbh_hostctrltransfer.c \
	  usbh_usbhosttransact.c \
	  usbh_waitusb_interrupt.c \
	  usbh_selecthubport.c usbh_enableroothubport.c usbh_resetroothubport.c \
	  usbh_sethostusbaddr.c \
	  usbh_setusbspeed.c \
	  usbh_analyseroothub.c usbh_disableroothubport.c 

include ../../Makefile.include

$(TARGET).lib: $(RELS)
	sdar r usbhost.lib $(notdir $(RELS))

all:	$(TARGET).lib
