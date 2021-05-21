#IAN MICHEAL Ported SDL+OPENGL USING SDL[DREAMHAL][GLDC][KOS2.0]
#2021

TARGET = SDLnehe20

KOS_CFLAGS+= -I$(KOS_BASE)/addons/include/SDL -I$(KOS_BASE)/addons/include -I. -DDREAMCAST 

KOS_CFLAGS+= -O2


KOS_CPPFLAGS=-fno-operator-names -fno-exceptions $(KOS_CFLAGS)

all: $(TARGET).bin

include $(KOS_BASE)/Makefile.rules


OBJS =	SDLnehe20.o romdisk.o

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin  romdisk.o romdisk.img

$(TARGET).elf: $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET).elf $(KOS_START) \
	$(OBJS) $(OBJEXTRA) -lSDL_gl_1213DH -lGLDC -lm $(KOS_LIBS)

$(TARGET).bin: $(TARGET).elf
	$(KOS_OBJCOPY) -R .stack -O binary $(TARGET).elf $(TARGET).bin

romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk romdisk.o
#	bin2odc romdisk.img romdisk.o

romdisk.img: data
	$(KOS_GENROMFS) -f romdisk.img -d data -v


run: $(TARGET).bin 
	$(KOS_LOADER) $(TARGET).bin

