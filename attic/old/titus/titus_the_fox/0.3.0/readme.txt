Readme for OpenTitus
Version 0.3.0

OpenTitus is released under the Gnu GPL version 3 or (at your option) any later version of the license, and it is released "as is"; without any warranty.

Author:
Eirik Stople

Thanks to:
Eric Zmiro and his team in Titus Interactive who made the great games Titus the Fox and Moktar!
Jesses, who made a fansite for the original game and an editor and provided me information how the original game files was built up, and assisted me with the makefile and windows compilation!


You need the original game files to make use of these applications. These applications parses the original files. I have only tested them on a Linux-based environment, but Jesse told me that both 0.1 and 0.2 also compiles in MinGW in windows. It works with both Titus and Moktar (the French version of the game).
You need SDL to compile, and maybe also to run the game. To compile, use the following commands:

All:
make

Clean:
make clean


To use the UnSQZ program, run
./unsqz <SQZ file>
This will extract the file to "output.lvl".

To run the level viewer, run
./levelviewer <level number>
This will display the upper left part of the level.
NB: the levelviewer is not included in this version.

To run the game, run
./opentitus
This will launch the game.


To quickly start up Titus, copy a Mitus game directory to the same folder as the opentitus executable, and name the folder "titus".
To quickly start up Moktar, copy a Moktar game directory to the same folder as the opentitus executable, and name the folder "moktar". You can modify titus.conf if you like, or use the preconfigured "titus_moktar.conf" which you need to rename "titus.conf".

You are able to move around in the levels "noclip". You can die with "F1", game over with "F2", view status with "F4", and "fake finish" with "F" to proceed to the next level. The current levelcodes are "0000" for level 1, "1111" for level 2 and so forth.


Please note: The game isn't finished yet! It still needs animation, movement engine and music. This is just a snapshot of the current development status of OpenTitus.

OpenTitus is not affiliated with Titus Interactive, the (now bankerrupted) company who made Titus the Fox and Moktar in 1991.


The levelviewer and the game is configured by a file in the same directory; "titus.conf". This file is parsed line by line, and lines starting with # will be skipped.
Keywords:
reswidth = Number of pixels wide, default 320 (originally 640, but the "pixels" in the original game was two pixels wide, so 320 should give the correct size)
resheight = Number of pixels height, default 200 (originally 400 (intro) or 384 (in-game), 200 should give the correct size)
bitdepth = Number of bits pr. pixel, default 32 (the game data files have a 16 colour palette, which means 4 bpp, but higher bpp will work as well)
ingamewidth = Numbers og pixels visible in-game, default 320.
ingameheight = Numbers og pixels visible in-game, default 192.
videomode = Video mode; 0 is window mode and 1 is fullscreen, default 0
levelcount = Number of levels, default 15 (15 in Titus the Fox and 16 in Moktar)
level 1-15 = Location of the level file for all levels, default LEVELx.SQZ (the order is "0J123456789BCEFG" for Moktar, Titus is equal except it doesn't have level F)
sprites = Location of the sprite file, default SPREXP.SQZ (SPRITES.SQZ in Moktar)
game = The game opentitus will parse; 0 for Titus the Fox and 1 for Moktar. This is necessary because of different sprite sizes, different menus and different level titles.
logo = The company logo file, default TITUS.SQZ
intro = The game intro image file, default TITRE.SQZ
menu = The menu, default MENU.SQZ
finish = The image displayed when game is completed, default LEVELA.SQZ (this doesn't exist in Moktar)
*format = Image format. 0: 16-colour grayscale, 1: 16-colour (ordinary colours), 2: 256 colours + palette. Logo, intro and menu are 256 colour, *EGA files are 16-colour, and LEVELA.SQZ are grayscale
font = Font file, default FONTS.SQZ
