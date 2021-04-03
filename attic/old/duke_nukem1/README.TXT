This update is just a minor bug fix update to the original v1.0 release
some six months ago, I've been meaning to get around to doing it lately.
I've also made some changes to the documentation, the main difference
being my email address (my old one is no longer valid).  There should be
no major problems now, however be aware of the following few issues:

VESA modes not functioning properly:
    If using the VESA screen resolutions causes the display to be drawn
    incorrectly, or if your system locks up, it might be that your video
    card's VESA BIOS implementation is buggy.  Try using Scitech Display
    Doctor, available from http://www.scitechsoft.com, and see if that
    fixes the problem, or just stick to standard EGA/VGA resolutions
    (320x200, 640x350, 640x480).

    If you know you have a VESA-compliant video card and cannot access
    resolutions above 640x480, it may be that your video card simply
    doesn't offer any 16-colour modes above 640x480 (DN1Mod queries for
    16-colour modes), but offers 256+ colour modes instead. No
    workaround here for now, but if you suspect this to be the case you
    can use Scitech Display Doctor to investigate the problem.

Corrupted/Hacked level files:
    If DN1Mod encounters any invalid cell values in a level, it will
    display the invalid cells as black boxes with red crosses in the
    upper left corner. Most of the time these invalid values won't do
    anything in the game, however if you find any values that have
    some interesting effect in the game, drop me an email and I'll
    add it in.

Compiling the sources:
    If you want to compile the source code, you'll need either Borland
    Pascal or Turbo Pascal 6.0 or later for DOS. It won't compile with
    earlier versions, including the 5.5 version that is now available as
    freeware off Borland's web site, as those versions do not support
    inline assembler, which DN1Mod uses.
