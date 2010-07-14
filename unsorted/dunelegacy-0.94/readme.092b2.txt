Dune Legacy 0.92b2 readme

IMPORTANT:
This software is provided as is, and you are running it at your own risk.  I'm not responsible if any harm results from you aquiring, or running this software.  If you distribute this software, make sure this readme file is included.

The program Dune Legacy is an modernized clone of the excellent Westwood Studios game "Dune 2".  This game requires the original dune.pak file from version 1.00 of dune2.  So aquire verson 1.00 of the dune.pak file and put it under the data directory and run the extractor exe.  Hit y for any questions and stuff it says, it wants to delete some temporary files it made.  It is rediculusly easy to find dune2 on the net anyways, but I won't provide it.  I think you can still even buy it from Westwood studios, so do that.  Super dune2 has version 1.00 of the dune.pak file with it.

The programs contained have been written almost entirely by me, with small segments of code greatfully borrowed from Olaf van der Spek's Mixer (dune2 data extraction), and Stefen Hendriks' ultra dune2(random map generator).  My program would not have been created without the use of the excellent SDL library and its extensions.  Thanks guys :).



INFO:
at any time press escape to quit, it will quit the game.
if u want to change the font just overight the font.ttf file under data with any true type font
To change music, chuck ANY mid, wav, mod or ogg files in data/sounds/music folders and they will play appropriately in game

extractor:
This is located under the data directory
For it to work u must place the dune.pak file in the data directory and the run the exe
The dune.pak must be from v1.00 of dune2 otherwise it won't work
Some files don't seem to extract correctly so I have packed them already extracted under data subdirs

in game:
left mouse to drag and select, right mouse to give order (move, attack etc.)
structures can be given waypoints with right click
use mouse wheel or buttons below/above bar to move build selection bar up/down
use f1 to toggle cheat/debug mode
try tab for toggle toolbar
try m for change music
with any of your selected units try command modes shortcut keys:
	a - attack move
Delete will delete one selected owned object, hold shift to delete all.
d will deploy a mcv.
use '-' and '+' keys to adjust game speed.
other controls and commands and things should be similar to other games.

Multiplayer:
Input an Ip address or alias in the input box in multiplayer menu
it uses the custom map map.dlm and all players must have an EXACT copy of it under main directory
or it will be completely out of sync, and error messages will be reported in game
The networking code is probably not secure yet so don't play against people you can't trust.  Also it is not completely finished yet - things like stand ground will not work in multiplayer.

in the single player menu you can choose your house
and the level you want to play 1-22 - the original maps for each house
although most of ordos maps will not work correctly atm

Editor:
make sure u save your map by pressing the s key, u can run it from single player custom map button
the editor will automatically open "map.dlm" if its there to edit, you can rename any original mission file or new dlm format file to map.dlm to automatically load it for editing, or play via single map button.



TODO:
syncronised network play
have it possible to select map to edit and load etc.
improve carryall behaviour, random circling when not in use like in other dune games.
ai expanision/use of mcvs.
design nicer looking ui instead of boring white buttons.
complete house voiceovers.
sellable buildings.
fog of war.
move/attack command responses.
fix other then 640x480 video modes gui, for now only its just a fancy feature.
ini file loading, for unit/structure properties.
improve networking efficiency.



BUG REPORTS/SUGGESTIONS:

to contact me:
Anthony Cole, TonyD
anthonycole81@hotmail.com.au

to contact Raal:
Raal Goff (_Gumby)
gumby@leemingwireless.net

theres a web page here for it, with my doom bot too
http://53073317.home.icq.com/

I often post to the www.dune2k.com/forum under "Dune Editing"
http://www.dune2k.com/forum/index.php?board=19

