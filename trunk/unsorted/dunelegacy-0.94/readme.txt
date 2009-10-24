Dune Legacy Readme

As people who has been following this project might have noticed, it has been
quite dead for a year.

This projet has now been revived again by me, Per Øyvind.
Not having too much c++ skills I've only been able to do some work, but I hope
to improve my skills over time and also attract new developers to contribute.
For now I'm the only devel while I have George aka MrFibble doing graphic work.

For anyone being interested in this project you should come by #dunelegacy on
irc.freenode.net and check out or site at http://sourceforge.net/projects/dunelegacy.

You can reach me at peroyvind@sintrax.net.

IMPORTANT:
This software is provided as is, and you are running it at your own risk.
We are not responsible if any harm results from you aquiring, or running this software.
If you distribute this software, make sure this readme file is included.

The Dune Legacy project was originally started by Anthony Cole, then helped out by Raal
Goff and Martin Reichl before it was abandoned by them. It cointains small segments of
code greatfully borrowed from Olaf van der Spek's Mixer (dune2 data extraction), and
Stefen Hendriks' Ultra Dune2(random map generator).
It's now revived, alive and hopefully soon kickin' again.. ;)

INFO:
At any time press escape to quit, it will quit the game.
If you want to change the font just overight the font.ttf file under data
with any true type font.
To change music, chuck ANY mid, wav, (mp3 only on linux,) mod or ogg files
in data/sounds/music folders and they will play appropriately in game.

In game:
Left mouse to drag and select, right mouse to give order (move, attack etc.)
Structures can be given waypoints with right click
Use mouse wheel or buttons below/above bar to move build selection bar up/down
Use 'f1' to toggle cheat/debug mode
Try 'tab' for toggle toolbar
Try 'm' for change music
With any of your selected units try command modes shortcut keys:
        'a' - attack move
Delete will delete one selected owned object, hold shift to delete all.
Use 'd' to deploy a mcv.
Use '-' and '+' keys to adjust game speed.
Use 'f' to toggle fog of war.
Other controls and commands and things should be similar to other games.

Multiplayer:
Type in an IP address or alias in the input box in multiplayer menu.
It uses the custom map map.dlm and all players must have an EXACT copy of
it under main directory, or it will be completely out of sync, and error
messages will be reported in game.
The networking code is probably not secure yet so don't play against people
you can't trust.  Also it is not completely finished yet - things like stand
ground will not work in multiplayer.

In the singleplayer menu you can choose your house and the level you want to play,
1-22 - the original maps for each house, although most of ordos maps will not work
correctly atm.

Editor:
Make sure you save your map by pressing the 's' key, you can run it from singleplayer
custom map button.
The editor will automatically open "map.dlm" if its there to edit, you can rename any
original mission file or new dlm format file to map.dlm to automatically load it for editing, 
or play via single map button.

TODO:
* clean code
* Reduce size of data even more:
        o Convert cutscenes to other format to make them a lot smaller
        o Maybe use speex as sound format to reduce file size even more
* Rather use the data extractor to extract most of the data
* Savegames aren't very useful in campaign mode and make problems with fog
  (turn it on and off once to get rid of  graphical problems)
* Mentat texts have to be completed
* Missions have to be completed
* Map selection screen is missing
* Debriefing is missing
* Synchronised network play
* Have it possible to select map to edit and load etc.
* Improve carryall behaviour, random circling when not in use like in other dune games.
* AI expanision/use of mcvs.
* Complete house voiceovers.
* Sellable buildings.
* Move/attack command responses.
* Fix other then 640x480 video modes gui, for now only its just a fancy feature.
* Ini file loading, for unit/structure properties.
* Improve networking efficiency.

BUG REPORTS/SUGGESTIONS:

Per Øyvind Karlsen, Dvalin (maintainer)
peroyvind@sintrax.net

George, MrFibble (artis)
mrflibble@list.ru

IRC: #dunelegacy @ irc.freenode.net

Web: http://sourceforge.net/projects/dunelegacy
