; Format:
;
; P			Page Marker (must be on seperate line)
; L"text"               Left justify text
; C"text"               Centre justify text (single line only)
; R"text"               Right justify text (single line only)
; .			Paragraph advance
; Y {+-}n		Set y position of text
; ML {+-}n		Set left margin
; MR {+-}n		Set right margin
; MS {+-}n		Set auto line spacing
; MP {+-}n		Set auto paragraph spacing
; MA			Set Anim y = Current y
; I                     Indent left margin
; U			Unindent left margin
; F x			Change font: x	0 = Save Font
;					1 = Normal Menu Font
;					2 = Selected Menu Font
; A n,{+-}x,{+-}y	Display anim n @ x,y
; S n,{+-}x,{+-}y	Display sprite n @ x,y
;
; Notes:
;
; {+-}			Optional, modifies current value by offset, otherwise
;			value is absolute.
;
; Comments:
;
; These notes/comments etc must be removed for release version.
;
; Special Keys:
; F1 = Help (in game)
; F2 = Save (in game)
; F3 = Load (in game)
; F10 = Fast Exit to DOS
; PrintScreen = Dump current screen to PCX
; Pause = Pause (in game)
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Icons 1/4".
ml +55 f 0 y -5
ma a 59,-10,+0,0 a 60,+25,+0,0
l"PowerUp icons increase your ship's fire power. Pulse (red) fires a constant
stream of bullets. Plasma (blue) fires a wide spread of bullets, which are not
quite as powerful as Pulse fire.".

ma a 58,-15,+0,7
l"Shield icons will boost your ship's shields by 50%. These icons will normally
flip between Shield and Credit icons, so if you see a Credit icon and your shields
need recharging, shoot it once and pick it up.".

ma a 53,+0,+0,14
l"You'd be lucky to find one of these icons, but be on the watch for them! You also
get a bonus life at 500,000 and 1,000,000 points and every million after that.".

;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Icons 2/4".
ml +55 f 0 y -5
ma a 61,-2,+1,0
a 61,+9,+6,32
a 61,-11,+6,64
l"Collect crystals to buy more powerful weapons. Each crystal is worth 15
credits, although surrounding structures contain ones worth 25 credits.
Collect them with your ship for an extra 25-500 points, depending on how
quickly they are picked up after appearing.".

ma a 51,+4,-4,0
l"Collect Credit icons for an instant bonus of 200 credits.".
y +8
ma a 54,+0,+0,7
l"Mega Credit icons, which are rare and hidden in most cases, give you an
instant bonus of 500 credits.".

;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Icons 3/4".
ml +55 f 0 y -5
ma a 57,+0,+0,0
l"There are two types of Nuke icons, some will explode when you touch them and
some are collectable which you can use later when it's really needed. Your
ship only holds 4 Nukes so if you pick up anymore they will automatically
explode.".

ma a 52,+0,+0,7
l"Invincible icons give you ability to fly over structures and into aliens
without causing damage to your ship, although this will only last for a short
period.".

ma a 55,+0,+0,14
l"Mega Points icons are worth 50,000 points, however if you shoot them they
usually change into something more useful.".

;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Icons 4/4".
ml +55 f 0 y -5
ma a 56,+0,+0,0
l"Mystery icons can be a blessing or a curse. They can be any other icon, just
pick one up for pot luck!".

ma a 50,+0,+5,7
l"There are also hidden icons around the structures, but you have to be on the
watch to find them! Collecting hidden icons will give you a bonus score at the
end of the stage.".

;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Credits".
f 0 mp 11 ml +25 mr -25 y -5
f 1 l"Designed and Produced by:".
i f 0
l"David Pevreal"        r"Project Leader &~Hardware Programmer".
l"Craig Allsop"         r"Lead Programmer".
l"James Podesta"        r"3D Modeler Artist &~Level Designs".
u
y -2
f 1 l"Supported by:".
i f 0
l"James Storey"         r"Background Artist".
l"Lee Jackson"          r"Music & Cinematic Sound".
l"Leo Plaw"             r"Artwork Design &~Additional Artwork".
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Credits".
f 0 mp 11 ml +25 mr -25 y -5
f 1 l"Supported by:".
i f 0
l"James Podesta & Jane Beale"         r"Voice Effects".
l"Scott Miller & George Broussard".
r"We thank you both for your advice and~guidance in the development of Stargunner.".
u f 1 l"Published by:".
i f 0
l"Apogee Software"
r"Copr. 1996 Apogee Software, Ltd.".
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Beta Testers".
f 1 mp 10 ml +24 mr -24 y -5
c"Our thanks go out to the following beta testers:"..
f 0 mp 14
l"Bill Blair"             c"Greg Hively"            r"Mike Duncan".
l"Doug Howell"            c"Jeff Rausch"            r"Nathan Anderson".
l"Eric Baker"             c"Kelly Rogers"           r"Rob Brown".
l"Erik Harris"            c"Ken Heckbert"           r"Wayne Benner".
l"Glenn Brensinger"       c"Ken Mayer"              r"Tom Boyles".
			  c"Jason Pratt".

;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Greetings".
mp 14 ml +24 mr -24 f 0 y -5
l"Joel Finch"           c"Alex Barkway"         r"Steven Atkinson".
l"Robert Crane"         c"Shad Ali"             r"Tony Ball".
l"Darren Fletcher"      c"Phillip Gage"         r"Darren Baker".

f 1 y 162
c"Thanks to Michael Richardson, Randy Jansen and Scott Hamilton~
  from Computer Hand Holding, Brisbane, for the excellent~
  hardware you provided us for development."
y +22 f 0
c"Made in Brisbane, Australia, 1996."
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
;p
;f 2 l"System".
;mp 14 ml +24 mr -24 f 0 y -5
;f 1
;l"Profile:".
;f 0 i
;l"Stargunner Version:"          r"1".
;l"Video Mode:"                  r"2".
;l"Video Refresh:"               r"3".
