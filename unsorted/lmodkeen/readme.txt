MODKEEN 2.0.1 - LINUX PORT (LMODKEEN I) NOTES
---------------------------------------------
Linux support added by Shadow Master (shadowm2006@gmail.com).

This is a package with some stuff and modifications by Ignacio R. Morelle, (known as the "Shadow Master" in the Keen community) to make ModKeen 2.0.1 (source code by Andy Durdin) compile and work with Linux. It's just Andy Durdin's work with some additional code or stuff, no new features added (except a command-line parameter for your scripts), it is provided AS-IS (as the original software and source code), and probably it's more dangerous than the original 32-bit DOS exe and source code.

So... USE IT AT YOUR OWN RISK (consider this warning even if you were using the original ModKeen 2.0.1 for Protected mode DOS) and make lots of backups if you don't want to risk your beatiful files. ;)

INSTALLATION
------------

Compile it. :) Make sure you have the GNU C Compiler (GCC) installed, and the latest version of the ncurses library, along with its development files (-devel pacakages).

Some Linux distros install GCC by default, others let you install it optionally after or during the system installation. You should install it (using your distro's packaging system) as a package named "gcc-(version number)".

Just run the "make" script from a terminal and you're ready. Don't despair if you see lots of warnings, that should be normal. If you see error messages (start with "error:"), then try to solve it yourself or ask me for help (no, not Andy Durdin, he didn't make this port). In any of these situations, e-mail me (see above) with the bugfix/complaint/etc.

RUNNING
-------

After successfully make-ing the source code, you should get a file named "modkeen" (no extensions, we're in Linux now). It's a binary executable file, but perhaps it won't be immediately recognized as such. If for some reason you have the file and it doesn't run when you try to do it, run the following command:

chmod +x /pathtobinary/modkeen

Easy. Now, to run it, just type modkeen from a terminal and press Enter. It's a text-mode program, as the original for DOS. And it even has the same command line parameters (see the original README below), except for the inclusion of the -nowait parameter, used by scripts to stop LModKeen from asking you for a keypress before returning to the shell.

I don't know if there are complete Graphical User interface programs for ModKeen yet, so please don't ask me for those.


Now, the original README for this program.

-----------------------------------------------------------------------
                          ORIGINAL README
-----------------------------------------------------------------------
MODKEEN 2.0.1
Greetings and thanks to Anders Gavare and Daniel Olson for their assistance.

Copyright (c)2002-2004 Andrew Durdin. (andy@durdin.net)

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.
Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject
to the following restrictions:
   1. The origin of this software must not be misrepresented; you must not
      claim that you wrote the original software. If you use this software in
      a product, an acknowledgment in the product documentation would be
      appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.



Revision History:

14 March 2004: Version 2.0.0
	Initial public release
1 April 2004: Version 2.0.1
	Fixed bug in Keen 1-3 importing where different data types were not
	paragraph-aligned.


INTRODUCTION
============

ModKeen allows the creation of Commander Keen mods. It is a command-line
utility that will export graphics from Commander Keen into BMP files, and
import those BMPs back into Commander Keen's graphics files.
ModKeen allows you to:

  * Change all the tiles in Commander Keen
  * Change the sprites in Commander Keen
  * Change other images used in the menus and game
  * Change the fonts used in the game
  
In fact, ModKeen allows you to totally recreate the appearance of Commander
Keen.


WHAT'S NEW
==========

ModKeen version 2.0 now supports modification of Commander Keen episodes
4, 5, and 6. In order to modify these episodes, you will need either UNP
or UNLZEXE to decompress your Commander Keen EXE files, and also Admiral
Bob's CKxPatch utilities, version 0.9.0 or later. You can get these here:

    http://durdin.net/andy/keen/modding/files/unp.zip
    http://durdin.net/andy/keen/modding/files/unlzexe.zip
    http://durdin.net/andy/keen/modding/files/ckpatch090.zip

ModKeen 2.0 has a totally reworked command-line interface and slightly
different operation to version 1.0, so users of the previous versions
should briefly review the "Usage" section below.

   
USAGE
=====

Before using ModKeen, set up a copy of the episode you wish to mod in its own
directory. I recommend creating a subdirectory in there to hold the BMP
files, but it's not necessary. If you're modding episode 4, 5, or 6, you'll
need to decompress the EXE file with UNP or UNLZEXE, and you'll need CKxPatch
to run the game after you've imported the graphics, so copy that into your
mod directory also.

Open a command prompt in the directory where your mod will be. To export the
graphics into BMP files, run ModKeen with the -export command (see below for
details of the switches). After editing the bitmaps, you can use the -import
command to load them back into Commander Keen.

ModKeen accepts the following switches:

  -episode=N
    Specifies the episode that you wish to work with. Give N as 1, 2, 3, 4, 5,
    or 6. This switch is required.

  -export
    Specifies that you wish to export all the Keen data into BMP files. Either
    this switch or -import must be specified.

  -import
    Specifies that you wish to import the BMP files into Keen. Either this switch
    or -export must be specified.

  -keendir="DIRECTORY"
    Specifies the directory where the Commander Keen files to export from or import
    to can be found. If the directory does not exist, then ModKeen will abort with
    an error message. If this switch is not specified, then ModKeen will look in
    the current directory.

  -bmpdir="DIRECTORY"
    Specifies the location where the BMP files to export to or import from should
    be created. If the directory does not exist, then ModKeen will abort with an error
    message. If this switch is not specified, then ModKeen will create the bitmaps
    in the current directory.

  -backup
    Specifies that ModKeen should backup all the files it changes. ModKeen will create
    backups by appending ".bak" and a number onto the filename. ModKeen will never
    delete or overwrite a previous backup, but will create a new backup instead.

  -help
    ModKeen will provide a brief summary of the switches that it supports.


OUTPUTS
=======

When exporting, ModKeen outputs the graphics into 16-colour BMP files in the directory
specified with the -bmpdir switch. Below is a listing of the files it produces. In the
filenames, "x" is the episode number, and "nnnn" a four-digit number; the first BMP file
for each type of data is numbered 0000.

  xBMPnnnn.BMP
    The variable-sized bitmap images used in the menus, ending sequences, and menus.
    These BMP files must always be a multiple of 8 pixels wide.

  xMBMnnnn.BMP
    Episodes 4, 5, and 6 only: Masked variable-width bitmaps used in-game. These BMP files
    must always be a multiple of 16 pixels wide. The right-hand half of the bitmap is the
    transparency mask; it is white where the bitmap should be transparent, and black where
    it should be opaque.

  xSPRnnnn.BMP
    The variable-sized sprite images used in the game. These BMP files must always be a
    multiple of 24 pixels wide, and consist of three equal sections. The left hand section
    is the sprite image; the middle is the transparency mask; and the right-hand edge
    shows the clipping rectangle for the sprite. In the game, the clipping rectangle marks
    the part of the sprite which collides with other objects. For episodes 1, 2, and 3, you
    can set the clipping rectangle by changing the size and/or location of the bright red
    rectangle. For episodes 4, 5, and 6, see the xSPRITES.TXT entry below.

  xTILnnnn.BMP
    These are the tiles used in the game. For episodes 1, 2, and 3, only xTIL0000.BMP will
    be present, containing all the 16x16 tiles, organised into 13 columns. For episodes 4,
    5, and 6, xTIL0000.BMP will contain all the 16x16 background-layer tiles, organised into
    18 columns; xTIL00001.BMP contains all the 16x16 foreground-layer masked tiles, organised
    into 18 columns, with the right-half of the bitmap containing the transparency masks;
    xTIL0002.BMP contains all the 8x8 tiles used in the game, arranged in a single column;
    and xTIL0003.BMP contains all the 8x8 masked tiles used in the game, also in a single
    column.

  xFONnnnn.BMP
    These are the fonts used in the game. The fonts are divided into 16x16 equal-sized cells,
    each containing a single character. For episodes 1, 2, and 3, only one font file is
    present; it is a multicoloured font with an opaque background. For episodes 4, 5, and 6,
    all the fonts are black and white only, although they can have variable-width characters:
    each character is drawn in white on a black background the width of the character, with
    dark grey filling the rest of the cell.

  xSPRITES.TXT
    Episodes 4, 5, and 6 only: This file contains extra information about each sprite. Each
    line in the file has the sprite number, followed by the four clipping rectangle co-
    ordinates in square brackets [top, left, bottom, right], followed by the sprite origin in
    square brackets [top, left], followed by the number of shifts the sprite uses.
    The origin of the sprite image is the point from which its location is calculated. For
    example, the hand sprite in Keen 5 (5SPR0291.BMP) has several images. The origin for each
    of these images is in the centre of the "eye", so that as the hand rotates, the different
    sprite images all appear to rotate about the eye. The origin coordinates are given in
    pixels from the top-left corner of the sprite image.
    The shifts is the number of different copies of the sprite image that are stored in memory,
    and can be 1, 2, or 4. As a general rule, the more shifts a sprite has, the smoother it
    moves, but the more memory it takes up. If you are making a very large sprite, you can
    reduce the number of shifts to save memory. But if you have a small sprite and want it to
    move more smoothly, increase the number of shifts.

  xTXTnnnn.TXT
    Episodes 4, 5, and 6 only: These text files contain the help, story, and end-game text.
    Each file consists of plain text, with embedded commands for displaying pictures and
    creating new pages. The commands you can use are:
      ^P           Marks the beginning of a page.
      ^Gy,x,n      Displays bitmap number (n - 6) at pixel location x,y on the screen.
      ^Cc          Changes the text to colour c, which is a single hex digit, 0-9 or A-F.
      ^Ly,x        Following text will be written beginning at pixel location x,y.
      ^Ty,x,n,t    After a delay of t ??UNITS??, displays bitmap number (n - 6) at pixel
                   location x, y on the screen.
      ^By,x,w,h,c  Fills the w-by-h-pixel rectangle at pixel location x,y with colour 4 (the
                   c parameter is ignored in version 1.4).
      ^E           Marks the end of the text.

  xMSCnnnn.BIN
    Episodes 4, 5, and 6 only: These are binary files needed for correct operation of Keen.
    Two of them are the initial "Initializing..." and the final "Thanks for Playing" color
    text screens, which you can edit with an ANSI editor. The other .BIN files should not
    be modified.

  DEMOn.CKx
    Episodes 4, 5, and 6 only: These are the demos that are shown in the game. Using the
    F10+D cheat, you can record your own demos, and then import them back into the game,
    so that you have demos of your own levels.

  KEENx.PAT
    Episodes 4, 5, and 6 only: After importing, this .PAT file is created in the directory
    specified with the -keendir switch. It contains the command "%egahead EGAHEAD.CKx",
    which causes CKxPatch to load the new EGAHEAD file into memory. After importing, you
    must run Keen using CKxPatch with this patch file, or it will crash.


That's it! Have fun with ModKeen!
============================================================================================