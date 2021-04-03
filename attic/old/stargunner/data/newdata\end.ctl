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
ml 2 mr 300 y 60
l"Order your copy of Stargunner today for only $29.95!"
ml 135 mr 289 y 166 mp 10
ml -8 l"{" ml +8
l"The registered version is much more fun. Don't wait any longer, order your
copy today, at $29.95 it's a steal!".
ml -8 l"{" ml +8
l"Shown here is the smart 'homing' Torpedo Launcher in action which really
helps blast your way through."

