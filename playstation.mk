# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

EE_BIN = game/breton.elf
include sources.mk
# SOURCES ONLY ON THE PS2
SOURCE_NAMES := $(SOURCE_NAMES) \
 	ez_iop \
	iomanX_irx fileXio_irx ps2dev9_irx ps2ip_irx ps2kbd_irx sior_irx \
	ps2atad_irx ps2hdd_irx ps2fs_irx usbd_irx usbhdfsd_irx mcman_irx mcserv_irx
EE_OBJS = $(SOURCE_NAMES:%=ps2obj/%.o)
$(info $$SOURCE_NAMES is [${SOURCE_NAMES}])
$(info $$EE_OBJS is [${EE_OBJS}])
# disable warnings because it's gcc 3.3
EE_CXXFLAGS = -I$(PS2SDK)/ports/include -w
EE_LDFLAGS = -L$(PS2SDK)/ports/lib -L$(PS2DEV)/gsKit/lib
EE_LIBS = -lsdlmain -lmc -lfileXio -lgskit -lmf -lc -lpad -lsior -lkernel -lstdc++ -lSDL_ttf -lSDL_image -lSDL_gfx -lfreetype -lsdl

ps2obj/%.o: src/%.cc
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

ps2obj/%_irx.s: $(PS2SDK)/iop/irx/%.irx
	bin2s $< $@ `echo $*_irx | tr A-Z a-z`

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS) ps2obj/*

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
