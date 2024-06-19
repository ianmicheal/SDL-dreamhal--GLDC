######################################################################
#               SDLQUAKE DREAMHAL kos2.0 DreamCast  V 1.0            #
######################################################################
#																	 #
#																	 #
#								  Makefile (c)2021 from Ian Micheal  #
######################################################################

# Files --------------------------------------------------------------------------- -std=c99

TARGET = quake.elf
BIN = quake_uns.bin
FIRSTREAD = quake_scr.bin
# Optimize -------------------------------------------------------------------------
//OPT= -DQUIET -DNDEBUG -O3 -g   -fstack-usage -Wstack-usage=512 -trigraphs  -ffast-math -mdiv=inv -maccumulate-outgoing-args -mpretend-cmove -Wl,--gc-sections 
 OPT=  -Os   -flto -fwhole-program -ffast-math -mfused-madd -fno-delayed-branch  -fno-optimize-sibling-calls -funroll-all-loops -fschedule-insns2 -fexpensive-optimizations -fomit-frame-pointer   -finline-functions
# Defines -------------------------------------------------------------------------
KOS_LOCAL_CFLAGS = -DDC  -flto -fwhole-program  -DUSE_ZLIB -DDREAMCAST -DMAX_MEM_LEVEL -DSDL -DLSB_FIRST -DALIGN_LONG -DINLINE -DDC -DUCHAR_MAX=0xff -DUSHRT_MAX=0xffff -DULONG_MAX=0xffffffff
KOS_CFLAGS += $(OPT)
# Files ---------------------------------------------------------------------------
OBJS = memcpy.o memset.o dcmath.o dc_sys.o vid_sdl.o chase.o \
   cl_demo.o cl_input.o cl_main.o cl_parse.o cl_tent.o cmd.o common.o \
   console.o crc.o cvar.o   host.o host_cmd.o keys.o mathlib.o menu.o \
   net_loop.o net_main.o net_none.o net_vcr.o  pr_cmds.o \
   pr_edict.o pr_exec.o  sbar.o  sv_main.o sv_move.o sv_phys.o sv_user.o  \
   view.o wad.o world.o zone.o vmuheader.o


SND_OBJS =  snd_sdl.o snd_dma.o snd_mem.o snd_mix.o cd_sdl.o


SOFT_OBJS = d_edge.o d_fill.o d_init.o d_modech.o d_part.o d_polyse.o d_scan.o \
	d_sky.o d_sprite.o d_surf.o d_vars.o d_zpoint.o draw.o \
	r_aclip.o r_alias.o r_bsp.o r_draw.o r_edge.o \
    r_efrag.o r_light.o r_main.o r_misc.o r_part.o r_sky.o r_sprite.o  \
     r_surf.o r_vars.o nonintel.o  screen.o \
	 model.o 




all: rm-elf $(TARGET)
# Base ----------------------------------------------------------------------------
KOS_LOCAL_CFLAGS += -DDC
KOS_LOCAL_CFLAGS += -I$(KOS_BASE)/include/SDL_image
KOS_LOCAL_CFLAGS += -I$(KOS_BASE)/include/SDL_ttf
include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(BIN) $(FIRSTREAD) $(OBJS) romdisk.*

rm-elf:
	-rm -f $(TARGET) romdisk.*
# Link ----------------------------------------------------------------------------
$(TARGET): $(OBJS) $(SND_OBJS)  $(SOFT_OBJS) romdisk.o
	$(KOS_CC) -flto -fwhole-program $(KOS_CFLAGS) -flto -fwhole-program $(KOS_LDFLAGS) -flto -fwhole-program -o $(TARGET) -flto -fwhole-program $(KOS_START) \
		$(OBJS) $(SND_OBJS) $(SOFT_OBJS) romdisk.o $(OBJEXTRA) -flto -fwhole-program -L$(KOS_BASE)/lib  -lSDLDH  -lm -lz -lkallisti  -lc -ldcplib  $(KOS_LIBS)

# ROM Disk Creation ---------------------------------------------------------------
romdisk.img:
	$(KOS_GENROMFS) -f romdisk.img -d romdisk -v

romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk romdisk.o

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS) romdisk.o romdisk.img
	$(KOS_STRIP) $(TARGET)
	$(KOS_CC_BASE)/bin/sh-elf-objcopy -O binary $(TARGET) $(BIN)
	$(KOS_CC_BASE)/bin/sh-elf-scramble $(BIN) $(FIRSTREAD)

#	for optimize problem
#screen.o : screen.c
#	$(KOS_CC) $(KOS_CFLAGS) -c -O1 $<
