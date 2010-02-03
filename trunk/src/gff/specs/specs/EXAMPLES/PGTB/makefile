LC1FLAGS = -cswuf -rr
LC2FLAGS = -v -s
ASMFLAGS =

tb: tb.o tbread.o tbsym.o tbdump.o tbrutil.o tbwutil.o
   @blink lib:c.o tb.o tbread.o tbsym.o tbdump.o tbrutil.o tbwutil.o to tb lib lib:lcr.lib lib:amiga.lib SC SD ND

clean:
   @delete tb.o tbrutil.o tbread.o tbsym.o tbwutil.o tbdump.o

print:
   @print tb.h
   @print tb.c
   @print tbrutil.c
   @print tbread.c
   @print tbsym.c
   @print tbwutil.c
   @print tbdump.c

ts: ts.o tbsym.o tbrutil.o
   @blink lib:c.o ts.o tbsym.o tbrutil.o to tb lib lib:lcr.lib lib:amiga.lib SC SD addsym

boom: boom.o
   @blink catch.o boom.o to boom lib lib:lcr.lib lib:amiga.lib SC SD addsym

# boom2 generates an Address Error GURU, also, utilizes the UDAT section

boom2: boom2.o
   @blink catch.o boom2.o lib lib:lc.lib sc sd addsym

boom3: boom3.o
   @blink catch.o boom3.o to boom3 lib lib:lc.lib lib:amiga.lib SC SD addsym

boomr: boomr.o boomsubrts.o
   @blink catch.o boomr.o boomsubrts.o to boomr lib lib:lc.lib lib:amiga.lib SC SD addsym

.c.o:
 @echo "Compiling $<"
 @lc1 -. $(LC1FLAGS) -oram: $<
 @go -. ram:$*
 @lc2 -. $(LC2FLAGS) -o$@ ram:$@

.a.o:
 @echo "Assembling $<"
 @asm -. $(ASMFLAGS) -iinclude: $*

EOF = 
