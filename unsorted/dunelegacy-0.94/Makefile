# -*- Mode: Makefile -*-

####################################################################################################
#                                                                                                  #
# Copyright (C) 2003 Bauhaus University Weimar                                                     #
#                                                                                                  #
####################################################################################################

# $Id: Makefile,v 1.14 2005/10/23 22:10:36 dvalin Exp $

RM = rm -f

CXX = c++
CC  = gcc

LIB = lib
OPTFLAGS = -g -pipe
CXXFLAGS = $(OPTFLAGS) $(shell sdl-config --cflags)
CFLAGS = $(OPTFLAGS) $(shell pkg-config zziplib --cflags)
LDFLAGS = $(shell sdl-config --libs) $(shell pkg-config zziplib --libs) -lSDL_mixer -lSDL_net -lSDL_ttf -lSDL_image -lGL -L/usr/X11R6/$(LIB) -lm

LIBS =-lm

%.o:	%.cpp Makefile
	$(CXX) $(CXXFLAGS) -o $@  -c $< 

RWOPS = zlib/SDL_rwops_zzip

SOURCES =\
	AiPlayerClass.cpp\
	BuildItemClass.cpp\
	BulletClass.cpp\
	CutScene.cpp\
	CommViewClass.cpp\
	DataFile.cpp\
	editor.cpp\
	Link.cpp\
	LinkedList.cpp\
	ListIterator.cpp\
	listStuff.cpp\
	MapClass.cpp\
	MapGenerator.cpp\
	MentatClass.cpp\
	Menu.cpp\
	Net.cpp\
	mmath.cpp\
	ObjectClass.cpp\
	PlayerClass.cpp\
	PriorityQ.cpp\
	sand.cpp\
	SoundPlayerClass.cpp\
	TerrainClass.cpp\
	$(NULL)\
	gui/Box.cpp\
	gui/Button.cpp\
	gui/Container.cpp\
	gui/Graphics.cpp\
	gui/HBox.cpp\
	gui/Entry.cpp\
	gui/Label.cpp\
	gui/MessageBox.cpp\
	gui/VBox.cpp\
	gui/Widget.cpp\
	gui/Window.cpp\
	$(NULL)\
	structures/BarracksClass.cpp\
	structures/BuilderClass.cpp\
	structures/ConstructionYardClass.cpp\
	structures/GunTurretClass.cpp\
	structures/HeavyFactoryClass.cpp\
	structures/HighTechFactoryClass.cpp\
	structures/IXClass.cpp\
	structures/LightFactoryClass.cpp\
	structures/PalaceClass.cpp\
	structures/RadarClass.cpp\
	structures/RefineryClass.cpp\
	structures/RepairYardClass.cpp\
	structures/RocketTurretClass.cpp\
	structures/SiloClass.cpp\
	structures/StarPortClass.cpp\
	structures/StructureClass.cpp\
	structures/WindTrapClass.cpp\
	structures/WallClass.cpp\
	structures/WORClass.cpp\
	$(NULL)\
	units/AirUnit.cpp\
	units/Carryall.cpp\
	units/DevastatorClass.cpp\
	units/DeviatorClass.cpp\
	units/FremenClass.cpp\
	units/Frigate.cpp\
	units/GroundUnit.cpp\
	units/HarvesterClass.cpp\
	units/InfantryClass.cpp\
	units/LauncherClass.cpp\
	units/MCVClass.cpp\
	units/QuadClass.cpp\
	units/SiegeTankClass.cpp\
	units/SonicTankClass.cpp\
	units/TankClass.cpp\
	units/TrackedUnit.cpp\
	units/TrikeClass.cpp\
	units/TrooperClass.cpp\
	units/Ornithopter.cpp\
	units/RaiderClass.cpp\
	units/Saboteur.cpp\
	units/SandWorm.cpp\
	units/SardaukarClass.cpp\
	units/UnitClass.cpp\
	$(NULL)


OBJECTS =$(SOURCES:.cpp=.o)

.PHONY:	default
default: dunelegacy

SDL_rwops: $(RWOPS).c	
	$(CC) $(CFLAGS) -c $(RWOPS).c -o $(RWOPS).o

dunelegacy:	Makefile $(OBJECTS) SDL_rwops 
	$(CXX) $(LDFLAGS) -o $@ $(OBJECTS) $(RWOPS).o 

.PHONY:	clean
clean:
	-$(RM) $(OBJECTS) $(RWOPS).o

.PHONY:	realclean
realclean:	clean
	-$(RM) dunelegacy
	-$(RM) $(shell find -type f -name \*~)

# $Id: Makefile,v 1.14 2005/10/23 22:10:36 dvalin Exp $
