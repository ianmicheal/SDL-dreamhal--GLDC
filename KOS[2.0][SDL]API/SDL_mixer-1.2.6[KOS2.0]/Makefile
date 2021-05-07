INCS = $(KOS_INCS) -I../SDL-1.2.9/include -I. -Imikmod -Itimidity
DEFS =  -DPACKAGE=\"SDL_mixer\" -DVERSION=\"1.2.6\" -DUSE_RWOPS -DCMD_MUSIC -DWAV_MUSIC -DMOD_MUSIC -DMID_MUSIC -DUSE_TIMIDITY_MIDI -DLITTLE_ENDIAN -DDC -UENOENT
#-DOGG_MUSIC
#-DMP3_MUSIC
#OPTFLAGS=-O2 -fomit-frame-pointer
KOS_CFLAGS+=$(DEFS) $(INCS) $(OPTFLAGS)


TARGET = libSDL_mixer_126.a

all: $(TARGET)

include $(KOS_BASE)/Makefile.rules

SRCS =	\
	load_aiff.c		\
	load_voc.c		\
	mixer.c			\
	music.c			\
	music_cmd.c		\
	music_ogg.c		\
	wavestream.c		\
	effect_position.c	\
	effect_stereoreverse.c	\
	effects_internal.c	\
	\
	timidity/common.c		\
	timidity/controls.c		\
	timidity/filter.c		\
	timidity/instrum.c		\
	timidity/mix.c			\
	timidity/output.c		\
	timidity/playmidi.c		\
	timidity/readmidi.c		\
	timidity/resample.c		\
	timidity/sdl_a.c		\
	timidity/sdl_c.c		\
	timidity/tables.c		\
	timidity/timidity.c		\
	\
	mikmod/drv_nos.c		\
	mikmod/drv_sdl.c		\
	mikmod/load_it.c		\
	mikmod/load_mod.c		\
	mikmod/load_s3m.c		\
	mikmod/load_xm.c		\
	mikmod/mdreg.c			\
	mikmod/mdriver.c		\
	mikmod/mloader.c		\
	mikmod/mlreg.c			\
	mikmod/mlutil.c		\
	mikmod/mmalloc.c		\
	mikmod/mmerror.c		\
	mikmod/mmio.c			\
	mikmod/mplayer.c		\
	mikmod/munitrk.c		\
	mikmod/mwav.c			\
	mikmod/npertab.c		\
	mikmod/sloader.c		\
	mikmod/virtch.c		\
	mikmod/virtch2.c		\
	mikmod/virtch_common.c


OBJS = $(SRCS:.c=.o)

clean:
	rm -f $(OBJS)

$(TARGET) : $(OBJS)
	rm -f $(KOS_BASE)/addons/lib/dreamcast/$(TARGET)
	$(KOS_AR) rcs $(KOS_BASE)/addons/lib/dreamcast/$(TARGET) $(OBJS)
	$(KOS_RANLIB) $(KOS_BASE)/addons/lib/dreamcast/$(TARGET)

mikmod/libmikmod.a:
	make -C mikmod -f Makefile.dc

timidity/libtimidity.a:
	make -C timidity -f Makefile.dc
