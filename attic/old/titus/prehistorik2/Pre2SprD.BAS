DECLARE SUB checkFile (AFilename AS STRING)
DECLARE SUB checkFiles ()
DECLARE SUB loadPalettes (AArray() AS STRING)
DECLARE SUB loadData (AArray() AS ANY)
DECLARE SUB writePalette (APalette AS STRING, APaletteNumber AS INTEGER)
DECLARE SUB drawPalette ()
DECLARE SUB drawMenu ()
DECLARE SUB drawCredits ()
DECLARE SUB drawSprite (ASpriteNumber AS INTEGER, AHeight AS INTEGER, AWidth AS INTEGER, AOffset AS LONG)
DECLARE SUB dumpSprite (AFilename AS STRING, ABMPHeader AS ANY, APalette AS STRING, AHeight AS INTEGER, AWidth AS INTEGER)
DECLARE SUB dumpAllSprites (AArray() AS ANY, APalettes() AS STRING, ABMPHeader AS ANY)
DECLARE FUNCTION createFilename$ (APaletteNumber AS INTEGER, ASpriteNumber AS INTEGER)

PRINT "Prehistorik 2 Sprite dumper by Frenkel, 20 Feb 2007."

checkFiles

CONST N = 460 'Number of sprites

TYPE BMPHeaderType
  id AS STRING * 2             'BM
  size AS LONG                 '&H76 + wid * hei \ 2
  reserved AS LONG             '0
  offset AS LONG               '&H76 = 118
  sizeofpartofheader AS LONG   '&H28 = 40
  wid AS LONG                  'WHO(i%).x
  hei AS LONG                  'WHO(i%).y
  planes AS INTEGER            '1
  bpp AS INTEGER               '4
  compression AS LONG          '0
  sizeimage AS LONG            '0
  xpelspermeter AS LONG        '0
  ypelspermeter AS LONG        '0
  clrused AS LONG              '0
  clrimportant AS LONG         '0
  pal AS STRING * 64           'palette
END TYPE
DIM BMPHeader AS BMPHeaderType
BMPHeader.id = "BM"
BMPHeader.offset = 118
BMPHeader.sizeofpartofheader = 40
BMPHeader.planes = 1
BMPHeader.bpp = 4

TYPE widthHeightOffsetRecord
  W AS INTEGER
  H AS INTEGER
  O AS LONG
END TYPE

DIM palettes(0 TO 9) AS STRING '* 64
loadPalettes palettes()

DIM WHO(0 TO N - 1) AS widthHeightOffsetRecord 'W(idth) H(eight) O(ffset)
loadData WHO()

DIM sn AS INTEGER 'sprite number
DIM pn AS INTEGER 'palette number

sn = 0
pn = 0

SCREEN 7
writePalette palettes(pn), pn
drawPalette
drawMenu
drawCredits
drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O

DIM k AS STRING
DO
  k = INKEY$
  IF k = CHR$(0) + "M" THEN                             'Right
    IF sn < N - 1 THEN
      sn = sn + 1
      drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O
    END IF
  ELSEIF k = CHR$(0) + "K" THEN                         'Left
    IF 0 < sn THEN
      sn = sn - 1
      drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O
    END IF
  ELSEIF k = CHR$(0) + "I" THEN                         'Page Down
    IF 0 = sn THEN
      'SKIP
    ELSEIF (1 <= sn) AND (sn <= 10) THEN
      sn = 0
      drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O
    ELSEIF 10 <= sn THEN
      sn = sn - 10
      drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O
    END IF
  ELSEIF k = CHR$(0) + "Q" THEN                         'Page Up
    IF N - 1 = sn THEN
      'SKIP
    ELSEIF (N - 10 <= sn) AND (sn < N - 1) THEN
      sn = N - 1
      drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O
    ELSEIF sn <= N - 10 THEN
      sn = sn + 10
      drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O
    END IF
  ELSEIF k = CHR$(0) + "G" THEN                         'Home
    IF sn <> 0 THEN
      sn = 0
      drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O
    END IF
  ELSEIF k = CHR$(0) + "O" THEN                         'End
    IF sn <> N - 1 THEN
      sn = N - 1
      drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O
    END IF
  ELSEIF UCASE$(k) = "D" THEN                           'D
    dumpSprite createFilename$(pn, sn), BMPHeader, palettes(pn), WHO(sn).W, WHO(sn).H
  ELSEIF UCASE$(k) = "A" THEN                           'A
    dumpAllSprites WHO(), palettes(), BMPHeader
    writePalette palettes(pn), pn
    drawSprite sn, WHO(sn).W, WHO(sn).H, WHO(sn).O
  ELSEIF ("0" <= k) AND (k <= "9") THEN                 '0-9
    pn = VAL(k)
    writePalette palettes(pn), pn
  ELSEIF k = CHR$(27) THEN                              'Escape
    KILL "Sprites.p2"
    END
  END IF
LOOP

SUB checkFile (AFilename AS STRING)
DIM f AS INTEGER
DIM l AS LONG    'length of file

f = FREEFILE

OPEN AFilename FOR BINARY AS f
l = LOF(f)
CLOSE f

IF l = 0 THEN
  KILL AFilename
  PRINT "Can't find "; AFilename; "."
  DO WHILE INKEY$ = "": LOOP
  END
END IF

END SUB

SUB checkFiles
checkFile "Diet.exe"
checkFile "Sprites.sqz"
checkFile "Pre2.pal"

SHELL "Diet -OSprites.p2 -R Sprites.sqz"

END SUB

FUNCTION createFilename$ (APaletteNumber AS INTEGER, ASpriteNumber AS INTEGER)
DIM filename AS STRING * 10

filename = "S***P*.bmp"
MID$(filename, 6, 1) = LTRIM$(STR$(APaletteNumber))
MID$(filename, 2, 1) = LTRIM$(STR$(ASpriteNumber \ 100))
MID$(filename, 3, 1) = LTRIM$(STR$((ASpriteNumber MOD 100) \ 10))
MID$(filename, 4, 1) = LTRIM$(STR$(ASpriteNumber MOD 10))

createFilename$ = filename
END FUNCTION

SUB drawCredits
CONST XCOOR = 28
CONST YCOOR = 18

COLOR 6
LOCATE YCOOR, XCOOR
PRINT "Prehistorik 2"
LOCATE , XCOOR
PRINT "Sprite dumper"
LOCATE , XCOOR
PRINT " By  Frenkel"
LOCATE , XCOOR
PRINT "www.sfprod.tk"
LOCATE , XCOOR
PRINT " 20 Feb 2007"
END SUB

