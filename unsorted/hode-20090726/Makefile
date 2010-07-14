
SDL_CFLAGS = `sdl-config --cflags`
SDL_LIBS = `sdl-config --libs`

CXX = g++
CXXFLAGS = -g -O -Wall $(SDL_CFLAGS) $(DEFINES)

OBJDIR = obj

SRCS = andy.cpp fileio.cpp game.cpp level1.cpp level2.cpp level3.cpp level4.cpp \
	lzw.cpp main.cpp mst.cpp paf.cpp random.cpp resource.cpp \
	scaler.cpp staticres.cpp systemstub_sdl.cpp \
	util.cpp video.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

all: $(OBJDIR) hode

hode: $(addprefix $(OBJDIR)/, $(OBJS))
	$(CXX) $(LDFLAGS) -o $@ $^ $(SDL_LIBS) $(VORBIS_LIBS)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d

-include $(addprefix $(OBJDIR)/, $(DEPS))
