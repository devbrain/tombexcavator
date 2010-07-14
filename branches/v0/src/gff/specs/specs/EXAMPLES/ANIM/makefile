#
# macros:
#
OBJ=grabanim.o animcontrol.o putanim.o skip.o
OBJ2=displayanim.o displayanim0.o displayanim1.o \
     playanim.o unvscomp.o preloadanim.o
#
IFF=  ilbmwx.o   iffw.o  packer.o
IFF2=  loadpict.o ilbmr.o  iffr.o unpacker.o
#
# dependency entries:
#
GrabANIM: $(OBJ) $(IFF) 
	ln -w -o ram:GrabANIM $(OBJ) $(IFF) -lc
DisplayANIM: $(OBJ2) $(IFF2)
	ln -w -o ram:DisplayANIM $(OBJ2) $(IFF2) -lc
#
unvscomp.o: unvscomp.asm
	as unvscomp.asm -o unvscomp.o
#