SUB drawMenu
COLOR 6

LOCATE 1, 14
PRINT "Sprite:"
LOCATE , 14
PRINT " Width:"
LOCATE , 14
PRINT "Height:"

LOCATE 10, 1
PRINT "Keys:"
PRINT "   " + CHR$(26) + ": Next sprite"
PRINT "   " + CHR$(27) + ": Previous sprite"
PRINT "PgUp: 10 sprites back"
PRINT "PgDn: 10 sprites ahead"
PRINT "Home: First sprite"
PRINT " End: Last sprite"
PRINT "   D: Dump sprite"
PRINT "   A: Dump all sprites"
PRINT " 0-9: Change palette"
PRINT " Esc: Exit program"
END SUB

SUB drawPalette
CONST XCOOR = 8 * 31
CONST YCOOR = 8 * 1
CONST W = 8 * 1
CONST H = 8 * 1

COLOR 6
LOCATE 1, XCOOR \ 8 + 1 - 2
PRINT "Palette:"
FOR i% = 0 TO 15
  LOCATE i% + YCOOR \ 8 + 1, XCOOR \ 8 + 1
  PRINT i%
  LINE (XCOOR, YCOOR + i% * H)-STEP(W - 1, H - 1), i%, BF
NEXT i%
END SUB

SUB drawSprite (ASpriteNumber AS INTEGER, AWidth AS INTEGER, AHeight AS INTEGER, AOffset AS LONG)
'Remove previous sprite
LINE (0, 0)-(103, 69), 0, BF
'Remove "Sprite dumped in S???P?.bmp."
LINE (0, 176)-(223, 183), 0, BF

COLOR 6
LOCATE 1, 22
PRINT USING "###"; ASpriteNumber
LOCATE 2, 22
PRINT USING "###"; AWidth;
LOCATE 3, 22
PRINT USING "###"; AHeight

DIM f AS INTEGER 'free file number
f = FREEFILE
DIM spriteData AS STRING
spriteData = SPACE$((AWidth * AHeight) \ 2)
OPEN "Sprites.p2" FOR BINARY AS f
GET f, AOffset + 1, spriteData
CLOSE f

DEF SEG = &HA000
k% = 1
j% = 1
FOR i% = 0 TO 3
  OUT &H3C4, 2
  OUT &H3C5, j%
  FOR y% = 0 TO AHeight - 1
    FOR x% = 0 TO (AWidth \ 8) - 1
      POKE y% * 40 + x%, ASC(MID$(spriteData, k%, 1))
      k% = k% + 1
    NEXT x%
  NEXT y%
  j% = j% * 2
NEXT i%
DEF SEG
OUT &H3C4, 2
OUT &H3C5, 15
END SUB

SUB dumpAllSprites (AArray() AS widthHeightOffsetRecord, APalettes() AS STRING, ABMPHeader AS BMPHeaderType)
FOR s% = 0 TO N - 1
  drawSprite s%, AArray(s%).W, AArray(s%).H, AArray(s%).O
  FOR p% = 0 TO 9
    writePalette APalettes(p%), p%
    dumpSprite createFilename$(p%, s%), ABMPHeader, APalettes(p%), AArray(s%).W, AArray(s%).H
  NEXT p%
NEXT s%
END SUB

SUB dumpSprite (AFilename AS STRING, ABMPHeader AS BMPHeaderType, APalette AS STRING, AWidth AS INTEGER, AHeight AS INTEGER)
DIM f AS INTEGER 'free file number

ABMPHeader.wid = AWidth
ABMPHeader.hei = AHeight
ABMPHeader.size = 118 + (AWidth * AHeight) \ 2
ABMPHeader.pal = APalette

f = FREEFILE
OPEN AFilename$ FOR BINARY AS f
PUT f, , ABMPHeader

DIM buffer AS STRING
buffer = SPACE$(AWidth * AHeight \ 2)
k% = 1
FOR y% = AHeight - 1 TO 0 STEP -1
  FOR x% = 0 TO AWidth - 1 STEP 2
    MID$(buffer, k%, 1) = CHR$((POINT(x%, y%) * 16) + POINT(x% + 1, y%))
    k% = k% + 1
  NEXT x%
NEXT y%
PUT f, , buffer
CLOSE f

COLOR 6
LOCATE 23, 1
PRINT "Sprite dumped in " + AFilename + "."
END SUB

