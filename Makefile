TARGET = reverzi
PSPSDK=$(shell psp-config --pspsdk-path)
PSPBIN = $(PSPSDK)/../bin
SDL_CONFIG = $(PSPBIN)/sdl-config

PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

BUILD_PRX=1
PSP_FW_VERSION = 371


OBJS =  reverzi.o player.o position.o SFont.o SDL_additives.o soundmanager.o

INCDIR = 
CFLAGS = -O2  -G0 -Wall -g $(shell $(SDL_CONFIG) --cflags) 
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -fsingle-precision-constant -mno-check-zero-division -ffast-math
#CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS = -lstdc++  -lSDL_mixer  -lvorbisidec   -lSDL_image -lpng -lz -ljpeg \
        -lSDL_gfx -lm $(shell $(SDL_CONFIG) --libs) -lpsprtc  -lpspvfpu  -lpsppower

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = reverzi

include $(PSPSDK)/lib/build.mak
