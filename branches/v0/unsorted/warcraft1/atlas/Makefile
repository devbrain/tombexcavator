CXX = g++
CXXFLAGS = -O2 -Wall -fomit-frame-pointer -funroll-loops -I. -I./in -I./out -I./hlp `wx-config --cxxflags`
#CXXFLAGS = -pg -I. -I./in -I./out -I./hlp `wx-config --cxxflags`
LDFLAGS = `wx-config --libs`
#LDFLAGS = -pg `wx-config --libs`
SOURCES = main.cpp in/in_warcraft.cpp out/out_png.cpp hlp/hlp_war.cpp hlp/hlp_mix.cpp in/in_cnctd.cpp in/in_war2.cpp hlp/hlp_mpq.cpp in/in_totala.cpp
OBJECTS=$(SOURCES:.cpp=.o)
NAME = stratlas

all: $(SOURCES) $(NAME)

$(NAME): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)
#	strip $(NAME)

clean:
	rm -f *.o */*.o $(NAME)

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $< -o $@