SUB loadData (AArray() AS widthHeightOffsetRecord)
AArray(0).W = 40:    AArray(0).H = 36:   AArray(0).O = 0
AArray(1).W = 32:    AArray(1).H = 35:   AArray(1).O = 720
AArray(2).W = 24:    AArray(2).H = 36:   AArray(2).O = 1280
AArray(3).W = 32:    AArray(3).H = 34:   AArray(3).O = 1712
AArray(4).W = 32:    AArray(4).H = 37:   AArray(4).O = 2256
AArray(5).W = 32:    AArray(5).H = 37:   AArray(5).O = 2848
AArray(6).W = 32:    AArray(6).H = 36:   AArray(6).O = 3440
AArray(7).W = 32:    AArray(7).H = 35:   AArray(7).O = 4016
AArray(8).W = 32:    AArray(8).H = 35:   AArray(8).O = 4576
AArray(9).W = 32:    AArray(9).H = 35:   AArray(9).O = 5136
AArray(10).W = 24:   AArray(10).H = 35:  AArray(10).O = 5696
AArray(11).W = 32:   AArray(11).H = 38:  AArray(11).O = 6116
AArray(12).W = 40:   AArray(12).H = 31:  AArray(12).O = 6724
AArray(13).W = 48:   AArray(13).H = 31:  AArray(13).O = 7344
AArray(14).W = 32:   AArray(14).H = 31:  AArray(14).O = 8088
AArray(15).W = 32:   AArray(15).H = 36:  AArray(15).O = 8584
AArray(16).W = 24:   AArray(16).H = 32:  AArray(16).O = 9160
AArray(17).W = 40:   AArray(17).H = 30:  AArray(17).O = 9544
AArray(18).W = 40:   AArray(18).H = 31:  AArray(18).O = 10144
AArray(19).W = 40:   AArray(19).H = 31:  AArray(19).O = 10764
AArray(20).W = 32:   AArray(20).H = 30:  AArray(20).O = 11384
AArray(21).W = 24:   AArray(21).H = 30:  AArray(21).O = 11864
AArray(22).W = 32:   AArray(22).H = 31:  AArray(22).O = 12224
AArray(23).W = 24:   AArray(23).H = 30:  AArray(23).O = 12720
AArray(24).W = 24:   AArray(24).H = 32:  AArray(24).O = 13080
AArray(25).W = 40:   AArray(25).H = 34:  AArray(25).O = 13464
AArray(26).W = 32:   AArray(26).H = 33:  AArray(26).O = 14144
AArray(27).W = 32:   AArray(27).H = 35:  AArray(27).O = 14672
AArray(28).W = 32:   AArray(28).H = 34:  AArray(28).O = 15232
AArray(29).W = 40:   AArray(29).H = 31:  AArray(29).O = 15776
AArray(30).W = 40:   AArray(30).H = 32:  AArray(30).O = 16396
AArray(31).W = 40:   AArray(31).H = 30:  AArray(31).O = 17036
AArray(32).W = 40:   AArray(32).H = 31:  AArray(32).O = 17636
AArray(33).W = 48:   AArray(33).H = 32:  AArray(33).O = 18256
AArray(34).W = 32:   AArray(34).H = 36:  AArray(34).O = 19024
AArray(35).W = 24:   AArray(35).H = 32:  AArray(35).O = 19600
AArray(36).W = 32:   AArray(36).H = 32:  AArray(36).O = 19984
AArray(37).W = 32:   AArray(37).H = 33:  AArray(37).O = 20496
AArray(38).W = 24:   AArray(38).H = 29:  AArray(38).O = 21024
AArray(39).W = 24:   AArray(39).H = 17:  AArray(39).O = 21372
AArray(40).W = 16:   AArray(40).H = 16:  AArray(40).O = 21576
AArray(41).W = 24:   AArray(41).H = 17:  AArray(41).O = 21704
AArray(42).W = 32:   AArray(42).H = 14:  AArray(42).O = 21908
AArray(43).W = 24:   AArray(43).H = 15:  AArray(43).O = 22132
AArray(44).W = 16:   AArray(44).H = 15:  AArray(44).O = 22312
AArray(45).W = 24:   AArray(45).H = 14:  AArray(45).O = 22432
AArray(46).W = 32:   AArray(46).H = 11:  AArray(46).O = 22600
AArray(47).W = 24:   AArray(47).H = 16:  AArray(47).O = 22776
AArray(48).W = 32:   AArray(48).H = 25:  AArray(48).O = 22968
AArray(49).W = 32:   AArray(49).H = 31:  AArray(49).O = 23368
AArray(50).W = 24:   AArray(50).H = 33:  AArray(50).O = 23864
AArray(51).W = 32:   AArray(51).H = 31:  AArray(51).O = 24260
AArray(52).W = 32:   AArray(52).H = 31:  AArray(52).O = 24756
AArray(53).W = 8:    AArray(53).H = 6:   AArray(53).O = 25252
AArray(54).W = 8:    AArray(54).H = 8:   AArray(54).O = 25276
AArray(55).W = 16:   AArray(55).H = 9:   AArray(55).O = 25308
AArray(56).W = 16:   AArray(56).H = 11:  AArray(56).O = 25380
AArray(57).W = 16:   AArray(57).H = 11:  AArray(57).O = 25468
AArray(58).W = 16:   AArray(58).H = 19:  AArray(58).O = 25556
AArray(59).W = 24:   AArray(59).H = 18:  AArray(59).O = 25708
AArray(60).W = 24:   AArray(60).H = 13:  AArray(60).O = 25924
AArray(61).W = 16:   AArray(61).H = 16:  AArray(61).O = 26080
AArray(62).W = 16:   AArray(62).H = 16:  AArray(62).O = 26208
AArray(63).W = 16:   AArray(63).H = 16:  AArray(63).O = 26336
AArray(64).W = 16:   AArray(64).H = 15:  AArray(64).O = 26464
AArray(65).W = 24:   AArray(65).H = 15:  AArray(65).O = 26584
AArray(66).W = 24:   AArray(66).H = 19:  AArray(66).O = 26764
AArray(67).W = 24:   AArray(67).H = 24:  AArray(67).O = 26992
AArray(68).W = 32:   AArray(68).H = 19:  AArray(68).O = 27280
AArray(69).W = 24:   AArray(69).H = 25:  AArray(69).O = 27584
AArray(70).W = 16:   AArray(70).H = 15:  AArray(70).O = 27884
AArray(71).W = 16:   AArray(71).H = 15:  AArray(71).O = 28004
AArray(72).W = 16:   AArray(72).H = 9:   AArray(72).O = 28124
AArray(73).W = 16:   AArray(73).H = 15:  AArray(73).O = 28196
AArray(74).W = 24:   AArray(74).H = 9:   AArray(74).O = 28316
AArray(75).W = 24:   AArray(75).H = 9:   AArray(75).O = 28424
AArray(76).W = 24:   AArray(76).H = 9:   AArray(76).O = 28532
AArray(77).W = 24:   AArray(77).H = 9:   AArray(77).O = 28640
AArray(78).W = 24:   AArray(78).H = 9:   AArray(78).O = 28748
AArray(79).W = 24:   AArray(79).H = 9:   AArray(79).O = 28856
AArray(80).W = 24:   AArray(80).H = 9:   AArray(80).O = 28964
AArray(81).W = 24:   AArray(81).H = 9:   AArray(81).O = 29072
AArray(82).W = 24:   AArray(82).H = 9:   AArray(82).O = 29180
AArray(83).W = 24:   AArray(83).H = 9:   AArray(83).O = 29288
AArray(84).W = 24:   AArray(84).H = 9:   AArray(84).O = 29396
AArray(85).W = 24:   AArray(85).H = 9:   AArray(85).O = 29504
AArray(86).W = 40:   AArray(86).H = 13:  AArray(86).O = 29612
AArray(87).W = 48:   AArray(87).H = 13:  AArray(87).O = 29872
AArray(88).W = 48:   AArray(88).H = 13:  AArray(88).O = 30184
AArray(89).W = 48:   AArray(89).H = 13:  AArray(89).O = 30496
AArray(90).W = 56:   AArray(90).H = 17:  AArray(90).O = 30808
AArray(91).W = 24:   AArray(91).H = 19:  AArray(91).O = 31284
AArray(92).W = 16:   AArray(92).H = 13:  AArray(92).O = 31512
AArray(93).W = 16:   AArray(93).H = 13:  AArray(93).O = 31616
AArray(94).W = 16:   AArray(94).H = 13:  AArray(94).O = 31720
AArray(95).W = 16:   AArray(95).H = 13:  AArray(95).O = 31824
AArray(96).W = 16:   AArray(96).H = 13:  AArray(96).O = 31928
AArray(97).W = 24:   AArray(97).H = 19:  AArray(97).O = 32032
AArray(98).W = 32:   AArray(98).H = 22:  AArray(98).O = 32260
AArray(99).W = 24:   AArray(99).H = 30:  AArray(99).O = 32612
AArray(100).W = 24:  AArray(100).H = 29: AArray(100).O = 32972
AArray(101).W = 24:  AArray(101).H = 33: AArray(101).O = 33320
AArray(102).W = 16:  AArray(102).H = 33: AArray(102).O = 33716
AArray(103).W = 24:  AArray(103).H = 33: AArray(103).O = 33980
AArray(104).W = 24:  AArray(104).H = 7:  AArray(104).O = 34376
AArray(105).W = 24:  AArray(105).H = 7:  AArray(105).O = 34460
AArray(106).W = 24:  AArray(106).H = 7:  AArray(106).O = 34544
AArray(107).W = 24:  AArray(107).H = 7:  AArray(107).O = 34628
AArray(108).W = 24:  AArray(108).H = 7:  AArray(108).O = 34712
AArray(109).W = 24:  AArray(109).H = 7:  AArray(109).O = 34796
AArray(110).W = 48:  AArray(110).H = 65: AArray(110).O = 34880
AArray(111).W = 40:  AArray(111).H = 44: AArray(111).O = 36440
AArray(112).W = 48:  AArray(112).H = 32: AArray(112).O = 37320
AArray(113).W = 40:  AArray(113).H = 46: AArray(113).O = 38088
AArray(114).W = 32:  AArray(114).H = 38: AArray(114).O = 39008
AArray(115).W = 40:  AArray(115).H = 35: AArray(115).O = 39616
AArray(116).W = 48:  AArray(116).H = 33: AArray(116).O = 40316
AArray(117).W = 40:  AArray(117).H = 33: AArray(117).O = 41108
AArray(118).W = 48:  AArray(118).H = 14: AArray(118).O = 41768
AArray(119).W = 48:  AArray(119).H = 29: AArray(119).O = 42104
AArray(120).W = 48:  AArray(120).H = 31: AArray(120).O = 42800
AArray(121).W = 48:  AArray(121).H = 19: AArray(121).O = 43544
AArray(122).W = 48:  AArray(122).H = 20: AArray(122).O = 44000
AArray(123).W = 48:  AArray(123).H = 23: AArray(123).O = 44480
AArray(124).W = 48:  AArray(124).H = 27: AArray(124).O = 45032
AArray(125).W = 48:  AArray(125).H = 23: AArray(125).O = 45680
AArray(126).W = 40:  AArray(126).H = 27: AArray(126).O = 46232
AArray(127).W = 40:  AArray(127).H = 34: AArray(127).O = 46772
AArray(128).W = 24:  AArray(128).H = 14: AArray(128).O = 47452
AArray(129).W = 16:  AArray(129).H = 15: AArray(129).O = 47620
AArray(130).W = 24:  AArray(130).H = 19: AArray(130).O = 47740
AArray(131).W = 16:  AArray(131).H = 18: AArray(131).O = 47968
AArray(132).W = 16:  AArray(132).H = 18: AArray(132).O = 48112
AArray(133).W = 24:  AArray(133).H = 12: AArray(133).O = 48256
AArray(134).W = 24:  AArray(134).H = 19: AArray(134).O = 48400
AArray(135).W = 16:  AArray(135).H = 25: AArray(135).O = 48628
AArray(136).W = 24:  AArray(136).H = 21: AArray(136).O = 48828
AArray(137).W = 24:  AArray(137).H = 23: AArray(137).O = 49080
AArray(138).W = 16:  AArray(138).H = 27: AArray(138).O = 49356
AArray(139).W = 16:  AArray(139).H = 20: AArray(139).O = 49572
AArray(140).W = 16:  AArray(140).H = 23: AArray(140).O = 49732
AArray(141).W = 16:  AArray(141).H = 21: AArray(141).O = 49916
AArray(142).W = 24:  AArray(142).H = 11: AArray(142).O = 50084
AArray(143).W = 24:  AArray(143).H = 11: AArray(143).O = 50216
AArray(144).W = 16:  AArray(144).H = 22: AArray(144).O = 50348
AArray(145).W = 16:  AArray(145).H = 22: AArray(145).O = 50524
AArray(146).W = 16:  AArray(146).H = 19: AArray(146).O = 50700
AArray(147).W = 16:  AArray(147).H = 18: AArray(147).O = 50852
AArray(148).W = 16:  AArray(148).H = 18: AArray(148).O = 50996
AArray(149).W = 16:  AArray(149).H = 19: AArray(149).O = 51140
AArray(150).W = 16:  AArray(150).H = 29: AArray(150).O = 51292
AArray(151).W = 16:  AArray(151).H = 21: AArray(151).O = 51524
AArray(152).W = 16:  AArray(152).H = 28: AArray(152).O = 51692
AArray(153).W = 24:  AArray(153).H = 20: AArray(153).O = 51916
AArray(154).W = 24:  AArray(154).H = 15: AArray(154).O = 52156
AArray(155).W = 16:  AArray(155).H = 15: AArray(155).O = 52336
AArray(156).W = 16:  AArray(156).H = 19: AArray(156).O = 52456
AArray(157).W = 16:  AArray(157).H = 21: AArray(157).O = 52608
AArray(158).W = 16:  AArray(158).H = 21: AArray(158).O = 52776
AArray(159).W = 24:  AArray(159).H = 17: AArray(159).O = 52944
AArray(160).W = 24:  AArray(160).H = 12: AArray(160).O = 53148
AArray(161).W = 24:  AArray(161).H = 13: AArray(161).O = 53292
AArray(162).W = 24:  AArray(162).H = 13: AArray(162).O = 53448
AArray(163).W = 24:  AArray(163).H = 17: AArray(163).O = 53604
AArray(164).W = 24:  AArray(164).H = 22: AArray(164).O = 53808
AArray(165).W = 16:  AArray(165).H = 13: AArray(165).O = 54072
AArray(166).W = 16:  AArray(166).H = 16: AArray(166).O = 54176
AArray(167).W = 16:  AArray(167).H = 24: AArray(167).O = 54304
AArray(168).W = 16:  AArray(168).H = 17: AArray(168).O = 54496
AArray(169).W = 24:  AArray(169).H = 11: AArray(169).O = 54632
AArray(170).W = 16:  AArray(170).H = 23: AArray(170).O = 54764
AArray(171).W = 24:  AArray(171).H = 18: AArray(171).O = 54948
AArray(172).W = 24:  AArray(172).H = 15: AArray(172).O = 55164
AArray(173).W = 24:  AArray(173).H = 15: AArray(173).O = 55344
AArray(174).W = 24:  AArray(174).H = 15: AArray(174).O = 55524
AArray(175).W = 16:  AArray(175).H = 19: AArray(175).O = 55704
AArray(176).W = 24:  AArray(176).H = 20: AArray(176).O = 55856
AArray(177).W = 16:  AArray(177).H = 18: AArray(177).O = 56096
AArray(178).W = 8:   AArray(178).H = 23: AArray(178).O = 56240
AArray(179).W = 16:  AArray(179).H = 21: AArray(179).O = 56332
AArray(180).W = 16:  AArray(180).H = 18: AArray(180).O = 56500
AArray(181).W = 16:  AArray(181).H = 12: AArray(181).O = 56644
AArray(182).W = 16:  AArray(182).H = 12: AArray(182).O = 56740
AArray(183).W = 16:  AArray(183).H = 12: AArray(183).O = 56836
AArray(184).W = 24:  AArray(184).H = 23: AArray(184).O = 56932
AArray(185).W = 16:  AArray(185).H = 18: AArray(185).O = 57208
AArray(186).W = 16:  AArray(186).H = 16: AArray(186).O = 57352
AArray(187).W = 16:  AArray(187).H = 14: AArray(187).O = 57480
AArray(188).W = 16:  AArray(188).H = 22: AArray(188).O = 57592
AArray(189).W = 32:  AArray(189).H = 13: AArray(189).O = 57768
AArray(190).W = 24:  AArray(190).H = 16: AArray(190).O = 57976
AArray(191).W = 24:  AArray(191).H = 18: AArray(191).O = 58168
AArray(192).W = 16:  AArray(192).H = 13: AArray(192).O = 58384
AArray(193).W = 24:  AArray(193).H = 14: AArray(193).O = 58488
AArray(194).W = 16:  AArray(194).H = 19: AArray(194).O = 58656
AArray(195).W = 16:  AArray(195).H = 25: AArray(195).O = 58808
AArray(196).W = 8:   AArray(196).H = 22: AArray(196).O = 59008
AArray(197).W = 16:  AArray(197).H = 13: AArray(197).O = 59096
AArray(198).W = 16:  AArray(198).H = 19: AArray(198).O = 59200
AArray(199).W = 24:  AArray(199).H = 16: AArray(199).O = 59352
AArray(200).W = 24:  AArray(200).H = 18: AArray(200).O = 59544
AArray(201).W = 24:  AArray(201).H = 13: AArray(201).O = 59760
AArray(202).W = 16:  AArray(202).H = 15: AArray(202).O = 59916
AArray(203).W = 16:  AArray(203).H = 28: AArray(203).O = 60036
AArray(204).W = 16:  AArray(204).H = 14: AArray(204).O = 60260
AArray(205).W = 16:  AArray(205).H = 24: AArray(205).O = 60372
AArray(206).W = 24:  AArray(206).H = 18: AArray(206).O = 60564
AArray(207).W = 32:  AArray(207).H = 20: AArray(207).O = 60780
AArray(208).W = 32:  AArray(208).H = 17: AArray(208).O = 61100
AArray(209).W = 16:  AArray(209).H = 11: AArray(209).O = 61372
AArray(210).W = 16:  AArray(210).H = 26: AArray(210).O = 61460
AArray(211).W = 24:  AArray(211).H = 18: AArray(211).O = 61668
AArray(212).W = 24:  AArray(212).H = 13: AArray(212).O = 61884
AArray(213).W = 24:  AArray(213).H = 12: AArray(213).O = 62040
AArray(214).W = 16:  AArray(214).H = 15: AArray(214).O = 62184
AArray(215).W = 16:  AArray(215).H = 19: AArray(215).O = 62304
AArray(216).W = 16:  AArray(216).H = 19: AArray(216).O = 62456
AArray(217).W = 16:  AArray(217).H = 19: AArray(217).O = 62608
AArray(218).W = 16:  AArray(218).H = 19: AArray(218).O = 62760
AArray(219).W = 16:  AArray(219).H = 19: AArray(219).O = 62912
AArray(220).W = 24:  AArray(220).H = 16: AArray(220).O = 63064
AArray(221).W = 24:  AArray(221).H = 22: AArray(221).O = 63256
AArray(222).W = 16:  AArray(222).H = 18: AArray(222).O = 63520
AArray(223).W = 24:  AArray(223).H = 16: AArray(223).O = 63664
AArray(224).W = 16:  AArray(224).H = 12: AArray(224).O = 63856
AArray(225).W = 16:  AArray(225).H = 12: AArray(225).O = 63952
AArray(226).W = 16:  AArray(226).H = 11: AArray(226).O = 64048
AArray(227).W = 32:  AArray(227).H = 16: AArray(227).O = 64136
AArray(228).W = 40:  AArray(228).H = 14: AArray(228).O = 64392
AArray(229).W = 40:  AArray(229).H = 14: AArray(229).O = 64672
AArray(230).W = 40:  AArray(230).H = 14: AArray(230).O = 64952
AArray(231).W = 32:  AArray(231).H = 14: AArray(231).O = 65232
AArray(232).W = 16:  AArray(232).H = 26: AArray(232).O = 65456
AArray(233).W = 16:  AArray(233).H = 18: AArray(233).O = 65664
AArray(234).W = 16:  AArray(234).H = 18: AArray(234).O = 65808
AArray(235).W = 24:  AArray(235).H = 18: AArray(235).O = 65952
AArray(236).W = 32:  AArray(236).H = 41: AArray(236).O = 66168
AArray(237).W = 32:  AArray(237).H = 29: AArray(237).O = 66824
AArray(238).W = 24:  AArray(238).H = 20: AArray(238).O = 67288
AArray(239).W = 40:  AArray(239).H = 33: AArray(239).O = 67528
AArray(240).W = 48:  AArray(240).H = 26: AArray(240).O = 68188
AArray(241).W = 16:  AArray(241).H = 11: AArray(241).O = 68812
AArray(242).W = 16:  AArray(242).H = 11: AArray(242).O = 68900
AArray(243).W = 16:  AArray(243).H = 11: AArray(243).O = 68988
AArray(244).W = 16:  AArray(244).H = 11: AArray(244).O = 69076
AArray(245).W = 16:  AArray(245).H = 11: AArray(245).O = 69164
AArray(246).W = 16:  AArray(246).H = 11: AArray(246).O = 69252
AArray(247).W = 16:  AArray(247).H = 11: AArray(247).O = 69340
AArray(248).W = 16:  AArray(248).H = 11: AArray(248).O = 69428
AArray(249).W = 16:  AArray(249).H = 11: AArray(249).O = 69516
AArray(250).W = 16:  AArray(250).H = 11: AArray(250).O = 69604
AArray(251).W = 16:  AArray(251).H = 11: AArray(251).O = 69692
AArray(252).W = 16:  AArray(252).H = 11: AArray(252).O = 69780
AArray(253).W = 16:  AArray(253).H = 11: AArray(253).O = 69868
AArray(254).W = 16:  AArray(254).H = 11: AArray(254).O = 69956
AArray(255).W = 16:  AArray(255).H = 11: AArray(255).O = 70044
AArray(256).W = 16:  AArray(256).H = 11: AArray(256).O = 70132
AArray(257).W = 16:  AArray(257).H = 11: AArray(257).O = 70220
AArray(258).W = 16:  AArray(258).H = 11: AArray(258).O = 70308
AArray(259).W = 16:  AArray(259).H = 11: AArray(259).O = 70396
AArray(260).W = 16:  AArray(260).H = 11: AArray(260).O = 70484
AArray(261).W = 16:  AArray(261).H = 11: AArray(261).O = 70572
AArray(262).W = 16:  AArray(262).H = 11: AArray(262).O = 70660
AArray(263).W = 16:  AArray(263).H = 11: AArray(263).O = 70748
AArray(264).W = 16:  AArray(264).H = 11: AArray(264).O = 70836
AArray(265).W = 16:  AArray(265).H = 11: AArray(265).O = 70924
AArray(266).W = 16:  AArray(266).H = 11: AArray(266).O = 71012
AArray(267).W = 16:  AArray(267).H = 11: AArray(267).O = 71100
AArray(268).W = 8:   AArray(268).H = 8:  AArray(268).O = 71188
AArray(269).W = 16:  AArray(269).H = 11: AArray(269).O = 71220
AArray(270).W = 8:   AArray(270).H = 11: AArray(270).O = 71308
AArray(271).W = 8:   AArray(271).H = 5:  AArray(271).O = 71352
AArray(272).W = 8:   AArray(272).H = 5:  AArray(272).O = 71372
AArray(273).W = 16:  AArray(273).H = 12: AArray(273).O = 71392
AArray(274).W = 48:  AArray(274).H = 40: AArray(274).O = 71488
AArray(275).W = 56:  AArray(275).H = 30: AArray(275).O = 72448
AArray(276).W = 48:  AArray(276).H = 48: AArray(276).O = 73288
AArray(277).W = 32:  AArray(277).H = 51: AArray(277).O = 74440
AArray(278).W = 48:  AArray(278).H = 70: AArray(278).O = 75256
AArray(279).W = 48:  AArray(279).H = 70: AArray(279).O = 76936
AArray(280).W = 40:  AArray(280).H = 27: AArray(280).O = 78616
AArray(281).W = 48:  AArray(281).H = 29: AArray(281).O = 79156
AArray(282).W = 56:  AArray(282).H = 14: AArray(282).O = 79852
AArray(283).W = 16:  AArray(283).H = 12: AArray(283).O = 80244
AArray(284).W = 16:  AArray(284).H = 12: AArray(284).O = 80340
AArray(285).W = 16:  AArray(285).H = 12: AArray(285).O = 80436
AArray(286).W = 16:  AArray(286).H = 12: AArray(286).O = 80532
AArray(287).W = 16:  AArray(287).H = 12: AArray(287).O = 80628
AArray(288).W = 16:  AArray(288).H = 12: AArray(288).O = 80724
AArray(289).W = 16:  AArray(289).H = 12: AArray(289).O = 80820
AArray(290).W = 16:  AArray(290).H = 12: AArray(290).O = 80916
AArray(291).W = 16:  AArray(291).H = 12: AArray(291).O = 81012
AArray(292).W = 16:  AArray(292).H = 12: AArray(292).O = 81108
AArray(293).W = 16:  AArray(293).H = 12: AArray(293).O = 81204
AArray(294).W = 16:  AArray(294).H = 12: AArray(294).O = 81300
AArray(295).W = 16:  AArray(295).H = 12: AArray(295).O = 81396
AArray(296).W = 16:  AArray(296).H = 12: AArray(296).O = 81492
AArray(297).W = 16:  AArray(297).H = 12: AArray(297).O = 81588
AArray(298).W = 16:  AArray(298).H = 12: AArray(298).O = 81684
AArray(299).W = 32:  AArray(299).H = 21: AArray(299).O = 81780
AArray(300).W = 8:   AArray(300).H = 10: AArray(300).O = 82116
AArray(301).W = 16:  AArray(301).H = 12: AArray(301).O = 82156
AArray(302).W = 16:  AArray(302).H = 12: AArray(302).O = 82252
AArray(303).W = 16:  AArray(303).H = 12: AArray(303).O = 82348
AArray(304).W = 64:  AArray(304).H = 31: AArray(304).O = 82444
AArray(305).W = 32:  AArray(305).H = 16: AArray(305).O = 83436
AArray(306).W = 8:   AArray(306).H = 10: AArray(306).O = 83692
AArray(307).W = 48:  AArray(307).H = 21: AArray(307).O = 83732
AArray(308).W = 16:  AArray(308).H = 12: AArray(308).O = 84236
AArray(309).W = 8:   AArray(309).H = 12: AArray(309).O = 84332
AArray(310).W = 16:  AArray(310).H = 12: AArray(310).O = 84380
AArray(311).W = 16:  AArray(311).H = 26: AArray(311).O = 84476
AArray(312).W = 24:  AArray(312).H = 24: AArray(312).O = 84684
AArray(313).W = 32:  AArray(313).H = 29: AArray(313).O = 84972
AArray(314).W = 24:  AArray(314).H = 30: AArray(314).O = 85436
AArray(315).W = 32:  AArray(315).H = 29: AArray(315).O = 85796
AArray(316).W = 48:  AArray(316).H = 40: AArray(316).O = 86260
AArray(317).W = 32:  AArray(317).H = 19: AArray(317).O = 87220
AArray(318).W = 32:  AArray(318).H = 25: AArray(318).O = 87524
AArray(319).W = 24:  AArray(319).H = 28: AArray(319).O = 87924
AArray(320).W = 32:  AArray(320).H = 14: AArray(320).O = 88260
AArray(321).W = 32:  AArray(321).H = 15: AArray(321).O = 88484
AArray(322).W = 32:  AArray(322).H = 14: AArray(322).O = 88724
AArray(323).W = 32:  AArray(323).H = 15: AArray(323).O = 88948
AArray(324).W = 16:  AArray(324).H = 28: AArray(324).O = 89188
AArray(325).W = 16:  AArray(325).H = 28: AArray(325).O = 89412
AArray(326).W = 16:  AArray(326).H = 28: AArray(326).O = 89636
AArray(327).W = 16:  AArray(327).H = 28: AArray(327).O = 89860
AArray(328).W = 24:  AArray(328).H = 21: AArray(328).O = 90084
AArray(329).W = 24:  AArray(329).H = 25: AArray(329).O = 90336
AArray(330).W = 32:  AArray(330).H = 23: AArray(330).O = 90636
AArray(331).W = 32:  AArray(331).H = 23: AArray(331).O = 91004
AArray(332).W = 24:  AArray(332).H = 32: AArray(332).O = 91372
AArray(333).W = 24:  AArray(333).H = 30: AArray(333).O = 91756
AArray(334).W = 32:  AArray(334).H = 22: AArray(334).O = 92116
AArray(335).W = 40:  AArray(335).H = 14: AArray(335).O = 92468
AArray(336).W = 40:  AArray(336).H = 12: AArray(336).O = 92748
AArray(337).W = 24:  AArray(337).H = 15: AArray(337).O = 92988
AArray(338).W = 32:  AArray(338).H = 11: AArray(338).O = 93168
AArray(339).W = 24:  AArray(339).H = 21: AArray(339).O = 93344
AArray(340).W = 16:  AArray(340).H = 10: AArray(340).O = 93596
AArray(341).W = 16:  AArray(341).H = 9:  AArray(341).O = 93676
AArray(342).W = 8:   AArray(342).H = 10: AArray(342).O = 93748
AArray(343).W = 16:  AArray(343).H = 10: AArray(343).O = 93788
AArray(344).W = 16:  AArray(344).H = 10: AArray(344).O = 93868
AArray(345).W = 24:  AArray(345).H = 7:  AArray(345).O = 93948
AArray(346).W = 16:  AArray(346).H = 14: AArray(346).O = 94032
AArray(347).W = 24:  AArray(347).H = 12: AArray(347).O = 94144
AArray(348).W = 24:  AArray(348).H = 11: AArray(348).O = 94288
AArray(349).W = 40:  AArray(349).H = 29: AArray(349).O = 94420
AArray(350).W = 32:  AArray(350).H = 28: AArray(350).O = 95000
AArray(351).W = 32:  AArray(351).H = 28: AArray(351).O = 95448
AArray(352).W = 40:  AArray(352).H = 24: AArray(352).O = 95896
AArray(353).W = 40:  AArray(353).H = 36: AArray(353).O = 96376
AArray(354).W = 40:  AArray(354).H = 31: AArray(354).O = 97096
AArray(355).W = 24:  AArray(355).H = 32: AArray(355).O = 97716
AArray(356).W = 32:  AArray(356).H = 31: AArray(356).O = 98100
AArray(357).W = 32:  AArray(357).H = 31: AArray(357).O = 98596
AArray(358).W = 32:  AArray(358).H = 31: AArray(358).O = 99092
AArray(359).W = 16:  AArray(359).H = 21: AArray(359).O = 99588
AArray(360).W = 16:  AArray(360).H = 21: AArray(360).O = 99756
AArray(361).W = 16:  AArray(361).H = 24: AArray(361).O = 99924
AArray(362).W = 24:  AArray(362).H = 24: AArray(362).O = 100116
AArray(363).W = 40:  AArray(363).H = 20: AArray(363).O = 100404
AArray(364).W = 40:  AArray(364).H = 18: AArray(364).O = 100804
AArray(365).W = 32:  AArray(365).H = 28: AArray(365).O = 101164
AArray(366).W = 8:   AArray(366).H = 5:  AArray(366).O = 101612
AArray(367).W = 24:  AArray(367).H = 7:  AArray(367).O = 101632
AArray(368).W = 32:  AArray(368).H = 12: AArray(368).O = 101716
AArray(369).W = 32:  AArray(369).H = 21: AArray(369).O = 101908
AArray(370).W = 24:  AArray(370).H = 6:  AArray(370).O = 102244
AArray(371).W = 24:  AArray(371).H = 8:  AArray(371).O = 102316
AArray(372).W = 24:  AArray(372).H = 22: AArray(372).O = 102412
AArray(373).W = 32:  AArray(373).H = 28: AArray(373).O = 102676
AArray(374).W = 24:  AArray(374).H = 31: AArray(374).O = 103124
AArray(375).W = 40:  AArray(375).H = 30: AArray(375).O = 103496
AArray(376).W = 32:  AArray(376).H = 26: AArray(376).O = 104096
AArray(377).W = 32:  AArray(377).H = 19: AArray(377).O = 104512
AArray(378).W = 16:  AArray(378).H = 19: AArray(378).O = 104816
AArray(379).W = 24:  AArray(379).H = 18: AArray(379).O = 104968
AArray(380).W = 24:  AArray(380).H = 11: AArray(380).O = 105184
AArray(381).W = 24:  AArray(381).H = 5:  AArray(381).O = 105316
AArray(382).W = 48:  AArray(382).H = 27: AArray(382).O = 105376
AArray(383).W = 48:  AArray(383).H = 29: AArray(383).O = 106024
AArray(384).W = 48:  AArray(384).H = 41: AArray(384).O = 106720
AArray(385).W = 48:  AArray(385).H = 32: AArray(385).O = 107704
AArray(386).W = 32:  AArray(386).H = 3:  AArray(386).O = 108472
AArray(387).W = 32:  AArray(387).H = 5:  AArray(387).O = 108520
AArray(388).W = 40:  AArray(388).H = 11: AArray(388).O = 108600
AArray(389).W = 48:  AArray(389).H = 19: AArray(389).O = 108820
AArray(390).W = 56:  AArray(390).H = 27: AArray(390).O = 109276
AArray(391).W = 64:  AArray(391).H = 32: AArray(391).O = 110032
AArray(392).W = 64:  AArray(392).H = 31: AArray(392).O = 111056
AArray(393).W = 64:  AArray(393).H = 31: AArray(393).O = 112048
AArray(394).W = 64:  AArray(394).H = 32: AArray(394).O = 113040
AArray(395).W = 48:  AArray(395).H = 39: AArray(395).O = 114064
AArray(396).W = 48:  AArray(396).H = 29: AArray(396).O = 115000
AArray(397).W = 48:  AArray(397).H = 30: AArray(397).O = 115696
AArray(398).W = 48:  AArray(398).H = 27: AArray(398).O = 116416
AArray(399).W = 64:  AArray(399).H = 34: AArray(399).O = 117064
AArray(400).W = 56:  AArray(400).H = 36: AArray(400).O = 118152
AArray(401).W = 40:  AArray(401).H = 32: AArray(401).O = 119160
AArray(402).W = 64:  AArray(402).H = 35: AArray(402).O = 119800
AArray(403).W = 64:  AArray(403).H = 43: AArray(403).O = 120920
AArray(404).W = 56:  AArray(404).H = 45: AArray(404).O = 122296
AArray(405).W = 32:  AArray(405).H = 37: AArray(405).O = 123556
AArray(406).W = 32:  AArray(406).H = 37: AArray(406).O = 124148
AArray(407).W = 32:  AArray(407).H = 34: AArray(407).O = 124740
AArray(408).W = 32:  AArray(408).H = 37: AArray(408).O = 125284
AArray(409).W = 48:  AArray(409).H = 46: AArray(409).O = 125876
AArray(410).W = 48:  AArray(410).H = 43: AArray(410).O = 126980
AArray(411).W = 56:  AArray(411).H = 30: AArray(411).O = 128012
AArray(412).W = 40:  AArray(412).H = 28: AArray(412).O = 128852
AArray(413).W = 40:  AArray(413).H = 24: AArray(413).O = 129412
AArray(414).W = 56:  AArray(414).H = 16: AArray(414).O = 129892
AArray(415).W = 32:  AArray(415).H = 49: AArray(415).O = 130340
AArray(416).W = 32:  AArray(416).H = 59: AArray(416).O = 131124
AArray(417).W = 32:  AArray(417).H = 48: AArray(417).O = 132068
AArray(418).W = 56:  AArray(418).H = 34: AArray(418).O = 132836
AArray(419).W = 64:  AArray(419).H = 27: AArray(419).O = 133788
AArray(420).W = 32:  AArray(420).H = 30: AArray(420).O = 134652
AArray(421).W = 32:  AArray(421).H = 28: AArray(421).O = 135132
AArray(422).W = 32:  AArray(422).H = 32: AArray(422).O = 135580
AArray(423).W = 40:  AArray(423).H = 21: AArray(423).O = 136092
AArray(424).W = 32:  AArray(424).H = 32: AArray(424).O = 136512
AArray(425).W = 32:  AArray(425).H = 31: AArray(425).O = 137024
AArray(426).W = 32:  AArray(426).H = 30: AArray(426).O = 137520
AArray(427).W = 48:  AArray(427).H = 28: AArray(427).O = 138000
AArray(428).W = 80:  AArray(428).H = 32: AArray(428).O = 138672
AArray(429).W = 40:  AArray(429).H = 30: AArray(429).O = 139952
AArray(430).W = 40:  AArray(430).H = 35: AArray(430).O = 140552
AArray(431).W = 40:  AArray(431).H = 62: AArray(431).O = 141252
AArray(432).W = 56:  AArray(432).H = 22: AArray(432).O = 142492
AArray(433).W = 56:  AArray(433).H = 24: AArray(433).O = 143108
AArray(434).W = 56:  AArray(434).H = 23: AArray(434).O = 143780
AArray(435).W = 40:  AArray(435).H = 16: AArray(435).O = 144424
AArray(436).W = 40:  AArray(436).H = 18: AArray(436).O = 144744
AArray(437).W = 32:  AArray(437).H = 14: AArray(437).O = 145104
AArray(438).W = 104: AArray(438).H = 41: AArray(438).O = 145328
AArray(439).W = 16:  AArray(439).H = 22: AArray(439).O = 147460
AArray(440).W = 24:  AArray(440).H = 23: AArray(440).O = 147636
AArray(441).W = 24:  AArray(441).H = 23: AArray(441).O = 147912
AArray(442).W = 24:  AArray(442).H = 23: AArray(442).O = 148188
AArray(443).W = 32:  AArray(443).H = 23: AArray(443).O = 148464
AArray(444).W = 24:  AArray(444).H = 24: AArray(444).O = 148832
AArray(445).W = 32:  AArray(445).H = 24: AArray(445).O = 149120
AArray(446).W = 32:  AArray(446).H = 24: AArray(446).O = 149504
AArray(447).W = 32:  AArray(447).H = 23: AArray(447).O = 149888
AArray(448).W = 32:  AArray(448).H = 27: AArray(448).O = 150256
AArray(449).W = 32:  AArray(449).H = 27: AArray(449).O = 150688
AArray(450).W = 32:  AArray(450).H = 24: AArray(450).O = 151120
AArray(451).W = 24:  AArray(451).H = 24: AArray(451).O = 151504
AArray(452).W = 32:  AArray(452).H = 20: AArray(452).O = 151792
AArray(453).W = 48:  AArray(453).H = 28: AArray(453).O = 152112
AArray(454).W = 64:  AArray(454).H = 28: AArray(454).O = 152784
AArray(455).W = 48:  AArray(455).H = 31: AArray(455).O = 153680
AArray(456).W = 48:  AArray(456).H = 38: AArray(456).O = 154424
AArray(457).W = 40:  AArray(457).H = 34: AArray(457).O = 155336
AArray(458).W = 32:  AArray(458).H = 26: AArray(458).O = 156016
AArray(459).W = 48:  AArray(459).H = 23: AArray(459).O = 156432
END SUB

SUB loadPalettes (AArray() AS STRING)
DIM p AS STRING * 64
DIM f AS INTEGER
f = FREEFILE

OPEN "Pre2.pal" FOR BINARY AS f
FOR i% = 0 TO 9
  GET f, , p
  AArray(i%) = p
NEXT i%
CLOSE f

END SUB

SUB writePalette (APalette AS STRING, APaletteNumber AS INTEGER)
COLOR 6
LOCATE 1, 38
PRINT APaletteNumber

FOR i% = 0 TO 15
  IF i% < 8 THEN
    OUT &H3C8, i%
  ELSE
    OUT &H3C8, 8 + i%
  END IF
  OUT &H3C9, ASC(MID$(APalette, i% * 4 + 3, 1)) \ 4
  OUT &H3C9, ASC(MID$(APalette, i% * 4 + 2, 1)) \ 4
  OUT &H3C9, ASC(MID$(APalette, i% * 4 + 1, 1)) \ 4
NEXT i%

END SUB

