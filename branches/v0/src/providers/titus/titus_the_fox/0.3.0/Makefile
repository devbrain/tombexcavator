INCLUDE = -I./include
CFLAGS  = $(INCLUDE)
UNSQZ_TARG := unsqz
# LEVELVIEWER_TARG := levelviewer
OPENTITUS_TARG := opentitus
UNSQZ_OBJS := \
	unsqz.o \
	src/sqz.o
# LEVELVIEWER_OBJS := \
#	levelviewer.o \
#	src/sqz.o \
#	src/settings.o \
#	src/sprites.o
OPENTITUS_OBJS := \
	opentitus.o \
	src/sqz.o \
	src/settings.o \
	src/sprites.o \
	src/viewimage.o \
	src/fonts.o \
	src/menu.o \
	src/engine.o \
	src/original.o \
	src/level.o \
	src/convert.o \
	src/keyboard.o
DIRS = \
	. \
	include \
	src

# all : $(UNSQZ_TARG) $(LEVELVIEWER_TARG) $(OPENTITUS_TARG)
all : $(UNSQZ_TARG) $(OPENTITUS_TARG)

$(UNSQZ_TARG) : $(UNSQZ_OBJS)
	$(CC) $(CFLAGS) -o $(UNSQZ_TARG) $(UNSQZ_OBJS)

# $(LEVELVIEWER_TARG) : $(LEVELVIEWER_OBJS)
# 	$(CC) $(CFLAGS) -o $(LEVELVIEWER_TARG) $(LEVELVIEWER_OBJS) `sdl-config --cflags --libs`

$(OPENTITUS_TARG) : $(OPENTITUS_OBJS)
	$(CC) $(CFLAGS) -o $(OPENTITUS_TARG) $(OPENTITUS_OBJS) `sdl-config --cflags --libs`

clean :
	rm -f $(foreach dir,$(DIRS),$(foreach suffix,/*.o, $(addsuffix $(suffix),$(dir))))
