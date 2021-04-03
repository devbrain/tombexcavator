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
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Order Now! ... Only $29.95".
f 0 y -5 mp -8
c"Order the full retail version now for only $29.95!".
f 1
c"To order, call 1-800-APOGEE1, 24 hours a day!".
f 0
c"We accept Visa, MasterCard, Discover, and American Express."

y 210
c"Available on CDROM only!"

a 62,158,170,0
s 485, 57, 155
s 486, 57, 185
s 484, 257, 155
s 487, 257, 185
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Registered Version".
f 1 y -5 mp -6
l"Features, only in the registered version:".

f 0 i l"{" ml +10
l"3 new missions to complete, totalling 27 NEW unique levels in all!".

ml -10 l"{" ml +10
l"2 new engines with more punch than the others. 3 awesome new weapons, the
heavy Magnum 3000DX missile launcher, the intense Flamer and the smart
'homing' Torpedo Launcher. Also, a more advanced satellite, the electric
Dynamo 500.".

ml -10 l"{" ml +10
l"Tons of new graphics, including morphing enemies and impressive bosses.".

ml -10 l"{" ml +10
l"Another 6 heart-pounding music tracks. You can even check out all the cool
music tracks before playing using the built-in jukebox!".

ml -10 l"{" ml +10
l"Rendered cinematics that show the game's story.".

ml -10
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Order by Mail".
f 1 y -5 mp -5 ml +20 mr -20
c"Order Stargunner by mail, too!".
f 0
l"Just send $29.95 plus $5 to pay for shipping and handling, to:".
i f 1
l"Apogee Software, Ltd.~P.O. Box 496389~Garland, TX 75049".
u f 0
l"Make check payable to Apogee. Money orders, cashiers checks and postal orders
are also accepted. (U.S. funds only.) Please allow 3 to 4 weeks to receive your
order.".

c"NOTE: Phone orders are much faster!".

;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Apogee is online!".
f 1 y -5 mp -4
l"You can visit Apogee online!".
i mp -5 mr -20
f 0 l"WWW Page:"		f 1 r"http://www.apogee1.com/".
f 0 l"America Online:"          f 1 r"Keyword: APOGEE".
f 0 l"CompuServe:"		f 1 r"GO APOGEE".
f 0 l"USENET:"			f 1 r"alt.games.apogee".
mr +20 y +5 f 0
i
l"Software Creations BBS public lines:".
i f 1
l"508-368-7036 (up to 14.4k bps)".
l"508-368-6604 (14.4k to 28.8k bps)".
u f 0
l"Member-only lines available, too.".
;컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
p
f 2 l"Three new missions!".
f 0 y -5
s 44,236,171
s 36,-78,-22
s 28,-78,+22
mc 236 y 171 y +35
c"Aquatic Combat"
mc -78 y -22
c"Terran Assault"
mc -78 y +22
c"Stellar Attack"
