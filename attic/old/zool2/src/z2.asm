; -------------------------------------------
; Zool 2 AGA - Enhanced version (based on the CD32 one).
; Game.
; Disassembled/modified by Franck Charlet.
; -------------------------------------------

; 1 = unlimited time, energy & always 99% of stage done
; 0 = normal mode
TRAINER             equ     0

; 0 to 5 (and 7 for the extra cd32 level with the toilet paper)
;
; 0: EGGS
; 1: BULBS
; 2: EGYPT
; 3: DICES/SNAKES
; 4: CANDIES/ICE
; 5: CUBES
; 7: PAPER
START_WORLD         equ     7

; 0 to 2 (except world 5 which only have 1 stage)
START_STAGE         equ     0

; exec
_LVOSupervisor      equ     -30
_LVODisable	        equ     -120
_LVOEnable	        equ     -126
_LVOFindTask	    equ     -294
_LVOGetMsg	        equ	    -372
_LVOWaitPort	    equ	    -384
_LVOOpenLibrary	    equ     -552

; gfx
_LVOLoadView	    equ     -222
_LVOWaitBlit	    equ     -228
_LVOOwnBlitter	    equ     -456
_LVODisownBlitter	equ     -462

; dos
_LVOOpen	        equ     -30
_LVOClose	        equ     -36
_LVORead	        equ     -42
_LVOWrite	        equ     -48

; lowlevel
_LVOAddKBInt        equ     -60
_LVOSystemControl   equ     -72
_LVOAddVBlankInt    equ     -108

ZOOL_GRIPPING       equ     3
ZOOL_RUNNING        equ     1
ZOOL_CROUCHING      equ     9
ZOOL_TUMBLING       equ     11

                    SECTION  Program, CODE

Program:            sub.l    a1, a1
                    move.l   4.w, a6
                    jsr      _LVOFindTask(a6)
                    move.l   d0, TASKPTR
                    move.l   d0, a4
                    tst.l    $AC(a4)
                    bne.s    _OPEN_LIBRARIES
                    lea      $5C(a4), a0
                    move.l   a0, a3
                    jsr      _LVOWaitPort(a6)
                    move.l   a3, a0
                    jsr      _LVOGetMsg(a6)
_OPEN_LIBRARIES:    jsr      OPEN_LIBRARIES
                    bsr      TAKEOVERSYSTEM
                    jsr      CDSTARTUP
                    bsr      GETBLITTER
                    jmp      MAIN_BEGIN

TAKEOVERSYSTEM:     movem.l  d1-d4/a0-a6, -(sp)
                    move.l   TASKPTR, a0
                    moveq    #-1, d0
                    move.l   d0, 184(a0)
                    move.l   GFXBASE, a6
                    sub.l    a1, a1
                    jsr      _LVOLoadView(a6)
                    movem.l  (sp)+, d1-d4/a0-a6
                    rts

GETBLITTER:         tst.b    GOTBLIT
                    bne.s    ALREADY_GOTBLIT
                    movem.l  d0/d1/a0/a1/a6, -(sp)
                    move.l   GFXBASE, a6
                    jsr      _LVOOwnBlitter(a6)
                    jsr      _LVOWaitBlit(a6)
                    st       GOTBLIT
                    movem.l  (sp)+, d0/d1/a0/a1/a6
ALREADY_GOTBLIT:    rts

GOTBLIT:            dc.b     0
                    even

FREEBLITTER:        tst.b    GOTBLIT
                    beq.s    ALREADY_FREEBLIT
                    movem.l  d0/d1/a0/a1/a6, -(sp)
                    move.l   GFXBASE, a6
                    jsr      _LVOWaitBlit(a6)
                    jsr      _LVODisownBlitter(a6)
                    sf       GOTBLIT
                    movem.l  (sp)+, d0/d1/a0/a1/a6
ALREADY_FREEBLIT:   rts

; not mandatory file
DOS_LOADFILE_OPT:   move.l   a5, FILE_DESTBUFFER
                    move.l   a0, FILENAME
                    jsr      FREEBLITTER
                    move.l   FILENAME, d1
                    move.l   #$3ED, d2
                    move.l   DOSBASE, a6
                    jsr      _LVOOpen(a6)
                    move.l   d0, FILEHANDLE
                    bne.s    READFILE
                    moveq    #0, d0
                    rts

; mandatory file
DOS_LOADFILE:       move.l   a5, FILE_DESTBUFFER
                    move.l   a0, FILENAME
                    jsr      FREEBLITTER
                    move.l   FILENAME, d1
                    move.l   #$3ED, d2
                    move.l   DOSBASE, a6
                    jsr      _LVOOpen(a6)
                    move.l   d0, FILEHANDLE
                    bne.s    READFILE
                    jmp      FATAL_ERROR

READFILE:           move.l   FILEHANDLE, d1
                    move.l   FILE_DESTBUFFER, d2
                    move.l   #500000, d3
                    move.l   DOSBASE, a6
                    jsr      _LVORead(a6)
                    move.l   d0, -(a7)
                    move.l   FILEHANDLE, d1
                    jsr      _LVOClose(a6)
                    jsr      GETBLITTER
                    move.l   (a7)+, d0
                    rts

DOS_SAVEFILE:       move.l   a5, FILE_DESTBUFFER
                    move.l   d0, FILE_LENGTH
                    move.l   a0, FILENAME
                    
                    jsr      FREEBLITTER
                    move.l   FILENAME, d1
                    move.l   #$3EE, d2
                    move.l   DOSBASE, a6
                    jsr      _LVOOpen(a6)
                    move.l   d0, FILEHANDLE
                    bne.s    WRITEFILE
                    moveq    #0, d0
                    rts

WRITEFILE:          move.l   FILEHANDLE, d1
                    move.l   FILE_DESTBUFFER, d2
                    move.l   FILE_LENGTH, d3
                    move.l   DOSBASE, a6
                    jsr      _LVOWrite(a6)
                    move.l   d0, -(a7)
                    move.l   FILEHANDLE, d1
                    jsr      _LVOClose(a6)
                    jsr      GETBLITTER
                    move.l   (a7)+, d0
                    rts

; ---------------------------------------------
; Retrieve the vector base register
                    mc68020
GET_VBR:            move.l  4.w, a6
                    move.b  $128+1(a6), d0
                    btst.l  #1, d0
                    beq.b   NO_68020
                    lea     SUPERVISOR_VBR(pc), a5
                    jmp     _LVOSupervisor(a6)
NO_68020:           rts
SUPERVISOR_VBR:     movec   vbr, d0
                    lea.l   CUR_VBR(pc), a0
                    move.l  d0, (a0)
                    rte
                    mc68000

FILENAME:           dc.l     0
FILE_DESTBUFFER:    dc.l     0
FILE_LENGTH:        dc.l     0

TASKPTR:            dc.l     0
FILEHANDLE:         dc.l     0

CUR_VBR:            dc.l     0

; ---------------------------------------------
; main program
MAIN_BEGIN:         bsr.b    GET_VBR
                    move.w   #0, $DFF180
START:              move.w   #$A0, $DFF096
                    move.l   #LAST_ICON, $DFF120
                    move.l   #LAST_ICON, $DFF124
                    move.l   #LAST_ICON, $DFF128
                    move.l   #LAST_ICON, $DFF12C
                    move.l   #LAST_ICON, $DFF130
                    move.l   #LAST_ICON, $DFF134
                    move.l   #LAST_ICON, $DFF138
                    move.l   #LAST_ICON, $DFF13C
                    sf       DISK_IN
                    jsr      REAL_RAND
                    lea      HISCR_TAB, a5
                    
                    lea.l    HISCR_TAB, a0
                    lea      HISCORE_NAME, a1
                    jsr      LOADFILE_OPT
                    cmp.l    #112, d0                   ; hiscore table mùust be 112 bytes
                    beq.b    _INIT_CHIP
                    jsr      FIRST_HISC                 ; reset the hiscore table
_INIT_CHIP:         
                    jsr      INIT_CHIP
                    bsr      INIT_SYS
                    move.b   #START_WORLD, LEVEL_NUM
                    move.b   #START_STAGE, STAGE_NUM
                    jsr      CLRPALET
                    move.l   #COPPER2, COP_LOC
                    move.w   #$8080, $DFF096

                    move.l   a7, STACKVAL
                    jsr      INTRO_SCR

RESTART2:           jsr      SET_SPR_ADS
                    st       ITS_ZOOL
                    jsr      LOAD_PSPRS
                    move.l   #BOT_CHIP, TILES
                    jsr      INIT_GAME
                    move.w   #0, ZOONB_GOT
RESTART:            move.l   STACKVAL, a7
                    move.l   #NULLCOP, COP_LOC
                    jsr      CLRPALET
                    sf       ON_BONUS

                    tst.b    BEG_GAME
                    beq      lbC001368
                    btst     #3, CHEAT+1
                    bne      lbC001362
                    cmp.w    #3, ZOONB_GOT
                    bne      lbC001368
lbC001362:          st       ON_BONUS
lbC001368:          move.w   #15, $DFF096
                    sf       MUSIC_ON
                    jsr      NEXT_STAGE
                    move.w   #15, $DFF096
                    sf       MUSIC_ON
                    jsr      INIT_SCRNS
                    move.l   PHYSADR, a0
                    jsr      CLR_SCRN

                    jsr      INIT_LEVEL
                    jsr      INIT_LDATA
                    jsr      INIT_SCRO
                    jsr      SET_PERC
                    tst.b    BEG_GAME
                    bne      _SETUPSCRO
                    st       BEG_GAME
                    tst.b    PLAYERS
                    beq      _SETUPSCRO
                    jsr      STORE_P2
_SETUPSCRO:         
                    jsr      SETUPSCRO
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    tst.b    AUDIO
                    ble      lbC001420
                    ;moveq    #0, d0
                    ;move.b   LEVEL_NUM, d0
                    ;cmp.b    #7, d0
                    ;bne      lbC00140C
                    ;moveq    #2, d0
                    ;bra      lbC00140E

;lbC00140C:          addq.l   #3, d0
;lbC00140E:          ;move.w   #0, MUSICON
                    ;jsr      PLAYTRACK
                    jsr      PLAY_MUSIC
                    bra      lbC001430

lbC001420:          move.w   #15, $DFF096
                    ;move.w   #1, MUSICON
lbC001430:          
                    jsr      INIT_DUAL
                    move.l   #COPPER_GAME, COP_LOC
                    move.l   #$FFFFFFFE, WRITE_COP
                    move.w   #$A0, $DFF096
                    move.l   #LAST_ICON, $DFF120
                    move.l   #LAST_ICON, $DFF124
                    move.l   #LAST_ICON, $DFF128
                    move.l   #LAST_ICON, $DFF12C
                    move.l   #LAST_ICON, $DFF130
                    move.l   #LAST_ICON, $DFF134
                    move.l   #LAST_ICON, $DFF138
                    move.l   #LAST_ICON, $DFF13C
                    jsr      RESET_COP
                    bsr      PRO_MAIN
                    bsr      SYNCRO
                    st       GAME_FADE
                    jsr      INIT_FDON
                    move.w   #$8080, $DFF096
                    jsr      VBL
                    bsr      GO_FRAME
                    jsr      RESET_EBAR
                    bsr      PRO_MAIN
                    bsr      GO_FRAME
                    bra      MLOOP

STACKVAL:           dc.l     0

MLOOP:              ;btst     #3, VBL_CNT
                    ;beq      _MAIN_RTN
                    ;jsr      CHECKAUDIOFINISHED
;_MAIN_RTN:          
                    bsr      MAIN_RTN
                    tst.w    END_OF_STG
                    bmi      ENDSTAGE
                    tst.b    ZOOL_DIES
                    beq.s    MLOOP
RETURN:             bsr      DIE_RTN
                    bra.s    MLOOP

MAIN_RTN:           bsr      PRO_MAIN
                    bsr      SYNCRO
                    bra      GO_FRAME

PRO_MAIN:           jsr      RANDOM
                    jsr      READ_JOY
                    sf       TEST_MODE
                    cmp.b    #$D7, KEYREC
                    bne      _PRO_ZOOL
                    btst     #7, CHEAT
                    beq      _PRO_ZOOL
                    st       TEST_MODE
                    bra      _DEF_SCROLL

_PRO_ZOOL:          bsr      PRO_ZOOL
                    bsr      PRO_SHOTS
                    bsr      ENV_FORCES
                    bsr      DEF_MAPXY
                    bsr      GET_GRADIENT
                    bsr      GRADIENT_FX
                    bsr      DEF_MOVE
                    bsr      TOKEN_COL
                    bsr      ZOOL_COL
                    bsr      X_LIMIT
                    bsr      PRO_SHADE
                    bsr      PRO_BACKFX
                    bsr      PRO_SECTL
                    bsr      PRO_TOKENS
_DEF_SCROLL:        bsr      DEF_SCROLL
                    bsr      MAP_LIMITS
                    bsr      CALC_PRLX
                    eor.b    #1, ANDYFRAME
                    jsr      PRO_BADDY
                    jsr      SCROLL_BUFF
                    jsr      PRO_SPRITES
                    bsr      DEF_RASTER
                    bsr      DRAW_RTN
                    rts

GO_FRAME:           jsr      SCRO_NOW
                    jsr      SCRO2_NOW
                    jsr      DOCOL
                    rts

CHEAT_START:        move.b   #START_WORLD, LEVEL_NUM
                    move.b   #START_STAGE, STAGE_NUM
                    btst     #0, CHEAT
                    beq      lbC001602
                    move.b   #1, LEVEL_NUM
                    rts

lbC001602:          btst     #1, CHEAT
                    beq      lbC001618
                    move.b   #2, LEVEL_NUM
                    rts

lbC001618:          btst     #2, CHEAT
                    beq      lbC00162E
                    move.b   #3, LEVEL_NUM
                    rts

lbC00162E:          btst     #3, CHEAT
                    beq      lbC001644
                    move.b   #4, LEVEL_NUM
                    rts

lbC001644:          btst     #4, CHEAT
                    beq      lbC001658
                    move.b   #5, LEVEL_NUM
lbC001658:          rts

CHEAT:              dc.w     0

ENDSTAGE:           tst.w    FADE_CNT
                    bgt      RETURN
                    tst.w    FADECOP_CNT
                    bgt      RETURN
                    move.l   #$FFFFFFFE, WRITE_COP
                    jsr      VBL
                    sf       ZOOL_DIES
                    jsr      CLRPALET
                    sf       GAME_FADE
                    tst.b    MUSIC_ON
                    ble      lbC00178E
                    st       MUSIC_ON
                    move.w   #1, MUSIC_VOL
                    bra      lbC001796

lbC00178E:          move.w   #15, $DFF096
lbC001796:          clr.w    END_OF_STG
                    cmp.b    #5, LEVEL_NUM
                    beq      lbC0017E4
                    tst.b    PLAYERS
                    beq      lbC0017CC
                    move.w   #-1, END_OF_STG
                    jsr      NEW_PLAYER
                    move.w   #$8080, $DFF096
                    bra      MLOOP

lbC0017CC:          addq.b   #1, STAGE_NUM
                    cmp.b    #3, STAGE_NUM
                    beq      lbC0017E4
                    jmp      RESTART

lbC0017E4:          sf       STAGE_NUM
                    addq.b   #1, LEVEL_NUM
                    cmp.b    #8, LEVEL_NUM
                    bne      _RESTART
                    sf       LEVEL_NUM
_RESTART:           jmp      RESTART

DRAW_RTN:           jsr      UNDRAW
                    jsr      DRAW_PERMS
                    jsr      DRAW_BACKSP
                    jsr      DRAW_BRICKS
                    jsr      DRAW_BADDY
                    jsr      DRAW_TILEFX
                    jsr      DRAW_SECTL
                    jsr      DRAW_ZOONB
                    jsr      DRAW_SPRS
                    jsr      DRAW_ARCHEX
                    jsr      DRAW_TOKENS
                    jsr      DRAW_HEART
                    jsr      DRAW_SHOTS
                    jsr      DRAW_SHADE
                    jsr      DRAW_ZOOL
                    jsr      DRAW_SMARTS
                    jsr      DRAW_SHIELD
                    jsr      DRAW_PBOMB
                    jsr      DRAW_BEACS
                    jsr      PRINT_PANEL
                    rts

VBL:                move.w   SWITCH_CNT, d0
lbC001888:          cmp.w    SWITCH_CNT, d0
                    beq.s    lbC001888
                    move.w   #$C8, d7
lbC001894:          dbra     d7, lbC001894
                    rts

SYNCRO:             cmp.w    #2, SWITCH_CNT
                    bpl      STOOSLOW
                    clr.w    SWITCH_CNT
SWAIT:              bsr      GET_VBEAM
SYNC_SET:           cmp.w    #288, d1
                    bmi.s    SWAIT
                    bra      SNORM

STOOSLOW:           clr.w    SWITCH_CNT
                    st       HOLD_SPRS
                    bsr      GET_VBEAM
                    cmp.w    #32, d1
                    bpl.s    SWAIT
SNORM:              rts

INIT_SYS:           move.l   #COPPER_GAME, COP_LOC
                    jsr      SCRO_NOW
                    rts

NULL_IRQ:           tst.b    $BFDD00
                    move.w   #$2000, $DFF09C
NULL_INT:           rte

PRO_BACKFX:         bsr      PRO_TILEFX
                    bsr      EXTRA_TOKS
                    lea      BACKFX_RTNS, a0
                    moveq    #0, d0
                    move.b   LEVEL_NUM, d0
                    lsl.w    #2, d0
                    move.l   0(a0,d0.w), a0
                    jmp      (a0)

BACKFX_RTNS:        dc.l     BIRD1_FXRTN
                    dc.l     BULB_FXRTN
                    dc.l     TOOT_FXRTN
                    dc.l     SNAKE_FXRTN
                    dc.l     ICES_FXRTN
                    dc.l     LAST_FXRTN
                    dc.l     NO_FXRTN
                    dc.l     NO_FXRTN

NO_FXRTN:           rts

LAST_FXRTN:         tst.b    ANDYFRAME
                    beq      lbC001A3E
                    move.w   #2, BLK_TRIES
_RANDOM3:           jsr      RANDOM
                    move.w   SEED, d7
                    and.w    #15, d7
                    addq.w   #1, d7
                    move.w   MAP_LINE, d3
                    lsr.w    #4, d3
                    add.w    d7, d3
                    move.w   d3, d1
                    mulu     MAP_WIDTH, d3
                    move.w   XPOS, d0
                    move.b   SEED, d7
                    and.w    #$1F, d7
                    subq.w   #6, d7
                    lsr.w    #4, d0
                    add.w    d7, d0
                    add.w    d0, d3
                    move.l   REF_MAP, a0
                    lea      0(a0,d3.w), a0
                    cmp.b    #$4E, (a0)
                    bne      lbC001A40
                    lsl.l    #1, d3
                    and.l    #$1FFFF, d3
                    move.l   CURRENT_MAP, a1
                    addq.l   #8, a1
                    add.l    d3, a1
                    move.b   SEED+3, d7
                    and.w    #3, d7
                    move.w   d7, d6
                    lsl.w    #2, d6
                    add.w    #$1B3, d6
                    move.w   d6, (a1)
                    addq.w   #1, d6
                    move.w   d6, 2(a1)
                    move.w   MAP_WIDTH, d5
                    lsl.w    #1, d5
                    lea      0(a1,d5.w), a1
                    addq.w   #1, d6
                    move.w   d6, (a1)
                    addq.w   #1, d6
                    move.w   d6, 2(a1)
                    add.w    #$19, d7
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    addq.w   #6, d1
                    bsr      ADD_PERM
lbC001A3E:          rts

lbC001A40:          subq.w   #1, BLK_TRIES
                    bne      _RANDOM3
                    rts

ICES_FXRTN:         bsr      DO_BUTTONS
                    bsr      LOLL_EYES
                    bsr      DRAW_LOLLS
                    rts

LOLL_EYES:          tst.b    INAIR
                    bne      lbC001AF6
                    cmp.w    #3, LOLLS_ON
                    beq      lbC001AF6
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    add.w    #$11, d1
                    bsr      CHECK_TILE2
                    tst.b    CUSTOM_ON
                    beq      lbC001AF6
                    move.w   CUST_TILE, d7
                    sub.w    #$1C, d7
                    bmi      lbC001AF6
                    add.w    #$20, d1
                    and.w    #$FFF0, d1
                    cmp.w    #4, d7
                    bpl      lbC001AF6
                    tst.b    d7
                    beq      lbC001AE8
                    cmp.b    #2, d7
                    bmi      lbC001ADA
                    beq      lbC001ACC
                    sub.w    #$40, d0
                    and.w    #$FFF0, d0
                    addq.w   #8, d0
                    bra      _ADD_LOLL

lbC001ACC:          sub.w    #$30, d0
                    and.w    #$FFF0, d0
                    addq.w   #8, d0
                    bra      _ADD_LOLL

lbC001ADA:          sub.w    #$20, d0
                    and.w    #$FFF0, d0
                    addq.w   #8, d0
                    bra      _ADD_LOLL

lbC001AE8:          sub.w    #$10, d0
                    and.w    #$FFF0, d0
                    addq.w   #8, d0
_ADD_LOLL:          bra      ADD_LOLL

lbC001AF6:          rts

ADD_LOLL:           lea      LOLL_TAB, a0
lbC001AFE:          tst.b    (a0)
                    bne      lbC001B26
lbC001B04:          addq.l   #8, a0
                    cmp.l    #LAST_LOLL, a0
                    bne.s    lbC001AFE
                    lea      LOLL_TAB, a0
lbC001B14:          tst.b    (a0)
                    beq      lbC001B34
                    addq.l   #8, a0
                    cmp.l    #LAST_LOLL, a0
                    bne.s    lbC001B14
                    rts

lbC001B26:          cmp.w    2(a0), d0
                    bne.s    lbC001B04
                    cmp.w    4(a0), d1
                    bne.s    lbC001B04
                    rts

lbC001B34:          st       (a0)
                    move.b   #4, 1(a0)
                    move.w   d0, 2(a0)
                    move.w   d1, 4(a0)
                    move.w   #$38, 6(a0)
                    addq.w   #1, LOLLS_ON
                    rts

DRAW_LOLLS:         tst.w    LOLLS_ON
                    beq      lbC001BF0
                    lea      LOLL_TAB, a0
lbC001B62:          tst.b    (a0)
                    beq      lbC001BE4
                    subq.b   #1, 1(a0)
                    bpl      lbC001BAA
                    cmp.w    #$38, 6(a0)
                    bne      lbC001B9E
                    bclr     #7, (a0)
                    bne      lbC001B8E
                    bchg     #4, SEED
                    beq      lbC001BF2
lbC001B8E:          move.b   #$14, 1(a0)
                    move.w   #$39, 6(a0)
                    bra      lbC001BAA

lbC001B9E:          move.b   #4, 1(a0)
                    move.w   #$38, 6(a0)
lbC001BAA:          move.w   2(a0), d0
                    move.w   4(a0), d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   6(a0), d7
                    add.w    BACKFX_SPRS, d7
                    move.w   d7, SPRITE
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
lbC001BE4:          addq.l   #8, a0
                    cmp.l    #LAST_LOLL, a0
                    bne      lbC001B62
lbC001BF0:          rts

lbC001BF2:          sf       (a0)
                    subq.w   #1, LOLLS_ON
                    bra.s    lbC001BE4

DO_BUTTONS:         move.l   LAST_LOLL, a5
                    move.b   #$24, TEMPW
lbC001C0A:          tst.b    (a5)
                    beq      lbC001CF6
                    bmi      lbC001C72
lbC001C14:          move.b   11(a5), d6
                    move.w   6(a5), d0
                    move.w   8(a5), d1
                    ext.w    d6
                    lea      BUTPLAT_AN, a0
                    move.b   0(a0,d6.w), d7
                    bpl      lbC001C3A
                    st       (a5)
                    subq.b   #1, 11(a5)
                    bra      lbC001DCE

lbC001C3A:          cmp.b    #2, (a5)
                    bne      lbC001C54
                    subq.b   #1, 11(a5)
                    bpl      lbC001C58
                    sf       (a5)
                    bsr      lbC001D92
                    bra      lbC001CF6

lbC001C54:          addq.b   #1, 11(a5)
lbC001C58:          lsl.w    #4, d0
                    lsl.w    #4, d1
                    ext.w    d7
                    add.w    #$21, d7
                    sub.w    #$10, d0
                    bsr      ADD_BACKSP
                    add.w    #$20, d0
                    bsr      ADD_BACKSP
lbC001C72:          tst.b    12(a5)
                    beq      lbC001CF6
                    move.b   10(a5), d6
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    ext.w    d6
                    lea      BUTSPR_AN, a0
                    move.b   0(a0,d6.w), d7
                    bpl      lbC001CD2
                    move.b   (a0), d7
                    sf       10(a5)
                    subq.b   #1, 1(a5)
                    bpl      lbC001CD2
                    cmp.b    #2, 12(a5)
                    bpl      lbC001CBE
                    move.l   a5, -(sp)
                    lea      SNOWF_FX, a5
                    jsr      ADD_SFX
                    move.l   (sp)+, a5
lbC001CBE:          move.b   #8, 1(a5)
                    addq.b   #1, 12(a5)
                    cmp.b    #6, 12(a5)
                    beq      lbC001D0C
lbC001CD2:          subq.b   #1, 13(a5)
                    bne      lbC001CE4
                    move.b   12(a5), 13(a5)
                    addq.b   #1, 10(a5)
lbC001CE4:          lsl.w    #4, d0
                    lsl.w    #4, d1
                    ext.w    d7
                    add.w    #$1E, d7
                    sub.w    #$10, d0
                    bsr      ADD_BACKSP
lbC001CF6:          lea      14(a5), a5
                    addq.b   #1, TEMPW
                    cmp.l    LAST_BUTT, a5
                    bne      lbC001C0A
                    rts

lbC001D0C:          move.l   a5, -(sp)
                    lea      ICEPLAT_FX, a5
                    jsr      ADD_SFX
                    move.l   (sp)+, a5
                    move.w   6(a5), d0
                    move.w   8(a5), d1
                    sf       12(a5)
                    move.b   #2, (a5)
                    subq.b   #1, 11(a5)
                    mulu     MAP_WIDTH, d1
                    add.w    d0, d1
                    move.l   REF_MAP, a0
                    add.l    d1, a0
                    sf       (a0)+
                    sf       (a0)+
                    sf       (a0)+
                    sf       (a0)+
                    move.l   CURRENT_MAP, a0
                    addq.l   #8, a0
                    add.l    d1, a0
                    add.l    d1, a0
                    clr.w    (a0)+
                    clr.w    (a0)+
                    clr.w    (a0)+
                    clr.w    (a0)+
                    move.w   MAP_WIDTH, d2
                    lsl.w    #1, d2
                    lea      -8(a0,d2.w), a0
                    clr.w    (a0)+
                    clr.w    (a0)+
                    clr.w    (a0)+
                    clr.w    (a0)+
                    move.w   #$FFDE, d7
                    move.w   6(a5), d0
                    move.w   8(a5), d1
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    bsr      ADD_PERM
                    add.w    #$20, d0
                    bsr      ADD_PERM
                    bra      lbC001C14

lbC001D92:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    mulu     MAP_WIDTH, d1
                    add.w    d0, d1
                    move.l   REF_MAP, a0
                    add.l    d1, a0
                    move.b   TEMPW, (a0)+
                    move.b   TEMPW, (a0)+
                    move.w   MAP_WIDTH, d6
                    lea      -2(a0,d6.w), a0
                    move.b   TEMPW, (a0)+
                    move.b   TEMPW, (a0)+
                    rts

lbC001DCE:          move.w   d1, d2
                    mulu     MAP_WIDTH, d2
                    add.w    d0, d2
                    move.l   REF_MAP, a0
                    add.l    d2, a0
                    move.b   #2, (a0)+
                    move.b   #2, (a0)+
                    move.b   #2, (a0)+
                    move.b   #2, (a0)+
                    move.l   CURRENT_MAP, a0
                    add.l    d2, a0
                    add.l    d2, a0
                    addq.l   #8, a0
                    move.w   #$1C2, (a0)+
                    move.w   #$1C3, (a0)+
                    move.w   #$1C2, (a0)+
                    move.w   #$1C3, (a0)+
                    move.w   MAP_WIDTH, d2
                    lsl.w    #1, d2
                    lea      -8(a0,d2.w), a0
                    move.w   #$1C4, (a0)+
                    move.w   #$1C5, (a0)+
                    move.w   #$1C4, (a0)+
                    move.w   #$1C5, (a0)+
                    move.w   #$24, d7
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    sub.w    #$10, d0
                    bsr      ADD_PERM
                    add.w    #$20, d0
                    bsr      ADD_PERM
                    bra      lbC001C72

BUTTON_ON:          cmp.b    -1(a1), d7
                    bne      lbC001E50
                    subq.l   #1, a1
                    subq.w   #1, d0
lbC001E50:          move.w   MAP_WIDTH, d6
                    cmp.b    0(a1,d6.w), d7
                    beq      lbC001E66
                    neg.w    d6
                    lea      0(a1,d6.w), a1
                    subq.w   #1, d1
lbC001E66:          move.l   LAST_LOLL, a6
                    sub.b    #$24, d7
                    ext.w    d7
                    mulu     #14, d7
                    lea      0(a6,d7.w), a6
                    move.b   #1, (a6)+
                    move.b   #6, (a6)+
                    move.w   d0, (a6)+
                    move.w   d1, (a6)+
                    addq.l   #4, a6
                    sf       (a6)+
                    sf       (a6)+
                    move.b   #1, (a6)+
                    move.b   #1, (a6)+
                    sf       (a1)+
                    sf       (a1)+
                    move.w   MAP_WIDTH, d6
                    lea      -2(a1,d6.w), a1
                    sf       (a1)+
                    sf       (a1)+
                    move.l   a5, -(sp)
                    lea      ICEPLAT_FX, a5
                    jsr      ADD_SFX
                    lea      SNOWF_FX, a5
                    jsr      ADD_SFX
                    move.l   (sp)+, a5
                    rts

BIRD1_FXRTN:        move.w   $DFF002, d7
                    and.w    #15, d7
                    bne      lbC001F4C
                    subq.w   #1, TWEET_CNT
                    bpl      lbC001F4C
                    move.b   SEED, d7
                    move.w   SEED+2, d6
                    and.w    #$3F, d6
                    move.w   d6, TWEET_CNT
                    and.w    #15, d6
                    addq.w   #2, d6
                    and.b    #3, d7
                    beq      lbC001F3C
                    cmp.b    #2, d7
                    bmi      lbC001F2C
                    beq      lbC001F1C
                    lea      TWEET4_FX, a5
                    move.w   d6, 6(a5)
                    jmp      ADD_SFX

lbC001F1C:          lea      TWEET3_FX, a5
                    move.w   d6, 6(a5)
                    jmp      ADD_SFX

lbC001F2C:          lea      TWEET2_FX, a5
                    move.w   d6, 6(a5)
                    jmp      ADD_SFX

lbC001F3C:          lea      TWEET1_FX, a5
                    move.w   d6, 6(a5)
                    jmp      ADD_SFX

lbC001F4C:          rts

FALL_BRICKS:        lea      BRICKS_TAB, a0
lbC001F54:          tst.w    (a0)
                    beq      lbC001FE4
                    addq.w   #2, 6(a0)
                    cmp.w    #$20, 6(a0)
                    ble      lbC001F6E
                    move.w   #$20, 6(a0)
lbC001F6E:          move.w   4(a0), d1
                    add.w    6(a0), d1
                    move.w   d1, 4(a0)
                    move.w   d1, d2
                    and.w    #$3F, d2
                    sub.w    6(a0), d2
                    bpl      lbC001FE4
                    lsr.w    #2, d1
                    move.w   2(a0), d0
                    lsr.w    #4, d0
                    lsr.w    #4, d1
                    move.l   REF_MAP, a1
                    mulu     MAP_WIDTH, d1
                    ext.l    d0
                    add.l    d0, d1
                    add.l    d1, a1
                    cmp.b    #$21, (a1)
                    bne      lbC001FE4
                    move.b   #2, (a1)
                    lsl.l    #1, d1
                    move.l   CURRENT_MAP, a1
                    addq.l   #8, a1
                    add.l    d1, a1
                    move.w   BRICK_TILE, (a1)
                    clr.w    (a0)
                    move.w   BRICKL_SPR, d7
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    lsr.w    #2, d1
                    and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
                    sub.w    #$10, d0
                    bsr      ADD_PERM
lbC001FE4:          addq.w   #8, a0
                    cmp.l    #END_BRICKS, a0
                    bne      lbC001F54
                    rts

ADD_BRICK:          lea      BRICKS_TAB, a5
                    add.l    #$32, SCORE
lbC002002:          tst.w    (a5)
                    bne      lbC002024
                    st       (a5)
                    and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
                    move.w   d0, 2(a5)
                    lsl.w    #2, d1
                    move.w   d1, 4(a5)
                    lsr.w    #2, d1
                    clr.w    6(a5)
                    rts

lbC002024:          addq.w   #8, a5
                    cmp.l    #END_BRICKS, a5
                    bne.s    lbC002002
                    rts

DRAW_BRICKS:        tst.b    BRICKS_ON
                    beq      lbC002092
                    lea      BRICKS_TAB, a0
lbC002040:          tst.w    (a0)
                    beq      lbC002088
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    sub.w    #$10, d0
                    lsr.w    #2, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   BRICKF_SPR, d7
                    add.w    BACKFX_SPRS, d7
                    move.w   d7, SPRITE
                    move.l   a0, -(sp)
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
lbC002088:          addq.w   #8, a0
                    cmp.l    #END_BRICKS, a0
                    bne.s    lbC002040
lbC002092:          rts

SNAKE_FXRTN:        bsr      SNAKE_BLINK
                    bsr      LAVA_SPITS
                    bsr      FALL_BRICKS
                    bsr      SNAKE_TAXI
                    tst.b    SNAKE_GROW
                    beq      lbC0020BA
                    bsr      SNAKE_TAXI
                    bsr      SNAKE_TAXI
                    bsr      SNAKE_TAXI
lbC0020BA:          rts

SNAKE_BLINK:        tst.w    SNAKE_BLNK
                    bne      lbC002142
                    jsr      RANDOM
                    move.w   SEED, d0
                    and.w    #$1F, d0
                    cmp.w    SNAKE_BLNKS, d0
                    bpl      lbC002138
                    move.l   END_BRICKS, a0
                    move.w   d0, SNAKE_BLNK
                    lsl.w    #1, d0
                    add.w    SNAKE_BLNK, d0
                    lsl.w    #1, d0
                    addq.w   #1, SNAKE_BLNK
                    lea      0(a0,d0.w), a0
                    move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    move.w   d0, d2
                    add.w    #$30, d2
                    sub.w    XPOS, d2
                    bmi      lbC00213A
                    cmp.w    #$180, d2
                    bpl      lbC00213A
                    move.w   d0, SNAKEBX
                    move.w   d1, SNAKEBY
                    move.w   (a0)+, SNAKEB_AN
                    clr.w    SNAKEB_F
                    bra      GO_FIREB

lbC002138:          rts

lbC00213A:          clr.w    SNAKE_BLNK
                    rts

lbC002142:          move.w   SNAKEB_F, d7
                    lea      SNAKEB1_AN, a0
                    tst.w    SNAKEB_AN
                    beq      lbC00215E
                    lea      SNAKEB2_AN, a0
lbC00215E:          addq.w   #1, SNAKEB_F
                    move.b   0(a0,d7.w), d7
                    bmi      lbC00218E
                    add.w    #$53, d7
                    move.w   SNAKEBX, d0
                    move.w   SNAKEBY, d1
                    sub.w    #$10, d0
                    bsr      ADD_BACKSP
                    add.w    #$20, d0
                    addq.w   #2, d7
                    bra      ADD_BACKSP

lbC00218E:          clr.w    SNAKE_BLNK
                    rts

GO_FIREB:           move.l   ENEMY_BULLS, a6
                    sub.l    STAGE_SPRS, a6
                    add.l    #SPR_DATA, a6
                    tst.b    (a6)
                    bne      _RANDOM4
                    lea      $30(a6), a6
                    tst.b    (a6)
                    bne      lbC00220A
_RANDOM4:           jsr      RANDOM
                    bchg     #7, SEED+3
                    beq      lbC00220A
                    move.w   SNAKEBX, d0
                    move.w   SNAKEBY, d1
                    jsr      ONSCR_CHK
                    beq      lbC00220A
                    move.w   d0, 2(a6)
                    add.w    #$1E, d1
                    move.w   d1, 4(a6)
                    lsl.w    #4, d1
                    move.w   d1, 10(a6)
                    move.w   #$60, 8(a6)
                    st       (a6)
                    sf       $1C(a6)
                    lea      HISS_FX, a5
                    jmp      ADD_SFX

lbC00220A:          rts

SNAKE_TAXI:         tst.b    SNAKE_STAT
                    beq      lbC00239C
                    bsr      GO_SNAKE
                    tst.b    SNAKE_ON
                    beq      lbC00239C
                    move.w   SNAKEX, d0
                    move.w   SNAKEY, d1
                    add.w    #2, d0
                    move.w   d0, SNAKEX
                    tst.b    BELL_RING
                    beq      lbC002276
                    tst.b    SNAKE_GROW
                    bne      lbC002276
                    move.w   ZOOL_X, d2
                    add.w    #$180, d2
                    sub.w    d0, d2
                    bmi      lbC002268
                    sf       BELL_RING
                    bra      lbC002276

lbC002268:          move.b   #$14, CLEAR_SNAKE
                    sf       BELL_RING
lbC002276:          move.l   REF_MAP, -(sp)
                    move.l   SNAKE_REF, REF_MAP
                    bsr      CHECK_TILET
                    move.l   (sp)+, REF_MAP
                    tst.b    CUSTOM_ON
                    bne      lbC0022E6
                    cmp.b    #$1F, d7
                    beq      lbC0022D8
                    cmp.b    #13, d7
                    beq      lbC0022B2
                    cmp.b    #2, d7
                    bne      lbC0022C4
lbC0022B2:          and.w    #$FFF0, SNAKEY
                    clr.w    SNBODY_SPR
                    bra      lbC002312

lbC0022C4:          addq.w   #8, SNAKEY
                    move.w   SNAKEX, d0
                    move.w   SNAKEY, d1
                    bra.s    lbC002276

lbC0022D8:          st       SNAKE_CHK
                    sf       SNAKE_ON
                    rts

lbC0022E6:          ext.w    d7
                    addq.w   #1, d7
                    add.w    d7, SNAKEY
                    move.w   CUST_TILE, d7
                    cmp.w    #7, d7
                    bpl      lbC00230A
                    addq.w   #1, d7
                    move.w   d7, SNBODY_SPR
                    bra      lbC002312

lbC00230A:          subq.w   #6, d7
                    move.w   d7, SNBODY_SPR
lbC002312:          move.w   SNAKEX, d0
                    move.w   SNAKEY, d1
                    move.w   d0, d2
                    and.w    #15, d2
                    cmp.w    #2, d2
                    bpl      lbC00234A
                    tst.b    CLEAR_SNAKE
                    ble      _REP_SNKTLS
                    subq.b   #1, CLEAR_SNAKE
                    bne      _PRO_TAIL
                    bra.s    lbC0022D8

_REP_SNKTLS:        bsr      REP_SNKTLS
                    bsr      HEAD_TILES
lbC00234A:          tst.b    SNAKE_GROW
                    bne      _PRO_TAIL
                    move.w   SNBODY_SPR, d7
                    lea      END_SN3, a0
                    lsl.w    #2, d7
                    add.w    SNBODY_SPR, d7
                    add.w    SNBODY_SPR, d7
                    move.w   SNAKEX, d0
                    move.w   SNAKEY, d1
                    add.w    2(a0,d7.w), d0
                    add.w    4(a0,d7.w), d1
                    move.w   d0, d2
                    move.w   d1, d3
                    move.w   0(a0,d7.w), d7
                    sub.w    #$19, d0
                    subq.w   #8, d1
                    add.w    #$4A, d7
                    bsr      ADD_BACKSP
_PRO_TAIL:          bsr      PRO_TAIL
lbC00239C:          rts

SN_HITZ:            tst.b    ZOOL_HIT
                    bne      lbC0023CA
                    tst.w    SHIELD_ON
                    bne      lbC0023CA
                    tst.b    SHADE_ON
                    bne      lbC0023CA
                    move.b   #$28, ZOOL_HIT
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
lbC0023CA:          rts

PRO_TAIL:           move.l   SNAKE_PTR, a0
                    move.w   SNAKEX, (a0)+
                    move.w   SNAKEY, (a0)+
                    move.w   SNBODY_SPR, (a0)+
                    move.l   a0, SNAKE_PTR
                    cmp.l    #SNAKE_PTR, a0
                    bne      lbC002408
                    lea      SNAKE_TAB, a0
                    move.l   a0, SNAKE_PTR
                    sf       SNAKE_GROW
lbC002408:          tst.b    SNAKE_GROW
                    bne      lbC0024A8
                    move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    move.w   (a0)+, d7
                    lea      $30(a0), a4
                    move.w   d0, d2
                    and.w    #15, d2
                    cmp.w    #2, d2
                    bpl      _DRAW_TAIL
                    move.l   REF_MAP, a0
                    move.w   d1, d2
                    move.w   d0, d3
                    lsr.w    #4, d2
                    lsr.w    #4, d3
                    move.w   MAP_WIDTH, d6
                    mulu     d6, d2
                    add.w    d3, d2
                    add.l    d2, a0
                    cmp.b    #6, 0(a0,d6.w)
                    beq      lbC00245A
                    sf       (a0)
                    sf       0(a0,d6.w)
                    lsl.w    #1, d6
                    sf       0(a0,d6.w)
lbC00245A:          move.l   CURRENT_MAP, a0
                    addq.l   #8, a0
                    add.l    d2, a0
                    add.l    d2, a0
                    clr.w    (a0)
                    lea      0(a0,d6.w), a0
                    clr.w    (a0)
                    lea      0(a0,d6.w), a0
                    clr.w    (a0)
                    move.w   d0, d2
                    move.w   d1, d3
                    move.w   d7, d6
                    and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
                    sub.w    #$10, d0
                    move.w   #$50, d7
                    bsr      ADD_PERM
                    add.w    #$10, d1
                    bsr      ADD_PERM
                    add.w    #$10, d1
                    bsr      ADD_PERM
                    move.w   d6, d7
                    move.w   d2, d0
                    move.w   d3, d1
_DRAW_TAIL:         bsr      DRAW_TAIL
lbC0024A8:          rts

DRAW_TAIL:          cmp.l    #SNAKE_PTR, a4
                    bmi      lbC0024C0
                    sub.l    #SNAKE_PTR, a4
                    add.l    #SNAKE_TAB, a4
lbC0024C0:          move.w   (a4)+, d0
                    move.w   (a4)+, d1
                    move.w   (a4)+, d7
                    lea      SNTL_TAB, a0
                    move.w   d7, d6
                    lsl.w    #1, d6
                    lsl.w    #2, d7
                    add.w    d6, d7
                    add.w    2(a0,d7.w), d0
                    add.w    4(a0,d7.w), d1
                    move.w   0(a0,d7.w), d7
                    sub.w    #$40, d0
                    subq.w   #8, d1
                    add.w    #$4A, d7
                    bra      ADD_BACKSP

INIT_SNAKE:         lea      SNAKE_TAB, a0
                    move.l   a0, SNAKE_PTR
lbC0024FC:          clr.l    (a0)+
                    clr.w    (a0)+
                    cmp.l    #SNAKE_PTR, a0
                    bne.s    lbC0024FC
                    st       SNAKE_GROW
                    st       SNAKE_ON
                    sf       SNAKE_CHK
                    sf       BELL_RING
                    rts

GO_SNAKE:           tst.b    SNAKE_ON
                    bne      lbC00257C
                    tst.b    SNAKE_CHK
                    beq      lbC00257C
                    move.l   LAST_GONE, a0
lbC00253C:          move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    lea      (a0), a1
                    sub.w    (a0)+, d0
                    bmi      lbC002570
                    cmp.w    (a0)+, d0
                    bpl      lbC002570
                    sub.w    (a0)+, d1
                    bmi      lbC002570
                    cmp.w    (a0)+, d1
                    bpl      lbC002570
                    move.w   (a0)+, SNAKEX
                    move.w   (a0)+, SNAKEY
                    bra.s    INIT_SNAKE

lbC002570:          lea      12(a1), a0
                    cmp.l    LAST_SNAKE, a0
                    bne.s    lbC00253C
lbC00257C:          rts

HEAD_TILES:         move.w   SNAKEX, d0
                    move.w   SNAKEY, d1
                    and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
                    sub.w    #$10, d0
                    move.w   SNBODY_SPR, d7
                    move.w   d7, d6
                    lsl.w    #1, d7
                    add.w    d6, d7
                    add.w    #$1D, d7
                    bsr      ADD_PERM
                    addq.w   #1, d7
                    add.w    #$10, d1
                    bsr      ADD_PERM
                    addq.w   #1, d7
                    add.w    #$10, d1
                    bra      ADD_PERM

REP_SNKTLS:         move.l   SNAKE_REF, a0
                    move.l   SNAKE_MAP, a1
                    lsr.w    #4, d0
                    lsr.w    #4, d1
                    mulu     MAP_WIDTH, d1
                    add.w    d0, d1
                    add.l    d1, a0
                    addq.l   #8, a1
                    add.l    d1, a1
                    add.l    d1, a1
                    move.l   REF_MAP, a2
                    move.l   CURRENT_MAP, a3
                    add.l    d1, a2
                    addq.l   #8, a3
                    add.l    d1, a3
                    add.l    d1, a3
                    move.w   MAP_WIDTH, d1
                    ext.l    d1
                    move.b   (a0), (a2)
                    add.l    d1, a0
                    add.l    d1, a2
                    move.b   (a0), (a2)
                    add.l    d1, a0
                    add.l    d1, a2
                    move.b   (a0), (a2)
                    move.w   (a1), (a3)
                    add.l    d1, a1
                    add.l    d1, a3
                    add.l    d1, a1
                    add.l    d1, a3
                    move.w   (a1), (a3)
                    add.l    d1, a1
                    add.l    d1, a3
                    add.l    d1, a1
                    add.l    d1, a3
                    move.w   (a1), (a3)
                    rts

BULB_FXRTN:         bsr      WALL_SPARKS
                    bra      GO_BEAMS

WALL_SPARKS:        subq.w   #1, SPARK_AN
                    bpl      lbC0026BE
                    clr.w    SPARK_AN
                    jsr      RANDOM
                    move.w   SEED, d7
                    and.w    #15, d7
                    move.w   MAP_LINE, d3
                    lsr.w    #4, d3
                    add.w    d7, d3
                    move.w   d3, d1
                    mulu     MAP_WIDTH, d3
                    move.w   XPOS, d0
                    move.b   SEED, d7
                    and.w    #$1F, d7
                    subq.w   #6, d7
                    lsr.w    #4, d0
                    add.w    d7, d0
                    add.w    d0, d3
                    move.l   REF_MAP, a0
                    lea      0(a0,d3.w), a0
                    cmp.b    #$15, (a0)
                    bne      lbC0026BC
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    addq.w   #8, d1
                    sub.w    #9, d0
                    move.w   #11, SPARK_AN
                    move.w   #$3F, d2
                    bsr      NOTOK
                    subq.w   #7, d0
                    subq.w   #8, d1
                    move.w   d0, SPARKX
                    move.w   d1, SPARKY
                    lea      SPARK_FX, a5
                    jmp      ADD_SFX

lbC0026BC:          rts

lbC0026BE:          lea      SPARK_TAB, a0
                    move.w   SPARK_AN, d7
                    move.b   0(a0,d7.w), d7
                    move.w   SPARKX, d0
                    move.w   SPARKY, d1
                    bra      ADD_BACKSP

GO_BEAMS:           move.l   TORCH_PTR, a0
                    move.w   TORCHES, d7
lbC0026EA:          dbra     d7, lbC0026F0
                    rts

lbC0026F0:          move.w   12(a0), d0
                    move.w   14(a0), d1
                    add.w    #$120, d0
                    sub.w    XPOS, d0
                    bmi      lbC002900
                    cmp.w    #$380, d0
                    bpl      lbC002900
                    add.w    #$40, d1
                    sub.w    MAP_LINE, d1
                    bmi      lbC002900
                    cmp.w    #$180, d1
                    bpl      lbC002900
                    tst.b    (a0)
                    beq      lbC0028EE
                    move.l   4(a0), a1
                    add.l    REF_MAP, a1
                    move.w   10(a0), d0
                    move.w   d0, d6
                    and.w    #3, d6
                    lsr.w    #2, d0
                    tst.b    1(a0)
                    bne      lbC002912
                    lea      0(a1,d0.w), a1
                    bsr      lbC0027E8
                    cmp.b    #$14, (a1)
                    beq      lbC00278A
                    move.b   #2, (a1)
                    move.w   8(a0), d0
                    cmp.w    2(a0), d0
                    bmi      lbC0027A6
                    addq.w   #1, 10(a0)
                    and.l    #$FFFF, d0
                    sub.l    d0, a1
                    bsr      lbC0028A8
                    cmp.b    #2, -1(a1)
                    bne      lbC0028F8
                    sf       -1(a1)
                    bra      lbC0028F8

lbC00278A:          move.w   8(a0), d0
                    beq      lbC0027DA
                    and.l    #$FFFF, d0
                    lea      (a1), a3
                    sub.l    d0, a1
                    bsr      lbC0028A8
                    sf       -1(a1)
                    lea      (a3), a1
lbC0027A6:          move.w   10(a0), d1
                    addq.w   #1, 10(a0)
                    and.w    #3, d1
                    addq.w   #1, d1
                    btst     #2, d1
                    beq      lbC0028F8
                    addq.w   #1, 8(a0)
                    and.w    #$FFFC, 10(a0)
                    cmp.b    #$14, (a1)
                    bne      lbC0028F8
                    subq.w   #2, 8(a0)
                    subq.w   #4, 10(a0)
                    bra      lbC0028F8

lbC0027DA:          bsr      lbC0028A8
                    sf       -1(a1)
                    sf       (a0)
                    bra      lbC0028F8

lbC0027E8:          move.w   d0, -(sp)
                    move.w   d7, -(sp)
                    move.l   a1, d0
                    move.l   CURRENT_MAP, a2
                    addq.l   #8, a2
                    sub.l    REF_MAP, d0
                    add.l    d0, a2
                    add.l    d0, a2
                    divu     MAP_WIDTH, d0
                    move.w   d0, d1
                    swap     d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    sub.w    #$10, d0
                    cmp.b    #$14, (a1)
                    bne      lbC00282C
                    cmp.w    #1, 8(a0)
                    ble      lbC00283C
                    bsr      lbC002842
                    bra      lbC00283C

lbC00282C:          move.w   LBEAM_SPRS, d7
                    add.w    d6, d7
                    bsr      ADD_BACKSP
                    bsr      lbC002842
lbC00283C:          move.w   (sp)+, d7
                    move.w   (sp)+, d0
lbC002840:          rts

lbC002842:          tst.b    d6
                    bne.s    lbC002840
                    cmp.b    #2, -1(a1)
                    bne.s    lbC002840
                    sub.w    #$10, d0
                    move.w   LBEAM_SPRS, d7
                    addq.w   #4, d7
                    bsr      ADD_PERM
                    move.w   LBEAM_TILE, -2(a2)
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    move.w   LBEAM_TILE, -2(a2,d7.w)
                    rts

lbC002878:          tst.b    d6
                    bne.s    lbC002840
                    cmp.b    #$14, -1(a1)
                    beq.s    lbC002840
                    sub.w    #$10, d0
                    move.w   LBEAM_SPRS, d7
                    add.w    #9, d7
                    bsr      ADD_PERM
                    clr.w    -2(a2)
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    clr.w    -2(a2,d7.w)
                    rts

lbC0028A8:          move.w   d0, -(sp)
                    move.w   d7, -(sp)
                    move.l   a1, d0
                    move.l   CURRENT_MAP, a2
                    addq.l   #8, a2
                    sub.l    REF_MAP, d0
                    add.l    d0, a2
                    add.l    d0, a2
                    divu     MAP_WIDTH, d0
                    move.w   d0, d1
                    swap     d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    sub.w    #$10, d0
                    tst.w    8(a0)
                    beq      lbC0028E8
                    move.w   LBEAM_SPRS, d7
                    addq.w   #8, d7
                    sub.w    d6, d7
                    bsr      ADD_BACKSP
lbC0028E8:          bsr.s    lbC002878
                    bra      lbC00283C

lbC0028EE:          clr.w    10(a0)
                    clr.w    8(a0)
                    st       (a0)
lbC0028F8:          tst.w    8(a0)
                    bmi      lbC002908
lbC002900:          lea      $10(a0), a0
                    bra      lbC0026EA

lbC002908:          sf       (a0)
                    lea      $10(a0), a0
                    bra      lbC0026EA

lbC002912:          and.l    #$FFFF, d0
                    sub.l    d0, a1
                    bsr      lbC002978
                    cmp.b    #$14, (a1)
                    beq      lbC002954
                    move.b   #2, (a1)
                    move.w   8(a0), d0
                    cmp.w    2(a0), d0
                    bmi      lbC0027A6
                    addq.w   #1, 10(a0)
                    and.l    #$FFFF, d0
                    add.l    d0, a1
                    bsr      lbC002A40
                    cmp.b    #2, 1(a1)
                    bne.s    lbC0028F8
                    sf       1(a1)
                    bra.s    lbC0028F8

lbC002954:          move.w   8(a0), d0
                    beq      lbC002A86
                    move.w   8(a0), d0
                    and.l    #$FFFF, d0
                    lea      (a1), a3
                    add.l    d0, a1
                    bsr      lbC002A40
                    sf       1(a1)
                    lea      (a3), a1
                    bra      lbC0027A6

lbC002978:          move.w   d0, -(sp)
                    move.w   d7, -(sp)
                    move.l   a1, d0
                    move.l   CURRENT_MAP, a2
                    addq.l   #8, a2
                    sub.l    REF_MAP, d0
                    add.l    d0, a2
                    add.l    d0, a2
                    divu     MAP_WIDTH, d0
                    move.w   d0, d1
                    swap     d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    sub.w    #$10, d0
                    cmp.b    #$14, (a1)
                    bne      lbC0029BC
                    cmp.w    #1, 8(a0)
                    ble      lbC00283C
                    bsr      lbC0029D2
                    bra      lbC00283C

lbC0029BC:          move.w   LBEAM_SPRS, d7
                    addq.w   #5, d7
                    add.w    d6, d7
                    bsr      ADD_BACKSP
                    bsr      lbC0029D2
                    bra      lbC00283C

lbC0029D2:          tst.b    d6
                    bne      lbC002840
                    cmp.b    #2, 1(a1)
                    bne      lbC002840
                    add.w    #$10, d0
                    move.w   LBEAM_SPRS, d7
                    addq.w   #4, d7
                    bsr      ADD_PERM
                    move.w   LBEAM_TILE, 2(a2)
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    move.w   LBEAM_TILE, 2(a2,d7.w)
                    rts

lbC002A0C:          tst.b    d6
                    bne      lbC002840
                    cmp.b    #$14, 1(a1)
                    beq      lbC002840
                    add.w    #$10, d0
                    move.w   LBEAM_SPRS, d7
                    add.w    #9, d7
                    bsr      ADD_PERM
                    clr.w    2(a2)
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    clr.w    2(a2,d7.w)
                    rts

lbC002A40:          move.w   d0, -(sp)
                    move.w   d7, -(sp)
                    move.l   a1, d0
                    move.l   CURRENT_MAP, a2
                    addq.l   #8, a2
                    sub.l    REF_MAP, d0
                    add.l    d0, a2
                    add.l    d0, a2
                    divu     MAP_WIDTH, d0
                    move.w   d0, d1
                    swap     d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    sub.w    #$10, d0
                    tst.w    8(a0)
                    beq      lbC002A80
                    move.w   LBEAM_SPRS, d7
                    addq.w   #3, d7
                    sub.w    d6, d7
                    bsr      ADD_BACKSP
lbC002A80:          bsr.s    lbC002A0C
                    bra      lbC00283C

lbC002A86:          bsr.s    lbC002A40
                    sf       1(a1)
                    sf       (a0)
                    bra      lbC0028F8

TOOT_FXRTN:         bsr      LAVA_SPITS
                    bsr      PHAROAHS
                    bsr      COL_LIFTS
                    bra      GO_COLS

                    rts

COL_LIFTS:          tst.b    INAIR
                    bne      lbC002AFC
                    cmp.b    #1, STAGE_NUM
                    bgt      lbC002AFE
                    beq      lbC002AFE
                    tst.b    LIFT1
                    bne      lbC002AFC
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sub.w    #$7E0, d0
                    bmi      lbC002AFC
                    cmp.w    #$40, d0
                    bpl      lbC002AFC
                    sub.w    #$720, d1
                    bpl      lbC002AFC
                    cmp.w    #$FE70, d1
                    bmi      lbC002AFC
                    or.b     #1, LIFT1
lbC002AFC:          rts

lbC002AFE:          tst.b    LIFT2
                    bne      lbC002B3E
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sub.w    #$210, d0
                    bmi      lbC002B3E
                    cmp.w    #$40, d0
                    bpl      lbC002B3E
                    sub.w    #$520, d1
                    bpl      lbC002B3E
                    cmp.w    #$FF20, d1
                    bmi      lbC002B3E
                    or.b     #1, LIFT2
                    rts

lbC002B3E:          tst.b    LIFT3
                    bne      lbC002B7C
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sub.w    #$670, d0
                    bmi      lbC002B7C
                    cmp.w    #$40, d0
                    bpl      lbC002B7C
                    sub.w    #$3C0, d1
                    bpl      lbC002B7C
                    cmp.w    #$FF60, d1
                    bmi      lbC002B7C
                    or.b     #1, LIFT3
lbC002B7C:          rts

PHAROAHS:           cmp.w    #ZOOL_GRIPPING, ZOOL_MOVE
                    beq      lbC002B96
                    tst.b    INAIR
                    beq      lbC002B96
                    rts

lbC002B96:          cmp.b    #1, STAGE_NUM
                    bgt      lbC002C56
                    beq      lbC002C44
                    lea      END_TRES, a0
                    move.w   #5, TEMPW
lbC002BB4:          move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    add.w    #$60, d0
                    sub.w    (a0), d0
                    bmi      lbC002C36
                    cmp.w    #$D0, d0
                    bpl      lbC002C36
                    add.w    #$52, d1
                    sub.w    2(a0), d1
                    bmi      lbC002C36
                    cmp.w    #$150, d1
                    bpl      lbC002C36
                    move.w   (a0), d0
                    move.w   2(a0), d1
                    add.w    #13, d1
                    cmp.w    #ZOOL_GRIPPING, ZOOL_MOVE
                    beq      lbC002C12
                    move.w   #$45, d7
                    sub.w    #$30, d0
                    bsr      ADD_BACKSP
                    add.w    #$30, d0
                    addq.w   #4, d7
                    bra      ADD_BACKSP

lbC002C12:          addq.w   #8, d0
                    move.w   #$46, d7
                    cmp.w    ZOOL_X, d0
                    bpl      lbC002C24
                    subq.w   #2, d7
lbC002C24:          sub.w    #$38, d0
                    bsr      ADD_BACKSP
                    addq.w   #4, d7
                    add.w    #$30, d0
                    bra      ADD_BACKSP

lbC002C36:          addq.l   #4, a0
                    subq.w   #1, TEMPW
                    bne      lbC002BB4
                    rts

lbC002C44:          lea      PHAREYE_TAB2, a0
                    move.w   #5, TEMPW
                    bra      lbC002BB4

lbC002C56:          lea      PHAREYE_TAB3, a0
                    move.w   #2, TEMPW
                    bra      lbC002BB4

LAVA_SPITS:         subq.w   #1, SPARK_AN
                    bpl      lbC002CFE
                    clr.w    SPARK_AN
                    jsr      RANDOM
                    move.w   SEED, d7
                    and.w    #15, d7
                    move.w   MAP_LINE, d3
                    lsr.w    #4, d3
                    add.w    d7, d3
                    move.w   d3, d1
                    mulu     MAP_WIDTH, d3
                    move.w   XPOS, d0
                    move.b   SEED, d7
                    and.w    #$1F, d7
                    subq.w   #6, d7
                    lsr.w    #4, d0
                    add.w    d7, d0
                    add.w    d0, d3
                    move.l   REF_MAP, a0
                    lea      0(a0,d3.w), a0
                    cmp.b    #$1D, (a0)
                    bne      lbC002CFC
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    subq.w   #4, d1
                    sub.w    #9, d0
                    move.w   #$10, SPARK_AN
                    move.w   LAVA_EXPSPR, d2
                    bsr      NOTOK
                    subq.w   #7, d0
                    subq.w   #6, d1
                    move.w   d0, SPARKX
                    move.w   d1, SPARKY
                    lea      LAVA_FX, a5
                    jmp      ADD_SFX

lbC002CFC:          rts

lbC002CFE:          move.l   LAVA_TAB, a0
                    move.w   SPARK_AN, d7
                    move.b   0(a0,d7.w), d7
                    move.w   SPARKX, d0
                    move.w   SPARKY, d1
                    bra      ADD_BACKSP

GO_COLS:            move.w   #$200, PTILE_PTR
                    move.l   COLUMN_PTR, a0
                    move.w   COLUMNS, d7
lbC002D32:          addq.w   #4, PTILE_PTR
                    dbra     d7, lbC002D3E
                    rts

lbC002D3E:          tst.b    (a0)
                    beq      lbC002E20
                    move.w   12(a0), d0
                    beq      lbC002D7C
                    move.w   14(a0), d1
                    add.w    #$120, d0
                    sub.w    XPOS, d0
                    bmi      lbC002E20
                    cmp.w    #$380, d0
                    bpl      lbC002E20
                    add.w    #$40, d1
                    sub.w    MAP_LINE, d1
                    bmi      lbC002E20
                    cmp.w    #$180, d1
                    bpl      lbC002E20
lbC002D7C:          st       FEET_CHECK
                    move.l   4(a0), a1
                    add.l    REF_MAP, a1
                    move.w   10(a0), d0
                    move.w   d0, d6
                    and.w    #15, d6
                    lsr.w    #4, d0
                    mulu     MAP_WIDTH, d0
                    tst.b    1(a0)
                    bne      lbC002E36
                    sub.l    d0, a1
                    bsr      lbC002EB6
                    tst.b    d6
                    bne      lbC002E02
                    tst.w    8(a0)
                    beq      lbC002E02
                    move.l   CURRENT_MAP, a2
                    addq.l   #8, a2
                    move.l   a1, d0
                    sub.l    REF_MAP, d0
                    add.l    d0, a2
                    add.l    d0, a2
                    move.w   MAP_WIDTH, d1
                    lea      0(a1,d1.w), a1
                    move.b   #$1B, (a1)+
                    move.b   #$1B, (a1)+
                    move.b   #$1B, (a1)+
                    move.b   #$1B, (a1)+
                    lsl.w    #1, d1
                    lea      0(a2,d1.w), a2
                    move.w   LBEAM_TILE, d5
                    move.w   d5, (a2)+
                    addq.w   #1, d5
                    move.w   d5, (a2)+
                    addq.w   #1, d5
                    move.w   d5, (a2)+
                    addq.w   #1, d5
                    move.w   d5, (a2)+
lbC002E02:          addq.w   #1, 10(a0)
                    addq.w   #1, d6
                    btst     #4, d6
                    beq      lbC002E20
                    addq.w   #1, 8(a0)
                    move.w   2(a0), d0
                    cmp.w    8(a0), d0
                    beq      lbC002E28
lbC002E20:          lea      $10(a0), a0
                    bra      lbC002D32

lbC002E28:          st       1(a0)
                    subq.w   #1, 10(a0)
                    subq.w   #1, 8(a0)
                    bra.s    lbC002E20

lbC002E36:          sub.l    d0, a1
                    bsr      lbC002EB6
                    cmp.b    #15, d6
                    bne      lbC002E88
                    move.w   2(a0), d0
                    subq.w   #1, d0
                    cmp.w    8(a0), d0
                    beq      lbC002E88
                    move.l   CURRENT_MAP, a2
                    addq.l   #8, a2
                    move.l   a1, d0
                    sub.l    REF_MAP, d0
                    add.l    d0, a2
                    add.l    d0, a2
                    move.w   MAP_WIDTH, d1
                    neg.w    d1
                    lea      0(a1,d1.w), a1
                    sf       (a1)+
                    sf       (a1)+
                    sf       (a1)+
                    sf       (a1)+
                    asl.w    #1, d1
                    lea      0(a2,d1.w), a2
                    clr.w    (a2)+
                    clr.w    (a2)+
                    clr.w    (a2)+
                    clr.w    (a2)+
lbC002E88:          subq.w   #1, 10(a0)
                    subq.w   #1, d6
                    bpl.s    lbC002E20
                    subq.w   #1, 8(a0)
                    cmp.w    #1, 8(a0)
                    bpl.s    lbC002E20
                    sf       1(a0)
                    addq.w   #1, 10(a0)
                    addq.w   #1, 8(a0)
                    tst.w    14(a0)
                    bne      lbC002E20
                    sf       (a0)
                    bra      lbC002E20

lbC002EB6:          move.w   d7, -(sp)
                    move.l   a1, d0
                    sub.l    REF_MAP, d0
                    divu     MAP_WIDTH, d0
                    move.w   d0, d1
                    swap     d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    add.w    #15, d1
                    sub.w    d6, d1
                    sub.w    #$10, d0
                    tst.b    1(a0)
                    bne      lbC002EEC
                    move.w   #$25, d7
                    addq.w   #1, d1
                    bsr      ADD_PERM
                    subq.w   #1, d1
lbC002EEC:          move.w   #6, d7
                    bsr      ADD_BACKSP
                    tst.b    1(a0)
                    beq      lbC002F06
                    move.w   #$26, d7
                    subq.w   #1, d1
                    bsr      ADD_PERM
lbC002F06:          move.b   COLUMN_REF, d5
                    add.b    d6, d5
                    tst.b    1(a0)
                    beq      lbC002F24
                    subq.w   #1, d5
                    tst.b    d6
                    bne      lbC002F32
                    sf       d5
                    bra      lbC002F32

lbC002F24:          addq.w   #1, d5
                    cmp.b    #15, d6
                    bne      lbC002F32
                    move.b   #$1E, d5
lbC002F32:          move.b   d5, (a1)+
                    move.b   d5, (a1)+
                    move.b   d5, (a1)+
                    move.b   d5, (a1)+
                    subq.l   #4, a1
                    move.l   CURRENT_MAP, a2
                    move.l   a1, d0
                    sub.l    REF_MAP, d0
                    addq.l   #8, a2
                    add.l    d0, a2
                    add.l    d0, a2
                    moveq    #0, d5
                    move.w   PTILE_PTR, d5
                    move.w   d5, (a2)+
                    addq.w   #1, d5
                    move.w   d5, (a2)+
                    addq.w   #1, d5
                    move.w   d5, (a2)+
                    addq.w   #1, d5
                    move.w   d5, (a2)+
                    subq.w   #3, d5
                    lsl.l    #7, d5
                    lea      BOT_CHIP, a3
                    add.l    #$11900, a3
                    move.w   d6, d0
                    lsl.w    #3, d0
                    lea      8(a3,d0.w), a3
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC002F92
                    bra      lbC002FB4

lbC002F92:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC002F92
                    tst.b    $DFF002
lbC002FB4:          lea      $DFF000, a5
                    move.w   #0, $64(a5)
                    move.w   #0, $66(a5)
                    move.w   #0, $46(a5)
                    move.w   #$FFFF, $44(a5)
                    move.w   #0, $42(a5)
                    move.w   #$9F0, $40(a5)
                    move.l   TILES, a2
                    add.l    d5, a2
                    move.l   a3, $50(a5)
                    move.l   a2, $54(a5)
                    move.w   #$1001, $58(a5)
                    lea      $80(a2), a2
                    lea      $100(a3), a3
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC003010
                    bra      lbC003032

lbC003010:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC003010
                    tst.b    $DFF002
lbC003032:          move.l   a3, $50(a5)
                    move.l   a2, $54(a5)
                    move.w   #$1001, $58(a5)
                    lea      $80(a2), a2
                    lea      $100(a3), a3
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00305C
                    bra      lbC00307E

lbC00305C:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00305C
                    tst.b    $DFF002
lbC00307E:          move.l   a3, $50(a5)
                    move.l   a2, $54(a5)
                    move.w   #$1001, $58(a5)
                    lea      $80(a2), a2
                    lea      $100(a3), a3
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC0030A8
                    bra      lbC0030CA

lbC0030A8:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC0030A8
                    tst.b    $DFF002
lbC0030CA:          move.l   a3, $50(a5)
                    move.l   a2, $54(a5)
                    move.w   #$1001, $58(a5)
                    move.w   (sp)+, d7
                    rts

DRAW_MINIS:         rts

                    lea      MINI_TAB, a0
lbC0030E4:          tst.b    (a0)
                    beq      lbC003180
                    move.w   BACKFX_SPRS, d2
                    move.b   1(a0), lbB01F0F5
                    add.w    d2, SPRITE
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    move.w   6(a0), d2
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    addq.w   #4, d2
                    cmp.w    #$3C, d2
                    bgt      lbC00318E
                    move.w   d2, 6(a0)
                    move.l   a0, -(sp)
                    move.w   d2, -(sp)
                    lsr.w    #1, d2
                    add.w    d2, d0
                    sub.w    d2, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    jsr      DUMPSPRITE
                    addq.w   #1, SPRITE
                    move.w   (sp), d2
                    add.w    d2, YCOORD
                    jsr      DUMPSPRITE
                    addq.w   #1, SPRITE
                    move.w   (sp), d2
                    sub.w    d2, XCOORD
                    jsr      DUMPSPRITE
                    addq.w   #1, SPRITE
                    move.w   (sp), d2
                    sub.w    d2, YCOORD
                    jsr      DUMPSPRITE
                    move.w   (sp)+, d2
                    move.l   (sp)+, a0
lbC003180:          addq.l   #8, a0
                    cmp.l    #END_MINIS, a0
                    bne      lbC0030E4
                    rts

lbC00318E:          sf       (a0)
                    bra.s    lbC003180

ADD_HEART:          lea      HEART_TAB, a0
lbC003198:          tst.b    (a0)
                    beq      lbC0031AC
                    lea      10(a0), a0
                    cmp.l    #SMART_TAB, a0
                    bne.s    lbC003198
                    rts

lbC0031AC:          st       (a0)+
                    st       (a0)+
                    move.w   d0, (a0)+
                    move.w   d1, (a0)+
                    st       (a0)+
                    sf       (a0)+
                    tst.w    d7
                    beq      lbC0031D2
                    lea      POWER_TAB, a1
                    lsl.w    #1, d7
                    move.w   -2(a1,d7.w), (a0)
                    move.b   #$14, -2(a0)
                    rts

lbC0031D2:          clr.w    (a0)+
                    rts

DRAW_HEART:         lea      HEART_TAB, a0
lbC0031DC:          tst.b    (a0)
                    beq      lbC00332A
                    btst     #0, 7(a0)
                    beq      lbC003220
                    subq.w   #1, 4(a0)
                    tst.b    1(a0)
                    bmi      lbC00320E
                    addq.w   #1, 2(a0)
                    subq.b   #1, 1(a0)
                    bne      lbC003220
                    move.b   #$FC, 1(a0)
                    bra      lbC003220

lbC00320E:          subq.w   #1, 2(a0)
                    addq.b   #1, 1(a0)
                    bne      lbC003220
                    move.b   #4, 1(a0)
lbC003220:          move.w   2(a0), d0
                    sub.w    XPOS, d0
                    cmp.w    #$FFF0, d0
                    ble      lbC00333A
                    cmp.w    #$140, d0
                    bpl      lbC00333A
                    move.w   4(a0), d1
                    sub.w    MAP_LINE, d1
                    cmp.w    #$FFD0, d1
                    ble      lbC00333A
                    cmp.w    #$100, d1
                    bpl      lbC00333A
                    tst.b    6(a0)
                    bne      lbC0032D0
                    move.w   ZOOL_SCRX, d2
                    move.w   ZOOL_SCRY, d3
                    add.w    #$10, d2
                    sub.w    d0, d2
                    bmi      lbC0032D0
                    cmp.w    #$30, d2
                    bpl      lbC0032D0
                    add.w    #$10, d3
                    sub.w    d1, d3
                    bmi      lbC0032D0
                    cmp.w    #$30, d3
                    bpl      lbC0032D0
                    move.w   8(a0), d7
                    beq      lbC003376
                    lea      COLPOW_FX, a5
                    jsr      ADD_SFX
                    sub.w    #$6F, d7
                    beq      lbC003346
                    cmp.w    #2, d7
                    bmi      _ADD_SMART
                    beq      lbC003354
                    cmp.w    #4, d7
                    bmi      _INIT_SHADE
                    beq      lbC00336C
                    cmp.w    #6, d7
                    bmi      lbC003362
                    beq      lbC00333E
                    bra      lbC00333A

lbC0032D0:          move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    tst.w    8(a0)
                    beq      lbC0032FC
                    addq.b   #1, 7(a0)
                    move.w   8(a0), d7
                    tst.b    6(a0)
                    beq      lbC00331A
                    subq.b   #1, 6(a0)
                    bra      lbC00331A

lbC0032FC:          addq.b   #1, 7(a0)
                    lea      HEART_AN, a1
                    moveq    #0, d7
                    move.b   7(a0), d7
                    move.b   0(a1,d7.w), d7
                    bpl      lbC00331A
                    move.b   (a1), d7
                    clr.w    6(a0)
lbC00331A:          move.w   d7, SPRITE
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
lbC00332A:          lea      10(a0), a0
                    cmp.l    #SMART_TAB, a0
                    bne      lbC0031DC
                    rts

lbC00333A:          sf       (a0)
                    bra.s    lbC00332A

lbC00333E:          st       REVIVE
                    bra.s    lbC00333A

lbC003346:          addq.w   #1, LIVES
                    bra.s    lbC00333A

_ADD_SMART:         bsr      ADD_SMART
                    bra.s    lbC00333A

lbC003354:          st       BIGB_POW
                    bra.s    lbC00333A

_INIT_SHADE:        bsr      INIT_SHADE
                    bra.s    lbC00333A

lbC003362:          move.w   #650, SHIELD_ON
                    bra.s    lbC00333A

lbC00336C:          add.w    #50, TIME
                    bra.s    lbC00333A

lbC003376:          lea      EAT_FX, a5
                    jsr      ADD_SFX
                    addq.w   #1, ENERGY
                    cmp.w    #5, ENERGY
                    bne.s    lbC00333A
                    move.w   #4, ENERGY
                    add.l    #$64, SCORE
                    bra.s    lbC00333A

INIT_MINI:          rts

                    lea      MINI_TAB, a0
lbC0033AE:          tst.b    (a0)
                    beq      lbC0033C0
                    addq.l   #8, a0
                    cmp.l    #END_MINIS, a0
                    bne.s    lbC0033AE
                    rts

lbC0033C0:          st       (a0)+
                    move.b   d2, (a0)+
                    move.w   d0, (a0)+
                    move.w   d1, (a0)+
                    clr.w    (a0)+
                    rts

ADD_SMART:          lea      SMART_TAB, a5
                    move.w   SMARTS, d2
                    cmp.w    #3, SMARTS
                    beq      lbC00342A
                    lsl.w    #4, d2
                    lea      0(a5,d2.w), a5
                    addq.w   #1, SMARTS
                    st       (a5)+
                    sf       (a5)+
                    move.w   ZOOL_X, d0
                    move.w   SMARTS, d2
                    lsl.w    #5, d2
                    tst.b    ZOOL_FACE
                    beq      lbC003412
                    sub.w    d2, d0
                    bra      lbC003414

lbC003412:          add.w    d2, d0
lbC003414:          ext.l    d0
                    lsl.l    #4, d0
                    move.l   d0, (a5)+
                    move.w   ZOOL_Y, d0
                    ext.l    d0
                    lsl.l    #4, d0
                    move.l   d0, (a5)+
                    clr.l    (a5)+
                    move.w   d2, (a5)+
lbC00342A:          rts

DEL_SMART:          move.w   SMARTS, d2
                    beq      NO_SMART
                    btst     #2, CHEAT+1
                    beq      lbC003454
                    cmp.w    #1, d2
                    bne      lbC003454
                    st       SMART_BOMB
                    bra      lbC003482

lbC003454:          lea      SMART_TAB, a5
                    subq.w   #1, d2
                    lsl.w    #4, d2
                    sf       0(a5,d2.w)
                    st       SMART_BOMB
                    subq.w   #1, SMARTS
                    move.l   2(a5,d2.w), d0
                    move.l   6(a5,d2.w), d1
                    asr.l    #4, d0
                    asr.l    #4, d1
                    clr.w    d2
                    jsr      NOTOK
lbC003482:          lea      WHITE_PAL, a0
                    jsr      NEWPALET
                    lea      SMART_FX, a5
                    jmp      ADD_SFX

NO_SMART:           rts

DRAW_SMARTS:        move.w   SMARTS, d2
                    beq      lbC0035C0
                    subq.w   #1, d2
                    lea      SMART_TAB, a5
lbC0034AE:          move.l   2(a5), d0
                    asr.l    #4, d0
                    move.w   ZOOL_X, d1
                    sub.w    #$18, d1
                    tst.b    ZOOL_FACE
                    beq      lbC0034D0
                    sub.w    14(a5), d1
                    bra      lbC0034D4

lbC0034D0:          add.w    14(a5), d1
lbC0034D4:          move.w   X_FORCE, d3
                    bpl      lbC0034E0
                    neg.w    d3
lbC0034E0:          add.w    #$20, d3
                    cmp.w    d1, d0
                    bpl      lbC0034FE
                    addq.w   #8, 10(a5)
                    cmp.w    10(a5), d3
                    bgt      lbC003510
                    move.w   d3, 10(a5)
                    bra      lbC003510

lbC0034FE:          neg.w    d3
                    subq.w   #8, 10(a5)
                    cmp.w    10(a5), d3
                    bmi      lbC003510
                    move.w   d3, 10(a5)
lbC003510:          move.w   #$60, d3
                    tst.w    ZOOL_MOVE
                    bne      lbC003522
                    move.w   #$20, d3
lbC003522:          move.l   6(a5), d0
                    asr.l    #4, d0
                    cmp.w    ZOOL_Y, d0
                    bpl      lbC003546
                    addq.w   #6, 12(a5)
                    cmp.w    12(a5), d3
                    bgt      lbC003558
                    move.w   d3, 12(a5)
                    bra      lbC003558

lbC003546:          neg.w    d3
                    subq.w   #6, 12(a5)
                    cmp.w    12(a5), d3
                    bmi      lbC003558
                    move.w   d3, 12(a5)
lbC003558:          move.w   10(a5), d0
                    ext.l    d0
                    add.l    2(a5), d0
                    move.l   d0, 2(a5)
                    asr.l    #4, d0
                    move.w   12(a5), d1
                    ext.l    d1
                    add.l    6(a5), d1
                    move.l   d1, 6(a5)
                    asr.l    #4, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   #$79, SPRITE
                    move.l   a5, -(sp)
                    move.w   d2, -(sp)
                    tst.b    1(a5)
                    beq      lbC0035C2
                    subq.b   #1, 1(a5)
                    addq.w   #1, SPRITE
_DUMPSPRITE:        jsr      DUMPSPRITE
                    move.w   (sp)+, d2
                    move.l   (sp)+, a5
                    lea      $10(a5), a5
                    dbra     d2, lbC0034AE
lbC0035C0:          rts

lbC0035C2:          move.b   SEED+1, d7
                    move.w   $DFF006, d6
                    eor.b    d6, d7
                    eor.b    d7, SEED+1
                    and.b    #$7F, d7
                    cmp.b    #5, d7
                    bpl.s    _DUMPSPRITE
                    move.b   #4, 1(a5)
                    bra.s    _DUMPSPRITE

ARCH2_EXPL:         cmp.w    #5, TOKENS_ON
                    bgt      NOTOK
                    bra      GOTOK

ARCH_EXPL:          tst.w    TOKENS_ON
                    bne      NOTOK
GOTOK:              bchg     #5, SEED+3
                    beq      TOKEN_EXPL
NOTOK:              lea      ARCHEX_TAB, a0
lbC003614:          tst.b    (a0)
                    beq      lbC003648
                    lea      $1A(a0), a0
                    cmp.l    #DEL_ARCH, a0
                    bne.s    lbC003614
                    move.w   d0, -(sp)
                    subq.w   #1, DEL_ARCH
                    bgt      lbC00363A
                    move.w   #3, DEL_ARCH
lbC00363A:          move.w   DEL_ARCH, d0
                    mulu     #$1A, d0
                    sub.l    d0, a0
                    move.w   (sp)+, d0
lbC003648:          st       (a0)+
                    move.b   d2, (a0)+
                    move.w   d0, (a0)+
                    move.w   d1, (a0)+
                    move.w   #$FFA8, (a0)+
                    clr.w    (a0)+
                    move.w   #$FFB0, (a0)+
                    clr.w    (a0)+
                    move.w   #$FFFF, (a0)+
                    clr.w    (a0)+
                    move.w   #$FFC0, (a0)+
                    clr.w    (a0)+
                    move.w   #$FFFE, (a0)+
                    clr.w    (a0)+
                    rts

DRAW_ARCHEX:        lea      ARCHEX_TAB, a0
                    move.b   ANDYFRAME, TEMPW
lbC003680:          tst.b    (a0)+
                    beq      lbC003796
                    moveq    #0, d7
                    move.b   (a0)+, d7
                    add.w    ARCHEX_SPR, d7
                    move.w   d7, SPRITE
                    move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, HOLDX
                    move.w   d1, HOLDY
                    move.w   (a0), d2
                    asr.w    #4, d2
                    addq.w   #5, (a0)+
                    add.w    d2, (a0)
                    add.w    (a0)+, d1
                    cmp.w    #$100, d1
                    bpl      lbC0037A6
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
                    move.w   HOLDX, d0
                    move.w   HOLDY, d1
                    move.w   (a0), d2
                    asr.w    #4, d2
                    addq.w   #5, (a0)+
                    add.w    d2, (a0)
                    add.w    (a0)+, d1
                    move.w   (a0)+, d2
                    add.w    d2, (a0)
                    add.w    (a0)+, d0
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
                    move.w   HOLDX, d0
                    move.w   HOLDY, d1
                    add.w    -6(a0), d1
                    sub.w    -2(a0), d0
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
                    move.w   HOLDX, d0
                    move.w   HOLDY, d1
                    move.w   (a0), d2
                    asr.w    #4, d2
                    addq.w   #5, (a0)+
                    add.w    d2, (a0)
                    add.w    (a0)+, d1
                    move.w   (a0)+, d2
                    add.w    d2, (a0)
                    add.w    (a0)+, d0
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
                    move.w   HOLDX, d0
                    move.w   HOLDY, d1
                    add.w    -6(a0), d1
                    sub.w    -2(a0), d0
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
                    bra      lbC00379A

lbC003796:          lea      $19(a0), a0
lbC00379A:          cmp.l    #DEL_ARCH, a0
                    bne      lbC003680
                    rts

lbC0037A6:          lea      -10(a0), a0
                    sf       (a0)+
                    bra.s    lbC003796

ADD_TOKEN:          lea      TOKEN_TAB, a0
lbC0037B4:          tst.b    (a0)
                    beq      lbC0037C8
                    lea      10(a0), a0
                    cmp.l    #TOKENS_ON, a0
                    bne.s    lbC0037B4
                    rts

lbC0037C8:          move.b   #1, (a0)+
                    add.w    TOKEN_SPR, d7
                    move.b   d7, (a0)+
                    move.w   d0, (a0)+
                    move.w   d1, (a0)+
                    move.w   d2, (a0)+
                    move.w   d3, (a0)+
                    addq.w   #1, TOKENS_ON
                    rts

TOKEN_EXPL:         move.w   d0, -(sp)
                    move.w   d1, -(sp)
                    jsr      RANDOM
                    move.w   (sp)+, d1
                    move.w   (sp)+, d0
                    move.l   SEED, d6
                    move.b   d6, d7
                    and.b    #3, d7
                    move.w   #$FFFE, d2
                    move.w   #$FFC0, d3
                    bsr.s    ADD_TOKEN
                    ror.l    #3, d6
                    move.b   d6, d7
                    and.b    #3, d7
                    move.w   #2, d2
                    move.w   #$FFC0, d3
                    bsr.s    ADD_TOKEN
                    ror.l    #3, d6
                    move.b   d6, d7
                    and.b    #3, d7
                    move.w   #$FFFF, d2
                    move.w   #$FFB0, d3
                    bsr.s    ADD_TOKEN
                    ror.l    #3, d6
                    move.b   d6, d7
                    and.b    #3, d7
                    move.w   #1, d2
                    move.w   #$FFB0, d3
                    bsr      ADD_TOKEN
                    ror.l    #3, d6
                    move.b   d6, d7
                    and.b    #3, d7
                    move.w   #0, d2
                    move.w   #$FFA8, d3
                    bra      ADD_TOKEN

PRO_TOKENS:         tst.w    TOKENS_ON
                    beq      lbC0038E4
                    lea      TOKEN_TAB, a5
lbC003864:          tst.b    (a5)
                    beq      lbC0038D8
                    btst     #2, (a5)
                    bne      _PICKUP_CHK
                    btst     #1, (a5)
                    beq      lbC003884
                    bsr      PICKUP_CHK
                    tst.b    (a5)
                    beq      lbC0038D8
lbC003884:          addq.l   #2, a5
                    move.w   (a5)+, d0
                    move.w   (a5)+, d1
                    move.w   (a5)+, d2
                    move.w   (a5), d3
                    addq.w   #5, (a5)
                    add.w    d2, d0
                    asr.w    #4, d3
                    add.w    d3, d1
                    cmp.w    #$40, (a5)
                    bmi      lbC0038CE
                    bset     #1, -8(a5)
                    cmp.w    #$60, (a5)
                    bmi      lbC0038B0
                    move.w   #$60, (a5)
lbC0038B0:          add.w    #$10, d1
                    add.w    #$10, d0
                    bsr      TOK_LANDCHK
                    sub.w    #$10, d0
                    sub.w    #$10, d1
                    tst.b    GROUNDED
                    bne      lbC0038E6
lbC0038CE:          move.w   d0, -6(a5)
                    move.w   d1, -4(a5)
                    subq.l   #8, a5
lbC0038D8:          lea      10(a5), a5
                    cmp.l    #TOKENS_ON, a5
                    bne.s    lbC003864
lbC0038E4:          rts

lbC0038E6:          bset     #2, -8(a5)
                    move.l   a5, -(sp)
                    lea      LANDTOK_FX, a5
                    jsr      ADD_SFX
                    move.l   (sp)+, a5
                    bra.s    lbC0038CE

_PICKUP_CHK:        bsr      PICKUP_CHK
                    bra.s    lbC0038D8

TOK_LANDCHK:        bsr      CHECK_TILE2
                    tst.b    CUSTOM_ON
                    bne      lbC003994
                    cmp.b    #3, d7
                    beq      lbC0039A0
                    tst.b    d7
                    beq      lbC00398C
                    cmp.b    #$1C, d7
                    beq      lbC00398C
                    cmp.b    #$23, d7
                    beq      lbC00398C
                    cmp.b    #4, d7
                    beq      lbC00398C
                    cmp.b    #10, d7
                    beq      lbC00398C
                    cmp.b    #$16, d7
                    beq      lbC00398C
                    cmp.b    #$47, d7
                    bmi      lbC003958
                    cmp.b    #$4D, d7
                    ble      lbC00398C
lbC003958:          cmp.b    #11, d7
                    beq      lbC00398C
                    cmp.b    #$16, d7
                    beq      lbC00398C
                    cmp.b    #12, d7
                    beq      lbC00398C
                    cmp.b    #$12, d7
                    beq      lbC00398C
                    cmp.b    #$13, d7
                    beq      lbC00398C
lbC003980:          and.w    #$FFF0, d1
                    move.b   d7, GROUNDED
                    rts

lbC00398C:          sf       GROUNDED
                    rts

lbC003994:          st       GROUNDED
                    ext.w    d7
                    add.w    d7, d1
                    rts

lbC0039A0:          move.w   MAP_WIDTH, d7
                    neg.w    d7
                    move.b   0(a0,d7.w), d7
                    bpl.s    lbC003980
                    move.l   CUST_REFS, a1
                    and.w    #$7F, d7
                    move.w   d7, CUST_TILE
                    lsl.w    #4, d7
                    lea      0(a1,d7.w), a1
                    move.b   0(a1,d2.w), d2
                    moveq    #15, d7
                    sub.b    d2, d7
                    sub.b    d3, d7
                    bra.s    lbC003994

PICKUP_CHK:         move.w   2(a5), d0
                    move.w   4(a5), d1
                    move.w   ZOOL_X, d2
                    move.w   ZOOL_Y, d3
                    addq.w   #8, d3
                    sub.w    d0, d2
                    bmi      lbC003A44
                    cmp.w    #$30, d2
                    bpl      lbC003A44
                    sub.w    d1, d3
                    bmi      lbC003A44
                    cmp.w    #$20, d3
                    bpl      lbC003A44
                    sf       (a5)
                    addq.w   #1, TOKENS
                    move.w   PERC_ADD, d7
                    add.w    d7, PERCENT
                    add.l    #10, SCORE
                    subq.w   #1, TOKENS_ON
                    clr.w    d2
                    move.b   #1, d3
                    move.w   #$300, d7
                    bsr      ADD_FXPIX
                    move.l   a5, -(sp)
                    lea      COLLECT_FX, a5
                    jsr      ADD_SFX
                    move.l   (sp)+, a5
lbC003A44:          rts

DRAW_TOKENS:        tst.w    TOKENS_ON
                    beq      lbC003ABE
                    lea      TOKEN_TAB, a5
lbC003A56:          tst.b    (a5)
                    beq      lbC003AB2
                    moveq    #0, d7
                    move.b   1(a5), d7
                    move.w   d7, SPRITE
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    cmp.w    #-48, d1
                    bmi      lbC003AC0
                    cmp.w    #$130, d1
                    bpl      lbC003AC0
                    cmp.w    #-48, d0
                    bmi      lbC003AC0
                    cmp.w    #$170, d0
                    bpl      lbC003AC0
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.l   a5, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a5
lbC003AB2:          lea      10(a5), a5
                    cmp.l    #TOKENS_ON, a5
                    bne.s    lbC003A56
lbC003ABE:          rts

lbC003AC0:          sf       (a5)
                    subq.w   #1, TOKENS_ON
                    bra.s    lbC003AB2

ADD_BACKSP:         lea      BACKSP_TAB, a6
lbC003BD6:          tst.w    (a6)
                    beq      lbC003BE8
                    addq.l   #8, a6
                    cmp.l    #BACKSP_LNS, a6
                    bne.s    lbC003BD6
                    rts

lbC003BE8:          move.b   #2, (a6)+
                    move.b   BACKSP_LNS, (a6)+
                    move.w   d0, (a6)+
                    move.w   d1, (a6)+
                    move.w   d7, (a6)+
                    sf       BACKSP_LNS
                    rts

DRAW_ENDIC:         tst.b    ENDICON_ON
                    beq      lbC003D04
                    move.w   ENDICON_X, d0
                    move.w   ENDICON_Y, d1
                    cmp.b    #99, PERCENT
                    bne      lbC003CA8
                    move.w   ZOOL_X, d2
                    sub.w    d0, d2
                    bmi      lbC003CA8
                    cmp.w    #$3C, d2
                    bpl      lbC003CA8
                    move.w   ZOOL_Y, d3
                    addq.w   #4, d3
                    sub.w    d1, d3
                    bmi      lbC003CA8
                    cmp.w    #$28, d3
                    bpl      lbC003CA8
                    move.w   #$FA, END_OF_STG
                    sf       ENDICON_ON
                    st       SCROLL_OFF
                    clr.w    SPR_CNT
                    clr.w    FIXED_SPRS
                    bsr      lbC003C88
                    add.w    #$10, d0
                    bsr      lbC003C88
                    add.w    #$10, d1
                    bsr      lbC003C88
                    sub.w    #$10, d0
                    bra      lbC003C88

lbC003C88:          clr.w    d2
                    move.b   #1, d3
                    move.w   #$300, d7
                    bsr      ADD_FXPIX
                    lea      COLPOW_FX, a5
                    jsr      ADD_SFX
                    jmp      DEL_SMART

lbC003CA8:          sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   #$6E, SPRITE
                    jsr      DUMPSPRITE
                    move.w   ENDIC_YDIS, d1
                    asr.w    #4, d1
                    add.w    d1, ENDICON_Y
                    addq.w   #6, ENDIC_YDIS
                    move.w   ENDICON_Y, d1
                    cmp.w    ENDIC_FLR, d1
                    ble      lbC003D04
                    move.w   ENDIC_FLR, ENDICON_Y
                    move.w   #$FF80, ENDIC_YDIS
lbC003D04:          rts

SKIP_STG:           move.w   #$FFFF, END_OF_STG
                    sf       FADECOL_ON
                    move.w   #$20, FADE_CNT
                    sf       ENDICON_ON
                    rts

EXTRA_TOKS:         tst.w    END_OF_STG
                    ble      EXRET
                    subq.w   #1, END_OF_STG
                    bne      EXCONT
                    move.w   #$FFFF, END_OF_STG
                    sf       FADECOL_ON
                    move.w   #$20, FADE_CNT
EXCONT:             cmp.b    #6, LEVEL_NUM
                    beq      EXRET
                    bsr      RANDOM
                    move.l   SEED, d7
                    btst     #2, d7
                    beq      EXRET
                    btst     #4, d7
                    bne      EXRET
                    btst     #15, d7
                    beq      EXRET
                    btst     #$19, d7
                    beq      EXRET
                    and.w    #$FF, d7
                    move.w   XPOS, d0
                    add.w    #$20, d0
                    add.w    d7, d0
                    swap     d7
                    move.w   MAP_LINE, d1
                    and.w    #$3F, d7
                    add.w    #$30, d1
                    add.w    d7, d1
                    and.w    #3, d7
                    move.w   d7, d2
                    bsr      ARCH2_EXPL
                    lea      BREAK_FX, a5
                    jmp      ADD_SFX

EXRET:              rts

DRAW_BACKSP:        bsr      DRAW_ENDIC
                    lea      BACKSP_TAB, a6
lbC003DC8:          tst.b    (a6)
                    bne      lbC003DDA
lbC003DCE:          addq.l   #8, a6
                    cmp.l    #BACKSP_LNS, a6
                    bne.s    lbC003DC8
                    rts

lbC003DDA:          sf       (a6)
                    move.w   2(a6), d0
                    move.w   4(a6), d1
                    move.w   BACKFX_SPRS, d2
                    add.w    6(a6), d2
                    move.w   d2, SPRITE
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.l   a6, -(sp)
                    jsr      DUMPSPRITE
                    sf       lbB01F0F7
                    move.l   (sp)+, a6
                    bra.s    lbC003DCE

ADD_FTILE:          lea      END_MINIS, a6
lbC003E24:          tst.w    (a6)
                    beq      lbC003E36
                    addq.l   #8, a6
                    cmp.l    #PERM_TAB, a6
                    bne.s    lbC003E24
                    rts

lbC003E36:          move.w   #2, (a6)+
                    move.w   d0, (a6)+
                    move.w   d1, (a6)+
                    move.w   d7, (a6)+
                    rts

DRAW_FTILE:         lea      END_MINIS, a6
lbC003E48:          tst.w    (a6)
                    bne      lbC003E5A
lbC003E4E:          addq.l   #8, a6
                    cmp.l    #PERM_TAB, a6
                    bne.s    lbC003E48
                    rts

lbC003E5A:          move.w   2(a6), d0
                    move.w   4(a6), d1
                    move.w   6(a6), d7
                    sub.w    XPOS, d0
                    add.w    #$10, d0
                    bmi      lbC003F2C
                    cmp.w    #$150, d0
                    bpl      lbC003F2C
                    sub.w    MAP_LINE, d1
                    cmp.w    #$110, d1
                    bpl      lbC003F2C
                    cmp.w    #$FFF0, d1
                    ble      lbC003F2C
                    muls     #$C0, d1
                    lsr.w    #3, d0
                    add.l    d0, d1
                    tst.b    ANDYFRAME
                    beq      lbC003EEA
                    subq.w   #1, (a6)
                    beq      lbC003EDE
                    move.l   BUFF_PTR, -(sp)
                    move.l   SCROLL, -(sp)
                    move.l   BUFF_PTR_C, BUFF_PTR
                    move.l   SCROLLC, SCROLL
                    move.l   a6, -(sp)
                    bsr      DUMP_FTILE
                    move.l   (sp)+, a6
                    move.l   (sp)+, SCROLL
                    move.l   (sp)+, BUFF_PTR
lbC003EDE:          move.l   a6, -(sp)
                    bsr      DUMP_FTILE
                    move.l   (sp)+, a6
                    bra      lbC003E4E

lbC003EEA:          subq.w   #1, (a6)
                    beq      lbC003F24
                    move.l   BUFF_PTR, -(sp)
                    move.l   SCROLL, -(sp)
                    move.l   BUFF_PTR_C, BUFF_PTR
                    move.l   SCROLLC, SCROLL
                    move.l   a6, -(sp)
                    bsr      DUMP_FTILE
                    move.l   (sp)+, a6
                    move.l   (sp)+, SCROLL
                    move.l   (sp)+, BUFF_PTR
lbC003F24:          move.l   a6, -(sp)
                    bsr      DUMP_FTILE
                    move.l   (sp)+, a6
lbC003F2C:          bra      lbC003E4E

DUMP_FTILE:         lea      $DFF000, a5
                    move.l   BUFF_PTR, a1
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC003F50
                    bra      lbC003F72

lbC003F50:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC003F50
                    tst.b    $DFF002
lbC003F72:          move.w   #0, $64(a5)
                    move.w   #$2E, $66(a5)
                    move.w   #$FFFF, $46(a5)
                    move.w   #$FFFF, $44(a5)
                    move.w   #0, $42(a5)
                    move.w   #$9F0, $40(a5)
                    lsl.w    #7, d7
                    move.l   TILES, a2
                    lea      0(a2,d7.w), a2
                    move.l   a2, $50(a5)
                    move.l   a1, $54(a5)
                    move.w   #$1001, $58(a5)
                    rts

ADD_PERM:           lea      PERM_TAB, a6
lbC003FB8:          tst.w    (a6)
                    beq      lbC003FCA
                    addq.l   #8, a6
                    cmp.l    #END_PERMS, a6
                    bne.s    lbC003FB8
                    rts

lbC003FCA:          move.w   #2, (a6)+
                    move.w   d0, (a6)+
                    move.w   d1, (a6)+
                    move.w   d7, (a6)+
                    rts

DRAW_PERMS:         lea      PERM_TAB, a6
                    st       P_DRAW
lbC003FE2:          tst.w    (a6)
                    bne      lbC003FFA
lbC003FE8:          addq.l   #8, a6
                    cmp.l    #END_PERMS, a6
                    bne.s    lbC003FE2
                    sf       P_DRAW
                    rts

lbC003FFA:          move.w   2(a6), d0
                    move.w   4(a6), d1
                    move.w   BACKFX_SPRS, d2
                    add.w    6(a6), d2
                    move.w   d2, SPRITE
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    tst.b    ANDYFRAME
                    beq      lbC00407E
                    subq.w   #1, (a6)
                    beq      lbC004070
                    move.l   BUFF_PTR, -(sp)
                    move.l   SCROLL, -(sp)
                    move.l   BUFF_PTR_C, BUFF_PTR
                    move.l   SCROLLC, SCROLL
                    move.l   a6, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a6
                    move.l   (sp)+, SCROLL
                    move.l   (sp)+, BUFF_PTR
lbC004070:          move.l   a6, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a6
                    bra      lbC003FE8

lbC00407E:          subq.w   #1, (a6)
                    beq      lbC0040BA
                    move.l   BUFF_PTR, -(sp)
                    move.l   SCROLL, -(sp)
                    move.l   BUFF_PTR_C, BUFF_PTR
                    move.l   SCROLLC, SCROLL
                    move.l   a6, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a6
                    move.l   (sp)+, SCROLL
                    move.l   (sp)+, BUFF_PTR
lbC0040BA:          move.l   a6, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a6
                    bra      lbC003FE8

PRO_TILEFX:         lea      TILEFX_TAB, a0
lbC0040CE:          tst.b    (a0)
                    bne      lbC0040E8
lbC0040D4:          lea      14(a0), a0
                    cmp.l    #END_TILEFX, a0
                    bne.s    lbC0040CE
                    sf       AFTER_EXP
                    rts

lbC0040E8:          tst.b    1(a0)
                    beq      lbC0040FA
                    subq.b   #1, 1(a0)
                    bpl.s    lbC0040D4
                    sf       1(a0)
lbC0040FA:          lea      TILEFX_ANIM, a1
                    cmp.b    #3, (a0)
                    bne      lbC00410C
                    subq.w   #2, 4(a0)
lbC00410C:          moveq    #0, d7
                    move.b   8(a0), d7
                    lsl.w    #2, d7
                    move.l   0(a1,d7.w), a1
                    addq.b   #1, 9(a0)
                    move.b   9(a0), d7
                    lsl.w    #1, d7
                    move.w   0(a1,d7.w), d7
                    bmi      lbC00413A
                    cmp.w    #$64, d7
                    bne      lbC004134
                    neg.w    d7
lbC004134:          move.w   d7, 10(a0)
                    bra.s    lbC0040D4

lbC00413A:          cmp.b    #2, (a0)
                    beq      lbC004162
                    cmp.b    #3, (a0)
                    beq      lbC00415C
                    cmp.b    #4, (a0)
                    beq      lbC00415C
                    sf       (a0)
                    bsr      lbC0041CA
                    bra      lbC0040D4

lbC00415C:          sf       (a0)
                    bra      lbC0040D4

lbC004162:          move.b   7(a0), d5
                    bne      lbC004220
                    bsr      lbC0041CA
                    lea      POP_FX, a5
                    jsr      ADD_SFX
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    neg.w    d7
                    cmp.w    #6, d7
                    bpl      lbC0041A4
                    move.w   d7, d2
                    subq.w   #1, d2
                    move.l   a0, -(sp)
                    and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
                    bsr      NOTOK
                    move.l   (sp)+, a0
                    bra      lbC0041BC

lbC0041A4:          move.w   d7, d2
                    subq.w   #5, d2
                    bra      lbC0041BC

                    move.l   a0, -(sp)
                    and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
                    bsr      INIT_MINI
                    move.l   (sp)+, a0
lbC0041BC:          move.w   12(a0), d7
                    bsr      ADD_PERM
                    sf       (a0)
                    bra      lbC0040D4

lbC0041CA:          move.l   CURRENT_MAP, a6
                    addq.l   #8, a6
                    move.l   REF_MAP, a5
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    lsr.w    #4, d0
                    lsr.w    #4, d1
                    mulu     MAP_WIDTH, d1
                    addq.w   #1, d0
                    add.w    d0, d1
                    move.b   AFTER_EXP, 0(a5,d1.w)
                    lsl.l    #1, d1
                    add.l    d1, a6
                    btst     #1, 6(a0)
                    beq      lbC004212
                    move.w   FILL_TILE2, (a6)
                    st       FEET_CHECK
                    rts

lbC004212:          move.w   FILL_TILE1, (a6)
                    st       FEET_CHECK
                    rts

lbC004220:          lea      BREAK_FX, a5
                    jsr      ADD_SFX
                    tst.b    6(a0)
                    beq      lbC00424C
lbC004234:          move.w   d7, -(sp)
                    bsr      lbC004264
                    move.w   (sp)+, d7
                    add.w    #$10, 4(a0)
                    subq.b   #1, d5
                    bpl.s    lbC004234
                    sf       (a0)
                    bra      lbC0040D4

lbC00424C:          move.w   d7, -(sp)
                    bsr      lbC004264
                    move.w   (sp)+, d7
                    add.w    #$10, 2(a0)
                    subq.b   #1, d5
                    bpl.s    lbC00424C
                    sf       (a0)
                    bra      lbC0040D4

lbC004264:          bsr      lbC0041CA
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    cmp.b    #1, d5
                    bne      lbC0042CC
                    neg.w    d7
                    cmp.w    #6, d7
                    bpl      lbC0042B4
                    move.w   d7, d2
                    subq.w   #1, d2
                    move.l   a0, -(sp)
                    and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
                    addq.w   #8, d1
                    bsr      NOTOK
                    subq.w   #8, d1
                    move.w   #$400, d7
                    clr.w    d2
                    move.w   SPLAT_ANIM, d3
                    move.w   FILLTILE_SPR, d4
                    bsr      ADD_FXPIX
                    move.l   (sp)+, a0
                    bra      lbC0042CC

lbC0042B4:          move.w   d7, d2
                    subq.w   #5, d2
                    bra      lbC0042CC

                    move.l   a0, -(sp)
                    and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
                    bsr      INIT_MINI
                    move.l   (sp)+, a0
lbC0042CC:          move.w   12(a0), d7
                    bra      ADD_PERM

ADD_TILEFX:         and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
ADD_FXPIX:          lea      TILEFX_TAB, a0
lbC0042E2:          tst.b    (a0)
                    beq      lbC0042F6
                    lea      14(a0), a0
                    cmp.l    #END_TILEFX, a0
                    bne.s    lbC0042E2
                    rts

lbC0042F6:          move.w   d7, (a0)+
                    move.w   d0, (a0)+
                    move.w   d1, (a0)+
                    move.w   d2, (a0)+
                    move.b   d3, (a0)+
                    sf       (a0)+
                    and.w    #$FF, d3
                    lea      TILEFX_ANIM, a1
                    lsl.w    #2, d3
                    move.l   0(a1,d3.w), a1
                    move.w   (a1)+, (a0)+
                    move.w   d4, (a0)+
                    rts

DRAW_TILEFX:        lea      TILEFX_TAB, a0
lbC00431E:          move.l   a0, -(sp)
                    tst.b    (a0)
                    beq      lbC004374
                    tst.b    1(a0)
                    bne      lbC004374
                    move.w   2(a0), d0
                    sub.w    XPOS, d0
                    move.w   4(a0), d1
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   10(a0), SPRITE
                    bmi      lbC004374
                    move.w   BACKFX_SPRS, d7
                    add.w    d7, SPRITE
                    tst.b    7(a0)
                    bne      lbC004384
                    jsr      DUMPSPRITE
lbC004374:          move.l   (sp)+, a0
                    lea      14(a0), a0
                    cmp.l    #END_TILEFX, a0
                    bne.s    lbC00431E
                    rts

lbC004384:          btst     #0, 6(a0)
                    beq      lbC0043AE
                    move.w   6(a0), d7
                    and.w    #$FF, d7
lbC004396:          move.w   d7, -(sp)
                    jsr      DUMPSPRITE
                    move.w   (sp)+, d7
                    subq.b   #1, d7
                    bmi.s    lbC004374
                    add.w    #$10, YCOORD
                    bra.s    lbC004396

lbC0043AE:          move.w   6(a0), d7
                    and.w    #$FF, d7
lbC0043B6:          move.w   d7, -(sp)
                    jsr      DUMPSPRITE
                    move.w   (sp)+, d7
                    subq.b   #1, d7
                    bmi.s    lbC004374
                    add.w    #$10, XCOORD
                    bra.s    lbC0043B6

GET_HEIGHT:         lea      SPR_TAB, a0
                    move.w   ZOOL_SPR, d7
                    lsl.w    #2, d7
                    move.l   0(a0,d7.w), a0
                    add.l    SPRITE_AD, a0
                    move.w   (a0), ZOOL_HGT
                    rts

PRO_ZOOL:           tst.b    SMART_BOMB
                    ble      lbC004408
                    subq.b   #1, SMART_BOMB
                    bne      lbC004408
                    jsr      LEVPALSET
lbC004408:          tst.b    MAINGUY_ON
                    beq      lbC004480
                    tst.b    SCROLL_OFF
                    beq      _PRO_POWER
                    bsr      AUTO_ZOOL
_PRO_POWER:         bsr      PRO_POWER
                    bsr.s    GET_HEIGHT
                    tst.b    COL_MODE
                    bne      COLOUR_CHG
                    bsr      KEY_CHECK
                    btst     #6, CHEAT+1
                    bne      lbC004450
                    tst.b    ZOOL_HIT
                    beq      lbC004450
                    subq.b   #1, ZOOL_HIT
lbC004450:          tst.w    CANNOT_KILL
                    beq      lbC004460
                    subq.w   #1, CANNOT_KILL
lbC004460:          lea      ZOOL_ACTIONS, a0
                    move.w   ZOOL_MOVE, d0
                    beq      lbC004478
                    move.w   #$28, BLINK_WAIT
lbC004478:          lsl.w    #2, d0
                    move.l   0(a0,d0.w), a0
                    jmp      (a0)

lbC004480:          rts

DIE_RTN:            tst.b    ZOOL_DIES
                    bpl      CONTIN
                    sf       MAINGUY_ON
                    tst.b    SCROLL_OFF
                    bne      NOFADE1
GAMEOV:             sf       FADECOL_ON
                    move.w   #$20, FADE_CNT
                    neg.b    ZOOL_DIES
                    rts

CONTIN:             tst.w    FADE_CNT
                    bne      RETURND
                    tst.w    FADECOP_CNT
                    bne      RETURND
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    move.l   #COPPER_GAME, COP_LOC
                    move.l   #$FFFFFFFE, WRITE_COP
                    jsr      VBL
                    jsr      CLRPALET
                    move.w   #$A0, $DFF096
                    move.l   #LAST_ICON, $DFF120
                    move.l   #LAST_ICON, $DFF124
                    move.l   #LAST_ICON, $DFF128
                    move.l   #LAST_ICON, $DFF12C
                    move.l   #LAST_ICON, $DFF130
                    move.l   #LAST_ICON, $DFF134
                    move.l   #LAST_ICON, $DFF138
                    move.l   #LAST_ICON, $DFF13C
                    jsr      RESET_COP
                    jsr      INIT_SCRNS
                    move.l   PHYSADR, a0
                    jsr      CLR_SCRN
                    jsr      INIT_SCRO
                    jsr      INIT_ZOOL
                    tst.w    LIVES
                    bpl      lbC004586
                    bsr      GAMEOVER
                    tst.b    PLAYERS
                    beq      STOPGAME
lbC004586:          lea      RESTART_TAB, a0
                    move.w   (a0)+, STARTX
                    move.w   (a0)+, STARTY
                    move.w   (a0)+, ZOOL_SCRX
                    move.w   (a0)+, ZOOL_SCRY
                    move.w   (a0)+, TIME
                    move.b   (a0)+, ZOOL_FACE
                    beq      lbC0045BC
                    move.w   #$31, ZOOL_SPR
lbC0045BC:          tst.b    PLAYERS
                    beq      ONEPLAYER
OFF_MUS:            tst.b    MUSIC_ON
                    beq      lbC0045DA
                    st       MUSIC_ON
                    bra      NEW_PLAYER

lbC0045DA:          move.w   #15, $DFF096
NEW_PLAYER:         tst.w    END_OF_STG
                    beq      STORE_GUY
                    cmp.b    #7, LEVEL_NUM
                    beq      lbC004604
                    cmp.b    #5, LEVEL_NUM
                    bpl      ENDSPLAY
lbC004604:          btst     #3, CHEAT+1
                    bne      lbC00461C
                    cmp.w    #3, ZOONB_GOT
                    bne      STORE_GUY
lbC00461C:          st       ON_BONUS
                    bra      SAMEP

STORE_GUY:          bsr      STORE_PLAYER
                    bsr      GET_NXP_STG
                    bsr      SET_NXPLAYER
                    tst.w    END_OF_STG
                    beq      NOTATEND
SAMEP:              addq.b   #1, STAGE_NUM
                    cmp.b    #3, STAGE_NUM
                    bne      NOTATEND
                    addq.b   #1, LEVEL_NUM
                    sf       STAGE_NUM
                    cmp.b    #8, LEVEL_NUM
                    bne      NOTATEND
                    sf       LEVEL_NUM
NOTATEND:           jsr      NEXT_STAGE
                    jsr      INIT_LEVEL
                    jsr      INIT_LDATA
                    jsr      INIT_SCRO
                    jsr      SET_PERC
                    jsr      RESET_COP
                    tst.w    END_OF_STG
                    beq      _SET_NXPLAYER
                    clr.w    END_OF_STG
                    bra      _INIT_SCRO

_SET_NXPLAYER:      bsr      SET_NXPLAYER
_INIT_SCRO:         jsr      INIT_SCRO
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    jsr      INIT_DUAL
                    tst.b    AUDIO
                    ble      ONEPLAYER
ONEPLAYER:          jsr      SETUPSCRO
                    sf       ZOOL_DIES
                    jsr      CLRPALET
                    lea      PANEL_PAL, a0
                    jsr      UPPERPALET
                    st       GAME_FADE
                    bsr      PRO_MAIN
                    move.l   #COPPER_GAME, COP_LOC
                    bsr      SYNCRO
                    move.w   #$8080, $DFF096
                    jsr      INIT_FDON
                    jsr      VBL
                    bsr      GO_FRAME
                    jsr      RESET_EBAR
                    bsr      PRO_MAIN
                    bsr      GO_FRAME
                    jmp      MLOOP

NOFADE1:            subq.b   #1, ZOOL_DIES
                    bmi      lbC004792
                    jsr      INIT_ZOOL
                    lea      RESTART_TAB, a0
                    move.w   (a0)+, STARTX
                    move.w   (a0)+, STARTY
                    move.w   (a0)+, ZOOL_SCRX
                    move.w   (a0)+, ZOOL_SCRY
                    move.w   (a0)+, TIME
                    move.b   (a0)+, ZOOL_FACE
                    beq      lbC004772
                    move.w   #$31, ZOOL_SPR
lbC004772:          sf       ZOOL_DIES
                    tst.w    LIVES
                    bpl      lbC004792
                    sf       MAINGUY_ON
                    st       ZOOL_DIES
                    bra      GAMEOV

lbC004792:          rts

STOPGAME:           tst.b    JUST_ENDED
                    beq      lbC0047CE
                    sf       JUST_ENDED
                    tst.b    PLAYERS
                    beq      lbC0047CE
                    move.w   #-1, LIVES
                    move.b   #7, LEVEL_NUM
                    clr.w    END_OF_STG
                    sf       P_DRAW
                    bra      OFF_MUS

lbC0047CE:          tst.b    MUSIC_ON
                    bne      _CLR_ALLBUFF
                    jsr      PLAY_MUSIC
_CLR_ALLBUFF:       jsr      CLR_ALLBUFF
                    jsr      TITLE_SCR
                    jmp      RESTART2

ENDSPLAY:           jmp      END_SEQ

RESET_EBAR:         move.w   #$600, lbB048BB4
                    move.w   #$A00, lbB048BB8
                    move.w   #$E00, lbB048BBC
                    move.w   #$600, lbB048BC4
                    move.w   #$A00, lbB048BC8
                    move.w   #$E00, lbB048BCC
                    move.w   #$FFFF, BLINK_CNT
RETURND:            rts

CLR_ALLBUFF:        lea      START_SCRN, a0
                    move.l   #$4DAF, d0
lbC00483C:          clr.l    (a0)+
                    clr.l    (a0)+
                    subq.l   #1, d0
                    bne.s    lbC00483C
                    rts

STORE_PLAYER:       lea      STORE_SPACE1, a0
                    tst.b    PLAYER_ON
                    beq      lbC004866
                    lea      STORE_SPACE2, a0
                    st       PLAY2_SAVED
                    bra      lbC00486C

lbC004866:          st       PLAY1_SAVED
lbC00486C:          move.b   LEVEL_NUM, (a0)+
                    move.b   STAGE_NUM, (a0)+
                    move.l   SCORE, (a0)+
                    move.w   PERCENT, (a0)+
                    move.w   LIVES, (a0)+
                    move.w   ZOONB_GOT, (a0)+
                    move.w   ZOONB_X, (a0)+
                    lea      BEACONS_TAB, a1
                    move.w   #5, d7
lbC0048A0:          move.l   (a1)+, (a0)+
                    move.l   (a1)+, (a0)+
                    dbra     d7, lbC0048A0
                    move.w   END_OF_STG, (a0)+
                    lea      RESTART_TAB, a1
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.b   (a1)+, (a0)+
NEXT_PLAYER:        eor.b    #1, PLAYER_ON
                    tst.w    LIVES
                    bpl      lbC0048D8
                    sf       PLAYERS
lbC0048D8:          rts

STORE_P2:           lea      STORE_SPACE2, a0
                    move.b   LEVEL_NUM, (a0)+
                    move.b   STAGE_NUM, (a0)+
                    move.l   SCORE, (a0)+
                    move.w   PERCENT, (a0)+
                    move.w   LIVES, (a0)+
                    move.w   ZOONB_GOT, (a0)+
                    move.w   ZOONB_X, (a0)+
                    lea      BEACONS_TAB, a1
                    move.w   #5, d7
lbC004914:          move.l   (a1)+, (a0)+
                    move.l   (a1)+, (a0)+
                    dbra     d7, lbC004914
                    clr.w    (a0)+
                    lea      RESTART_TAB, a1
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.b   (a1)+, (a0)+
                    rts

GET_NXP_STG:        lea      STORE_SPACE1, a0
                    tst.b    PLAYER_ON
                    beq      lbC004948
                    lea      STORE_SPACE2, a0
lbC004948:          move.b   (a0)+, LEVEL_NUM
                    move.b   (a0)+, STAGE_NUM
                    move.w   60(a0), END_OF_STG
                    rts

SET_NXPLAYER:       jsr      INIT_ZOOL
                    lea      STORE_SPACE1, a0
                    tst.b    PLAYER_ON
                    beq      lbC00497A
                    lea      STORE_SPACE2, a0
lbC00497A:          addq.l   #2, a0
                    move.l   (a0)+, SCORE
                    move.w   (a0)+, PERCENT
                    move.w   (a0)+, LIVES
                    move.w   (a0)+, ZOONB_GOT
                    move.w   (a0)+, ZOONB_X
                    lea      BEACONS_TAB, a1
                    move.w   #5, d7
lbC0049A4:          move.l   (a0)+, (a1)+
                    move.l   (a0)+, (a1)+
                    dbra     d7, lbC0049A4
                    move.w   (a0)+, END_OF_STG
                    lea      RESTART_TAB, a1
                    move.w   (a0)+, (a1)+
                    move.w   (a0)+, (a1)+
                    move.w   (a0)+, (a1)+
                    move.w   (a0)+, (a1)+
                    move.w   (a0)+, (a1)+
                    move.b   (a0)+, (a1)+
                    addq.l   #1, a0
                    lea      RESTART_TAB, a0
                    move.w   (a0)+, STARTX
                    move.w   (a0)+, STARTY
                    move.w   (a0)+, ZOOL_SCRX
                    move.w   (a0)+, ZOOL_SCRY
                    move.w   (a0)+, TIME
                    move.b   (a0)+, ZOOL_FACE
                    beq      lbC0049FC
                    move.w   #$31, ZOOL_SPR
lbC0049FC:          rts

GAMEOVER:           move.b   #7, LEVEL_NUM
                    tst.b    MUSIC_ON
                    ble      lbC004A22
                    move.w   #1, MUSIC_VOL
                    st       MUSIC_ON
                    bra      _GET_DISK

lbC004A22:          move.w   #15, $DFF096
_GET_DISK:          jsr      GET_DISK
                    clr.w    YSCROLL
                    clr.w    XSCROLL
                    
                    move.l   #lbL148548, a0
                    lea      OVER_NAME, a1
                    jsr      LOADFILE
                    
                    jsr      INIT_SCRNS
                    jsr      INIT_SCRO
                    lea      BUFF1_START, a2
                    move.l   a2, PHYSADR
                    move.l   PHYSADR, BUFF_PTR
                    jsr      SETSCREEN
                    clr.w    YSCRO
                    jsr      CLR_ALLSCR
                    
                    jsr      SET_CSCRO
                    move.l   #COPPER2, COP_LOC
                    jsr      VBL
                    sf       GAME_FADE
                    move.l   #STAGE_PAL, a0
                    jsr      NEWPALET
                    jsr      INS_HISCORE
                    tst.b    INSERT_HSCR
                    beq      lbC004B80
                    jsr      LOAD_INTRO
                    jsr      CLRPALET
                    move.l   #NULLCOP, COP_LOC
                    move.l   #$FFFFFFFE, WRITE_COP
                    jsr      INIT_SCRO
                    jsr      SCRO_NOW
                    jsr      DOCOL
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    clr.w    OLD_YSCR
                    clr.w    OLD_XSCR
                    jsr      HISCORE_SCR
                    move.l   #NULLCOP, COP_LOC
                    jsr      CLRPALET
                    move.l   #$FFFFFFFE, WRITE_COP
                    jsr      LOAD_INTRO
                    move.l   #NULLCOP, COP_LOC
                    jsr      CLRPALET
                    move.l   #NULLCOP, COP_LOC
                    move.l   #$FFFFFFFE, WRITE_COP
                    rts

lbC004B80:          move.w   #$C8, d7
                    jsr      WAIT_SECS
                    jsr      CLRPALET
                    move.l   #NULLCOP, COP_LOC
                    move.l   #$FFFFFFFE, WRITE_COP
                    jsr      LOAD_INTRO
                    jsr      CLRPALET
                    move.l   #NULLCOP, COP_LOC
                    move.l   #$FFFFFFFE, WRITE_COP
                    rts

LOAD_INTRO:         move.l   #INTRO_SPSTART, SPRITE_AD
                    move.l   #BOT_CHIP, MASK_AD
                    add.l    #$117BA, MASK_AD
                    move.l   MASK_AD, a0
                    add.l    #$1173A, a0
                    move.l   a0, MUSIC_PTR
                    move.l   a0, MUSIC_AD
                    move.l   #BLACK_BAND, COLBAND
                    clr.w    COLBAND_PTR
                    jsr      RESET_COP
                    move.w   #$14, MAP_WIDTH
                    move.l   #BOT_CHIP, a0
                    move.l   a0, TILES
                    move.w   #$3C, d7
lbC004CCE:          clr.l    (a0)+
                    dbra     d7, lbC004CCE
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    
                    move.l   SPRITE_AD, a0
                    lea      INTSP_NAME, a1
                    jsr      LOADFILE
                    
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    lea      INTMUS_NAME, a1
                    move.l   MUSIC_PTR, a0
                    jsr      LOADFILE
                    rts

PRO_POWER:          tst.w    INVULNERABLE
                    ble      lbC004D72
                    subq.w   #1, INVULNERABLE
lbC004D60:          tst.w    DESTROYER
                    ble      lbC004D7A
                    subq.w   #1, DESTROYER
                    rts

lbC004D72:          clr.w    INVULNERABLE
                    bra.s    lbC004D60

lbC004D7A:          clr.w    DESTROYER
                    rts

AUTO_ZOOL:          tst.b    NO_BADDY
                    bne      lbC004DB8
                    cmp.b    #1, BADDY_ON
                    beq      lbC004DB8
                    cmp.w    #$50, ZOOL_SCRX
                    bpl      lbC004DBA
                    cmp.w    #$30, ZOOL_SCRX
                    bpl      lbC004DC4
                    move.b   #8, JOYPOS
lbC004DB8:          rts

lbC004DBA:          move.b   #4, JOYPOS
                    rts

lbC004DC4:          sf       JOYPOS
                    st       ZOOL_FACE
                    rts

PRO_SHADE:          tst.b    SHADE_ON
                    beq      lbC004E04
                    subq.w   #1, SHADE_TIME
                    bmi      lbC004E06
                    move.l   END_SHADE, a0
                    move.w   ZOOL_X, (a0)+
                    move.w   ZOOL_Y, (a0)+
                    move.w   ZOOL_SPR, (a0)+
                    move.w   ZOOL_FACE, (a0)+
lbC004E04:          rts

lbC004E06:          sf       SHADE_ON
                    rts

INIT_SHADE:         st       SHADE_ON
                    move.l   #SHADE_BUFF, END_SHADE
                    st       SHADE_WAIT
                    move.w   #1000, SHADE_TIME
                    rts

KEY_CHECK:          move.b   KEYREC, d0
                    cmp.b    #$BD, d0
                    beq      lbC004EE6
                    cmp.b    #$91, d0
                    beq      lbC004EFC
                    cmp.b    #$CD, d0
                    beq      _READ_JOY
                    cmp.b    #$95, d0
                    beq      SMARTB
                    sf       OLDKEY
                    btst     #6, CHEAT
                    beq      CHEAT_KEYS
                    cmp.b    #$77, d0
                    bne      CHEAT_KEYS
                    tst.w    END_OF_STG
                    bmi      CHEAT_KEYS
                    jsr      SKIP_STG

CHEAT_KEYS:         btst     #7, CHEAT
                    beq      lbC004EE4
                    cmp.b    #$D9, d0
                    beq      lbC004ED4
                    cmp.b    #$FD, d0
                    beq      LEV0
                    cmp.b    #$FB, d0
                    beq      LEV1
                    cmp.b    #$F9, d0
                    beq      LEV2
                    cmp.b    #$BD, d0
                    beq      SLIPPY_SET
                    cmp.b    #$93, d0
                    beq      NORMG_SET
                    cmp.b    #$D5, d0
                    beq      SHADOW_ON
                    cmp.b    #$75, d0
                    beq      lbC004F5E
                    cmp.b    #$99, d0
                    beq      lbC004F1A
                    rts

lbC004ED4:          tst.b    INAIR
                    bne      lbC004EE4
                    jmp      REMEMBER

lbC004EE4:          rts

lbC004EE6:          tst.b    AUDIO
                    bmi.s    lbC004EE4
                    sf       MUSIC_ON
                    st       AUDIO
                    rts

lbC004EFC:          tst.b    AUDIO
                    bgt.s    lbC004EE4
                    jsr      PLAY_MUSIC
                    move.b   #1, AUDIO
                    rts

lbC004F1A:          move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    add.w    #$10, d1
                    and.l    #$FFF0, d0
                    and.l    #$FFF0, d1
                    move.w   #$FFEC, d7
                    sub.w    #$10, d0
                    bsr      ADD_BACKSP
                    add.w    #$10, d0
                    asr.w    #4, d0
                    asr.w    #4, d1
                    mulu     #$3E8, d0
                    add.l    d1, d0
                    move.l   d0, SCORE
                    st       TEST_MODE
                    rts

lbC004F5E:          move.b   #99, PERCENT
                    rts

_READ_JOY:          jsr      READ_JOY
                    cmp.b    #$CD, KEYREC
                    beq.s    _READ_JOY
_READ_JOY0:         jsr      READ_JOY
                    cmp.b    #$75, KEYREC
                    beq      EXIT_GAME
                    cmp.b    #$CD, KEYREC
                    bne.s    _READ_JOY0
_READ_JOY1:         jsr      READ_JOY
                    cmp.b    #$CD, KEYREC
                    beq.s    _READ_JOY1
                    rts

EXIT_GAME:          move.w   #0, ENERGY
                    move.w   #0, LIVES
                    jmp      ZOOL_DAMAGE

SMARTB:             cmp.b    OLDKEY, d0
                    beq      lbC004FD2
                    move.b   d0, OLDKEY
                    jmp      DEL_SMART

lbC004FD2:          rts

SHADOW_ON:          bra      INIT_SHADE

LEV0:               move.b   #0, LEVEL_NUM
                    jsr      CLRPALET
                    bra      START

LEV1:               move.b   #1, LEVEL_NUM
                    jsr      CLRPALET
                    bra      START

LEV2:               move.b   #2, LEVEL_NUM
                    jsr      CLRPALET
                    bra      START

SLIPPY_SET:         st       SLIPPY_SLP
                    rts

NORMG_SET:          sf       SLIPPY_SLP
                    rts

COLOUR_CHG:         move.b   KEYREC, d0
                    cmp.b    OLDKEY, d0
                    beq      lbC005074
                    move.b   d0, OLDKEY
                    cmp.b    #$67, d0
                    beq      lbC00507E
                    cmp.b    #$65, d0
                    beq      lbC005090
                    cmp.b    #$FD, d0
                    beq      lbC00509A
                    cmp.b    #$DF, d0
                    beq      lbC0050BC
                    cmp.b    #$FB, d0
                    beq      lbC0050D4
                    cmp.b    #$DD, d0
                    beq      lbC0050E0
                    cmp.b    #$F9, d0
                    beq      lbC005108
                    cmp.b    #$DB, d0
                    beq      lbC005124
lbC005074:          rts

                    clr.b    OLDJOY
                    rts

lbC00507E:          subq.w   #1, BAND_POS
                    bpl      lbC005168
                    clr.w    BAND_POS
                    rts

lbC005090:          addq.w   #1, BAND_POS
                    bra      lbC005168

lbC00509A:          add.w    #$100, COLOUR
                    move.w   COLOUR, d0
                    and.w    #$F000, d0
                    beq      lbC005150
                    and.w    #$FF, COLOUR
                    bra      lbC005150

lbC0050BC:          sub.b    #1, COLOUR
                    bpl      lbC005150
                    move.b   #15, COLOUR
                    bra      lbC005150

lbC0050D4:          add.b    #$10, COLOUR+1
                    bra      lbC005150

lbC0050E0:          sub.b    #$10, COLOUR+1
                    bcc      lbC005150
                    add.b    #$10, COLOUR+1
                    and.b    #$F, COLOUR+1
                    or.b     #$F0, COLOUR+1
                    bne      lbC005150
lbC005108:          move.b   COLOUR+1, d0
                    and.b    #15, d0
                    add.b    #1, d0
                    btst     #4, d0
                    beq      lbC00513A
                    clr.b    d0
                    bra      lbC00513A

lbC005124:          move.b   COLOUR+1, d0
                    and.b    #15, d0
                    sub.b    #1, d0
                    bpl      lbC00513A
                    move.b   #15, d0
lbC00513A:          move.b   COLOUR+1, d1
                    and.b    #$F0, d1
                    or.b     d1, d0
                    move.b   d0, COLOUR+1
                    bra      lbC005150

lbC005150:          lea      BONUS_BAND, a0
                    move.w   BAND_POS, d0
                    add.w    d0, d0
                    move.w   COLOUR, 0(a0,d0.w)
                    rts

lbC005168:          lea      BONUS_BAND, a0
                    move.w   BAND_POS, d0
                    add.w    d0, d0
                    move.w   0(a0,d0.w), COLOUR
                    rts

ZOOL_ACTIONS:       dc.l     ZOOL_STAND
                    dc.l     ZOOL_RUN
                    dc.l     ZOOL_INAIR
                    dc.l     ZOOL_GRIP
                    dc.l     ZOOL_ZOOM
                    dc.l     ZOOL_SLIDE
                    dc.l     ZOOL_PUNCH
                    dc.l     ZOOL_SKID
                    dc.l     ZOOL_SIDEK
                    dc.l     ZOOL_CROUCH
                    dc.l     ZOOL_SWGRIP
                    dc.l     ZOOL_TUMBLE
                    dc.l     ZOOL_FLIP

ZOOL_FLIP:          tst.b    ZOOL_FACE
                    beq      lbC005294
                    cmp.w    #$55, ZOOL_SPR
                    bpl      lbC005238
                    subq.w   #1, ZOOL_ANIM
                    bpl      lbC005268
                    addq.w   #1, ZOOL_SPR
                    cmp.w    #$55, ZOOL_SPR
                    beq      lbC0051FC
                    move.w   #2, ZOOL_ANIM
                    lea      SPIN_FX, a5
                    jsr      ADD_SFX
                    rts

lbC0051FC:          lea      FLIP_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX10
                    lea      FLIPZZ_FX, a5
_ADD_SFX10:         jsr      ADD_SFX
                    st       INAIR
                    move.w   #-96, X_FORCE
                    move.w   #-208, Y_FORCE
                    move.w   #3, ZOOL_ANIM
                    rts

lbC005238:          subq.w   #1, ZOOL_ANIM
                    bpl      lbC005264
                    move.w   #3, ZOOL_ANIM
                    addq.w   #1, ZOOL_SPR
                    cmp.w    #$59, ZOOL_SPR
                    bne      lbC005264
                    move.w   #$55, ZOOL_SPR
lbC005264:          bra      lbC00526A

lbC005268:          rts

lbC00526A:          btst     #2, JOYPOS
                    beq      lbC00527E
                    sub.w    #2, X_FORCE
lbC00527E:          btst     #3, JOYPOS
                    beq      lbC005292
                    add.w    #2, X_FORCE
lbC005292:          rts

lbC005294:          cmp.w    #$4B, ZOOL_SPR
                    bpl      lbC00530C
                    subq.w   #1, ZOOL_ANIM
                    bpl.s    lbC005268
                    addq.w   #1, ZOOL_SPR
                    cmp.w    #$4B, ZOOL_SPR
                    beq      lbC0052D0
                    move.w   #2, ZOOL_ANIM
                    lea      SPIN_FX, a5
                    jsr      ADD_SFX
                    rts

lbC0052D0:          lea      FLIP_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX11
                    lea      FLIPZZ_FX, a5
_ADD_SFX11:         jsr      ADD_SFX
                    st       INAIR
                    move.w   #$60, X_FORCE
                    move.w   #$FF30, Y_FORCE
                    move.w   #3, ZOOL_ANIM
                    rts

lbC00530C:          subq.w   #1, ZOOL_ANIM
                    bpl      lbC005338
                    move.w   #3, ZOOL_ANIM
                    addq.w   #1, ZOOL_SPR
                    cmp.w    #$4F, ZOOL_SPR
                    bne      lbC005338
                    move.w   #$4B, ZOOL_SPR
lbC005338:          bra      lbC00526A

ZOOL_TUMBLE:        subq.b   #1, ZOOL_ANIM
                    bpl      lbC005374
                    move.b   #1, ZOOL_ANIM
                    addq.w   #1, ZOOL_SPR
                    cmp.w    #$45, ZOOL_SPR
                    bmi      lbC005374
                    lea      SPIN_FX, a5
                    jsr      ADD_SFX
                    move.w   #$41, ZOOL_SPR
lbC005374:          move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    tst.w    X_FORCE
                    beq      lbC0053AE
                    bmi      lbC00539E
                    add.w    #$10, d0
                    bsr      CHECK_TILE2
                    tst.b    d7
                    bne      lbC0053D8
                    rts

lbC00539E:          sub.w    #$10, d0
                    bsr      CHECK_TILE2
                    tst.b    d7
                    bne      lbC0053D8
                    rts

lbC0053AE:          tst.w    Y_FORCE
                    bmi      lbC0053C8
                    add.w    #$10, d1
                    bsr      CHECK_TILE2
                    tst.b    d7
                    bne      lbC0053D8
                    rts

lbC0053C8:          sub.w    #$10, d1
                    bsr      CHECK_TILE2
                    tst.b    d7
                    bne      lbC0053D8
lbC0053D6:          rts

lbC0053D8:          cmp.b    #1, d7
                    beq.s    lbC0053D6
                    cmp.b    #$17, d7
                    beq      lbC00544A
                    cmp.b    #$18, d7
                    beq      lbC00545E
                    cmp.b    #$19, d7
                    beq      lbC005472
                    cmp.b    #$1A, d7
                    beq      lbC005486
                    tst.w    X_FORCE
                    beq      lbC00540A
                    rts

lbC00540A:          tst.w    Y_FORCE
                    bmi      _ROOF_BURST
                    rts

_ROOF_BURST:        bsr      ROOF_BURST
                    add.l    REF_MAP, a0
                    moveq    #0, d0
                    move.w   MAP_WIDTH, d0
                    add.l    d0, a0
                    move.b   #1, (a0)+
                    move.b   #1, (a0)+
                    move.b   #1, (a0)+
                    sf       GRAV_WAIT
                    bsr      INIT_JUMP
                    move.w   #$FF40, Y_FORCE
                    rts

lbC00544A:          bsr      lbC00549A
                    clr.w    X_FORCE
                    move.w   #$FF00, Y_FORCE
                    rts

lbC00545E:          bsr      lbC00549A
                    clr.w    Y_FORCE
                    move.w   #$100, X_FORCE
                    rts

lbC005472:          bsr      lbC00549A
                    clr.w    X_FORCE
                    move.w   #$100, Y_FORCE
                    rts

lbC005486:          bsr      lbC00549A
                    clr.w    Y_FORCE
                    move.w   #$FF00, X_FORCE
                    rts

lbC00549A:          and.w    #$FFF0, d0
                    and.w    #$FFF0, d1
                    subq.w   #8, d0
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, ZOOL_SCRX
                    move.w   d1, ZOOL_SCRY
                    rts

ZOOL_STAND:         btst     #2, JOYPOS
                    bne      lbC00556A
                    btst     #3, JOYPOS
                    bne      lbC00559E
                    btst     #0, JOYPOS
                    bne      _INIT_JUMP0
                    btst     #1, JOYPOS
                    bne      _INIT_CROUCH
                    btst     #7, JOYPOS
                    bne      INIT_PUNCH
                    bclr     #7, OLDJOY
                    tst.b    BIGB_POW
                    beq      lbC005528
                    cmp.w    #10, FIRE_REPT
                    bmi      lbC005528
                    clr.w    FIRE_REPT
                    st       BIG_BOMB
                    bra      INIT_PUNCH

lbC005528:          clr.w    FIRE_REPT
                    tst.w    BLINK_WAIT
                    bmi      lbC00554C
                    subq.w   #1, BLINK_WAIT
                    bpl      lbC00554A
                    move.w   #$FFFC, BLINK_WAIT
lbC00554A:          rts

lbC00554C:          addq.w   #1, BLINK_WAIT
                    bmi.s    lbC00554A
                    move.w   SEED, d7
                    and.w    #$3F, d7
                    add.w    #$14, d7
                    move.w   d7, BLINK_WAIT
                    rts

lbC00556A:          sf       ZOOL_FACE
                    cmp.w    #$29, ZOOL_SPR
                    beq      lbC005586
                    move.w   #$29, ZOOL_SPR
                    rts

lbC005586:          sf       ZOOL_FACE
                    clr.w    ZOOL_ANIM
                    move.w   #ZOOL_RUNNING, ZOOL_MOVE
                    bra      ZOOL_RUN

lbC00559E:          st       ZOOL_FACE
                    cmp.w    #$31, ZOOL_SPR
                    beq      lbC0055BA
                    move.w   #$31, ZOOL_SPR
                    rts

lbC0055BA:          clr.w    ZOOL_ANIM
                    move.w   #ZOOL_RUNNING, ZOOL_MOVE
                    bra      ZOOL_RUN

_INIT_JUMP0:        bra      INIT_JUMP

_INIT_CROUCH:       bra      INIT_CROUCH

INIT_STAND:         clr.w    ZOOL_ANIM
                    clr.w    ZOOL_MOVE
                    clr.w    Y_FORCE
                    move.w   #$29, ZOOL_SPR
                    tst.b    ZOOL_FACE
                    beq      lbC005600
                    move.w   #$31, ZOOL_SPR
lbC005600:          rts

INIT_JUMP:          move.l   a5, -(sp)
                    tst.b    INAIR
                    bne      lbC00567E
                    tst.w    Y_FORCE
                    bgt      lbC00567E
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sub.w    #$18, d1
                    bsr      CHECK_TILE2
                    tst.b    CUSTOM_ON
                    bne      lbC00567E
                    cmp.b    #1, d7
                    beq      _INIT_STAND
                    cmp.b    #6, d7
                    beq      _INIT_STAND
                    cmp.b    #8, d7
                    beq      _INIT_STAND
                    cmp.b    #9, d7
                    beq      _INIT_STAND
                    cmp.b    #13, d7
                    beq      _INIT_STAND
                    cmp.b    #$1B, d7
                    beq      _INIT_STAND
                    cmp.b    #$1E, d7
                    beq      _INIT_STAND
                    cmp.b    #$4E, d7
                    beq      lbC0056B8
                    cmp.b    #$4F, d7
                    beq      lbC0056B8
lbC00567E:          move.w   #2, ZOOL_MOVE
                    sub.w    #$A0, Y_FORCE
                    move.w   #1, X_FRICTION
                    clr.w    ZOOL_ANIM
                    st       INAIR
                    clr.w    ZOOL_SPR
                    bsr      ZOOL_INAIR
                    move.l   (sp)+, a5
                    rts

_INIT_STAND:        bsr      INIT_STAND
                    move.l   (sp)+, a5
                    rts

lbC0056B8:          cmp.w    #$2B, ZOOL_SPR
                    bpl.s    lbC00567E
                    bra.s    _INIT_STAND

ZOOL_RUN:           btst     #7, JOYPOS
                    beq      lbC0056E4
                    bset     #7, OLDJOY
                    bne      lbC0056EE
                    bsr      ZOOL_SHOOTA
                    bra      lbC0056EE

lbC0056E4:          sf       OLDJOY
                    bra      lbC0056F4

lbC0056EE:          addq.w   #1, FIRE_REPT
lbC0056F4:          btst     #0, JOYPOS
                    beq      lbC005716
                    st       OLD_FSTEP
                    lea      FSTEP_FX, a5
                    jsr      ADD_SFX
                    bra      INIT_JUMP

lbC005716:          btst     #2, JOYPOS
                    beq      lbC00573E
                    subq.w   #8, X_FORCE
                    bpl      lbC00573E
                    move.w   X_FORCE, d0
                    neg.w    d0
                    lsr.w    #5, d0
                    addq.w   #1, d0
                    add.w    d0, ZOOL_ANIM
lbC00573E:          btst     #3, JOYPOS
                    beq      lbC005764
                    addq.w   #8, X_FORCE
                    ble      lbC005764
                    move.w   X_FORCE, d0
                    lsr.w    #5, d0
                    addq.w   #1, d0
                    add.w    d0, ZOOL_ANIM
lbC005764:          move.b   JOYPOS, d1
                    and.b    #12, d1
                    beq      INIT_SKID
                    btst     #1, JOYPOS
                    bne      INIT_SIDEK
                    bsr      GET_FACE
                    move.w   ZOOL_ANIM, d0
                    cmp.w    #$60, d0
                    bmi      lbC00579C
                    sub.w    #$60, ZOOL_ANIM
                    sub.w    #$60, d0
lbC00579C:          lea      RUN_ANIM, a0
                    move.b   0(a0,d0.w), d7
                    move.b   d7, ZOOL_SPR+1
                    beq      lbC0057CA
                    cmp.b    #4, d7
                    beq      lbC0057CA
lbC0057B8:          tst.b    ZOOL_FACE
                    beq      lbC0057C8
                    addq.b   #8, ZOOL_SPR+1
lbC0057C8:          rts

lbC0057CA:          cmp.b    OLD_FSTEP, d7
                    beq.s    lbC0057B8
                    move.b   d7, OLD_FSTEP
                    lea      FSTEP_FX, a5
                    jsr      ADD_SFX
                    bra.s    lbC0057B8

ZOOL_INAIR:         st       INAIR
                    cmp.w    #$36, ZOOL_SPR
                    bpl      lbC005972
                    sf       GRAV_WAIT
                    cmp.w    #$2B, ZOOL_SPR
                    bpl      lbC00592E
                    btst     #7, JOYPOS
                    beq      lbC00585A
                    bset     #7, OLDJOY
                    bne      lbC00582A
                    bsr      ZOOL_SHOOTA
                    bra      lbC00586A

lbC00582A:          subq.b   #1, FIRE_CNT
                    bpl      lbC00586A
                    
                    tst.b    SPINNING
                    bne      lbC005848
                    addq.b   #1, FIRE_CNT
                    bra      lbC00586A

lbC005848:          clr.w    ZOOL_ANIM
                    move.w   #$2B, ZOOL_SPR
                    bra      lbC0058A0

lbC00585A:          bclr     #7, OLDJOY
                    move.b   #2, FIRE_CNT
lbC00586A:          move.w   #$17, ZOOL_SPR
                    move.w   Y_FORCE, d0
                    bmi      lbC005890
                    cmp.w    #$40, d0
                    bmi      lbC0058A0
                    move.w   #$18, ZOOL_SPR
                    bra      lbC0058A0

lbC005890:          cmp.w    #$FFE0, d0
                    bgt      lbC0058A0
                    move.w   #$16, ZOOL_SPR
lbC0058A0:          btst     #0, JOYPOS
                    beq      lbC0058B2
                    subq.w   #3, Y_FORCE
lbC0058B2:          btst     #2, JOYPOS
                    beq      lbC0058E0
                    tst.b    PAUSE_MOVE
                    bpl      lbC0058D2
                    addq.b   #1, PAUSE_MOVE
                    bra      lbC0058E0

lbC0058D2:          sub.w    #$18, X_FORCE
                    sf       ZOOL_FACE
lbC0058E0:          btst     #3, JOYPOS
                    beq      lbC00590E
                    tst.b    PAUSE_MOVE
                    ble      lbC005900
                    subq.b   #1, PAUSE_MOVE
                    bra      lbC00590E

lbC005900:          st       ZOOL_FACE
                    add.w    #$18, X_FORCE
lbC00590E:          cmp.w    #$2B, ZOOL_SPR
                    bpl      lbC00592C
                    tst.b    ZOOL_FACE
                    beq      lbC00592C
                    add.w    #9, ZOOL_SPR
lbC00592C:          rts

lbC00592E:          addq.w   #1, DESTROYER
                    lea      SPIN_ANIM, a0
                    addq.w   #2, ZOOL_ANIM
                    move.w   ZOOL_ANIM, d0
                    move.w   0(a0,d0.w), d0
                    bpl      lbC005968
                    move.w   #0, ZOOL_ANIM
                    move.w   SPIN_ANIM, d0
                    lea      SPIN_FX, a5
                    jsr      ADD_SFX
lbC005968:          move.w   d0, ZOOL_SPR
                    bra      lbC0058A0

lbC005972:          addq.w   #1, DESTROYER
                    subq.w   #3, Y_FORCE
                    tst.w    Y_FORCE
                    bpl      lbC0059F8
                    tst.b    FORCE_LR
                    bmi      lbC005A1A
                    bgt      lbC005A2A
                    btst     #2, JOYPOS
                    beq      lbC0059B0
                    sf       ZOOL_FACE
                    sub.w    #$18, X_FORCE
lbC0059B0:          btst     #3, JOYPOS
                    beq      lbC0059CA
                    st       ZOOL_FACE
                    add.w    #$18, X_FORCE
lbC0059CA:          subq.w   #1, ZOOL_ANIM
                    bpl      lbC0059F6
                    addq.w   #1, ZOOL_SPR
                    move.w   #2, ZOOL_ANIM
                    cmp.w    #$3A, ZOOL_SPR
                    bne      lbC0059F6
                    move.w   #$36, ZOOL_SPR
lbC0059F6:          rts

lbC0059F8:          sf       FORCE_LR
                    move.w   #$18, ZOOL_SPR
                    tst.b    ZOOL_FACE
                    beq      lbC00592C
                    move.w   #$21, ZOOL_SPR
                    rts

lbC005A1A:          sub.w    #$18, X_FORCE
                    sf       ZOOL_FACE
                    bra.s    lbC0059CA

lbC005A2A:          add.w    #$18, X_FORCE
                    st       ZOOL_FACE
                    bra.s    lbC0059CA

ZOOL_SHOOTA:        move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sub.w    #$12, d0
                    sub.w    #10, d1
                    move.w   #8, d2
                    tst.b    ZOOL_FACE
                    bne      lbC005A5E
                    neg.w    d2
lbC005A5E:          move.w   #$2D, d3
                    bsr      ADD_SHOT
                    tst.b    SHADE_ON
                    bne      lbC005A72
                    rts

lbC005A72:          move.l   END_SHADE, a0
                    move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    sub.w    #$12, d0
                    sub.w    #10, d1
                    move.w   #8, d2
                    tst.b    2(a0)
                    bne      lbC005A92
                    neg.w    d2
lbC005A92:          move.w   #$2D, d3
                    bra      ADD_SHOT

ZOOL_SHOOTB:        move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sub.w    #$12, d0
                    move.w   #8, d2
                    tst.b    ZOOL_FACE
                    bne      lbC005ABA
                    neg.w    d2
lbC005ABA:          move.w   #$2D, d3
                    bsr      ADD_SHOT
                    tst.b    SHADE_ON
                    bne      lbC005ACE
                    rts

lbC005ACE:          move.l   END_SHADE, a0
                    move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    sub.w    #$12, d0
                    move.w   #8, d2
                    tst.b    2(a0)
                    bne      lbC005AEA
                    neg.w    d2
lbC005AEA:          move.w   #$2D, d3
                    bra      ADD_SHOT

ADD_SHOT:           lea      SHOT_TAB, a0
lbC005AF8:          tst.b    (a0)
                    beq      lbC005B0A
                    addq.l   #8, a0
                    cmp.l    #Y_CENTRE, a0
                    bne.s    lbC005AF8
                    rts

lbC005B0A:          tst.b    BIG_BOMB
                    beq      lbC005B22
                    sf       BIG_BOMB
                    move.b   #1, (a0)+
                    bra      lbC005B24

lbC005B22:          st       (a0)+
lbC005B24:          move.b   d3, (a0)+
                    add.w    d2, d0
                    move.w   d0, (a0)+
                    move.w   d1, (a0)+
                    move.w   d2, (a0)+
                    lea      FIRE_FX, a5
                    jsr      ADD_SFX
                    rts

PRO_SHOTS:          lea      SHOT_TAB, a0
lbC005B42:          tst.b    (a0)
                    beq      lbC005BB4
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    move.l   REF_MAP, a1
                    addq.w   #8, d1
                    addq.w   #8, d0
                    lsr.w    #4, d0
                    lsr.w    #4, d1
                    addq.w   #1, d0
                    move.w   d1, d7
                    mulu     MAP_WIDTH, d7
                    add.w    d0, d7
                    lea      0(a1,d7.w), a1
                    move.b   (a1), d7
                    ble      lbC005BB4
                    cmp.b    #$1C, d7
                    beq      lbC005BF6
                    cmp.b    #$1C, d7
                    beq      lbC005BF2
                    cmp.b    #$50, d7
                    beq      lbC005BC0
                    cmp.b    #$4E, d7
                    beq      CORN_BLK
                    cmp.b    #$4F, d7
                    beq      HIT_BLK
                    cmp.b    #8, d7
                    beq      lbC005C40
                    cmp.b    #6, d7
                    beq      lbC005C10
                    cmp.b    #$24, d7
                    bpl      lbC005BE2
lbC005BB4:          addq.l   #8, a0
                    cmp.l    #Y_CENTRE, a0
                    bne.s    lbC005B42
                    rts

lbC005BC0:          sf       (a0)
                    st       BELL_RING
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    addq.w   #3, d0
                    move.w   #$400, d7
                    clr.w    d2
                    move.w   SPLAT_ANIM, d3
                    bra      ADD_FXPIX

                    rts

lbC005BE2:          cmp.b    #$47, d7
                    bpl.s    lbC005BB4
                    sf       (a0)
                    bsr      BUTTON_ON
                    sf       d7
                    bra.s    lbC005BB4

lbC005BF2:          subq.l   #1, a1
                    subq.w   #1, d0
lbC005BF6:          cmp.b    #$29, 1(a0)
                    bpl      lbC005C0C
                    sf       (a0)
                    sf       HINT_ONLY
                    bsr      SECRET_ON
lbC005C0C:          sf       d7
                    bra.s    lbC005BB4

lbC005C10:          subq.w   #1, d1
                    move.b   #8, d2
                    move.w   MAP_WIDTH, d7
                    neg.w    d7
                    lea      0(a1,d7.w), a1
                    cmp.b    (a1), d2
                    beq      lbC005C40
                    subq.w   #1, d1
                    lea      0(a1,d7.w), a1
                    cmp.b    (a1), d2
                    beq      lbC005C40
                    subq.w   #1, d1
                    lea      0(a1,d7.w), a1
                    cmp.b    (a1), d2
                    bne      lbC005BB4
lbC005C40:          lsl.w    #4, d0
                    lsl.w    #4, d1
                    subq.w   #1, d0
                    move.w   #$200, d7
                    move.w   #$103, d2
                    move.b   END_TILEFX, d3
                    move.w   FILLTILE_SPR, d4
                    move.l   a0, -(sp)
                    bsr      ADD_TILEFX
                    move.l   (sp)+, a0
                    tst.b    (a0)
                    bpl      lbC005BB4
                    sf       (a0)
                    bra      lbC005BB4

HIT_BLK:            subq.l   #1, a1
                    subq.w   #1, d0
                    cmp.b    #$4E, (a1)
                    beq      CORN_BLK
                    move.w   MAP_WIDTH, d7
                    neg.w    d7
                    lea      0(a1,d7.w), a1
                    subq.w   #1, d1
                    cmp.b    #$4E, (a1)
                    beq      CORN_BLK
                    addq.w   #1, d0
                    addq.l   #1, a1
CORN_BLK:           tst.b    (a0)
                    bpl      lbC005CA0
                    sf       (a0)
                    bra      lbC005CA4

lbC005CA0:          move.b   #1, (a0)
lbC005CA4:          move.w   MAP_WIDTH, d7
                    sf       (a1)
                    sf       0(a1,d7.w)
                    sf       1(a1)
                    sf       1(a1,d7.w)
                    move.l   a1, d7
                    sub.l    REF_MAP, d7
                    lsl.l    #1, d7
                    move.l   CURRENT_MAP, a1
                    addq.l   #8, a1
                    add.l    d7, a1
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    clr.w    (a1)
                    clr.w    2(a1)
                    clr.w    0(a1,d7.w)
                    clr.w    2(a1,d7.w)
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    move.w   FILLTILE_SP2, d7
                    bsr      ADD_PERM
                    add.w    #$10, d0
                    add.w    #$10, d1
                    move.w   #2, d2
                    jsr      NOTOK
                    move.l   a5, -(sp)
                    lea      BREAK_FX, a5
                    jsr      ADD_SFX
                    move.l   (sp)+, a5
                    rts

DRAW_SHOTS:         lea      SHOT_TAB, a0
                    move.w   SHOT_SPR, d7
                    lsr.w    #1, d7
                    add.w    #$59, d7
                    move.w   d7, SPRITE
lbC005D2E:          tst.b    (a0)+
                    bne      lbC005D58
                    addq.l   #7, a0
lbC005D36:          cmp.l    #Y_CENTRE, a0
                    bne.s    lbC005D2E
                    move.w   SHOT_SPR, d7
                    addq.w   #1, d7
                    cmp.w    #6, d7
                    bne      lbC005D50
                    clr.w    d7
lbC005D50:          move.w   d7, SHOT_SPR
                    rts

lbC005D58:          subq.b   #1, (a0)+
                    beq      lbC005DA2
                    move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    move.w   (a0)+, d2
                    add.w    d2, -6(a0)
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    cmp.w    #$FFF0, d0
                    ble      lbC005DAA
                    cmp.w    #$140, d0
                    bpl      lbC005DAA
                    move.w   d1, YCOORD
                    move.l   a0, -(sp)
                    tst.b    -8(a0)
                    bpl      lbC005DB0
                    bsr      DUMPSPRITE
lbC005D9E:          move.l   (sp)+, a0
                    bra.s    lbC005D36

lbC005DA2:          sf       -2(a0)
                    addq.l   #6, a0
                    bra.s    lbC005D36

lbC005DAA:          sf       -8(a0)
                    bra.s    lbC005D36

lbC005DB0:          move.w   SPRITE, -(sp)
                    move.w   #$6A, d7
                    add.b    ANDYFRAME, d7
                    tst.w    -2(a0)
                    bmi      lbC005DCA
                    addq.w   #2, d7
lbC005DCA:          move.w   d7, SPRITE
                    bsr      DUMPSPRITE
                    move.w   (sp)+, SPRITE
                    bra.s    lbC005D9E

ZOOL_ZOOM:          btst     #0, JOYPOS
                    bne      INIT_JUMP
                    move.b   GRADIENT, d0
                    tst.w    X_FORCE
                    bmi      lbC005E00
                    bgt      lbC005E4A
                    bra      INIT_RUN

lbC005E00:          move.w   #$11, ZOOL_SPR
                    cmp.b    #$FD, d0
                    bmi      lbC005E36
                    move.w   #$10, ZOOL_SPR
                    tst.b    d0
                    ble      lbC005E36
                    move.w   #$24, ZOOL_SPR
                    cmp.b    #3, d0
                    ble      lbC005E36
                    move.w   #$25, ZOOL_SPR
lbC005E36:          btst     #2, JOYPOS
                    beq      lbC005E92
                    subq.w   #8, X_FORCE
                    rts

lbC005E4A:          move.w   #$1A, ZOOL_SPR
                    cmp.b    #3, d0
                    bgt      lbC005E80
                    move.w   #$19, ZOOL_SPR
                    tst.b    d0
                    bpl      lbC005E80
                    move.w   #$22, ZOOL_SPR
                    cmp.b    #$FD, d0
                    bpl      lbC005E80
                    move.w   #$23, ZOOL_SPR
lbC005E80:          btst     #3, JOYPOS
                    beq      lbC005E92
                    addq.w   #8, X_FORCE
lbC005E92:          rts

INIT_ZOOM:          move.w   #4, ZOOL_MOVE
                    clr.w    ZOOL_ANIM
                    clr.b    INAIR
                    bra      ZOOL_ZOOM

ZOOL_SLIDE:         subq.w   #1, SLIDE_SND
                    bpl      lbC005ECA
                    move.w   #8, SLIDE_SND
                    lea      SLIDE_FX, a5
                    jsr      ADD_SFX
lbC005ECA:          btst     #7, JOYPOS
                    beq      lbC005EEA
                    bset     #7, OLDJOY
                    bne      lbC005EF0
                    bsr      ZOOL_SHOOTA
                    bra      lbC005EF0

lbC005EEA:          sf       OLDJOY
lbC005EF0:          btst     #0, JOYPOS
                    beq      lbC005F2E
                    tst.b    DRAGGED
                    bne      lbC005F48
                    move.b   GRADIENT, d0
                    bpl      lbC005F12
                    neg.b    d0
lbC005F12:          cmp.b    #3, d0
                    ble      lbC005F24
                    subq.b   #1, SLIPPY_CNT
                    bpl      lbC005F2E
lbC005F24:          sf       SLIPPY_CNT
                    bne      INIT_JUMP
lbC005F2E:          tst.b    DRAGGED
                    bne      lbC005F48
                    btst     #1, JOYPOS
                    beq      lbC005F48
                    bra      INIT_SIDEK

lbC005F48:          tst.b    SLIPPY_SLP
                    beq      INIT_RUN
                    move.b   GRADIENT, d0
                    tst.b    ZOOL_FACE
                    beq      lbC005FE2
lbC005F62:          tst.b    DRAGGED
                    ble      lbC005FB2
                    move.w   X_FORCE, d7
                    btst     #2, JOYPOS
                    beq      lbC005F84
                    subq.w   #2, d7
                    bra      lbC005F9E

lbC005F84:          btst     #3, JOYPOS
                    beq      lbC005F9E
                    addq.w   #4, d7
                    cmp.w    #$90, d7
                    bmi      lbC005F9E
                    move.w   #$8C, d7
lbC005F9E:          cmp.w    #$30, d7
                    bpl      lbC005FA8
                    addq.w   #8, d7
lbC005FA8:          move.w   d7, X_FORCE
                    bra      lbC005FBE

lbC005FB2:          btst     #2, JOYPOS
                    bne      lbC006062
lbC005FBE:          cmp.b    #$FD, d0
                    bmi      lbC0060BE
                    cmp.b    #$FF, d0
                    bmi      lbC0060B4
                    cmp.b    #1, d0
                    ble      lbC006112
                    cmp.b    #3, d0
                    ble      lbC00608A
                    bra      lbC0060AA

lbC005FE2:          tst.b    DRAGGED
                    bpl      lbC006032
                    move.w   X_FORCE, d7
                    btst     #2, JOYPOS
                    beq      lbC006010
                    subq.w   #4, d7
                    cmp.w    #$FF70, d7
                    bgt      lbC00601E
                    move.w   #$FF74, d7
                    bra      lbC00601E

lbC006010:          btst     #3, JOYPOS
                    beq      lbC00601E
                    addq.w   #2, d7
lbC00601E:          cmp.w    #$FFD0, d7
                    ble      lbC006028
                    subq.w   #8, d7
lbC006028:          move.w   d7, X_FORCE
                    bra      lbC00603E

lbC006032:          btst     #3, JOYPOS
                    bne      lbC00606C
lbC00603E:          cmp.b    #$FD, d0
                    bmi      lbC006080
                    cmp.b    #$FF, d0
                    bmi      lbC006076
                    cmp.b    #1, d0
                    ble      lbC0060F2
                    cmp.b    #3, d0
                    ble      lbC0060C8
                    bra      lbC0060E8

lbC006062:          sf       ZOOL_FACE
                    bra      lbC005FE2

lbC00606C:          st       ZOOL_FACE
                    bra      lbC005F62

lbC006076:          move.w   #$10, ZOOL_SPR
                    rts

lbC006080:          move.w   #$11, ZOOL_SPR
                    rts

lbC00608A:          tst.b    DRAGGED
                    bne      lbC0060A0
                    btst     #3, JOYPOS
                    bne      lbC006132
lbC0060A0:          move.w   #$19, ZOOL_SPR
                    rts

lbC0060AA:          move.w   #$1A, ZOOL_SPR
                    rts

lbC0060B4:          move.w   #$22, ZOOL_SPR
                    rts

lbC0060BE:          move.w   #$23, ZOOL_SPR
                    rts

lbC0060C8:          tst.b    DRAGGED
                    bne      lbC0060DE
                    btst     #2, JOYPOS
                    bne      lbC006160
lbC0060DE:          move.w   #$24, ZOOL_SPR
                    rts

lbC0060E8:          move.w   #$25, ZOOL_SPR
                    rts

lbC0060F2:          tst.b    DRAGGED
                    bne      lbC006108
                    btst     #2, JOYPOS
                    bne      lbC006160
lbC006108:          move.w   #$2D, ZOOL_SPR
                    rts

lbC006112:          tst.b    DRAGGED
                    bne      lbC006128
                    btst     #3, JOYPOS
                    bne      lbC006132
lbC006128:          move.w   #$35, ZOOL_SPR
                    rts

lbC006132:          subq.w   #1, ZOOL_ANIM
                    bpl      lbC00615E
                    move.w   #3, ZOOL_ANIM
                    addq.w   #1, ZOOL_SPR
                    cmp.w    #$10, ZOOL_SPR
                    bmi      lbC00615E
                    move.w   #8, ZOOL_SPR
lbC00615E:          rts

lbC006160:          subq.w   #1, ZOOL_ANIM
                    bpl.s    lbC00615E
                    move.w   #3, ZOOL_ANIM
                    addq.w   #1, ZOOL_SPR
                    cmp.w    #8, ZOOL_SPR
                    bmi.s    lbC00615E
                    move.w   #0, ZOOL_SPR
                    rts

INIT_SLIDE:         move.w   #5, ZOOL_MOVE
                    clr.w    ZOOL_ANIM
                    clr.b    INAIR
                    move.w   #2, X_FRICTION
                    bsr      GET_GRADIENT
                    clr.w    SLIDE_SND
                    bra      ZOOL_SLIDE

ZOOL_PUNCH:         subq.w   #1, ZOOL_ANIM
                    bne      lbC00620C
                    move.w   #3, ZOOL_ANIM
                    tst.b    WHIP_ON
                    beq      lbC0061D8
                    move.w   #5, ZOOL_ANIM
lbC0061D8:          tst.b    ZOOL_FACE
                    beq      lbC0061F8
                    cmp.w    #$1D, ZOOL_SPR
                    beq      _WALL_PUNCH
                    move.w   #$1D, ZOOL_SPR
                    rts

lbC0061F8:          cmp.w    #$14, ZOOL_SPR
                    beq      _WALL_PUNCH
                    move.w   #$14, ZOOL_SPR
lbC00620C:          rts

_WALL_PUNCH:        bsr      WALL_PUNCH
                    sf       WHIP_ON
                    bra      INIT_STAND

INIT_PUNCH:         btst     #7, OLDJOY
                    bne      lbC006260
                    move.w   #3, ZOOL_ANIM
                    bsr      ZOOL_SHOOTA
                    bset     #7, OLDJOY
                    move.w   #6, ZOOL_MOVE
                    tst.b    ZOOL_FACE
                    beq      lbC006258
                    move.w   #$1C, ZOOL_SPR
                    rts

lbC006258:          move.w   #$13, ZOOL_SPR
lbC006260:          addq.w   #1, FIRE_REPT
                    rts

WALL_PUNCH:         move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sub.w    #$24, d1
                    add.w    #14, d0
                    tst.b    ZOOL_FACE
                    bne      _CHECK_TILE2
                    sub.w    #$1C, d0
_CHECK_TILE2:       bsr      CHECK_TILE2
                    tst.b    CUSTOM_ON
                    bne      lbC006312
                    cmp.b    #8, d7
                    bne      lbC006312
                    add.l    #$19, SCORE
                    bsr      GETXY_A0
                    move.w   #$200, d7
                    move.w   #$103, d2
                    move.b   END_TILEFX, d3
                    move.w   FILLTILE_SPR, d4
                    bsr      ADD_TILEFX
                    add.w    #$10, d0
                    tst.b    ZOOL_FACE
                    bne      lbC0062D8
                    sub.w    #$20, d0
lbC0062D8:          add.w    #$10, d0
                    bsr      CHECK_TILE2
                    sub.w    #$10, d0
                    tst.b    CUSTOM_ON
                    bne      lbC006312
                    cmp.b    #8, d7
                    bne      lbC006312
                    move.w   #$208, d7
                    move.w   #$103, d2
                    move.w   FILLTILE_SPR, d4
                    move.b   END_TILEFX, d3
                    move.w   #$103, d2
                    bsr      ADD_TILEFX
lbC006312:          rts

INIT_SKID:          cmp.w    #$FFF0, X_FORCE
                    bmi      lbC006330
                    cmp.w    #$10, X_FORCE
                    bgt      lbC006330
                    bra      INIT_STAND

lbC006330:          move.w   #7, ZOOL_MOVE
                    clr.w    ZOOL_ANIM
                    move.w   X_FORCE, d7
                    bpl      lbC00634A
                    neg.w    d7
lbC00634A:          lea      SKID_FX, a5
                    cmp.w    #$30, d7
                    bpl      _ADD_SFX
                    lea      SKID2_FX, a5
                    cmp.w    #$18, d7
                    bpl      _ADD_SFX
                    lea      SKID3_FX, a5
_ADD_SFX:           jsr      ADD_SFX
                    bra      ZOOL_SKID

ZOOL_SKID:          btst     #7, JOYPOS
                    beq      lbC006396
                    bset     #7, OLDJOY
                    bne      lbC00639C
                    bsr      ZOOL_SHOOTA
                    bra      lbC00639C

lbC006396:          sf       OLDJOY
lbC00639C:          btst     #0, JOYPOS
                    bne      INIT_JUMP
                    tst.w    X_FORCE
                    beq      INIT_STAND
                    tst.b    ZOOL_FACE
                    beq      lbC0063DE
                    move.w   #$32, ZOOL_SPR
                    btst     #2, JOYPOS
                    beq      lbC0063DC
                    sub.w    #12, X_FORCE
                    ble      INIT_RUN
lbC0063DC:          rts

lbC0063DE:          move.w   #$2A, ZOOL_SPR
                    btst     #3, JOYPOS
                    beq.s    lbC0063DC
                    add.w    #12, X_FORCE
                    bpl      INIT_RUN
                    rts

INIT_SIDEK:         move.w   #2, X_FRICTION
                    move.w   #8, ZOOL_MOVE
                    move.w   #4, ZOOL_ANIM
                    move.w   #$15, ZOOL_SPR
                    tst.b    ZOOL_FACE
                    beq      lbC006430
                    move.w   #$1E, ZOOL_SPR
lbC006430:          move.w   X_FORCE, d7
                    bpl      lbC00643C
                    neg.w    d7
lbC00643C:          tst.b    SLIPPY_SLP
                    bne      lbC00646E
                    lea      SKID_FX, a5
                    cmp.w    #$30, d7
                    bpl      _ADD_SFX0
                    lea      SKID2_FX, a5
                    cmp.w    #$18, d7
                    bpl      _ADD_SFX0
                    lea      SKID3_FX, a5
_ADD_SFX0:          jsr      ADD_SFX
lbC00646E:          rts

ZOOL_SIDEK:         subq.w   #1, ZOOL_ANIM
                    bne      lbC0064CE
                    btst     #7, JOYPOS
                    beq      lbC00649A
                    bset     #7, OLDJOY
                    bne      lbC0064A2
                    bsr      ZOOL_SHOOTB
                    bra      lbC0064A2

lbC00649A:          bclr     #7, OLDJOY
lbC0064A2:          btst     #1, JOYPOS
                    beq      INIT_RUN
                    move.w   #1, ZOOL_ANIM
                    bsr      WALL_PUNCH
                    tst.w    X_FORCE
                    bne      lbC0064CE
                    bsr      INIT_CROUCH
                    addq.w   #1, ZOOL_SPR
lbC0064CE:          rts

INIT_CROUCH:        move.w   #ZOOL_CROUCHING, ZOOL_MOVE
                    move.w   #3, ZOOL_ANIM
                    move.w   #$26, ZOOL_SPR
                    tst.b    ZOOL_FACE
                    beq      lbC0064FA
                    move.w   #$2E, ZOOL_SPR
lbC0064FA:          rts

ZOOL_CROUCH:        btst     #1, JOYPOS
                    beq      _INIT_RUN
                    btst     #2, JOYPOS
                    beq      lbC006530
                    sf       ZOOL_FACE
                    cmp.w    #$2E, ZOOL_SPR
                    bmi      lbC006554
                    subq.w   #8, ZOOL_SPR
                    bra      lbC006554

lbC006530:          btst     #3, JOYPOS
                    beq      lbC006554
                    st       ZOOL_FACE
                    cmp.w    #$2E, ZOOL_SPR
                    bpl      lbC006554
                    addq.w   #8, ZOOL_SPR
lbC006554:          btst     #7, JOYPOS
                    beq      lbC006574
                    bset     #7, OLDJOY
                    bne      lbC0065B0
                    bsr      ZOOL_SHOOTB
                    bra      INIT_SIDEK

lbC006574:          bclr     #7, OLDJOY
                    tst.b    BIGB_POW
                    beq      lbC0065A6
                    cmp.w    #10, FIRE_REPT
                    bmi      lbC0065A6
                    st       BIG_BOMB
                    clr.w    FIRE_REPT
                    bsr      ZOOL_SHOOTB
                    bra      INIT_SIDEK

lbC0065A6:          clr.w    FIRE_REPT
                    bra      lbC0065B6

lbC0065B0:          addq.w   #1, FIRE_REPT
lbC0065B6:          subq.w   #1, ZOOL_ANIM
                    bmi      lbC0065E2
                    beq      lbC0065E0
                    tst.b    ZOOL_FACE
                    beq      lbC0065D8
                    move.w   #$2F, ZOOL_SPR
                    rts

lbC0065D8:          move.w   #$27, ZOOL_SPR
lbC0065E0:          rts

lbC0065E2:          addq.w   #1, ZOOL_ANIM
                    rts

_INIT_RUN:          bra      INIT_RUN

INIT_RUN:           move.w   #ZOOL_RUNNING, ZOOL_MOVE
                    move.w   #4, X_FRICTION
                    clr.b    INAIR
                    clr.w    ZOOL_ANIM
                    clr.w    ZOOL_SPR
                    bsr      GET_FACE
                    tst.b    ZOOL_FACE
                    beq      ZOOL_RUN
                    move.w   #8, ZOOL_SPR
                    bra      ZOOL_RUN

GET_FACE:           btst     #2, JOYPOS
                    beq      lbC00663E
                    sf       ZOOL_FACE
                    rts

lbC00663E:          btst     #3, JOYPOS
                    beq      lbC006650
                    st       ZOOL_FACE
lbC006650:          rts

X_LIMIT:            cmp.w    #ZOOL_TUMBLING, ZOOL_MOVE
                    beq      lbC00669E
                    tst.b    INAIR
                    bne      lbC0066A0
                    tst.w    X_FORCE
                    beq      lbC00669E
                    bmi      lbC00668A
                    cmp.w    #$60, X_FORCE
                    bmi      lbC00669E
                    sub.w    #$10, X_FORCE
lbC00668A:          cmp.w    #$FFA0, X_FORCE
                    bgt      lbC00669E
                    add.w    #$10, X_FORCE
lbC00669E:          rts

lbC0066A0:          tst.b    GRAV_WAIT
                    bne      lbC0066E8
                    cmp.w    #12, ZOOL_MOVE
                    beq      lbC00671A
                    tst.w    X_FORCE
                    beq.s    lbC00669E
                    bmi      lbC0066D4
                    cmp.w    #$40, X_FORCE
                    bmi.s    lbC00669E
                    sub.w    #$1C, X_FORCE
lbC0066D4:          cmp.w    #$FFC0, X_FORCE
                    bgt.s    lbC00669E
                    add.w    #$1C, X_FORCE
                    rts

lbC0066E8:          tst.w    X_FORCE
                    beq.s    lbC00669E
                    bmi      lbC006706
                    cmp.w    #128, X_FORCE
                    bmi.s    lbC00669E
                    sub.w    #28, X_FORCE
lbC006706:          cmp.w    #$FF80, X_FORCE
                    bgt.s    lbC00669E
                    add.w    #28, X_FORCE
                    rts

lbC00671A:          tst.w    X_FORCE
                    beq      lbC00669E
                    bmi      lbC00673C
                    cmp.w    #96, X_FORCE
                    bmi      lbC00669E
                    sub.w    #28, X_FORCE
lbC00673C:          cmp.w    #-96, X_FORCE
                    bgt      lbC00669E
                    add.w    #28, X_FORCE
                    rts

INIT_GRIP:          move.w   #ZOOL_GRIPPING, ZOOL_MOVE
                    clr.w    Y_FORCE
                    ;clr.w    ZOOL_YDIS                 ; produced a bug if zool was too near from a wall
                    clr.b    INAIR
                    move.b   JOYPOS, OLDJOY
                    clr.w    ZOOL_ANIM
                    sf       PAUSE_MOVE
                    clr.w    SLIDE_SND
                    rts

ZOOL_GRIP:          bsr      GRIP_ACT
                    cmp.w    #ZOOL_GRIPPING, ZOOL_MOVE
                    beq.b    ZOOL_CLIMB
                    cmp.w    #10, ZOOL_MOVE
                    bne.b    lbC0067AC
                    clr.w    Y_FORCE
lbC0067AC:          rts

ZOOL_CLIMB:         tst.w    Y_FORCE
                    beq      lbC006832
                    bpl      lbC006834
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    subq.w   #2, d0
                    tst.b    ZOOL_FACE
                    beq      lbC0067D6
                    addq.w   #4, d0
lbC0067D6:          sub.w    #$24, d1
                    bsr      CHECK_TILE2
                    tst.b    d7
                    beq      lbC006832
                    cmp.b    #$47, d7
                    bmi      lbC0067F4
                    cmp.b    #$4D, d7
                    ble      lbC006832
lbC0067F4:          cmp.b    #11, d7
                    beq      lbC006832
                    cmp.b    #$16, d7
                    beq      lbC006832
                    cmp.b    #12, d7
                    beq      lbC006832
                    move.w   #$12, ZOOL_SPR
                    tst.b    ZOOL_FACE
                    beq      lbC006826
                    move.w   #$1B, ZOOL_SPR
lbC006826:          sf       SLIP_WALL
                    clr.w    Y_FORCE
lbC006832:          rts

lbC006834:          move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    subq.w   #2, d0
                    tst.b    ZOOL_FACE
                    beq      lbC00684E
                    addq.w   #4, d0
lbC00684E:          add.w    #$12, d1
                    bsr      CHECK_TILE2
                    tst.b    d7
                    beq.s    lbC006832
                    cmp.b    #$47, d7
                    bmi      lbC006868
                    cmp.b    #$4D, d7
                    ble.s    lbC006832
lbC006868:          cmp.b    #11, d7
                    beq.s    lbC006832
                    cmp.b    #$16, d7
                    beq.s    lbC006832
                    cmp.b    #12, d7
                    beq.s    lbC006832
                    cmp.b    #$12, d7
                    beq.s    lbC006832
                    cmp.b    #$13, d7
                    beq.s    lbC006832
                    clr.w    Y_FORCE
                    sf       SLIP_WALL
                    bra      NORM_GRND

                    rts

GRIP_ACT:           move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sf       GRIP_FIRE
                    btst     #7, JOYPOS
                    beq      lbC0068BC
                    st       GRIP_FIRE
lbC0068BC:          tst.b    ZOOL_FACE
                    beq      lbC00695E
                    btst     #2, JOYPOS
                    bne      lbC0069F2
                    sf       STOP_OFF
                    tst.b    AGRIP_WALL
                    bne      lbC006A1E
                    tst.b    SLIP_WALL
                    beq      lbC006A52
                    subq.w   #1, SLIDE_SND
                    bpl      lbC00690A
                    move.w   #8, SLIDE_SND
                    lea      SLIDE_FX, a5
                    jsr      ADD_SFX
lbC00690A:          add.w    #4, Y_FORCE
                    cmp.w    #$60, Y_FORCE
                    ble      lbC006926
                    move.w   #$60, Y_FORCE
lbC006926:          add.w    #14, d0
                    bsr      CHECK_TILE2
                    tst.b    d7
                    beq      lbC0069E0
                    cmp.b    #13, d7
                    beq      lbC006A1C
                    cmp.b    #14, d7
                    beq      lbC006A1C
lbC006944:          sf       SLIP_WALL
                    move.w   #2, ZOOL_MOVE
                    clr.w    ZOOL_SPR
                    bra      ZOOL_INAIR

lbC00695E:          btst     #3, JOYPOS
                    bne      lbC0069F2
                    sf       STOP_OFF
                    tst.b    AGRIP_WALL
                    bne      lbC006A38
                    tst.b    SLIP_WALL
                    beq      lbC006A60
                    subq.w   #1, SLIDE_SND
                    bpl      lbC0069A2
                    move.w   #8, SLIDE_SND
                    lea      SLIDE_FX, a5
                    jsr      ADD_SFX
lbC0069A2:          add.w    #4, Y_FORCE
                    cmp.w    #$60, Y_FORCE
                    ble      lbC0069BE
                    move.w   #$60, Y_FORCE
lbC0069BE:          sub.w    #15, d0
                    bsr      CHECK_TILE2
                    tst.b    d7
                    beq      lbC0069E0
                    cmp.b    #13, d7
                    beq      lbC006A1C
                    cmp.b    #14, d7
                    beq      lbC006A1C
                    bra      lbC006944

lbC0069E0:          move.w   Y_FORCE, -(sp)
                    bsr      _INIT_JUMP
                    move.w   (sp)+, Y_FORCE
                    rts

lbC0069F2:          tst.b    STOP_OFF
                    bne      lbC006A1C
                    tst.b    SLIP_WALL
                    bne.s    lbC0069E0
_INIT_JUMP:         bsr      INIT_JUMP
                    btst     #0, JOYPOS
                    bne      lbC006A1C
                    add.w    #$A0, Y_FORCE
lbC006A1C:          rts

lbC006A1E:          bsr      lbC006A52
                    tst.b    GRIP_FIRE
                    beq.s    lbC006A1C
                    btst     #3, JOYPOS
                    beq.s    lbC006A1C
                    bra      INIT_SWAP

lbC006A38:          bsr      lbC006A60
                    tst.b    GRIP_FIRE
                    beq.s    lbC006A1C
                    btst     #2, JOYPOS
                    beq.s    lbC006A1C
                    bra      INIT_SWAP

lbC006A52:          clr.w    Y_FORCE
                    add.w    #14, d0
                    bra      lbC006A6A

lbC006A60:          sub.w    #14, d0
                    clr.w    Y_FORCE
lbC006A6A:          btst     #0, JOYPOS
                    beq      lbC006B2C
                    move.w   #$FFE0, Y_FORCE
                    subq.w   #2, d1
                    bsr      CHECK_TILE2
                    tst.b    d7
                    beq      lbC006BDE
                    cmp.b    #4, d7
                    bne      lbC006A98
                    st       AGRIP_WALL
lbC006A98:          cmp.b    #6, d7
                    beq      lbC006BDE
                    cmp.b    #$1E, d7
                    beq      lbC006BDE
                    cmp.b    #$1B, d7
                    beq      lbC006BDE
                    cmp.b    #$15, d7
                    beq      lbC006BFC
                    cmp.b    #13, d7
                    beq      lbC006BC6
lbC006AC0:          subq.w   #1, ZOOL_ANIM
                    bpl      lbC006A1C
                    move.w   #6, ZOOL_ANIM
                    addq.w   #1, ZOOL_SPR
                    cmp.w    #$49, ZOOL_SPR
                    bne      lbC006AEE
                    move.w   #$45, ZOOL_SPR
                    rts

lbC006AEE:          cmp.w    #$53, ZOOL_SPR
                    bne      lbC006B04
                    move.w   #$4F, ZOOL_SPR
                    rts

lbC006B04:          cmp.w    #$45, ZOOL_SPR
                    bpl      lbC006A1C
                    move.w   #$45, ZOOL_SPR
                    tst.b    ZOOL_FACE
                    beq      lbC006A1C
                    move.w   #$4F, ZOOL_SPR
                    rts

lbC006B2C:          btst     #1, JOYPOS
                    beq      lbC006BDE
                    move.w   #$30, Y_FORCE
                    addq.w   #5, d1
                    bsr      CHECK_TILE2
                    tst.b    d7
                    beq      lbC006BDE
                    cmp.b    #4, d7
                    bne      lbC006B5A
                    st       AGRIP_WALL
lbC006B5A:          cmp.b    #6, d7
                    beq      lbC006BDE
                    cmp.b    #$1E, d7
                    beq      lbC006BDE
                    cmp.b    #$1B, d7
                    beq      lbC006BDE
                    cmp.b    #$15, d7
                    beq      lbC006BFC
                    cmp.b    #13, d7
                    beq      lbC006BD6
lbC006B82:          subq.w   #1, ZOOL_ANIM
                    bpl      lbC006A1C
                    move.w   #5, ZOOL_ANIM
                    subq.w   #1, ZOOL_SPR
                    cmp.w    #$44, ZOOL_SPR
                    bne      lbC006BB0
                    move.w   #$48, ZOOL_SPR
                    rts

lbC006BB0:          cmp.w    #$4E, ZOOL_SPR
                    bne      lbC006B04
                    move.w   #$52, ZOOL_SPR
                    rts

lbC006BC6:          clr.w    Y_FORCE
                    subq.w   #1, ZOOL_ANIM
                    bra      lbC006AC0

lbC006BD6:          st       SLIP_WALL
                    bra.s    lbC006B82

lbC006BDE:          move.w   #$1B, ZOOL_SPR
                    tst.b    ZOOL_FACE
                    bne      INIT_GRIP
                    move.w   #$12, ZOOL_SPR
                    bra      INIT_GRIP

lbC006BFC:          tst.b    ZOOL_FACE
                    bne      BAD_WALL_R
                    bra      BAD_WALL_L

INIT_SWAP:          move.w   #10, ZOOL_MOVE
                    sf       AGRIP_WALL
                    st       STOP_OFF
                    rts

ZOOL_SWGRIP:        tst.b    ZOOL_FACE
                    beq      lbC006C8A
                    btst     #3, JOYPOS
                    bne      lbC006C3C
                    sf       STOP_OFF
lbC006C3C:          lea      GRIPSWR_AN, a0
                    moveq    #0, d7
                    move.b   AGRIP_WALL, d7
                    add.w    d7, d7
                    move.w   0(a0,d7.w), ZOOL_SPR
                    bmi      lbC006C66
                    addq.b   #1, AGRIP_WALL
                    addq.w   #3, ZOOL_SCRX
                    rts

lbC006C66:          st       AGRIP_WALL
                    sf       ZOOL_FACE
                    move.w   #$12, ZOOL_SPR
                    lea      FSTEP_FX, a5
                    jsr      ADD_SFX
                    bra      INIT_GRIP

lbC006C8A:          btst     #2, JOYPOS
                    bne      lbC006C9C
                    sf       STOP_OFF
lbC006C9C:          lea      GRIPSWL_AN, a0
                    moveq    #0, d7
                    move.b   AGRIP_WALL, d7
                    add.w    d7, d7
                    move.w   0(a0,d7.w), ZOOL_SPR
                    bmi      lbC006CC6
                    addq.b   #1, AGRIP_WALL
                    subq.w   #3, ZOOL_SCRX
                    rts

lbC006CC6:          st       AGRIP_WALL
                    st       ZOOL_FACE
                    move.w   #$1B, ZOOL_SPR
                    lea      FSTEP_FX, a5
                    jsr      ADD_SFX
                    bra      INIT_GRIP

                    rts

ENV_FORCES:         tst.b    INAIR
                    beq      lbC006D2E
                    cmp.w    #ZOOL_TUMBLING, ZOOL_MOVE
                    beq      lbC006D9C
                    cmp.w    #$36, ZOOL_SPR
                    bpl      lbC006D9E
lbC006D0E:          move.w   GRAVITY, d1
                    add.w    d1, Y_FORCE
                    cmp.w    #$80, Y_FORCE
                    ble      lbC006D2E
                    move.w   #$80, Y_FORCE
lbC006D2E:          move.w   Y_FRICTION, d1
                    tst.w    Y_FORCE
                    beq      lbC006D66
                    bmi      lbC006D56
                    sub.w    d1, Y_FORCE
                    bpl      lbC006D66
                    clr.w    Y_FORCE
                    bra      lbC006D66

lbC006D56:          add.w    d1, Y_FORCE
                    ble      lbC006D66
                    clr.w    Y_FORCE
lbC006D66:          move.w   X_FRICTION, d1
                    tst.w    X_FORCE
                    beq      lbC006D9C
                    bmi      lbC006D8C
                    sub.w    d1, X_FORCE
                    bpl      lbC006D9C
                    clr.w    X_FORCE
                    rts

lbC006D8C:          add.w    d1, X_FORCE
                    ble      lbC006D9C
                    clr.w    X_FORCE
lbC006D9C:          rts

lbC006D9E:          subq.b   #1, GRAV_WAIT
                    bpl.s    lbC006D9C
                    sf       GRAV_WAIT
                    bra      lbC006D0E

                    rts

DEF_MAPXY:          move.w   MAP_LINE, d0
                    add.w    ZOOL_SCRY, d0
                    move.w   d0, ZOOL_Y
                    move.w   XPOS, d0
                    add.w    #$10, d0
                    add.w    ZOOL_SCRX, d0
                    move.w   d0, ZOOL_X
                    rts

ZOOL_COL:           cmp.w    #ZOOL_TUMBLING, ZOOL_MOVE
                    beq      lbC006E26
                    tst.b    MAINGUY_ON
                    beq      lbC006E26
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    move.w   ZOOL_XDIS, d5
                    move.w   ZOOL_YDIS, d6
                    tst.b    INAIR
                    bne      _HORZ_COLA
                    sf       PAUSE_MOVE
                    bra      HORZ_COLG

_HORZ_COLA:         bsr      HORZ_COLA
                    bra      VERT_COLA

lbC006E26:          rts

HORZ_COLA:          move.w   d0, -(sp)
                    bsr      CHK_WALLA
                    move.w   (sp)+, d0
                    add.w    d5, d0
                    rts

CHK_WALLA:          add.w    d6, d1
                    add.w    d5, d0
                    tst.w    d5
                    beq      lbC007010
                    sf       AGRIP_WALL
                    bmi      lbC006F7C
                    add.w    #9, d0
                    subq.w   #4, d1
                    bsr      CHECK_TILE2
                    addq.w   #4, d1
                    tst.b    CUSTOM_ON
                    bne      lbC006EC8
                    cmp.b    #6, d7
                    beq      lbC006F6C
                    cmp.b    #$1E, d7
                    beq      lbC006F6C
                    cmp.b    #$4E, d7
                    beq      lbC006F6C
                    cmp.b    #$4F, d7
                    beq      lbC006F6C
                    cmp.b    #$1B, d7
                    beq      lbC006F6C
                    cmp.b    #4, d7
                    beq      lbC006F20
                    cmp.b    #1, d7
                    beq      lbC006F34
                    cmp.b    #8, d7
                    beq      lbC006F34
                    cmp.b    #5, d7
                    beq      lbC006F34
                    cmp.b    #13, d7
                    beq      lbC006EFC
                    cmp.b    #14, d7
                    beq      lbC006EFC
                    cmp.b    #$15, d7
                    beq      BAD_WALL_R
                    cmp.b    #$51, d7
                    bpl      lbC006EDC
                    rts

lbC006EC8:          move.w   d1, d4
                    and.w    #15, d4
                    neg.w    d4
                    add.w    #15, d4
                    sub.w    d7, d4
                    ble      lbC006F6C
                    rts

lbC006EDC:          cmp.b    #$65, d7
                    bpl      lbC007010
                    cmp.b    #$5B, d7
                    bpl      lbC007010
                    tst.w    d6
                    bpl      SPRING_LEFT
                    move.w   d5, d4
                    add.w    d6, d4
                    bgt      SPRING_LEFT
                    rts

lbC006EFC:          move.w   Y_FORCE, -(sp)
                    move.w   ZOOL_YDIS, -(sp)
                    bsr      lbC006F34
                    move.w   (sp)+, ZOOL_YDIS
                    move.w   (sp)+, Y_FORCE
                    st       SLIP_WALL
                    rts

lbC006F20:          subq.w   #7, d0
                    bsr      CHECK_TILE2
                    tst.b    d7
                    bne      lbC007010
                    st       AGRIP_WALL
                    addq.w   #7, d0
lbC006F34:          move.w   ZOOL_X, d7
                    and.w    #$FFF0, d0
                    subq.w   #8, d0
                    sub.w    d7, d0
                    add.w    d0, ZOOL_SCRX
                    sf       SLIP_WALL
                    move.w   #$1B, ZOOL_SPR
                    lea      FSTEP_FX, a5
                    jsr      ADD_SFX
                    bsr      INIT_GRIP
                    st       ZOOL_FACE
lbC006F6C:          clr.w    ZOOL_XDIS
                    clr.w    X_FORCE
                    clr.w    d5
                    rts

lbC006F7C:          sub.w    #10, d0
                    subq.w   #4, d1
                    bsr      CHECK_TILE2
                    addq.w   #4, d1
                    tst.b    CUSTOM_ON
                    bne      lbC006EC8
                    cmp.b    #6, d7
                    beq.s    lbC006F6C
                    cmp.b    #$1E, d7
                    beq.s    lbC006F6C
                    cmp.b    #$4E, d7
                    beq.s    lbC006F6C
                    cmp.b    #$4F, d7
                    beq.s    lbC006F6C
                    cmp.b    #$1B, d7
                    beq.s    lbC006F6C
                    cmp.b    #1, d7
                    beq      lbC007048
                    cmp.b    #4, d7
                    beq      lbC007036
                    cmp.b    #8, d7
                    beq      lbC007048
                    cmp.b    #5, d7
                    beq      lbC007048
                    cmp.b    #13, d7
                    beq      lbC007012
                    cmp.b    #14, d7
                    beq      lbC007012
                    cmp.b    #$15, d7
                    beq      BAD_WALL_L
                    cmp.b    #$51, d7
                    bpl      lbC006FF2
                    rts

lbC006FF2:          cmp.b    #$65, d7
                    bpl      lbC007010
                    cmp.b    #$5B, d7
                    bmi      lbC007010
                    tst.w    d6
                    bpl      SPRING_RIGHT
                    move.w   d5, d4
                    sub.w    d6, d4
                    bmi      SPRING_RIGHT
lbC007010:          rts

lbC007012:          move.w   Y_FORCE, -(sp)
                    move.w   ZOOL_YDIS, -(sp)
                    bsr      lbC007048
                    move.w   (sp)+, ZOOL_YDIS
                    move.w   (sp)+, Y_FORCE
                    st       SLIP_WALL
                    rts

lbC007036:          addq.w   #6, d0
                    bsr      CHECK_TILE2
                    tst.b    d7
                    bne.s    lbC007010
                    subq.w   #6, d0
                    st       AGRIP_WALL
lbC007048:          move.w   ZOOL_X, d7
                    and.w    #$FFF0, d0
                    add.w    #$14, d0
                    sub.w    d7, d0
                    add.w    d0, ZOOL_SCRX
                    sf       SLIP_WALL
                    move.w   #$12, ZOOL_SPR
                    lea      FSTEP_FX, a5
                    jsr      ADD_SFX
                    bsr      INIT_GRIP
                    sf       ZOOL_FACE
                    bra      lbC006F6C

CHK_WALLG:          add.w    d5, d0
                    tst.w    d5
                    beq      lbC0071B8
                    bmi      lbC007152
                    add.w    #9, d0
                    bsr      CHECK_TILE2
                    sub.w    #9, d0
                    tst.b    CUSTOM_ON
                    bne      lbC0071B8
                    cmp.b    #1, d7
                    beq      lbC0070FA
                    cmp.b    #13, d7
                    beq      lbC0070FA
                    cmp.b    #14, d7
                    beq      lbC0070FA
                    cmp.b    #6, d7
                    beq      lbC0070FA
                    cmp.b    #$1E, d7
                    beq      lbC0070FA
                    cmp.b    #$4E, d7
                    beq      lbC0070FA
                    cmp.b    #$4F, d7
                    beq      lbC0070FA
                    cmp.b    #$1B, d7
                    beq      lbC0070FA
                    cmp.b    #8, d7
                    beq      lbC0070FA
                    cmp.b    #$15, d7
                    beq      BAD_WALL_R
                    rts

lbC0070FA:          move.w   d0, d7
                    add.w    #9, d7
                    and.w    #$FFF0, d7
                    sub.w    #9, d7
                    sub.w    XPOS, d7
                    sub.w    #$10, d7
                    move.w   d7, ZOOL_SCRX
                    cmp.w    #$40, X_FORCE
                    ble      lbC007142
                    btst     #7, JOYPOS
                    beq      lbC007142
                    neg.w    X_FORCE
                    neg.w    ZOOL_XDIS
                    clr.w    d5
                    bsr      INIT_FLIP
lbC007142:          clr.w    ZOOL_XDIS
                    clr.w    X_FORCE
                    clr.w    d5
                    rts

lbC007152:          sub.w    #10, d0
                    bsr      CHECK_TILE2
                    add.w    #10, d0
                    tst.b    CUSTOM_ON
                    bne      lbC0071B8
                    cmp.b    #1, d7
                    beq      lbC0071BA
                    cmp.b    #6, d7
                    beq      lbC0071BA
                    cmp.b    #$1E, d7
                    beq      lbC0071BA
                    cmp.b    #$4E, d7
                    beq      lbC0071BA
                    cmp.b    #$4F, d7
                    beq      lbC0071BA
                    cmp.b    #13, d7
                    beq      lbC0071BA
                    cmp.b    #14, d7
                    beq      lbC0071BA
                    cmp.b    #$1B, d7
                    beq      lbC0071BA
                    cmp.b    #8, d7
                    beq      lbC0071BA
                    cmp.b    #$15, d7
                    beq      BAD_WALL_L
lbC0071B8:          rts

lbC0071BA:          move.w   d0, d7
                    sub.w    #10, d7
                    and.w    #$FFF0, d7
                    add.w    #$1A, d7
                    sub.w    XPOS, d7
                    sub.w    #$10, d7
                    move.w   d7, ZOOL_SCRX
                    cmp.w    #$FFC0, X_FORCE
                    bpl      lbC007142
                    btst     #7, JOYPOS
                    beq      lbC007142
                    neg.w    X_FORCE
                    neg.w    ZOOL_XDIS
                    clr.w    d5
                    bsr      INIT_FLIP
                    bra      lbC007142

INIT_FLIP:          move.w   #12, ZOOL_MOVE
                    move.w   #2, ZOOL_ANIM
                    move.w   #$49, ZOOL_SPR
                    move.w   #1, X_FRICTION
                    addq.w   #6, ZOOL_SCRX
                    tst.b    ZOOL_FACE
                    beq      lbC007246
                    sub.w    #12, ZOOL_SCRX
                    move.w   #$53, ZOOL_SPR
lbC007246:          rts

BAD_WALL_R:         move.w   #$FFA0, X_FORCE
                    move.w   #$FFFA, ZOOL_XDIS
                    move.b   #$14, PAUSE_MOVE
                    bra      ZOOL_SHOCK

BAD_WALL_L:         move.w   #$60, X_FORCE
                    move.w   #6, ZOOL_XDIS
                    move.b   #$EC, PAUSE_MOVE
                    bra      ZOOL_SHOCK

ZOOL_SHOCK:         tst.w    SHIELD_ON
                    bne      lbC0072C0
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
                    move.b   #$50, ZOOL_HIT
                    bsr      INIT_JUMP
                    move.w   #$FF90, Y_FORCE
                    lea      ZLDAMA_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX1
                    lea      ZZDAMA_FX, a5
_ADD_SFX1:          jmp      ADD_SFX

lbC0072C0:          rts

TOKEN_COL:          move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    add.w    #8, d0
                    add.w    #$10, d1
                    move.l   REF_MAP, a0
                    moveq    #0, d3
                    move.w   MAP_WIDTH, d3
                    move.w   d3, d2
                    move.w   d1, d7
                    lsr.w    #4, d7
                    mulu     d7, d2
                    move.w   d0, d7
                    lsr.w    #4, d7
                    add.w    d7, d2
                    lea      0(a0,d2.w), a0
                    move.b   (a0), d7
                    bmi      lbC007342
                    cmp.b    #$22, d7
                    bne      lbC007326
                    tst.b    ZOOL_HIT
                    bne      lbC007326
                    tst.b    SHADE_ON
                    bne      lbC007326
                    tst.w    SHIELD_ON
                    bne      lbC007326
                    bsr      lbC0074D0
lbC007326:          cmp.b    #$16, d7
                    beq      _COLLECT_TOK
                    cmp.b    #12, d7
                    beq      _COLLECT_TOK
                    cmp.b    #11, d7
                    bne      lbC007342
_COLLECT_TOK:       bsr      COLLECT_TOK
lbC007342:          sub.l    d3, a0
                    move.b   (a0), d7
                    bmi      lbC0073A2
                    cmp.w    #2, ZOOL_MOVE
                    bne      lbC00737A
                    cmp.w    #$2B, ZOOL_SPR
                    bmi      lbC00737A
                    cmp.b    #$12, d7
                    bne      lbC00737A
                    bsr      lbC007510
                    cmp.b    #$13, d7
                    bne      lbC00737A
                    bsr      lbC007510
lbC00737A:          cmp.b    #$22, d7
                    bne      lbC007386
                    bsr      lbC0074D0
lbC007386:          cmp.b    #$16, d7
                    beq      _COLLECT_TOK0
                    cmp.b    #12, d7
                    beq      _COLLECT_TOK0
                    cmp.b    #11, d7
                    bne      lbC0073A2
_COLLECT_TOK0:      bsr      COLLECT_TOK
lbC0073A2:          sub.l    d3, a0
                    move.b   (a0), d7
                    bmi      lbC0073F0
                    tst.w    Y_FORCE
                    bpl      lbC0073C8
                    cmp.b    #$47, d7
                    bmi      lbC0073C8
                    cmp.b    #$4D, d7
                    bgt      lbC0073C8
                    bsr      lbC007522
lbC0073C8:          cmp.b    #$22, d7
                    bne      lbC0073D4
                    bsr      lbC0074D0
lbC0073D4:          cmp.b    #$16, d7
                    beq      _COLLECT_TOK1
                    cmp.b    #12, d7
                    beq      _COLLECT_TOK1
                    cmp.b    #11, d7
                    bne      lbC0073F0
_COLLECT_TOK1:      bsr      COLLECT_TOK
lbC0073F0:          subq.l   #1, a0
                    move.b   (a0), d7
                    bmi      lbC00743E
                    tst.w    Y_FORCE
                    bpl      lbC007416
                    cmp.b    #$47, d7
                    bmi      lbC007416
                    cmp.b    #$4D, d7
                    bgt      lbC007416
                    bsr      lbC007522
lbC007416:          cmp.b    #$22, d7
                    bne      lbC007422
                    bsr      lbC0074D0
lbC007422:          cmp.b    #$16, d7
                    beq      _COLLECT_TOK2
                    cmp.b    #12, d7
                    beq      _COLLECT_TOK2
                    cmp.b    #11, d7
                    bne      lbC00743E
_COLLECT_TOK2:      bsr      COLLECT_TOK
lbC00743E:          add.l    d3, a0
                    move.b   (a0), d7
                    bmi      lbC00749E
                    cmp.w    #2, ZOOL_MOVE
                    bne      lbC007476
                    cmp.w    #$2B, ZOOL_SPR
                    bmi      lbC007476
                    cmp.b    #$12, d7
                    bne      lbC007476
                    bsr      lbC007510
                    cmp.b    #$13, d7
                    bne      lbC007476
                    bsr      lbC007510
lbC007476:          cmp.b    #$22, d7
                    bne      lbC007482
                    bsr      lbC0074D0
lbC007482:          cmp.b    #$16, d7
                    beq      _COLLECT_TOK3
                    cmp.b    #12, d7
                    beq      _COLLECT_TOK3
                    cmp.b    #11, d7
                    bne      lbC00749E
_COLLECT_TOK3:      bsr      COLLECT_TOK
lbC00749E:          add.l    d3, a0
                    move.b   (a0), d7
                    bmi      lbC0074CE
                    cmp.b    #$22, d7
                    bne      lbC0074B2
                    bsr      lbC0074D0
lbC0074B2:          cmp.b    #$16, d7
                    beq      _COLLECT_TOK4
                    cmp.b    #12, d7
                    beq      _COLLECT_TOK4
                    cmp.b    #11, d7
                    bne      lbC0074CE
_COLLECT_TOK4:      bsr      COLLECT_TOK
lbC0074CE:          rts

lbC0074D0:          tst.b    ZOOL_HIT
                    bne      lbC00750E
                    tst.w    SHIELD_ON
                    bne      lbC00750E
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
                    move.b   #$50, ZOOL_HIT
                    lea      ZLDAMA_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX2
                    lea      ZZDAMA_FX, a5
_ADD_SFX2:          jmp      ADD_SFX

lbC00750E:          rts

lbC007510:          move.l   a0, -(sp)
                    move.w   d7, -(sp)
                    move.l   d3, -(sp)
                    bsr      LOB_OFF
                    move.l   (sp)+, d3
                    move.w   (sp)+, d7
                    move.l   (sp)+, a0
                    rts

lbC007522:          move.l   a0, -(sp)
                    move.w   d7, -(sp)
                    move.l   d3, -(sp)
                    bsr      POWER_HIT
                    move.l   (sp)+, d3
                    move.w   (sp)+, d7
                    move.l   (sp)+, a0
                    rts

COLLECT_TOK:        cmp.b    #12, d7
                    beq      lbC0075E0
                    lea      COLLECT_FX, a5
                    jsr      ADD_SFX
lbC007548:          move.l   a0, -(sp)
                    move.w   d7, -(sp)
                    sf       (a0)
                    move.l   a0, d6
                    sub.l    REF_MAP, d6
                    move.l   CURRENT_MAP, a1
                    addq.l   #8, a1
                    move.l   d6, d5
                    lsl.l    #1, d5
                    add.l    d5, a1
                    move.w   FILL_TILE1, (a1)
                    cmp.b    #$16, d7
                    bne      lbC007578
                    move.w   FILL_TILE2, (a1)
lbC007578:          divu     d3, d6
                    move.w   d6, d1
                    swap     d6
                    move.w   d6, d0
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    cmp.b    #$16, d7
                    bne      lbC007598
                    move.w   FILLTILE_SP2, d7
                    bra      _ADD_PERM

lbC007598:          move.w   FILLTILE_SPR, d7
_ADD_PERM:          bsr      ADD_PERM
                    clr.w    d2
                    move.b   #1, d3
                    move.w   #$300, d7
                    move.w   FILLTILE_SPR, d4
                    bsr      ADD_TILEFX
                    addq.w   #1, TOKENS
                    move.w   PERC_ADD, d7
                    add.w    d7, PERCENT
                    add.l    #10, SCORE
                    moveq    #0, d3
                    move.w   MAP_WIDTH, d3
                    move.w   (sp)+, d7
                    move.l   (sp)+, a0
                    rts

lbC0075E0:          lea      CHUPPA_FX, a5
                    jsr      ADD_SFX
                    bsr      lbC007548
                    add.l    d3, a0
                    bsr      lbC007548
                    sub.l    d3, a0
                    rts

HORZ_COLG:          bsr      CHK_WALLG
                    tst.w    d5
                    beq      lbC007874
lbC007604:          sf       DRAGGED
                    sf       FEET_CHECK
                    add.w    #$10, d1
                    bsr      CHECK_TILET
                    tst.b    CUSTOM_ON
                    bne      lbC007744
                    sf       SLIPPY_SLP
                    tst.b    d7
                    beq      lbC0076C8
                    cmp.b    #$1C, d7
                    beq      lbC0076C8
                    cmp.b    #$23, d7
                    beq      lbC0076C8
                    cmp.b    #11, d7
                    beq      lbC0076C8
                    cmp.b    #12, d7
                    beq      lbC0076C8
                    cmp.b    #$16, d7
                    beq      lbC0076C8
                    cmp.b    #$12, d7
                    beq      lbC0076C8
                    cmp.b    #$13, d7
                    beq      lbC0076C8
                    cmp.b    #$11, d7
                    beq      lbC0076C8
                    cmp.b    #$20, d7
                    beq      lbC00768C
                    cmp.b    #$22, d7
                    beq      lbC0076C8
                    cmp.b    #$21, d7
                    beq      lbC0076C8
                    bra      NORM_GRND2

                    rts

lbC00768C:          move.b   #2, (a0)
                    sub.l    REF_MAP, a0
                    add.l    a0, a0
                    add.l    CURRENT_MAP, a0
                    addq.l   #8, a0
                    move.w   BRICK_TILE, (a0)
                    move.w   d0, -(sp)
                    move.w   d1, -(sp)
                    move.b   #2, d7
                    bsr      NORM_GRND2
                    move.w   (sp)+, d1
                    move.w   (sp)+, d0
                    bsr      ADD_BRICK
                    sub.w    #$10, d0
                    move.w   BRICKL_SPR, d7
                    bra      ADD_PERM

lbC0076C8:          add.w    #$10, d1
                    bsr      CHECK_TILET
                    tst.b    CUSTOM_ON
                    beq      lbC0076E2
                    add.b    #$10, d7
                    bra      lbC007758

lbC0076E2:          tst.b    d7
                    beq      INIT_FALL
                    cmp.b    #$1C, d7
                    beq      INIT_FALL
                    cmp.b    #$23, d7
                    beq      INIT_FALL
                    cmp.b    #11, d7
                    beq      INIT_FALL
                    cmp.b    #12, d7
                    beq      INIT_FALL
                    cmp.b    #$16, d7
                    beq      INIT_FALL
                    cmp.b    #$12, d7
                    beq      INIT_FALL
                    cmp.b    #$13, d7
                    beq      INIT_FALL
                    cmp.b    #$11, d7
                    beq      INIT_FALL
                    cmp.b    #$21, d7
                    beq      INIT_FALL
                    cmp.b    #$22, d7
                    beq      INIT_FALL
                    add.b    #$10, ZOOL_SCRY+1
                    bra      NORM_GRND2

lbC007744:          bra      lbC007758

                    bra      lbC0078E0

                    add.b    #$10, ZOOL_SCRY+1
                    bra      INIT_FALL

lbC007758:          add.b    d7, ZOOL_SCRY+1
                    move.b   (a2), d6
                    beq      lbC00786C
                    cmp.b    #2, d6
                    bmi      lbC007856
                    beq      lbC00784C
                    cmp.b    #4, d6
                    bmi      lbC0077D4
                    beq      lbC0077C2
                    cmp.b    #6, d6
                    bmi      lbC0077B2
                    btst     #1, JOYPOS
                    beq      lbC0077C2
                    sub.b    d7, ZOOL_SCRY+1
                    add.w    #$10, ZOOL_SCRY
                    add.w    #$10, d1
                    bsr      CHECK_TILE2
                    ext.w    d7
                    add.w    d7, ZOOL_SCRY
                    bra      lbC0077C2

lbC0077B2:          st       DRAGGED
                    sf       ZOOL_FACE
                    bra      lbC007856

lbC0077C2:          move.b   #1, DRAGGED
                    st       ZOOL_FACE
                    bra      lbC007856

lbC0077D4:          tst.w    X_FORCE
                    bmi      lbC007818
                    move.b   #$9C, PAUSE_MOVE
                    bsr      INIT_JUMP
                    move.w   X_FORCE, Y_FORCE
                    neg.w    Y_FORCE
                    sub.w    #$10, Y_FORCE
                    add.w    #$10, X_FORCE
                    st       ZOOL_FACE
                    st       AFTER_DRAG
                    rts

lbC007818:          move.b   #$64, PAUSE_MOVE
                    bsr      INIT_JUMP
                    move.w   X_FORCE, Y_FORCE
                    neg.w    Y_FORCE
                    sub.w    #$10, Y_FORCE
                    sub.w    #$10, X_FORCE
                    sf       ZOOL_FACE
                    rts

lbC00784C:          clr.w    X_FRICTION
                    bra      lbC00786C

lbC007856:          st       SLIPPY_SLP
                    cmp.w    #5, ZOOL_MOVE
                    beq      lbC007872
                    bra      INIT_SLIDE

lbC00786C:          sf       SLIPPY_SLP
lbC007872:          rts

lbC007874:          cmp.w    #5, ZOOL_MOVE
                    beq      lbC007604
                    cmp.w    #10, ZOOL_MOVE
                    beq.s    lbC007872
                    cmp.w    #ZOOL_GRIPPING, ZOOL_MOVE
                    beq.s    lbC007872
                    tst.b    FEET_CHECK
                    bne      lbC007604
                    add.w    #$10, d1
                    bsr      CHECK_TILE2
                    tst.b    CUSTOM_ON
                    bne.s    lbC007872
                    tst.b    d7
                    beq      lbC0076C8
                    rts

                    tst.w    d5
                    bmi      lbC0078CE
                    move.b   GRADIENT, d6
                    ble      INIT_FALL
                    add.b    #$10, d7
                    bra      lbC007758

lbC0078CE:          move.b   GRADIENT, d6
                    bpl      INIT_FALL
                    add.b    #$10, d7
                    bra      lbC007758

lbC0078E0:          tst.w    d5
                    bmi      lbC0078F4
                    move.b   GRADIENT, d6
                    ble      INIT_FALL
                    bra      lbC007758

lbC0078F4:          move.b   GRADIENT, d6
                    bpl      INIT_FALL
                    bra      lbC007758

GET_GRADIENT:       tst.b    INAIR
                    bne      lbC00796A
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    add.w    #$10, d1
                    addq.w   #2, d0
                    bsr      CHECK_TILET
                    tst.b    CUSTOM_ON
                    bne      lbC00793C
                    tst.b    d7
                    bne      lbC00793A
                    bsr      lbC007972
                    bra      lbC00793C

lbC00793A:          clr.b    d7
lbC00793C:          move.b   d7, GRADIENT
                    subq.w   #4, d0
                    bsr      CHECK_TILET
                    tst.b    CUSTOM_ON
                    bne      lbC007962
                    tst.b    d7
                    bne      lbC007960
                    bsr      lbC007972
                    bra      lbC007962

lbC007960:          clr.b    d7
lbC007962:          sub.b    d7, GRADIENT
                    rts

lbC00796A:          sf       GRADIENT
                    rts

lbC007972:          add.w    #$10, d1
                    bsr      CHECK_TILET
                    sub.w    #$10, d1
                    tst.b    CUSTOM_ON
                    beq      lbC00798E
                    add.b    #$10, d7
                    rts

lbC00798E:          tst.b    d7
                    beq      lbC007998
                    clr.b    d7
                    rts

lbC007998:          move.b   #$10, d7
                    rts

GRADIENT_FX:        move.b   GRADIENT, d7
                    beq      lbC0079C6
                    ext.w    d7
                    tst.b    SLIPPY_SLP
                    beq      lbC0079C8
                    tst.b    DRAGGED
                    bne      lbC0079C6
                    asl.w    #1, d7
                    add.w    d7, X_FORCE
lbC0079C6:          rts

lbC0079C8:          cmp.w    #1, ZOOL_MOVE
                    bne.s    lbC0079C6
                    btst     #2, JOYPOS
                    beq      lbC0079F0
                    add.w    d7, X_FORCE
                    bmi.s    lbC0079C6
                    move.w   #$FFFE, X_FORCE
                    rts

lbC0079F0:          btst     #3, JOYPOS
                    beq.s    lbC0079C6
                    add.w    d7, X_FORCE
                    bgt.s    lbC0079C6
                    move.w   #2, X_FORCE
                    rts

INIT_FALL:          move.w   GRAVITY, d7
                    add.w    d7, Y_FORCE
                    move.w   Y_FORCE, d5
                    bsr      INIT_JUMP
                    addq.w   #2, ZOOL_SPR
                    move.w   d5, Y_FORCE
                    move.w   X_FORCE, d7
                    tst.b    GRADIENT
                    beq      lbC007A52
                    bmi      lbC007A4A
                    move.w   d7, Y_FORCE
                    rts

lbC007A4A:          neg.w    d7
                    move.w   d7, Y_FORCE
lbC007A52:          rts

VERT_COLA:          tst.w    Y_FORCE
                    bpl      lbC007B2A
                    sub.w    #$14, d1
                    bsr      CHECK_TILE2
                    tst.b    CUSTOM_ON
                    bne      lbC007C5A
                    cmp.b    #$47, d7
                    bmi      lbC007A90
                    cmp.b    #$4D, d7
                    ble      lbC007A90
                    cmp.b    #$4E, d7
                    beq      lbC007ADA
                    cmp.b    #$4F, d7
                    beq      lbC007ADA
lbC007A90:          cmp.b    #1, d7
                    beq      NORM_ROOF
                    cmp.b    #6, d7
                    beq      NORM_ROOF
                    cmp.b    #$1B, d7
                    beq      NORM_ROOF
                    cmp.b    #9, d7
                    beq      ROOF_EXPL
                    cmp.b    #13, d7
                    beq      NORM_ROOF
                    cmp.b    #4, d7
                    beq      NORM_ROOF
                    cmp.b    #14, d7
                    beq      NORM_ROOF
                    cmp.b    #$10, d7
                    beq      NORM_ROOF
                    cmp.b    #$15, d7
                    beq      NORM_ROOF
                    rts

lbC007ADA:          lea      (a0), a1
                    move.w   d1, -(sp)
                    bsr      NORM_ROOF
                    move.w   (sp)+, d1
                    asr.w    #4, d0
                    asr.w    #4, d1
                    cmp.b    #$4E, d7
                    beq      CORN_BLK
                    bra      HIT_BLK

                    tst.b    ZOOL_HIT
                    bne      lbC007B28
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
                    move.b   #$50, ZOOL_HIT
                    lea      ZLDAMA_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX12
                    lea      ZZDAMA_FX, a5
_ADD_SFX12:         jsr      ADD_SFX
lbC007B28:          rts

lbC007B2A:          add.w    #$10, d1
                    bsr      CHECK_TILET
                    tst.b    CUSTOM_ON
                    bne      lbC007BF2
                    tst.b    d7
                    beq      lbC007C5A
                    cmp.b    #$20, d7
                    beq      lbC007C1E
                    cmp.b    #$21, d7
                    beq      lbC007C5A
                    cmp.b    #4, d7
                    beq      lbC007C5A
                    cmp.b    #$16, d7
                    beq      lbC007C5A
                    cmp.b    #$47, d7
                    bmi      lbC007B72
                    cmp.b    #$4D, d7
                    ble      lbC007C5A
lbC007B72:          cmp.b    #11, d7
                    beq      lbC007C5A
                    cmp.b    #$1C, d7
                    beq      lbC007C5A
                    cmp.b    #$23, d7
                    beq      lbC007C5A
                    cmp.b    #12, d7
                    beq      lbC007C5A
                    cmp.b    #$16, d7
                    beq      lbC007C5A
                    cmp.b    #$22, d7
                    beq      lbC007C5A
                    cmp.b    #$10, d7
                    beq      FLOOR_EXPL
                    cmp.b    #$12, d7
                    beq      LOB_OFF
                    cmp.b    #$13, d7
                    beq      LOB_OFF
                    cmp.b    #$51, d7
                    bpl      lbC007BDE
                    cmp.b    #$4E, d7
                    beq      NORM_GRND
                    cmp.b    #$4F, d7
                    beq      NORM_GRND
                    cmp.b    #$24, d7
                    bpl      lbC007C5A
                    bra      NORM_GRND

lbC007BDE:          cmp.b    #$65, d7
                    bpl      NORM_GRND
                    cmp.b    #$5B, d7
                    bpl      SPRING_RIGHT
                    bra      SPRING_LEFT

lbC007BF2:          tst.b    d7
                    bgt      lbC007C5A
                    sf       SLIPPY_SLP
                    tst.b    (a2)
                    beq      lbC007C12
                    st       SLIPPY_SLP
                    move.b   #12, SLIPPY_CNT
lbC007C12:          add.b    d6, d7
                    add.b    d7, ZOOL_SCRY+1
                    bra      GROUND_IT

lbC007C1E:          move.b   #2, (a0)
                    sub.l    REF_MAP, a0
                    add.l    a0, a0
                    add.l    CURRENT_MAP, a0
                    addq.l   #8, a0
                    move.w   BRICK_TILE, (a0)
                    move.w   d0, -(sp)
                    move.w   d1, -(sp)
                    move.b   #2, d7
                    bsr      NORM_GRND
                    move.w   (sp)+, d1
                    move.w   (sp)+, d0
                    bsr      ADD_BRICK
                    move.w   BRICKL_SPR, d7
                    sub.w    #$10, d0
                    bra      ADD_PERM

lbC007C5A:          rts

MAP_ERROR:          clr.b    d7
                    sf       CUSTOM_ON
                    rts

LOB_OFF:            cmp.w    #$2B, ZOOL_SPR
                    bmi      lbC007D5A
                    lea      BREAK_FX, a5
                    jsr      ADD_SFX
                    add.l    #15, SCORE
                    move.w   d0, -(sp)
                    move.w   d1, -(sp)
                    move.w   d5, -(sp)
                    move.w   d6, -(sp)
                    cmp.b    -1(a0), d7
                    bne      lbC007C9A
                    subq.l   #1, a0
lbC007C9A:          move.l   a0, d6
                    sf       (a0)+
                    sf       (a0)
                    sub.l    REF_MAP, d6
                    move.l   CURRENT_MAP, a1
                    addq.l   #8, a1
                    move.l   d6, d5
                    lsl.l    #1, d5
                    add.l    d5, a1
                    divu     d3, d6
                    move.w   d6, d1
                    swap     d6
                    move.w   d6, d0
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    add.w    SMASH_XOFF, d0
                    add.w    SMASH_YOFF, d1
                    cmp.b    #$12, d7
                    beq      lbC007D00
                    move.w   SMASH2_SPR, d7
                    bsr      ADD_PERM
                    cmp.b    #4, LEVEL_NUM
                    bpl      lbC007DFA
                    move.w   SMASH2L, (a1)+
                    move.w   SMASH2R, (a1)+
                    move.b   #0, d2
                    bra      lbC007D4A

lbC007D00:          move.w   END_PERMS, d7
                    bsr      ADD_PERM
                    cmp.b    #1, LEVEL_NUM
                    beq      lbC007D5C
                    cmp.b    #2, LEVEL_NUM
                    beq      lbC007D9A
                    cmp.b    #4, LEVEL_NUM
                    beq      lbC007DC0
                    cmp.b    #5, LEVEL_NUM
                    beq      lbC007DC0
                    move.w   SMASH1L, (a1)+
                    move.w   SMASH1R, (a1)+
                    move.b   #3, d2
lbC007D4A:          addq.w   #8, d0
                    addq.w   #4, d1
                    bsr      ARCH_EXPL
                    move.w   (sp)+, d6
                    move.w   (sp)+, d5
                    move.w   (sp)+, d1
                    move.w   (sp)+, d0
lbC007D5A:          rts

lbC007D5C:          moveq    #0, d7
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    sub.l    d7, a1
                    subq.l   #2, a1
                    lea      BULB_SMASH, a2
                    move.w   (a2)+, (a1)
                    move.w   (a2)+, 2(a1)
                    move.w   (a2)+, 4(a1)
                    add.l    d7, a1
                    move.w   (a2)+, (a1)
                    move.w   (a2)+, 2(a1)
                    move.w   (a2)+, 4(a1)
                    add.l    d7, a1
                    move.w   (a2)+, (a1)
                    move.w   (a2)+, 2(a1)
                    move.w   (a2)+, 4(a1)
                    move.b   #3, d2
                    subq.w   #4, d0
                    bra.s    lbC007D4A

lbC007D9A:          moveq    #0, d7
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    sub.l    d7, a1
                    lea      URN_SMASH, a2
                    move.w   (a2)+, (a1)
                    move.w   (a2)+, 2(a1)
                    add.l    d7, a1
                    move.w   (a2)+, (a1)
                    move.w   (a2)+, 2(a1)
                    move.b   #3, d2
                    bra.s    lbC007D4A

lbC007DC0:          moveq    #0, d7
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    sub.l    d7, a1
                    lea      ICE_SMASH1, a2
                    cmp.b    #4, LEVEL_NUM
                    beq      lbC007DE4
                    lea      LAST_SMASH1, a2
lbC007DE4:          move.w   (a2)+, (a1)
                    move.w   (a2)+, 2(a1)
                    add.l    d7, a1
                    move.w   (a2)+, (a1)
                    move.w   (a2)+, 2(a1)
                    move.b   #3, d2
                    bra      lbC007D4A

lbC007DFA:          moveq    #0, d7
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    sub.l    d7, a1
                    lea      ICE_SMASH2, a2
                    cmp.b    #4, LEVEL_NUM
                    beq      lbC007E1E
                    lea      LAST_SMASH2, a2
lbC007E1E:          move.w   (a2)+, (a1)
                    move.w   (a2)+, 2(a1)
                    add.l    d7, a1
                    move.w   (a2)+, (a1)
                    move.w   (a2)+, 2(a1)
                    move.b   #3, d2
                    bra      lbC007D4A

CHECK_TILE2:        sf       CUSTOM_ON
                    move.w   #$FFFF, CUST_TILE
                    move.l   REF_MAP, a0
                    move.w   MAP_WIDTH, d2
                    move.w   d1, d7
                    lsr.w    #4, d7
                    mulu     d7, d2
                    move.w   d0, d7
                    lsr.w    #4, d7
                    add.w    d7, d2
                    lea      0(a0,d2.w), a0
                    move.b   (a0), d7
                    bmi      lbC007E66
                    rts

lbC007E66:          st       CUSTOM_ON
                    move.w   d0, d2
                    and.w    #15, d2
                    move.w   d1, d3
                    and.w    #15, d3
                    move.l   CUST_ATTRBS, a2
                    move.l   CUST_REFS, a1
                    and.w    #$7F, d7
                    move.w   d7, CUST_TILE
                    lea      0(a2,d7.w), a2
                    lsl.w    #4, d7
                    lea      0(a1,d7.w), a1
                    move.b   0(a1,d2.w), d2
                    moveq    #15, d7
                    sub.b    d2, d7
                    sub.b    d3, d7
                    rts

CHECK_TILET:        sf       CUSTOM_ON
                    move.w   #$FFFF, CUST_TILE
                    move.l   REF_MAP, a0
                    move.w   MAP_WIDTH, d2
                    move.w   d1, d7
                    bmi      lbC007F02
                    lsr.w    #4, d7
                    mulu     d7, d2
                    move.w   d0, d7
                    bmi      lbC007F02
                    lsr.w    #4, d7
                    add.w    d7, d2
                    lea      0(a0,d2.w), a0
                    move.b   #15, d4
                    move.b   (a0), d7
                    bmi      lbC007F06
                    cmp.b    #3, d7
                    beq      lbC007F50
                    cmp.b    #5, d7
                    beq      lbC007F50
                    cmp.b    #$1B, d7
                    beq      lbC007F50
                    cmp.b    #14, d7
                    beq      lbC007F50
                    rts

lbC007F02:          sf       d7
                    rts

lbC007F06:          st       CUSTOM_ON
lbC007F0C:          move.w   d0, d2
                    and.w    #15, d2
                    move.w   d1, d3
                    and.w    #15, d3
                    move.l   CUST_ATTRBS, a2
                    move.l   CUST_REFS, a1
                    and.w    #$7F, d7
                    move.w   d7, CUST_TILE
                    lea      0(a2,d7.w), a2
                    lsl.w    #4, d7
                    lea      0(a1,d7.w), a1
                    move.b   0(a1,d2.w), d2
                    beq      lbC007F72
                    btst     #5, d2
                    bne      lbC007F50
                    move.b   d4, d7
                    sub.b    d2, d7
                    sub.b    d3, d7
                    rts

lbC007F50:          st       CUSTOM_ON
                    move.w   MAP_WIDTH, d2
                    neg.w    d2
                    lea      0(a0,d2.w), a0
                    move.b   (a0), d7
                    bgt      lbC007F9A
                    beq      lbC007F94
                    sub.b    #$10, d4
                    bra.s    lbC007F0C

lbC007F72:          st       CUSTOM_ON
                    move.w   MAP_WIDTH, d2
                    lea      0(a0,d2.w), a0
                    move.b   (a0), d7
                    beq      lbC007FB8
                    bpl      MAP_ERROR
                    add.b    #$10, d4
                    bra      lbC007F0C

lbC007F94:          move.b   #$F1, d7
                    rts

lbC007F9A:          cmp.b    #$1E, d7
                    beq      lbC007FB0
                    cmp.b    #$1B, d7
                    bne      MAP_ERROR
                    sub.b    #$10, d4
                    bra.s    lbC007F50

lbC007FB0:          sf       CUSTOM_ON
                    rts

lbC007FB8:          sf       CUSTOM_ON
                    clr.b    d7
                    rts

NORM_GRND:          and.w    #$FFF0, d1
                    sub.w    MAP_LINE, d1
                    sub.w    #$10, d1
                    move.w   d1, ZOOL_SCRY
                    cmp.b    #$65, d7
                    bpl      INIT_BOUNCE
                    cmp.b    #$11, d7
                    beq      SPIKED
                    cmp.b    #$1D, d7
                    beq      SPIKED
                    bsr      SPECIAL_TL
GROUND_IT:          clr.w    Y_SPEED
                    clr.w    Y_FORCE
                    clr.w    ZOOL_YDIS
                    clr.b    INAIR
                    move.w   #4, X_FRICTION
                    clr.b    GRADIENT
                    tst.b    SLIPPY_SLP
                    bne      INIT_SLIDE
                    bra      INIT_RUN

NORM_ROOF:          and.w    #$FFF0, d1
                    sub.w    MAP_LINE, d1
                    add.w    #$28, d1
                    move.w   d1, ZOOL_SCRY
                    clr.w    Y_SPEED
                    clr.w    Y_FORCE
                    clr.w    ZOOL_YDIS
                    rts

POWER_HIT:          lea      BREAK_FX, a5
                    jsr      ADD_SFX
                    move.w   ZOOL_Y, d1
                    and.w    #$FFF0, d1
                    sub.w    MAP_LINE, d1
                    add.w    #$18, d1
                    move.w   d1, ZOOL_SCRY
                    clr.w    Y_SPEED
                    clr.w    Y_FORCE
                    clr.w    ZOOL_YDIS
                    bsr      GETXY_A0
                    move.b   (a0), d6
                    cmp.b    #$47, -1(a0)
                    bmi      lbC0080A0
                    cmp.b    #$4D, -1(a0)
                    ble      lbC0080A6
lbC0080A0:          add.w    #$10, d0
                    addq.l   #1, a0
lbC0080A6:          sf       (a0)
                    sf       -1(a0)
                    move.w   MAP_WIDTH, d7
                    neg.w    d7
                    sf       0(a0,d7.w)
                    sf       -1(a0,d7.w)
                    move.l   a0, d2
                    sub.l    REF_MAP, d2
                    lsl.l    #1, d2
                    move.l   CURRENT_MAP, a0
                    addq.l   #8, a0
                    add.l    d2, a0
                    clr.w    (a0)
                    clr.w    -2(a0)
                    asl.w    #1, d7
                    clr.w    0(a0,d7.w)
                    clr.w    -2(a0,d7.w)
                    and.w    #$FF, d6
                    addq.w   #1, d6
                    move.w   d6, d7
                    sub.w    #$47, d7
                    subq.w   #8, d0
                    subq.w   #8, d1
                    bsr      ADD_HEART
                    clr.w    d2
                    bsr      NOTOK
                    subq.w   #8, d0
                    subq.w   #8, d1
                    move.w   #$FFDE, d7
                    bra      ADD_PERM

SPIKED:             move.w   #0, Y_FORCE
                    tst.b    ZOOL_HIT
                    bne      INIT_JUMP
                    tst.w    SHIELD_ON
                    bne      INIT_JUMP
                    tst.w    END_OF_STG
                    bne      INIT_JUMP
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
                    move.b   #$50, ZOOL_HIT
                    lea      ZLDAMA_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX3
                    lea      ZZDAMA_FX, a5
_ADD_SFX3:          jsr      ADD_SFX
                    bra      INIT_JUMP

INIT_BOUNCE:        move.w   #2, ZOOL_MOVE
                    sf       FORCE_LR
                    lea      SPRING_FX, a5
                    jsr      ADD_SFX
                    sub.b    #$65, d7
                    lsl.b    #1, d7
                    move.b   d7, GRAV_WAIT
                    move.w   #$FF40, Y_FORCE
                    clr.w    ZOOL_YDIS
                    bsr      GETXY_A0
                    cmp.b    #$65, -(a0)
                    bmi      lbC00821C
                    sub.w    #$10, d0
                    cmp.b    #$65, -(a0)
                    bmi      lbC00821C
                    sub.w    #$10, d0
                    cmp.b    #$65, -(a0)
                    bmi      lbC00821C
                    sub.w    #$10, d0
lbC00821C:          move.w   #$36, ZOOL_SPR
                    move.w   #2, ZOOL_ANIM
                    move.w   #$400, d7
                    clr.w    d2
                    move.b   #2, d3
                    cmp.b    #7, LEVEL_NUM
                    bne      lbC00824A
                    sub.w    #$20, d1
                    move.b   #$10, d3
lbC00824A:          cmp.b    #2, LEVEL_NUM
                    bne      ADD_TILEFX
                    sub.w    #$10, d1
                    bra      ADD_TILEFX

SPRING_LEFT:        move.w   #2, ZOOL_MOVE
                    sub.b    #$51, d7
                    lsl.b    #1, d7
                    move.b   d7, GRAV_WAIT
                    st       FORCE_LR
                    lea      SPRING_FX, a5
                    jsr      ADD_SFX
                    move.w   #-192, Y_FORCE
                    move.w   #-192, X_FORCE
                    clr.w    ZOOL_XDIS
                    clr.w    ZOOL_YDIS
                    bsr      GETXY_A0
                    move.w   MAP_WIDTH, d3
                    lea      0(a0,d3.w), a0
                    tst.b    -(a0)
                    beq      lbC0082E0
                    lea      0(a0,d3.w), a0
                    add.w    #$10, d1
                    sub.w    #$10, d0
                    tst.b    -(a0)
                    beq      lbC0082E0
                    lea      0(a0,d3.w), a0
                    sub.w    #$10, d0
                    add.w    #$10, d1
                    tst.b    -(a0)
                    beq      lbC0082E0
                    add.w    #$10, d1
                    sub.w    #$10, d0
lbC0082E0:          sub.w    #$1D, d1
                    addq.w   #3, d0
                    move.w   #$36, ZOOL_SPR
                    move.w   #2, ZOOL_ANIM
                    sf       ZOOL_FACE
                    move.w   #$400, d7
                    clr.w    d2
                    move.b   #6, d3
                    bra      ADD_FXPIX

SPRING_RIGHT:       move.w   #2, ZOOL_MOVE
                    sub.b    #$5B, d7
                    lsl.b    #1, d7
                    move.b   d7, GRAV_WAIT
                    move.b   #1, FORCE_LR
                    lea      SPRING_FX, a5
                    jsr      ADD_SFX
                    move.w   #-192, Y_FORCE
                    move.w   #192, X_FORCE
                    clr.w    ZOOL_YDIS
                    clr.w    ZOOL_XDIS
                    bsr      GETXY_A0
                    move.w   MAP_WIDTH, d3
                    lea      1(a0,d3.w), a0
                    tst.b    (a0)
                    beq      lbC00838E
                    lea      1(a0,d3.w), a0
                    add.w    #$10, d1
                    add.w    #$10, d0
                    tst.b    (a0)
                    beq      lbC00838E
                    lea      1(a0,d3.w), a0
                    add.w    #$10, d0
                    add.w    #$10, d1
                    tst.b    (a0)
                    beq      lbC00838E
                    add.w    #$10, d1
                    add.w    #$10, d0
lbC00838E:          sub.w    #$1D, d1
                    sub.w    #$33, d0
                    move.w   #$36, ZOOL_SPR
                    move.w   #2, ZOOL_ANIM
                    st       ZOOL_FACE
                    move.w   #$400, d7
                    clr.w    d2
                    move.b   #7, d3
                    bra      ADD_FXPIX

GETXY_A0:           move.l   a0, d7
                    sub.l    REF_MAP, d7
                    divu     MAP_WIDTH, d7
                    move.w   d7, d1
                    swap     d7
                    move.w   d7, d0
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    rts

PLAT_EXPL:          add.l    #5, SCORE
                    move.l   a0, d7
                    move.b   #10, (a0)
                    sub.l    REF_MAP, d7
                    divu     MAP_WIDTH, d7
                    move.w   d7, d1
                    swap     d7
                    move.w   d7, d0
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    move.w   #$200, d7
                    clr.w    d2
                    move.b   PLATEX_ANIM, d3
                    move.w   FILLTILE_SPR, d4
                    bra      ADD_TILEFX

ROOF_EXPL:          cmp.w    #$FF60, Y_FORCE
                    bgt      NORM_ROOF
ROOF_BURST:         add.l    #$19, SCORE
                    cmp.b    #9, -(a0)
                    bne      lbC00844C
                    cmp.b    #9, -(a0)
                    bne      lbC00844C
                    cmp.b    #9, -(a0)
                    bne      lbC00844C
                    cmp.b    #9, -(a0)
                    bne      lbC00844C
                    subq.l   #1, a0
lbC00844C:          addq.l   #1, a0
                    cmp.b    #9, 3(a0)
                    bne      _REMEMBER
                    tst.b    ITS_ZOOL
                    beq      NORM_ROOF
                    cmp.b    #3, LEVEL_NUM
                    beq      lbC0084B4
                    cmp.b    #4, LEVEL_NUM
                    beq      lbC0084B4
                    move.l   a0, d7
                    sf       (a0)+
                    sf       (a0)+
                    sf       (a0)+
                    sf       (a0)+
                    sf       (a0)+
                    moveq    #4, d2
lbC008488:          sub.l    REF_MAP, d7
                    divu     MAP_WIDTH, d7
                    move.w   d7, d1
                    swap     d7
                    move.w   d7, d0
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    move.w   #$200, d7
                    move.b   ROOFEX_ANIM, d3
                    move.w   FILLTILE_SPR, d4
                    bra      ADD_TILEFX

lbC0084B4:          move.l   a0, d7
                    sf       (a0)+
                    sf       (a0)+
                    sf       (a0)+
                    sf       (a0)+
                    moveq    #3, d2
                    bra.s    lbC008488

_REMEMBER:          jsr      REMEMBER
                    move.l   a0, d7
                    sf       (a0)+
                    sf       (a0)+
                    sf       (a0)+
                    moveq    #2, d2
                    move.l   a0, -(sp)
                    bsr.s    lbC008488
                    move.l   (sp)+, a0
                    sub.l    REF_MAP, a0
                    subq.l   #3, a0
                    move.l   a0, d0
                    divu     MAP_WIDTH, d0
                    move.w   d0, d1
                    swap     d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    add.w    #$10, d1
                    add.w    #$18, d0
                    sub.w    ZOOL_X, d0
                    add.w    d0, ZOOL_SCRX
                    sub.w    ZOOL_Y, d1
                    add.w    d1, ZOOL_SCRY
                    bra      INIT_TUMBLE

SECRET_ON:          move.w   MAP_WIDTH, d7
                    cmp.b    #$1C, 0(a1,d7.w)
                    beq      lbC00852C
                    subq.w   #1, d1
                    neg.w    d7
                    lea      0(a1,d7.w), a1
lbC00852C:          tst.b    HINT_ONLY
                    bne      lbC0085A8
                    move.w   MAP_WIDTH, d7
                    move.b   #6, (a1)
                    move.b   #6, 1(a1)
                    move.b   #6, 0(a1,d7.w)
                    move.b   #6, 1(a1,d7.w)
                    move.l   a1, d7
                    sub.l    REF_MAP, d7
                    lsl.l    #1, d7
                    move.l   CURRENT_MAP, a1
                    addq.l   #8, a1
                    add.l    d7, a1
                    move.w   MAP_WIDTH, d7
                    lsl.w    #1, d7
                    move.w   SECRET_TILE, d6
                    move.w   d6, (a1)+
                    addq.w   #1, d6
                    move.w   d6, (a1)
                    addq.w   #1, d6
                    lea      -2(a1,d7.w), a1
                    move.w   d6, (a1)+
                    addq.w   #1, d6
                    move.w   d6, (a1)
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    bsr      GO_SECTILE
                    move.w   SECRET_SPR, d7
                    sub.w    #$10, d0
                    bsr      ADD_PERM
                    lea      PLATF_FX, a5
                    jmp      ADD_SFX

lbC0085A8:          lsl.w    #4, d0
                    lsl.w    #4, d1
                    sf       HINT_ONLY
                    bsr      GO_SECTILE
GO_SECTILE:         move.w   d0, SECTL_X
                    move.w   d1, SECTL_Y
                    move.w   #1, SECTL_AN
                    rts

PRO_SECTL:          tst.w    SECTL_AN
                    bne      lbC008628
                    st       HINT_ONLY
                    bsr      RANDOM
                    move.w   SEED, d7
                    and.w    #15, d7
                    move.w   MAP_LINE, d3
                    lsr.w    #4, d3
                    add.w    d7, d3
                    move.w   d3, d1
                    mulu     MAP_WIDTH, d3
                    move.w   XPOS, d0
                    move.b   SEED, d7
                    and.w    #$1F, d7
                    subq.w   #6, d7
                    lsr.w    #4, d0
                    add.w    d7, d0
                    add.w    d0, d3
                    move.l   REF_MAP, a1
                    lea      0(a1,d3.w), a1
                    moveq    #0, d7
                    cmp.b    #$1C, (a1)
                    beq      SECRET_ON
lbC008628:          rts

DRAW_SECTL:         tst.w    SECTL_AN
                    beq      lbC008690
                    lea      SECTL_TAB, a0
                    addq.w   #1, SECTL_AN
                    move.w   SECTL_AN, d7
                    move.b   -1(a0,d7.w), d7
                    bmi      lbC00868A
                    add.w    SECHINT_SPR, d7
                    add.w    #13, d7
                    move.w   d7, SPRITE
                    move.w   SECTL_X, d0
                    sub.w    XPOS, d0
                    sub.w    #$10, d0
                    move.w   d0, XCOORD
                    move.w   SECTL_Y, d1
                    sub.w    MAP_LINE, d1
                    move.w   d1, YCOORD
                    bra      DUMPSPRITE

lbC00868A:          clr.w    SECTL_AN
lbC008690:          rts

INIT_TUMBLE:        move.w   #ZOOL_TUMBLING, ZOOL_MOVE
                    clr.w    X_FORCE
                    clr.w    ZOOL_XDIS
                    move.w   #-256, Y_FORCE
                    move.w   #-16, ZOOL_YDIS
                    move.w   #65, ZOOL_SPR
                    move.b   #1, ZOOL_ANIM
                    sf       ZOOL_FACE
                    rts

FLOOR_EXPL:         tst.b    ITS_ZOOL
                    bne      NORM_GRND
                    cmp.w    #$2B, ZOOL_SPR
                    bmi      NORM_GRND
                    add.l    #$19, SCORE
                    move.w   #$FFC0, Y_FORCE
                    cmp.b    #$10, -(a0)
                    bne      lbC008720
                    cmp.b    #$10, -(a0)
                    bne      lbC008720
                    cmp.b    #$10, -(a0)
                    bne      lbC008720
                    cmp.b    #$10, -(a0)
                    bne      lbC008720
                    cmp.b    #$10, -(a0)
                    bne      lbC008720
                    subq.l   #1, a0
lbC008720:          addq.l   #1, a0
                    cmp.b    #3, LEVEL_NUM
                    beq      lbC00877C
                    cmp.b    #4, LEVEL_NUM
                    beq      lbC00877C
                    move.l   a0, d7
                    sf       (a0)
                    sf       1(a0)
                    sf       2(a0)
                    sf       3(a0)
                    sf       4(a0)
                    moveq    #5, d2
lbC008750:          sub.l    REF_MAP, d7
                    divu     MAP_WIDTH, d7
                    move.w   d7, d1
                    swap     d7
                    move.w   d7, d0
                    subq.w   #1, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    move.w   #$200, d7
                    move.b   ROOFEX_ANIM, d3
                    move.w   FILLTILE_SPR, d4
                    bra      ADD_TILEFX

lbC00877C:          move.l   a0, d7
                    sf       (a0)
                    sf       1(a0)
                    sf       2(a0)
                    sf       3(a0)
                    moveq    #3, d2
                    bra.s    lbC008750

NORM_GRND2:         and.w    #$FFF0, d1
                    sub.w    MAP_LINE, d1
                    sub.w    #$10, d1
                    move.w   d1, ZOOL_SCRY
                    clr.w    Y_SPEED
                    clr.w    Y_FORCE
                    clr.w    ZOOL_YDIS
                    clr.b    INAIR
                    bsr      SPECIAL_TL
                    rts

NORM_WALL:          clr.w    ZOOL_XDIS
                    clr.w    X_FORCE
                    clr.w    X_SPEED
                    rts

SPECIAL_TL:         cmp.b    #7, d7
                    beq      PLAT_EXPL
                    cmp.b    #15, d7
                    beq      INIT_SLIDE
                    cmp.b    #$1E, d7
                    beq      UPZOOL
                    cmp.w    #5, ZOOL_MOVE
                    bne      lbC008802
                    cmp.b    #15, d7
                    bne      INIT_RUN
lbC008802:          rts

                    cmp.b    #6, d7
                    beq      NORM_ROOF
UPZOOL:             subq.w   #2, ZOOL_SCRY
                    rts

CONV_FORCE:         move.w   X_FORCE, d5
                    move.w   Y_FORCE, d6
                    move.w   d5, X_SPEED
                    move.w   d6, Y_SPEED
                    rts

DEF_MOVE:           bsr.s    CONV_FORCE
                    move.w   Y_SPEED, d0
                    move.b   d0, d1
                    tst.b    d1
                    bmi      lbC00886A
                    asr.w    #4, d0
                    and.b    #15, d1
                    add.b    d1, YSPD_REM
                    cmp.b    #$10, YSPD_REM
                    bmi      lbC008860
                    sub.b    #$10, YSPD_REM
                    addq.w   #1, d0
lbC008860:          move.w   d0, ZOOL_YDIS
                    bra      lbC00889A

lbC00886A:          add.w    #15, d0
                    and.b    #15, d1
                    or.b     #$F0, d1
                    asr.w    #4, d0
                    add.b    d1, YSPD_REM
                    cmp.b    #$F0, YSPD_REM
                    bpl      lbC008894
                    add.b    #$10, YSPD_REM
                    subq.w   #1, d0
lbC008894:          move.w   d0, ZOOL_YDIS
lbC00889A:          move.w   X_SPEED, d0
                    move.b   d0, d1
                    tst.b    d1
                    bmi      lbC0088D2
                    asr.w    #4, d0
                    and.b    #15, d1
                    add.b    d1, XSPD_REM
                    cmp.b    #$10, XSPD_REM
                    bmi      lbC0088CA
                    sub.b    #$10, XSPD_REM
                    addq.w   #1, d0
lbC0088CA:          move.w   d0, ZOOL_XDIS
                    rts

lbC0088D2:          add.w    #15, d0
                    and.b    #15, d1
                    or.b     #$F0, d1
                    asr.w    #4, d0
                    add.b    d1, XSPD_REM
                    cmp.b    #$F0, XSPD_REM
                    bpl      lbC0088FC
                    add.b    #$10, XSPD_REM
                    subq.w   #1, d0
lbC0088FC:          move.w   d0, ZOOL_XDIS
                    rts

CALC_PRLX:          lea      CPRLX_TAB, a0
                    moveq    #0, d7
                    move.w   YSCROLL, -(sp)
                    move.w   XSCROLL, -(sp)
                    move.b   LEVEL_NUM, d7
                    lsl.w    #2, d7
                    move.l   0(a0,d7.w), a0
                    move.w   XBEGSCROLL, d0
                    beq      lbC008940
                    add.w    XSCROLL, d0
                    clr.w    XBEGSCROLL
                    move.w   d0, XSCROLL
lbC008940:          move.w   YBEGSCROLL, d0
                    beq      lbC00895C
                    add.w    YSCROLL, d0
                    clr.w    YBEGSCROLL
                    move.w   d0, YSCROLL
lbC00895C:          jsr      (a0)
                    move.w   (sp)+, XSCROLL
                    move.w   (sp)+, YSCROLL
                    rts

CPRLX_TAB:          dc.l     CALC_PRLX4
                    dc.l     CALC_PRLX4
                    dc.l     CALC_PRLX4
                    dc.l     CALC_PRLX4
                    dc.l     CALC_PRLX4
                    dc.l     CALC_PRLX4
                    dc.l     CALC_PRLX4
                    dc.l     CALC_PRLX4

CALC_PRLX0:         clr.w    XSCROLL
                    clr.w    YSCROLL
                    rts

CALC_PRLX1:         bsr      CALC_XPAR1
                    bra      CALC_YPAR1

CALC_PRLX2:         bsr      CALC_XPAR2
                    bra      CALC_YPAR2

CALC_PRLX3:         clr.w    YSCROLL
                    bra      CALC_XPAR1

CALC_PRLX4:         bsr      CALC_XPAR1
                    bra      CALC_YPAR2

CALC_PRLX7:         bsr      CALC_XPAR3
                    bsr      CALC_YPAR3
                    rts

CALC_XPAR1:         moveq    #0, d0
                    move.w   XSCROLL, d0
                    swap     d0
                    asr.l    #1, d0
                    add.l    d0, PRLX_XDIS
                    move.w   PRLX_XDIS, PARA_XSCRO
                    clr.w    PRLX_XDIS
                    rts

CALC_YPAR1:         moveq    #0, d0
                    move.w   YSCROLL, d0
                    swap     d0
                    asr.l    #1, d0
                    add.l    d0, PRLX_YDIS
                    move.w   PRLX_YDIS, PARA_YSCRO
                    clr.w    PRLX_YDIS
                    rts

CALC_XPAR2:         moveq    #0, d0
                    move.w   XSCROLL, d0
                    swap     d0
                    asr.l    #2, d0
                    add.l    d0, PRLX_XDIS
                    move.w   PRLX_XDIS, PARA_XSCRO
                    clr.w    PRLX_XDIS
                    rts

CALC_YPAR2:         moveq    #0, d0
                    move.w   YSCROLL, d0
                    swap     d0
                    asr.l    #2, d0
                    add.l    d0, PRLX_YDIS
                    move.w   PRLX_YDIS, PARA_YSCRO
                    clr.w    PRLX_YDIS
                    rts

CALC_XPAR3:         moveq    #0, d0
                    move.w   XSCROLL, d0
                    swap     d0
                    asl.l    #1, d0
                    add.l    d0, PRLX_XDIS
                    move.w   PRLX_XDIS, PARA_XSCRO
                    clr.w    PRLX_XDIS
                    rts

CALC_YPAR3:         moveq    #0, d0
                    move.w   YSCROLL, d0
                    swap     d0
                    asl.l    #1, d0
                    add.l    d0, PRLX_YDIS
                    move.w   PRLX_YDIS, PARA_YSCRO
                    clr.w    PRLX_YDIS
                    rts

DEF_SCROLL:         clr.w    XSCROLL
                    clr.w    YSCROLL
                    tst.b    SCROLL_OFF
                    bne      lbC008C1C
                    tst.b    MAINGUY_ON
                    beq      lbC008C1C
                    tst.b    TEST_MODE
                    bne      TESTMODE
                    move.w   ZOOL_XDIS, d0
                    add.w    d0, ZOOL_SCRX
                    move.w   ZOOL_SCRX, d0
                    cmp.w    #$10, d0
                    bpl      lbC008AF0
                    move.w   #$10, d0
                    move.w   d0, ZOOL_SCRX
                    bra      lbC008B02

lbC008AF0:          cmp.w    #$130, d0
                    ble      lbC008B02
                    move.w   #$130, d0
                    move.w   d0, ZOOL_SCRX
lbC008B02:          tst.w    X_FORCE
                    ble      lbC008B16
                    tst.b    DRAGGED
                    bne      lbC008B94
lbC008B16:          tst.b    AFTER_DRAG
                    bne      lbC008BC0
lbC008B20:          sub.w    #$A0, d0
                    move.w   d0, XSCROLL
                    move.w   #$A0, ZOOL_SCRX
lbC008B32:          move.w   ZOOL_YDIS, d0
                    add.w    d0, ZOOL_SCRY
                    move.w   ZOOL_SCRY, d0
                    cmp.w    #$10, d0
                    bpl      lbC008B5A
                    move.w   #$10, d0
                    move.w   d0, ZOOL_SCRY
                    bra      lbC008B6C

lbC008B5A:          cmp.w    #$F0, d0
                    ble      lbC008B6C
                    move.w   #$F0, d0
                    move.w   d0, ZOOL_SCRY
lbC008B6C:          sub.w    Y_CENTRE, d0
                    beq      lbC008C1C
                    bmi      lbC008BF2
                    cmp.w    #$80, d0
                    bgt      lbC008BDE
                    asr.w    #3, d0
                    addq.w   #1, d0
                    sub.w    d0, ZOOL_SCRY
                    move.w   d0, YSCROLL
                    rts

lbC008B94:          cmp.w    #$50, PULLBACK
                    beq      lbC008BA6
                    subq.w   #2, PULLBACK
lbC008BA6:          sub.w    PULLBACK, d0
                    move.w   d0, XSCROLL
                    move.w   PULLBACK, ZOOL_SCRX
                    bra      lbC008B32

lbC008BC0:          cmp.w    #$A0, PULLBACK
                    beq      lbC008BD4
                    addq.w   #2, PULLBACK
                    bra.s    lbC008BA6

lbC008BD4:          sf       AFTER_DRAG
                    bra      lbC008B20

lbC008BDE:          sub.w    #$80, d0
                    move.w   #256, ZOOL_SCRY
                    move.w   d0, YSCROLL
                    rts

lbC008BF2:          cmp.w    #-128, d0
                    bmi      lbC008C0A
                    asr.w    #3, d0
                    sub.w    d0, ZOOL_SCRY
                    move.w   d0, YSCROLL
                    rts

lbC008C0A:          add.w    #$80, d0
                    move.w   #0, ZOOL_SCRY
                    move.w   d0, YSCROLL
lbC008C1C:          rts

TESTMODE:           btst     #0, JOYPOS
                    beq      lbC008C32
                    move.w   #-4, YSCROLL
lbC008C32:          btst     #1, JOYPOS
                    beq      lbC008C46
                    move.w   #4, YSCROLL
lbC008C46:          btst     #2, JOYPOS
                    beq      lbC008C5A
                    move.w   #-4, XSCROLL
lbC008C5A:          btst     #3, JOYPOS
                    beq      lbC008C6E
                    move.w   #4, XSCROLL
lbC008C6E:          rts

DEF_RASTER:         move.w   MAP_LINE, d0
                    lsr.w    #2, d0
                    move.w   XPOS, d1
                    ext.l    d1
                    divu     #640, d1
                    add.w    d1, d0
                    addq.w   #1, d0
                    move.w   d0, RASTER_BEG
                    and.w    #$f, RASTER_BEG
                    lsr.w    #3, d0
                    and.w    #$FFFE, d0
                    move.w   d0, COLBAND_PTR
                    rts

MAP_LIMITS:         tst.b    SCROLL_OFF
                    bne      lbC008D62
                    move.w   XSCROLL, d0
                    move.w   YSCROLL, d1
                    add.w    XPOS, d0
                    bmi      lbC008CF2
                    sub.w    RIGHT_MARG, d0
                    ble      lbC008D12
                    sub.w    d0, XSCROLL
                    add.w    d0, ZOOL_SCRX
                    cmp.w    #312, ZOOL_SCRX
                    bmi      lbC008D12
                    move.w   #312, ZOOL_SCRX
                    bra      lbC008D12

lbC008CF2:          sub.w    d0, XSCROLL
                    add.w    d0, ZOOL_SCRX
                    cmp.w    #8, ZOOL_SCRX
                    bpl      lbC008D12
                    move.w   #8, ZOOL_SCRX
lbC008D12:          add.w    MAP_LINE, d1
                    bmi      lbC008D48
                    sub.w    BOT_MARG, d1
                    ble      lbC008D60
                    sub.w    d1, YSCROLL
                    add.w    d1, ZOOL_SCRY
                    cmp.w    #256, ZOOL_SCRY
                    ble      lbC008D60
                    move.w   #256, ZOOL_SCRY
                    rts

lbC008D48:          sub.w    d1, YSCROLL
                    add.w    d1, ZOOL_SCRY
                    bpl      lbC008D60
                    move.w   #0, ZOOL_SCRY
lbC008D60:          rts

lbC008D62:          clr.w    XSCROLL
                    clr.w    YSCROLL
                    move.w   ZOOL_XDIS, d0
                    move.w   ZOOL_YDIS, d1
                    tst.w    d0
                    bmi      lbC008DA2
                    beq      lbC008DBC
                    add.w    d0, ZOOL_SCRX
                    cmp.w    #304, ZOOL_SCRX
                    bmi      lbC008DBC
                    move.w   #304, ZOOL_SCRX
                    bra      lbC008DBC

lbC008DA2:          add.w    d0, ZOOL_SCRX
                    cmp.w    #10, ZOOL_SCRX
                    bpl      lbC008DBC
                    move.w   #10, ZOOL_SCRX
lbC008DBC:          add.w    d1, ZOOL_SCRY
                    move.w   END_YB, d0
                    sub.w    #$60, d0
                    cmp.w    MAP_LINE, d0
                    beq.s    lbC008D60
                    bpl      lbC008DE8
                    addq.w   #1, ZOOL_SCRY
                    move.w   #-1, YSCROLL
                    rts

lbC008DE8:          subq.w   #1, ZOOL_SCRY
                    move.w   #1, YSCROLL
                    rts

GET_SCROLLS:        clr.w    YSCROLL
                    clr.w    XSCROLL
                    btst     #0, JOYPOS
                    beq      lbC008E18
                    move.w   #-4, YSCROLL
lbC008E18:          btst     #1, JOYPOS
                    beq      lbC008E2C
                    move.w   #4, YSCROLL
lbC008E2C:          btst     #3, JOYPOS
                    beq      lbC008E40
                    move.w   #4, XSCROLL
lbC008E40:          btst     #2, JOYPOS
                    beq      lbC008E54
                    move.w   #-4, XSCROLL
lbC008E54:          rts

DRAW_ZOONB:         move.w   ZOONB_X, d0
                    bmi      lbC008F28
                    move.w   ZOONB_Y, d1
                    add.w    ZOONB_YDIS, d1
                    move.w   d1, ZOONB_Y
                    tst.b    ANDYFRAME
                    beq      lbC008E96
                    addq.w   #1, ZOONB_YDIS
                    cmp.w    #7, ZOONB_YDIS
                    bne      lbC008E96
                    move.w   #-6, ZOONB_YDIS
lbC008E96:          move.w   ZOOL_X, d2
                    addq.w   #8, d2
                    sub.w    d0, d2
                    bmi      lbC008F04
                    cmp.w    #$30, d2
                    bpl      lbC008F04
                    move.w   ZOOL_Y, d3
                    subq.w   #8, d3
                    sub.w    d1, d3
                    bmi      lbC008F04
                    cmp.w    #$30, d3
                    bpl      lbC008F04
                    addq.w   #1, ZOONB_GOT
                    move.w   #-1, ZOONB_X
                    move.w   #1, d2
                    move.b   #1, d3
                    move.w   #$300, d7
                    bsr      ADD_TILEFX
                    add.w    #$10, d1
                    move.b   #1, d3
                    bsr      ADD_TILEFX
                    add.l    #$28, SCORE
                    lea      BARK_FX, a5
                    jsr      ADD_SFX
                    rts

lbC008F04:          sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   #$82, SPRITE
                    bra      DUMPSPRITE

lbC008F28:          rts

DRAW_BEACS:         lea      BEACONS_TAB, a0
lbC008F30:          tst.b    (a0)
                    beq      lbC009010
                    tst.b    1(a0)
                    bpl      lbC008F6C
                    addq.w   #1, 6(a0)
                    move.w   6(a0), d1
                    cmp.w    #$11, d1
                    bne      lbC008F56
                    move.w   #$FFF0, d1
                    move.w   d1, 6(a0)
lbC008F56:          add.w    4(a0), d1
                    move.w   d1, 4(a0)
                    asr.w    #2, d1
                    move.w   2(a0), d0
                    move.w   #$78, d7
                    bra      lbC008FEA

lbC008F6C:          move.w   2(a0), d0
                    move.w   4(a0), d1
                    subq.w   #1, 6(a0)
                    bpl      lbC008F88
                    eor.b    #1, 1(a0)
                    move.w   #6, 6(a0)
lbC008F88:          move.w   #$76, d7
                    add.b    1(a0), d7
                    tst.b    INAIR
                    bne      lbC008FEA
                    move.w   ZOOL_X, d2
                    sub.w    d0, d2
                    bmi      lbC008FEA
                    cmp.w    #$30, d2
                    bpl      lbC008FEA
                    move.w   ZOOL_Y, d3
                    addq.w   #8, d3
                    sub.w    d1, d3
                    bmi      lbC008FEA
                    cmp.w    #$20, d3
                    bpl      lbC008FEA
                    subq.w   #4, d1
                    move.w   d1, d6
                    lsl.w    #2, d6
                    move.w   d6, 4(a0)
                    move.w   #$FFF0, 6(a0)
                    st       1(a0)
                    jsr      REMEMBER
                    lea      RESTART_FX, a5
                    jsr      ADD_SFX
lbC008FEA:          sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   d7, SPRITE
                    move.l   a0, -(sp)
                    bsr      DUMPSPRITE
                    move.l   (sp)+, a0
lbC009010:          addq.l   #8, a0
                    cmp.l    #BEACONS, a0
                    bne      lbC008F30
                    rts

DRAW_ZOOL:          tst.b    MAINGUY_ON
                    beq      lbC009112
                    tst.b    SHADE_ON
                    beq      lbC00903C
                    tst.b    ANDYFRAME
                    bne      lbC009112
lbC00903C:          move.w   ZOOL_SCRY, YCOORD
                    move.w   ZOOL_SCRX, XCOORD
                    sub.w    #$2F, YCOORD
                    sub.w    #$1C, XCOORD
                    tst.b    ZOOL_FACE
                    beq      lbC009072
                    add.w    #10, XCOORD
lbC009072:          move.w   ZOOL_SPR, d7
                    lea      ZOOL_XYOFFS, a0
                    lsl.w    #2, d7
                    move.w   0(a0,d7.w), d0
                    move.w   2(a0,d7.w), d1
                    add.w    d1, YCOORD
                    add.w    d0, XCOORD
                    bsr      lbC009114
                    move.w   ZOOL_SPR, SPRITE
                    tst.b    SHADE_ON
                    bne      DUMPSPRITE
                    btst     #0, ZOOL_HIT
                    bne      lbC009112
                    bsr      DUMPSPRITE
                    tst.w    ZOOL_MOVE
                    bne      lbC009112
                    tst.w    BLINK_WAIT
                    bpl      lbC009112
                    add.w    #$10, XCOORD
                    add.w    #$18, YCOORD
                    move.w   #$7B, SPRITE
                    tst.b    ZOOL_FACE
                    beq      lbC0090FE
                    addq.w   #2, XCOORD
                    addq.w   #1, SPRITE
lbC0090FE:          tst.b    ITS_ZOOL
                    bne      _DUMPSPRITE0
                    addq.w   #2, SPRITE
_DUMPSPRITE0:       bra      DUMPSPRITE

lbC009112:          rts

lbC009114:          tst.b    ITS_ZOOL
                    bne.s    lbC009112
                    move.w   ZOOL_SPR, d7
                    tst.b    ZOOL_FACE
                    beq      lbC0091C6
                    cmp.w    #$1C, d7
                    beq      lbC009198
                    bmi.s    lbC009112
                    cmp.w    #$1D, d7
                    bne.s    lbC009112
                    st       WHIP_ON
                    lea      WHIP_FX, a5
                    jsr      ADD_SFX
                    move.w   ZOOL_X, WHIP_X
                    move.w   ZOOL_Y, WHIP_Y
                    add.w    #$10, WHIP_X
                    move.w   #$60, SPRITE
                    add.w    #$1F, YCOORD
                    add.w    #$27, XCOORD
                    bsr      DUMPSPRITE
                    sub.w    #$1F, YCOORD
                    sub.w    #$27, XCOORD
                    rts

lbC009198:          sub.w    #$18, XCOORD
                    add.w    #$10, YCOORD
                    move.w   #$5F, SPRITE
                    bsr      DUMPSPRITE
                    sub.w    #$10, YCOORD
                    add.w    #$18, XCOORD
                    rts

lbC0091C6:          cmp.w    #$13, d7
                    beq      lbC009236
                    bmi      lbC009112
                    cmp.w    #$14, d7
                    bne      lbC009112
                    st       WHIP_ON
                    lea      WHIP_FX, a5
                    jsr      ADD_SFX
                    move.w   ZOOL_X, WHIP_X
                    move.w   ZOOL_Y, WHIP_Y
                    sub.w    #$44, WHIP_X
                    move.w   #$5E, SPRITE
                    sub.w    #$36, XCOORD
                    add.w    #$1F, YCOORD
                    bsr      DUMPSPRITE
                    sub.w    #$1F, YCOORD
                    add.w    #$36, XCOORD
                    rts

lbC009236:          add.w    #$2A, XCOORD
                    move.w   #$5D, SPRITE
                    add.w    #$10, YCOORD
                    bsr      DUMPSPRITE
                    sub.w    #$10, YCOORD
                    sub.w    #$2A, XCOORD
                    rts

DRAW_SHIELD:        tst.w    SHIELD_ON
                    beq      lbC009364
                    subq.w   #1, SHIELD_ON
                    clr.w    d5
                    clr.w    d6
                    subq.w   #8, SHIELD_PTR
                    bpl      lbC00928A
                    add.w    #$168, SHIELD_PTR
lbC00928A:          cmp.w    #$3C, SHIELD_ON
                    bpl      lbC0092A0
                    tst.b    ANDYFRAME
                    beq      lbC009364
lbC0092A0:          move.w   SHIELD_PTR, d7
                    move.w   d7, -(sp)
                    bsr      GET_XY
                    add.w    ZOOL_SCRX, d0
                    add.w    ZOOL_SCRY, d1
                    lea      SHIELD_AN, a0
                    addq.w   #1, SHIELD_SPR
                    move.w   SHIELD_SPR, d7
                    lsl.w    #1, d7
                    move.w   0(a0,d7.w), SPRITE
                    bpl      lbC0092E4
                    move.w   (a0), SPRITE
                    clr.w    SHIELD_SPR
lbC0092E4:          subq.w   #8, d0
                    subq.w   #8, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    bsr      DUMPSPRITE
                    move.w   (sp)+, d7
                    sub.w    #$B4, d7
                    bpl      lbC009306
                    add.w    #$168, d7
lbC009306:          clr.w    d5
                    clr.w    d6
                    bsr      GET_XY
                    add.w    ZOOL_SCRX, d0
                    add.w    ZOOL_SCRY, d1
                    subq.w   #8, d0
                    subq.w   #8, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    bra      DUMPSPRITE

                    move.w   (sp)+, d7
                    sub.w    #$78, d7
                    bpl      lbC00933C
                    add.w    #$168, d7
lbC00933C:          clr.w    d5
                    clr.w    d6
                    bsr      GET_XY
                    add.w    ZOOL_SCRX, d0
                    add.w    ZOOL_SCRY, d1
                    subq.w   #8, d0
                    subq.w   #8, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    bra      DUMPSPRITE

lbC009364:          rts

SHIELD_AN:          dcb.w    4, $7F
                    dcb.w    4, $80
                    dcb.w    4, $81
                    dcb.w    4, $80
                    dc.w     $FFFF

DRAW_SHADE:         tst.b    SHADE_ON
                    beq      lbC009472
                    tst.b    SHADE_WAIT
                    bne      lbC009434
                    move.l   END_SHADE, a0
                    addq.l   #8, a0
                    move.l   a0, END_SHADE
                    cmp.l    #END_SHADE, a0
                    bne      lbC0093C4
                    move.l   #SHADE_BUFF, END_SHADE
                    lea      SHADE_BUFF, a0
lbC0093C4:          move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    move.w   (a0)+, d7
                    move.w   (a0)+, SHADE_FACE
lbC0093D0:          tst.b    ANDYFRAME
                    beq      lbC009472
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    sub.w    #$2C, d0
                    sub.w    #$2F, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    tst.b    SHADE_FACE
                    beq      lbC00940C
                    add.w    #10, XCOORD
lbC00940C:          move.w   d7, SPRITE
                    lea      ZOOL_XYOFFS, a0
                    lsl.w    #2, d7
                    move.w   0(a0,d7.w), d0
                    move.w   2(a0,d7.w), d1
                    add.w    d1, YCOORD
                    add.w    d0, XCOORD
                    bsr      DUMPSPRITE
                    rts

lbC009434:          lea      SHADE_BUFF, a0
                    move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    move.w   (a0)+, d7
                    move.w   (a0)+, SHADE_FACE
                    move.l   END_SHADE, a0
                    addq.l   #8, a0
                    move.l   a0, END_SHADE
                    cmp.l    #END_SHADE, a0
                    bne      lbC0093D0
                    move.l   #SHADE_BUFF, END_SHADE
                    sf       SHADE_WAIT
                    bra      lbC0093D0

lbC009472:          rts

ZOOL_XYOFFS:        dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 6, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 9, 0
                    dc.w     5, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 2, 0, 2, -8, 0, -8
                    dc.w     0, -8, 0, -8, 0, 4, 0, 0, 0, 0, 0, 4, 12, 4, 12, 4, 12, 4, 12
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

DRAW_PBOMB:         tst.b    BIGB_POW
                    beq      lbC009616
                    cmp.w    #10, FIRE_REPT
                    bmi      lbC009616
                    move.w   ZOOL_SCRX, d0
                    move.w   ZOOL_SCRY, d1
                    tst.w    ZOOL_MOVE
                    beq      lbC009618
                    cmp.w    #ZOOL_CROUCHING, ZOOL_MOVE
                    beq      lbC009638
                    clr.w    FIRE_REPT
lbC009616:          rts

lbC009618:          tst.b    ITS_ZOOL
                    beq      lbC009686
                    subq.w   #2, d0
                    tst.b    ZOOL_FACE
                    beq      lbC009630
                    subq.w   #7, d0
lbC009630:          sub.w    #10, d1
                    bra      lbC00964C

lbC009638:          addq.w   #6, d1
                    sub.w    #11, d0
                    tst.b    ZOOL_FACE
                    beq      lbC00964C
                    add.w    #10, d0
lbC00964C:          addq.w   #1, BIGB_SPR
                    cmp.w    #$10, BIGB_SPR
                    bne      lbC009664
                    clr.w    BIGB_SPR
lbC009664:          move.w   BIGB_SPR, d7
                    lsr.w    #1, d7
                    add.w    #$62, d7
                    move.w   d7, SPRITE
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    bra      DUMPSPRITE

lbC009686:          sub.w    #12, d0
                    tst.b    ZOOL_FACE
                    beq      lbC009698
                    add.w    #13, d0
lbC009698:          sub.w    #10, d1
                    bra.s    lbC00964C

LOAD_SPRS:          lea      SPR_DATA, a1
                    move.l   STAGE_SPRS, a0
                    move.w   SPR_CNT, d5
                    add.w    FIXED_SPRS, d5
                    subq.w   #1, d5
                    bmi      lbC0096CE
lbC0096BC:          move.l   24(a0), d7
                    lsr.w    #1, d7
                    subq.w   #1, d7
lbC0096C4:          move.w   (a0)+, (a1)+
                    dbra     d7, lbC0096C4
                    dbra     d5, lbC0096BC
lbC0096CE:          rts

DRAW_TEST:          move.w   #$64, d0
                    add.w    XPOS, d0
                    move.w   #$94, d1
                    add.w    MAP_LINE, d1
                    move.w   #$C8, d7
                    sub.w    BACKFX_SPRS, d7
                    jmp      ADD_PERM

DRAW_SPRS:          tst.b    END_FIGHT
                    bne      END_RTN
                    move.l   SPRITE_BANK, a0
                    move.w   SPR_CNT, TEMPW3
                    bsr      NEXT_SPD
DRAW_FSPRS:         move.l   FIXED_BANK, a0
                    move.w   FIXED_SPRS, TEMPW3
NEXT_SPD:           subq.w   #1, TEMPW3
                    bmi      lbC00979A
                    tst.b    (a0)
                    beq      lbC009794
                    move.b   19(a0), d7
                    bmi      lbC00979C
                    btst     #0, d7
                    beq      lbC00974C
                    btst     #1, d7
                    beq      lbC009794
lbC00974C:          move.w   30(a0), d7
                    cmp.w    #1000, d7
                    bpl      lbC009794
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    add.w    16(a0), d1
                    add.w    14(a0), d0
                    move.w   d1, YCOORD
                    move.w   d0, XCOORD
                    add.w    LEVEL_SPRS, d7
                    move.w   d7, SPRITE
                    move.l   a0, -(sp)
                    bsr      DUMPSPRITE
                    move.l   (sp)+, a0
lbC009794:          add.l    24(a0), a0
                    bra.s    NEXT_SPD

lbC00979A:          rts

lbC00979C:          btst     #0, d7
                    beq      lbC0097AA
                    btst     #1, d7
                    beq.s    lbC009794
lbC0097AA:          move.w   $1E(a0), d7
                    cmp.w    #$3E8, d7
                    bpl.s    lbC009794
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    add.w    $10(a0), d1
                    add.w    14(a0), d0
                    move.w   d1, YCOORD
                    move.w   d0, XCOORD
                    add.w    LEVEL_SPRS, d7
                    move.w   d7, SPRITE
                    move.b   $34(a0), lbB01F0F7
                    move.l   a0, -(sp)
                    bsr      DUMPSPRITE
                    move.l   (sp)+, a0
                    sf       lbB01F0F7
                    bra.s    lbC009794

END_RTN:            rts

GET_VBEAM:          move.w   $DFF004, d0
                    move.w   $DFF006, d1
                    roxr.w   #1, d0
                    roxr.w   #8, d1
                    and.w    #$1FF, d1
                    move.w   d1, VBEAM_POS
                    rts

PRO_SPRITES:        tst.b    END_FIGHT
                    bne      _PRO_FSPRS
                    jsr      PRO_ZONES
                    bclr     #0, HOLD_SPRS
                    move.l   SPRITE_BANK, a5
                    move.w   SPR_CNT, d5
                    subq.w   #1, d5
                    bmi      _PRO_FSPRS
lbC009848:          tst.b    (a5)
                    beq      lbC009888
                    btst     #0, 19(a5)
                    bne      lbC009878
_SPRITE_HIT:        bsr      SPRITE_HIT
                    bsr      SPRITE_ZHIT
                    move.l   20(a5), a6
                    jsr      (a6)
                    move.l   32(a5), a6
                    jsr      (a6)
lbC00986C:          add.l    24(a5), a5
                    dbra     d5, lbC009848
_PRO_FSPRS:         bra      PRO_FSPRS

lbC009878:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq.s    lbC00986C
                    bra.s    _SPRITE_HIT

lbC009888:          tst.b    $27(a5)
                    beq.s    lbC00986C
                    bmi      _DIRRESET
                    tst.b    $37(a5)
                    beq      lbC0098AE
                    tst.b    ANDYFRAME
                    beq.s    lbC00986C
                    subq.b   #1, $36(a5)
                    bne.s    lbC00986C
                    move.b   #1, $36(a5)
lbC0098AE:          lea      (a5), a4
                    lea      (a5), a6
                    sub.l    #SPR_DATA, a6
                    add.l    STAGE_SPRS, a6
                    lea      (a6), a5
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    bne      lbC0098DE
                    lea      (a4), a5
                    bsr      RESET_SPR
                    move.b   $37(a5), $36(a5)
                    bra.s    lbC00986C

lbC0098DE:          lea      (a4), a5
                    bra.s    lbC00986C

_DIRRESET:          bsr      DIRRESET
                    bra.s    lbC00986C

PRO_FSPRS:          bclr     #0, HOLD_SPRS
                    move.l   FIXED_BANK, a5
                    move.w   FIXED_SPRS, d5
                    subq.w   #1, d5
                    bmi      lbC009930
lbC009904:          tst.b    (a5)
                    beq      lbC009958
                    btst     #0, 19(a5)
                    bne      lbC009948
_SPRITE_HIT0:       bsr      SPRITE_HIT
                    bsr      SPRITE_ZHIT
                    move.l   20(a5), a6
                    jsr      (a6)
                    move.l   32(a5), a6
                    jsr      (a6)
lbC009928:          add.l    24(a5), a5
                    dbra     d5, lbC009904
lbC009930:          tst.b    SMART_BOMB
                    bpl      lbC009946
                    move.b   #4, SMART_BOMB
                    bsr      SMART_KILL
lbC009946:          rts

lbC009948:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq.s    lbC009928
                    bra.s    _SPRITE_HIT0

lbC009958:          tst.b    $27(a5)
                    beq.s    lbC009928
                    bmi      _DIRRESET0
                    tst.b    $37(a5)
                    beq      lbC00997E
                    tst.b    ANDYFRAME
                    bne.s    lbC009928
                    subq.b   #1, $36(a5)
                    bne.s    lbC009928
                    move.b   #1, $36(a5)
lbC00997E:          lea      (a5), a4
                    lea      (a5), a6
                    sub.l    #SPR_DATA, a6
                    add.l    STAGE_SPRS, a6
                    lea      (a6), a5
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    bne      lbC0099B0
                    lea      (a4), a5
                    bsr      RESET_SPR
                    move.b   $37(a5), $36(a5)
                    bra      lbC009928

lbC0099B0:          lea      (a4), a5
                    bra      lbC009928

_DIRRESET0:         bsr      DIRRESET
                    bra      lbC009928

RESET_SPR:          move.l   $18(a6), d7
                    lsr.w    #1, d7
                    subq.w   #1, d7
lbC0099C6:          move.w   (a6)+, (a4)+
                    dbra     d7, lbC0099C6
                    rts

DIRRESET:           tst.b    $37(a5)
                    beq      lbC0099E4
                    subq.b   #1, $36(a5)
                    bne      lbC009A44
                    move.b   #1, $36(a5)
lbC0099E4:          move.w   $38(a5), d0
                    move.w   $3A(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC009A44
                    move.b   $37(a5), $36(a5)
                    cmp.b    #4, LEVEL_NUM
                    beq      lbC009A6A
                    tst.w    X_FORCE
                    bmi      lbC009A46
                    bgt      lbC009A20
                    bchg     #7, SEED+3
                    beq      lbC009A46
lbC009A20:          moveq    #0, d0
                    move.w   XPOS, d0
                    add.w    #$170, d0
                    move.w   d0, 2(a5)
                    asl.l    #4, d0
                    move.l   d0, $30(a5)
                    st       (a5)
                    tst.w    6(a5)
                    bmi      lbC009A44
                    neg.w    6(a5)
lbC009A44:          rts

lbC009A46:          moveq    #0, d0
                    move.w   XPOS, d0
                    sub.w    #$30, d0
                    move.w   d0, 2(a5)
                    asl.l    #4, d0
                    move.l   d0, $30(a5)
                    st       (a5)
                    tst.w    6(a5)
                    bpl.s    lbC009A44
                    neg.w    6(a5)
                    rts

lbC009A6A:          move.w   SEED, d0
                    and.w    #$FF, d0
                    add.w    XPOS, d0
                    add.w    #$20, d0
                    move.w   d0, 2(a5)
                    ext.l    d0
                    asl.l    #4, d0
                    move.l   d0, $30(a5)
                    asr.l    #4, d0
                    sub.w    #$20, d0
                    move.w   d0, 10(a5)
                    add.w    #$40, d0
                    move.w   d0, 12(a5)
                    move.w   #$30, 6(a5)
                    st       (a5)
                    move.w   MAP_LINE, d0
                    sub.w    #$30, d0
                    move.w   d0, 4(a5)
                    rts

SMART_KILL:         move.l   SPRITE_BANK, a5
                    move.w   SPR_CNT, d5
                    subq.w   #1, d5
                    bmi      _KILL_FSPRS
lbC009AC6:          move.w   d5, -(sp)
                    move.l   a5, -(sp)
                    tst.b    (a5)
                    beq      lbC009AF6
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC009AF6
                    cmp.b    #$FD, $26(a5)
                    beq      _KILL_SPR
                    tst.b    $26(a5)
                    bmi      lbC009AF6
_KILL_SPR:          bsr      KILL_SPR
lbC009AF6:          move.l   (sp)+, a5
                    move.w   (sp)+, d5
                    add.l    $18(a5), a5
                    dbra     d5, lbC009AC6
_KILL_FSPRS:        bra      KILL_FSPRS

KILL_FSPRS:         move.l   FIXED_BANK, a5
                    move.w   FIXED_SPRS, d5
                    subq.w   #1, d5
                    bmi      lbC009B54
lbC009B18:          move.w   d5, -(sp)
                    move.l   a5, -(sp)
                    tst.b    (a5)
                    beq      lbC009B48
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC009B48
                    cmp.b    #$FD, 38(a5)
                    beq      _KILL_SPR0
                    tst.b    38(a5)
                    bmi      lbC009B48
_KILL_SPR0:         bsr      KILL_SPR
lbC009B48:          move.l   (sp)+, a5
                    move.w   (sp)+, d5
                    add.l    24(a5), a5
                    dbra     d5, lbC009B18
lbC009B54:          rts

KILL_SPR:           move.l   a5, -(sp)
                    lea      EDIE_FX, a5
                    jsr      ADD_SFX
                    move.l   (sp)+, a5
                    sf       (a5)
                    move.b   #6, 1(a5)
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    add.w    14(a5), d0
                    add.w    $10(a5), d1
                    st       $24(a5)
                    move.b   $26(a5), d2
                    bpl      lbC009B8C
                    sf       d2
lbC009B8C:          add.l    #$32, SCORE
                    subq.b   #1, HEART_CNT
                    bpl      lbC009BB0
                    move.b   HEART_RES, HEART_CNT
                    clr.w    d7
                    bsr      ADD_HEART
lbC009BB0:          btst     #6, $13(a5)
                    bne      _NOTOK
                    bsr      ARCH_EXPL
lbC009BBE:          move.w   #$400, d7
                    clr.w    d2
                    move.w   SPLAT_ANIM, d3
                    bra      ADD_FXPIX

_NOTOK:             bsr      NOTOK
                    bra.s    lbC009BBE

SPRITE_HIT:         sf       $24(a5)
                    move.w   2(a5), d2
                    move.w   4(a5), d3
                    add.w    $28(a5), d2
                    add.w    $2A(a5), d3
                    sub.w    #$10, d2
                    sub.w    #$10, d3
                    lea      SHOT_TAB, a4
lbC009BF6:          tst.b    (a4)
                    beq      lbC009C48
                    move.w   2(a4), d0
                    move.w   4(a4), d1
                    sub.w    d2, d0
                    bmi      lbC009C48
                    cmp.w    $2C(a5), d0
                    bpl      lbC009C48
                    add.w    #10, d1
                    sub.w    d3, d1
                    bmi      lbC009C48
                    sub.w    #10, d1
                    cmp.w    $2E(a5), d1
                    bpl      lbC009C48
                    tst.b    $26(a5)
                    bpl      lbC009C36
                    neg.w    6(a4)
                    rts

lbC009C36:          tst.b    (a4)
                    bpl      _KILL_SPR1
                    sf       (a4)
_KILL_SPR1:         bsr      KILL_SPR
                    move.b   BULL_POWER, d7
lbC009C48:          addq.l   #8, a4
                    cmp.l    #lbL01F788, a4
                    bne.s    lbC009BF6
                    rts

WHIP_COL:           tst.b    $26(a5)
                    bmi      lbC009CB2
                    move.w   2(a5), d2
                    move.w   4(a5), d3
                    add.w    $28(a5), d2
                    add.w    $2A(a5), d3
                    sub.w    #$10, d2
                    sub.w    #$10, d3
                    move.w   WHIP_X, d0
                    move.w   WHIP_Y, d1
                    move.w   $2C(a5), d4
                    add.w    #$38, d0
                    add.w    #$38, d4
                    sub.w    d2, d0
                    bmi      lbC009CB2
                    cmp.w    d4, d0
                    bpl      lbC009CB2
                    sub.w    d3, d1
                    bmi      lbC009CB2
                    cmp.w    $2E(a5), d1
                    bpl      lbC009CB2
                    tst.b    $26(a5)
                    bmi      lbC009CB2
                    bsr      KILL_SPR
lbC009CB2:          rts

SPRITE_ZHIT:        cmp.w    #ZOOL_TUMBLING, ZOOL_MOVE
                    beq      lbC009DDA
                    cmp.w    #1000, 40(a5)
                    beq      lbC009DDA
                    move.w   2(a5), d2
                    move.w   4(a5), d3
                    add.w    40(a5), d2
                    add.w    42(a5), d3
                    sub.w    #$20, d2
                    sub.w    #$20, d3
                    move.w   ZOOL_X, d0
                    sub.w    #$18, d0
                    move.w   ZOOL_Y, d1
                    sub.w    d2, d0
                    bmi      lbC009DDA
                    move.w   44(a5), d7
                    add.w    #$20, d7
                    cmp.w    d7, d0
                    bpl      lbC009DDA
                    sub.w    d3, d1
                    bmi      lbC009DDA
                    move.w   46(a5), d7
                    add.w    ZOOL_HGT, d7
                    sub.w    d7, d1
                    bpl      lbC009DDA
                    cmp.b    #$FE, 38(a5)
                    bgt      lbC009D5C
                    bmi      lbC009DDA
                    tst.b    ZOOL_HIT
                    bne      lbC009DDA
                    cmp.w    #2, ZOOL_MOVE
                    bne      lbC009D5C
                    cmp.w    #$2B, ZOOL_SPR
                    bmi      lbC009D5C
                    neg.w    6(a5)
                    asl.w    6(a5)
                    move.w   #1000, 40(a5)
                    rts

lbC009D5C:          tst.b    38(a5)
                    bmi      lbC009DAE
                    tst.w    Y_FORCE
                    ble      lbC009D90
                    cmp.w    #-20, d1
                    bpl      lbC009D90
                    tst.w    CANNOT_KILL
                    bne      lbC009DDA
                    bsr      INIT_JUMP
                    move.w   #-64, Y_FORCE
                    bra      lbC009DDC

lbC009D90:          tst.w    DESTROYER
                    bne      lbC009DDC
                    cmp.w    #8, ZOOL_MOVE
                    bne      lbC009DAE
                    cmp.w    #-20, d1
                    bmi      lbC009DDC
lbC009DAE:          tst.w    10(a5)
                    bpl      lbC009DB8
                    sf       (a5)
lbC009DB8:          tst.b    ZOOL_HIT
                    bne      lbC009DDA
                    tst.b    SHADE_ON
                    bne      lbC009DDA
                    tst.w    SHIELD_ON
                    bne      lbC009DDA
                    bsr      ZOOL_DAMAGE
lbC009DDA:          rts

lbC009DDC:          tst.b    38(a5)
                    bmi      lbC009DE8
                    bsr      KILL_SPR
lbC009DE8:          rts

ZOOL_DAMAGE:        move.l   a5, -(sp)
                    lea      ZLDAMA_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX4
                    lea      ZZDAMA_FX, a5
_ADD_SFX4:          jsr      ADD_SFX
                    move.l   (sp)+, a5
                    move.b   #80, ZOOL_HIT
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
                    rts

MCIRCULAR:          move.w   d5, -(sp)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC009E5E
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    bra      lbC009E68

lbC009E5E:          sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), $1F(a5)
lbC009E68:          move.w   $34(a5), d7
                    lsr.w    #4, d7
                    move.b   $30(a5), d5
                    move.b   $31(a5), d6
                    bsr      GET_XY
                    move.w   d0, d2
                    move.w   d1, d3
                    add.w    6(a5), d0
                    add.w    8(a5), d1
                    move.w   d0, 2(a5)
                    move.w   d1, 4(a5)
                    cmp.b    #4, LEVEL_NUM
                    beq      lbC009EDC
                    tst.b    $36(a5)
                    beq      lbC009EDC
                    move.w   6(a5), d0
                    move.w   8(a5), d1
                    move.b   $37(a5), d7
                    ext.w    d7
                    bsr      ADD_BACKSP
                    asr.w    #1, d2
                    asr.w    #1, d3
                    add.w    d2, d0
                    add.w    d3, d1
                    bsr      ADD_BACKSP
                    move.w   d0, d4
                    move.w   d1, d6
                    asr.w    #1, d2
                    asr.w    #1, d3
                    sub.w    d2, d0
                    sub.w    d3, d1
                    bsr      ADD_BACKSP
                    move.w   d4, d0
                    move.w   d6, d1
                    add.w    d2, d0
                    add.w    d3, d1
                    bsr      ADD_BACKSP
lbC009EDC:          tst.w    10(a5)
                    beq      lbC009F10
                    move.w   $34(a5), d7
                    cmp.w    #$B40, d7
                    bpl      lbC009F02
                    lsr.w    #4, d7
                    cmp.w    12(a5), d7
                    bpl      lbC009F10
                    addq.b   #1, $12(a5)
                    bra      lbC009F10

lbC009F02:          lsr.w    #4, d7
                    cmp.w    10(a5), d7
                    ble      lbC009F10
                    subq.b   #1, $12(a5)
lbC009F10:          move.b   $12(a5), d7
                    ext.w    d7
                    add.w    d7, $34(a5)
                    bmi      lbC009F2C
                    cmp.w    #$1680, $34(a5)
                    bpl      lbC009F36
                    move.w   (sp)+, d5
                    rts

lbC009F2C:          add.w    #$1680, $34(a5)
                    move.w   (sp)+, d5
                    rts

lbC009F36:          sub.w    #$1680, $34(a5)
                    move.w   (sp)+, d5
                    rts

GET_XY:             lea      XY_DATA, a1
                    lsl.w    #2, d7
                    move.w   0(a1,d7.w), d0
                    move.w   2(a1,d7.w), d1
                    move.b   d5, d7
                    beq      lbC009F84
                    cmp.b    #2, d7
                    bmi      lbC009F80
                    beq      lbC009F76
lbC009F62:          move.b   d6, d7
                    beq      lbC009F96
                    cmp.b    #2, d7
                    bmi      lbC009F92
                    beq      lbC009F88
                    rts

lbC009F76:          asr.w    #1, d0
                    move.w   d0, d7
                    asr.w    #1, d7
                    add.w    d7, d0
                    bra.s    lbC009F62

lbC009F80:          asr.w    #1, d0
                    bra.s    lbC009F62

lbC009F84:          asr.w    #2, d0
                    bra.s    lbC009F62

lbC009F88:          asr.w    #1, d1
                    move.w   d1, d7
                    asr.w    #1, d7
                    add.w    d7, d1
                    rts

lbC009F92:          asr.w    #1, d1
                    rts

lbC009F96:          asr.w    #2, d1
                    rts

REP_CALLS:          lea      BD1_SPRS, a5
                    lea      END_SPRDATA, a6
lbC009FA6:          move.l   20(a5), d0
                    lea      SPRITE_MOVE, a0
                    lsl.w    #2, d0
                    move.l   0(a0,d0.w), a0
                    move.l   a0, 20(a5)
                    move.l   32(a5), d0
                    lea      SPRITE_CONT, a0
                    lsl.w    #2, d0
                    move.l   0(a0,d0.w), a0
                    move.l   a0, 32(a5)
                    add.l    24(a5), a5
                    cmp.l    a6, a5
                    bmi.s    lbC009FA6
                    rts

SPRITE_MOVE:        dc.l     MNO_MOVE
                    dc.l     MCIRCULAR
                    dc.l     MPENJUMP
                    dc.l     MPARACHUTE
                    dc.l     MMARSHMAL
                    dc.l     MSPHINX
                    dc.l     MHOMING
                    dc.l     MMUMMY_FISH
                    dc.l     MWINDOW
                    dc.l     MVANSPIT
                    dc.l     MFLYBJET
                    dc.l     MJUST_ANIM
                    dc.l     MSWBJET
                    dc.l     MBOMBER
                    dc.l     MANGLEPOSE
                    dc.l     MBULLETX
                    dc.l     MFIREBY
                    dc.l     MEGGROLL
                    dc.l     MEGGBERT
                    dc.l     MEGGSHAKE
                    dc.l     MEGGCHICK
                    dc.l     MEGGHELMET
                    dc.l     MGANNET
                    dc.l     MBOUNCER
                    dc.l     MWALLPATROL
                    dc.l     MFLYBIRD
                    dc.l     MFLYBIRD2
                    dc.l     MFLYBIRD3
                    dc.l     MSPITTER
                    dc.l     MSWAN_BEAK
                    dc.l     MSWAN_EYE
                    dc.l     MPEACOCK
                    dc.l     MNESTIES
                    dc.l     MFROGGY
                    dc.l     MGRNDPATROL

SPRITE_CONT:        dc.l     CNO_CTRL
                    dc.l     CBOOK_CTRL
                    dc.l     CPARA_CTRL
                    dc.l     CSPHINX_CTRL
                    dc.l     CANKH_CTRL
                    dc.l     CSNF_CTRL
                    dc.l     CGLOVE_CTRL
                    dc.l     CCONE_CTRL
                    dc.l     CLOLL_CTRL
                    dc.l     CPYRA_CTRL
                    dc.l     CFLYB3_CTRL
                    dc.l     CTORCH_CTRL
                    dc.l     CSWBULB_CTRL
                    dc.l     CANGLEP_CTRL
                    dc.l     CEGGB_CTRL
                    dc.l     CROLL_CTRL
                    dc.l     CEGGSHK_CTRL
                    dc.l     CEGGC_CTRL
                    dc.l     CEGGH_CTRL
                    dc.l     CGANNET_CTRL
                    dc.l     CFLYB_CTRL
                    dc.l     CFLYB2_CTRL
                    dc.l     CSPIT_CTRL
                    dc.l     CPLANE_CTRL

CBOOK_CTRL:         tst.w    6(a5)
                    bgt      lbC00A0D0
                    bra      lbC00A0D4

lbC00A0D0:          addq.w   #3, $1E(a5)
lbC00A0D4:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC00A0E6
                    rts

lbC00A0E6:          sf       (a5)
                    rts

MPENJUMP:           tst.b    $12(a5)
                    bne      _GET_SPR
                    move.w   8(a5), d1
                    add.w    GRAVITY, d1
                    cmp.w    #$80, d1
                    ble      lbC00A108
                    move.w   #$80, d1
lbC00A108:          move.w   d1, 8(a5)
                    asr.w    #4, d1
                    add.w    4(a5), d1
                    move.w   d1, 4(a5)
                    move.w   2(a5), d0
                    add.w    6(a5), d0
                    move.w   d0, 2(a5)
                    tst.w    8(a5)
                    bmi      lbC00A1B8
                    bsr      SPR_LANDCHK
                    tst.b    GROUNDED
                    bne      lbC00A140
                    move.w   #2, $1E(a5)
                    rts

lbC00A140:          st       $12(a5)
                    sf       $1C(a5)
                    move.w   d1, 4(a5)
_GET_SPR:           bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00A160
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    rts

lbC00A160:          tst.b    $12(a5)
                    bne      lbC00A170
                    move.b   -1(a4), $1F(a5)
                    rts

lbC00A170:          sf       $12(a5)
                    move.w   #$FF80, 8(a5)
                    sf       $1C(a5)
                    move.w   #2, $1E(a5)
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    cmp.w    10(a5), d0
                    bmi      lbC00A19E
                    cmp.w    12(a5), d0
                    bgt      lbC00A1AC
                    rts

lbC00A19E:          tst.w    6(a5)
                    bpl      lbC00A1B8
                    neg.w    6(a5)
                    rts

lbC00A1AC:          tst.w    6(a5)
                    bmi      lbC00A1B8
                    neg.w    6(a5)
lbC00A1B8:          rts

MPARACHUTE:         move.l   $30(a5), d0
                    move.w   6(a5), d2
                    ext.l    d2
                    add.l    d2, d0
                    move.l   d0, $30(a5)
                    asr.l    #4, d0
                    move.w   d0, 2(a5)
                    move.w   4(a5), d1
                    add.w    8(a5), d1
                    move.w   d1, 4(a5)
                    cmp.w    10(a5), d0
                    bmi      lbC00A1EE
                    cmp.w    12(a5), d0
                    bpl      lbC00A228
                    rts

lbC00A1EE:          move.w   #$11, $1E(a5)
                    addq.w   #4, 6(a5)
                    tst.w    6(a5)
                    bmi      lbC00A260
                    move.w   #$12, $1E(a5)
                    cmp.w    #$10, 6(a5)
                    ble      lbC00A260
                    move.w   #14, $1E(a5)
                    cmp.w    #$30, 6(a5)
                    ble      lbC00A260
                    move.w   #$30, 6(a5)
                    rts

lbC00A228:          move.w   #15, $1E(a5)
                    subq.w   #4, 6(a5)
                    tst.w    6(a5)
                    bpl      lbC00A260
                    move.w   #$10, $1E(a5)
                    cmp.w    #$FFF0, 6(a5)
                    bgt      lbC00A260
                    move.w   #14, $1E(a5)
                    cmp.w    #$FFD0, 6(a5)
                    bgt      lbC00A260
                    move.w   #$FFD0, 6(a5)
lbC00A260:          rts

CPARA_CTRL:         move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC00A274
                    rts

lbC00A274:          sf       (a5)
                    rts

MMARSHMAL:          addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00A28E
                    sub.l    d2, a4
                    move.b   (a4), d7
                    sf       $1C(a5)
lbC00A28E:          move.b   d7, $1F(a5)
                    addq.w   #4, 8(a5)
                    move.w   8(a5), d0
                    cmp.w    #$60, d0
                    bpl      lbC00A2B4
                    ext.l    d0
                    add.l    d0, $30(a5)
lbC00A2A8:          move.l   $30(a5), d0
                    asr.l    #4, d0
                    move.w   d0, 4(a5)
                    rts

lbC00A2B4:          and.l    #$FFFFFF00, $30(a5)
                    move.w   #-96, 8(a5)
                    bra.s    lbC00A2A8

MSPHINX:            addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00A2E4
                    sub.l    d2, a4
                    move.b   (a4), d7
                    move.b   #$1D, $1D(a5)
                    sf       $1C(a5)
                    sf       $12(a5)
lbC00A2E4:          move.b   d7, $1F(a5)
                    tst.b    $12(a5)
                    bne      lbC00A372
                    move.w   2(a5), d0
                    tst.b    $1C(a5)
                    beq      lbC00A326
                    cmp.b    #3, $1C(a5)
                    beq      lbC00A326
                    cmp.b    #9, $1C(a5)
                    bmi      lbC00A32E
                    beq      lbC00A322
                    cmp.b    #12, $1C(a5)
                    beq      lbC00A322
                    bra      lbC00A32E

lbC00A322:          add.w    6(a5), d0
lbC00A326:          add.w    6(a5), d0
                    move.w   d0, 2(a5)
lbC00A32E:          move.w   4(a5), d1
                    addq.w   #8, d1
                    bsr      SPR_LANDCHK
                    move.w   d1, 4(a5)
                    cmp.w    10(a5), d0
                    bmi      lbC00A34E
                    cmp.w    12(a5), d0
                    bgt      lbC00A360
                    rts

lbC00A34E:          tst.b    $1C(a5)
                    beq      lbC00A358
                    rts

lbC00A358:          move.w   #1, 6(a5)
                    rts

lbC00A360:          tst.b    $1C(a5)
                    beq      lbC00A36A
                    rts

lbC00A36A:          move.w   #$FFFF, 6(a5)
                    rts

lbC00A372:          rts

CSPHINX_CTRL:       cmp.b    #$1E, $1D(a5)
                    beq      lbC00A3CA
                    tst.w    6(a5)
                    bmi      lbC00A38C
                    add.w    #$23, $1E(a5)
lbC00A38C:          tst.b    $1C(a5)
                    bne      lbC00A3C8
                    btst     #1, SEED
                    beq      lbC00A3C8
                    btst     #2, SEED
                    bne      lbC00A3C8
                    btst     #3, SEED
                    beq      lbC00A3C8
                    move.b   #$1E, $1D(a5)
                    move.w   #9, $1E(a5)
                    st       $12(a5)
lbC00A3C8:          rts

lbC00A3CA:          cmp.b    #$19, $1C(a5)
                    beq      lbC00A3EC
                    cmp.b    #$28, $1C(a5)
                    bne.s    lbC00A3C8
                    sf       $12(a5)
                    sf       $1C(a5)
                    move.b   #$1D, $1D(a5)
                    rts

lbC00A3EC:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    sub.w    #12, d0
                    bsr      FIRE_HOME
                    rts

FIRE_HOME:          move.l   HOME_BULLS, a0
                    sub.l    STAGE_SPRS, a0
                    add.l    #SPR_DATA, a0
                    tst.b    (a0)
                    beq      lbC00A422
                    lea      $44(a0), a0
                    tst.b    (a0)
                    beq      lbC00A422
                    rts

lbC00A422:          move.w   d0, 2(a0)
                    move.w   d1, 4(a0)
                    asl.w    #3, d0
                    asl.w    #3, d1
                    move.w   d0, $30(a0)
                    move.w   d1, $40(a0)
                    clr.w    6(a0)
                    clr.w    8(a0)
                    st       (a0)
                    sf       $1C(a0)
                    move.b   13(a0), 12(a0)
                    rts

MHOMING:            tst.b    $1D(a5)
                    beq      lbC00A474
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00A46A
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    bra      lbC00A474

lbC00A46A:          sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), $1F(a5)
lbC00A474:          move.w   6(a5), d0
                    add.w    d0, $30(a5)
                    move.w   8(a5), d0
                    add.w    d0, $40(a5)
                    move.w   $30(a5), d0
                    move.w   $40(a5), d1
                    asr.w    #3, d0
                    asr.w    #3, d1
                    move.w   d0, 2(a5)
                    move.w   d1, 4(a5)
                    cmp.w    ZOOL_X, d0
                    bpl      lbC00A4BA
                    addq.w   #1, 6(a5)
                    cmp.w    #$20, 6(a5)
                    ble      lbC00A4CE
                    move.w   #$20, 6(a5)
                    bra      lbC00A4CE

lbC00A4BA:          subq.w   #1, 6(a5)
                    cmp.w    #$FFE0, 6(a5)
                    bpl      lbC00A4CE
                    move.w   #$FFE0, 6(a5)
lbC00A4CE:          cmp.w    ZOOL_Y, d1
                    bpl      lbC00A4F0
                    addq.w   #1, 8(a5)
                    cmp.w    #$20, 8(a5)
                    ble      lbC00A504
                    move.w   #$20, 8(a5)
                    bra      lbC00A504

lbC00A4F0:          subq.w   #1, 8(a5)
                    cmp.w    #$FFE0, 8(a5)
                    bpl      lbC00A504
                    move.w   #$FFE0, 8(a5)
lbC00A504:          subq.b   #1, 12(a5)
                    bne      lbC00A50E
                    sf       (a5)
lbC00A50E:          rts

MMUMMY_FISH:        tst.b    $12(a5)
                    beq      lbC00A5AC
                    move.w   8(a5), d1
                    asr.w    #4, d1
                    add.w    d1, 4(a5)
                    addq.w   #4, 8(a5)
                    move.w   6(a5), d0
                    add.w    d0, 2(a5)
                    move.w   #4, d7
                    cmp.w    #$FFFC, d1
                    ble      lbC00A55A
                    addq.w   #1, d7
                    cmp.w    #$FFFE, d1
                    ble      lbC00A55A
                    addq.w   #1, d7
                    cmp.w    #2, d1
                    bmi      lbC00A55A
                    addq.w   #1, d7
                    cmp.w    #4, d1
                    bmi      lbC00A55A
                    addq.w   #1, d7
lbC00A55A:          tst.w    6(a5)
                    bmi      lbC00A566
                    add.w    #$24, d7
lbC00A566:          move.w   d7, $1E(a5)
                    move.w   $30(a5), d1
                    sub.w    4(a5), d1
                    bpl      lbC00A584
                    neg.w    d1
                    cmp.w    #$19, d1
                    bpl      lbC00A586
                    move.b   d1, $34(a5)
lbC00A584:          rts

lbC00A586:          sf       $34(a5)
                    sf       $12(a5)
                    neg.w    6(a5)
                    move.w   #-128, 8(a5)
                    move.w   #1000, $1E(a5)
                    bsr      RANDOM
                    move.b   SEED, $35(a5)
                    rts

lbC00A5AC:          subq.b   #1, $35(a5)
                    bne.s    lbC00A584
                    st       $12(a5)
                    rts

CANKH_CTRL:         tst.w    6(a5)
                    bgt      lbC00A5C4
                    bra      lbC00A5CA

lbC00A5C4:          add.w    #$20, $1E(a5)
lbC00A5CA:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC00A5DC
                    rts

lbC00A5DC:          sf       (a5)
                    rts

CPLANE_CTRL:        tst.w    6(a5)
                    bgt      lbC00A5EC
                    bra      lbC00A5F2

lbC00A5EC:          add.w    #4, $1E(a5)
lbC00A5F2:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC00A604
                    rts

lbC00A604:          sf       (a5)
                    rts

CSNF_CTRL:          tst.w    6(a5)
                    bgt      lbC00A614
                    bra      lbC00A61A

lbC00A614:          add.w    #$16, $1E(a5)
lbC00A61A:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC00A62C
                    rts

lbC00A62C:          sf       (a5)
                    rts

CGLOVE_CTRL:        tst.w    6(a5)
                    bgt      lbC00A63A
                    rts

lbC00A63A:          add.w    #$16, $1E(a5)
                    rts

CCONE_CTRL:         tst.w    6(a5)
                    bgt      lbC00A64C
                    rts

lbC00A64C:          add.w    #$12, $1E(a5)
                    rts

CLOLL_CTRL:         move.w   $34(a5), d7
                    add.w    #$160, d7
                    cmp.w    #$1680, d7
                    bmi      lbC00A668
                    sub.w    #$1680, d7
lbC00A668:          ext.l    d7
                    lsr.w    #4, d7
                    divu     #$2D, d7
                    add.w    #2, d7
                    cmp.w    #8, d7
                    bmi      lbC00A67E
                    subq.w   #8, d7
lbC00A67E:          add.w    #6, d7
                    move.w   d7, $1E(a5)
                    rts

MWINDOW:            cmp.w    #1000, $1E(a5)
                    bne      _GET_SPR0
                    subq.w   #1, 10(a5)
                    bpl      lbC00A6DE
                    move.l   HOME_BULLS, a0
                    sub.l    STAGE_SPRS, a0
                    add.l    #SPR_DATA, a0
                    tst.b    (a0)
                    beq      lbC00A6B6
                    bra      _RANDOM5

lbC00A6B6:          clr.b    $26(a5)
                    move.b   #2, $1C(a5)
_GET_SPR0:          bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      _RANDOM5
                    addq.b   #1, $1C(a5)
                    ext.w    d7
                    move.w   d7, $1E(a5)
                    cmp.b    #$1C, $1C(a5)
                    beq      lbC00A6E0
lbC00A6DE:          rts

lbC00A6E0:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    sub.w    #12, d0
                    bra      FIRE_HOME

_RANDOM5:           bsr      RANDOM
                    move.w   SEED+2, d0
                    and.w    #$7F, d0
                    add.w    #$1E, d0
                    move.w   d0, 10(a5)
                    sf       $1C(a5)
                    move.w   #1000, $1E(a5)
                    move.b   #$FD, $26(a5)
                    rts

CPYRA_CTRL:         tst.w    6(a5)
                    ble      lbC00A726
                    add.w    #$24, $1E(a5)
lbC00A726:          rts

MVANSPIT:           subq.w   #1, 10(a5)
                    bpl      lbC00A768
                    cmp.w    #-75, 10(a5)
                    bmi      lbC00A76A
                    bsr      RANDOM
                    cmp.b    #10, SEED+3
                    bpl      lbC00A768
                    move.w   d0, d2
                    and.w    #7, d2
                    moveq    #-$60, d3
                    clr.w    d7
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    subq.w   #4, d2
                    bne      _SPIT_FIRE
                    addq.w   #4, d2
_SPIT_FIRE:         bsr      SPIT_FIRE
lbC00A768:          rts

lbC00A76A:          move.w   #$96, 10(a5)
                    rts

CFLYB3_CTRL:        move.b   #1, $44(a5)
                    bset     #1, $57(a5)
                    move.b   #$11, $61(a5)
                    move.w   #11, $52(a5)
                    tst.w    6(a5)
                    bgt      lbC00A796
                    bra      lbC00A7A8

lbC00A796:          add.w    #$18, $1E(a5)
                    move.b   #$12, $61(a5)
                    move.w   #-55, $52(a5)
lbC00A7A8:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC00A7C4
                    tst.b    (a5)
                    bne      lbC00A7C2
                    sf       $44(a5)
lbC00A7C2:          rts

lbC00A7C4:          sf       (a5)
                    sf       $44(a5)
                    rts

MFLYBJET:           bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00A7E2
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    bra      lbC00A7EC

lbC00A7E2:          sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), $1F(a5)
lbC00A7EC:          move.w   -$42(a5), 2(a5)
                    move.w   -$40(a5), 4(a5)
                    rts

MJUST_ANIM:         bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00A80E
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    rts

lbC00A80E:          sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), $1F(a5)
                    rts

CTORCH_CTRL:        tst.w    6(a5)
                    ble      lbC00A828
                    add.w    #14, $1E(a5)
lbC00A828:          rts

CSWBULB_CTRL:       bsr      RANDOM
                    move.b   SEED, d1
                    lsr.b    #1, d1
                    cmp.b    #$72, d1
                    bmi      lbC00A86E
                    move.w   d0, d2
                    and.w    #7, d2
                    moveq    #-$20, d3
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    add.w    #$12, d0
                    add.w    #$24, d1
                    subq.w   #4, d2
                    bne      lbC00A85E
                    addq.w   #4, d2
lbC00A85E:          move.b   #$3C, $1C(a5)
                    move.w   #$13, $1E(a5)
                    bsr      DROP_BOMB
lbC00A86E:          move.b   #1, $38(a5)
                    bset     #1, $4B(a5)
                    tst.b    (a5)
                    bne      lbC00A884
                    sf       $38(a5)
lbC00A884:          rts

MSWBJET:            bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00A89C
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    bra      lbC00A8A6

lbC00A89C:          sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), $1F(a5)
lbC00A8A6:          move.w   -$36(a5), 2(a5)
                    move.w   -$34(a5), 4(a5)
                    rts

MBOMBER:            tst.b    $1D(a5)
                    beq      lbC00A8DC
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00A8D2
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    bra      lbC00A8DC

lbC00A8D2:          sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), $1F(a5)
lbC00A8DC:          move.w   2(a5), d0
                    add.w    6(a5), d0
                    move.w   d0, 2(a5)
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    btst     #1, $13(a5)
                    beq      lbC00A938
                    move.w   8(a5), d2
                    addq.w   #4, d2
                    cmp.w    #$80, d2
                    ble      lbC00A90C
                    move.w   #$80, d2
lbC00A90C:          move.w   d2, 8(a5)
                    asr.w    #4, d2
                    add.w    d2, d1
                    move.w   d1, 4(a5)
                    bsr      SPR_LANDCHK
                    tst.b    GROUNDED
                    beq      lbC00A93A
                    move.w   #$400, d7
                    clr.w    d2
                    subq.w   #8, d0
                    move.w   SPLAT_ANIM, d3
                    bsr      ADD_FXPIX
lbC00A938:          sf       (a5)
lbC00A93A:          rts

DROP_BOMB:          move.l   EXP_BOMBS, a6
                    sub.l    STAGE_SPRS, a6
                    add.l    #SPR_DATA, a6
                    tst.b    (a6)
                    bne      lbC00A970
                    move.w   d0, 2(a6)
                    move.w   d1, 4(a6)
                    clr.w    $28(a6)
                    st       (a6)
                    move.w   d3, 8(a6)
                    move.w   d2, 6(a6)
                    move.w   #4, ccr
                    rts

lbC00A970:          lea      $30(a6), a6
                    tst.b    (a6)
                    bne      lbC00A996
                    move.w   d0, 2(a6)
                    move.w   d1, 4(a6)
                    clr.w    $28(a6)
                    st       (a6)
                    move.w   d3, 8(a6)
                    move.w   d2, 6(a6)
                    move.w   #4, ccr
                    rts

lbC00A996:          lea      $30(a6), a6
                    tst.b    (a6)
                    bne      lbC00A9BA
                    move.w   d0, 2(a6)
                    move.w   d1, 4(a6)
                    clr.w    $28(a6)
                    st       (a6)
                    move.w   d3, 8(a6)
                    move.w   d2, 6(a6)
                    move.w   #4, ccr
lbC00A9BA:          rts

MFROGGY:            cmp.b    #$25, $1C(a5)
                    bmi      _GET_SPR1
                    cmp.b    #$66, $1C(a5)
                    bpl      _GET_SPR1
                    move.l   $30(a5), d1
                    moveq    #0, d2
                    move.w   8(a5), d2
                    ext.l    d2
                    add.l    d2, d1
                    move.l   d1, $30(a5)
                    asr.l    #4, d1
                    move.w   d1, 4(a5)
                    addq.w   #1, 8(a5)
                    cmp.w    #$21, 8(a5)
                    bmi      _GET_SPR1
                    move.w   #$FFE0, 8(a5)
                    move.b   #$65, $1C(a5)
_GET_SPR1:          bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00AA16
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    rts

lbC00AA16:          sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), d7
                    move.b   d7, $1F(a5)
                    rts

MANGLEPOSE:         tst.b    $12(a5)
                    bne      _GET_SPR2
                    move.w   8(a5), d1
                    add.w    GRAVITY, d1
                    cmp.w    #$80, d1
                    ble      lbC00AA42
                    move.w   #$80, d1
lbC00AA42:          move.w   d1, 8(a5)
                    asr.w    #4, d1
                    add.w    4(a5), d1
                    move.w   d1, 4(a5)
                    move.w   2(a5), d0
                    add.w    6(a5), d0
                    move.w   d0, 2(a5)
                    tst.w    8(a5)
                    bmi      _GET_SPR2
                    bsr      SPR_LANDCHK
                    tst.b    GROUNDED
                    beq      _GET_SPR2
                    st       $12(a5)
                    sf       $1C(a5)
                    move.b   #$17, $1D(a5)
                    move.w   d1, 4(a5)
_GET_SPR2:          bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00AA98
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    rts

lbC00AA98:          tst.b    $12(a5)
                    bne      lbC00AAA8
                    move.b   -1(a4), $1F(a5)
                    rts

lbC00AAA8:          sf       $12(a5)
                    move.w   #$FF80, 8(a5)
                    move.b   #$18, $1D(a5)
                    sf       $1C(a5)
                    bsr.s    _GET_SPR2
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    cmp.w    10(a5), d0
                    bmi      lbC00AAD8
                    cmp.w    12(a5), d0
                    bgt      lbC00AAE6
                    rts

lbC00AAD8:          tst.w    6(a5)
                    bpl      lbC00AAF2
                    neg.w    6(a5)
                    rts

lbC00AAE6:          tst.w    6(a5)
                    bmi      lbC00AAF2
                    neg.w    6(a5)
lbC00AAF2:          rts

CANGLEP_CTRL:       cmp.b    #9, $1C(a5)
                    bne      lbC00AB42
                    cmp.b    #$17, $1D(a5)
                    bne      lbC00AB42
                    move.l   ENEMY_BULLS, a6
                    sub.l    STAGE_SPRS, a6
                    add.l    #SPR_DATA, a6
                    tst.b    (a6)
                    bne      lbC00AB42
                    move.w   ZOOL_X, d2
                    sub.w    2(a5), d2
                    bpl      lbC00AB3A
                    tst.w    6(a5)
                    bmi      _RANDOM6
                    bra      lbC00AB42

lbC00AB3A:          tst.w    6(a5)
                    bgt      _RANDOM6
lbC00AB42:          tst.w    6(a5)
                    bmi      lbC00AB50
                    add.w    #$1C, $1E(a5)
lbC00AB50:          rts

_RANDOM6:           bsr      RANDOM
                    btst     #3, d0
                    beq.s    lbC00AB42
                    sf       $1C(a5)
                    move.w   6(a5), d2
                    asl.w    #2, d2
                    move.w   d2, 6(a6)
                    move.w   2(a5), 2(a6)
                    move.w   4(a5), 4(a6)
                    sub.w    #$14, 4(a6)
                    sub.w    #$14, 2(a6)
                    add.w    d2, 2(a6)
                    add.w    d2, 2(a6)
                    st       (a6)
                    bra.s    lbC00AB42

MBULLETX:           tst.b    $1D(a5)
                    beq      lbC00ABB6
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00ABAC
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    bra      lbC00ABB6

lbC00ABAC:          sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), $1F(a5)
lbC00ABB6:          move.w   2(a5), d0
                    add.w    6(a5), d0
                    move.w   d0, 2(a5)
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    btst     #1, $13(a5)
                    beq      lbC00ABD6
                    rts

lbC00ABD6:          sf       (a5)
                    rts

MFIREBY:            tst.b    $1D(a5)
                    beq      lbC00ABF4
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00AC10
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
lbC00ABF4:          move.w   10(a5), d1
                    add.w    8(a5), d1
                    move.w   d1, 10(a5)
                    lsr.w    #4, d1
                    move.w   d1, 4(a5)
                    subq.w   #2, 8(a5)
                    move.w   2(a5), d0
                    rts

lbC00AC10:          sf       (a5)
                    rts

CEGGB_CTRL:         tst.w    8(a5)
                    bgt      lbC00AC2C
lbC00AC1C:          tst.w    6(a5)
                    bmi      lbC00AC2A
                    add.w    #$29, 30(a5)
lbC00AC2A:          rts

lbC00AC2C:          cmp.w    #$10, 8(a5)
                    bpl.s    lbC00AC1C
                    bsr      RANDOM
                    and.w    #$7FFF, d0
                    cmp.w    #$7D0, d0
                    bpl.s    lbC00AC1C
                    move.b   #0, 18(a5)
                    move.w   #4, 30(a5)
                    move.b   #2, 29(a5)
                    sf       $1C(a5)
                    lea      SPRITE_MOVE, a0
                    move.l   68(a0), 20(a5)
                    move.w   #-24, 16(a5)
                    lea      SPRITE_CONT, a0
                    move.l   60(a0), 32(a5)
                    sf       1(a5)
                    move.b   #$FF, 38(a5)
                    asr.w    6(a5)
                    bmi      lbC00AC8E
                    move.b   #3, 29(a5)
lbC00AC8E:          rts

CROLL_CTRL:         rts

MEGGROLL:           tst.b    18(a5)
                    bne      lbC00AD20
                    move.w   8(a5), d1
                    add.w    GRAVITY, d1
                    cmp.w    #$80, d1
                    ble      lbC00ACB0
                    move.w   #$80, d1
lbC00ACB0:          move.w   d1, 8(a5)
                    asr.w    #4, d1
                    add.w    4(a5), d1
                    move.w   d1, 4(a5)
                    move.w   2(a5), d0
                    add.w    6(a5), d0
                    move.w   d0, 2(a5)
                    tst.w    8(a5)
                    bmi      _GET_SPR3
                    bsr      SPR_LANDCHK
                    tst.b    GROUNDED
                    beq      _GET_SPR3
                    st       $12(a5)
                    cmp.w    #1, 6(a5)
                    beq      lbC00ACF8
                    cmp.w    #-1, 6(a5)
                    bne      _GET_SPR3
lbC00ACF8:          asl.w    6(a5)
                    asl.w    6(a5)
_GET_SPR3:          bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00AD14
                    addq.b   #1, 28(a5)
                    move.b   d7, 31(a5)
                    rts

lbC00AD14:          sf       $1C(a5)
                    sub.l    d2, a4
                    tst.b    1(a5)
                    rts

lbC00AD20:          bsr.s    _GET_SPR3
                    move.w   2(a5), d0
                    add.w    6(a5), d0
                    move.w   d0, 2(a5)
                    move.w   4(a5), d1
                    bsr      SPR_LANDCHK
                    tst.b    GROUNDED
                    beq      lbC00ADB0
                    move.w   d1, 4(a5)
                    cmp.w    10(a5), d0
                    bmi      lbC00AD56
                    cmp.w    12(a5), d0
                    bgt      lbC00ADA4
lbC00AD54:          rts

lbC00AD56:          tst.w    6(a5)
                    bpl.s    lbC00AD54
                    neg.w    6(a5)
lbC00AD60:          sf       18(a5)
                    lea      SPRITE_MOVE, a0
                    move.l   72(a0), 20(a5)
                    sf       28(a5)
                    move.b   #1, 29(a5)
                    move.w   #-160, 8(a5)
                    move.w   #2, 30(a5)
                    move.b   #2, 38(a5)
                    lea      SPRITE_CONT, a0
                    move.l   56(a0), 32(a5)
                    asr.w    6(a5)
                    move.w   #-32, 16(a5)
                    rts

lbC00ADA4:          tst.w    6(a5)
                    bmi.s    lbC00AD54
                    neg.w    6(a5)
                    bra.s    lbC00AD60

lbC00ADB0:          sf       18(a5)
                    clr.w    8(a5)
                    rts

MEGGBERT:           tst.b    $12(a5)
                    bne      _GET_SPR4
                    move.w   8(a5), d1
                    add.w    GRAVITY, d1
                    cmp.w    #$80, d1
                    ble      lbC00ADD8
                    move.w   #$80, d1
lbC00ADD8:          move.w   d1, 8(a5)
                    asr.w    #4, d1
                    add.w    4(a5), d1
                    move.w   d1, 4(a5)
                    move.w   2(a5), d0
                    add.w    6(a5), d0
                    move.w   d0, 2(a5)
                    tst.w    8(a5)
                    bmi      _GET_SPR4
                    bsr      SPR_LANDCHK
                    tst.b    GROUNDED
                    beq      _GET_SPR4
                    st       $12(a5)
                    sf       $1C(a5)
                    move.b   #0, $1D(a5)
                    move.w   d1, 4(a5)
_GET_SPR4:          bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00AE42
                    addq.b   #1, $1C(a5)
                    tst.b    1(a5)
                    bne      lbC00AE36
                    move.b   d7, $1F(a5)
                    rts

lbC00AE36:          subq.b   #1, 1(a5)
                    move.b   $26(a5), $1F(a5)
                    rts

lbC00AE42:          tst.b    $12(a5)
                    bne      lbC00AE58
                    tst.b    1(a5)
                    bne.s    lbC00AE36
                    move.b   -1(a4), $1F(a5)
                    rts

lbC00AE58:          sf       $12(a5)
                    move.w   #-160, 8(a5)
                    move.b   #1, $1D(a5)
                    sf       $1C(a5)
                    bsr.s    _GET_SPR4
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    cmp.w    10(a5), d0
                    bmi      lbC00AE88
                    cmp.w    12(a5), d0
                    bgt      lbC00AE96
                    rts

lbC00AE88:          tst.w    6(a5)
                    bpl      lbC00AEA4
                    neg.w    6(a5)
                    rts

lbC00AE96:          tst.w    6(a5)
                    bmi      lbC00AEA4
                    neg.w    6(a5)
                    rts

lbC00AEA4:          rts

MEGGSHAKE:          move.w   10(a5), d0
                    addq.w   #1, d0
                    cmp.w    12(a5), d0
                    bne      lbC00AEB6
                    clr.w    d0
lbC00AEB6:          move.w   d0, 10(a5)
                    lsl.w    #1, d0
                    and.w    #$FFFC, d0
                    move.l   $30(a5), a0
                    move.w   0(a0,d0.w), 14(a5)
                    move.w   2(a0,d0.w), $10(a5)
                    rts

CEGGSHK_CTRL:       tst.w    10(a5)
                    bne      lbC00AF30
                    bsr      RANDOM
                    and.w    #$7FFF, d0
                    cmp.w    #$2400, d0
                    bpl      lbC00AF30
                    sf       (a5)
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    move.w   #8, d2
                    bsr      INIT_MINI
                    add.w    #$1C, d0
                    add.w    #$14, d1
                    move.w   d0, $36(a5)
                    move.w   d1, $38(a5)
                    lsl.w    #4, d0
                    move.w   d0, $66(a5)
                    move.w   #-32, $3C(a5)
                    move.b   #$FF, $46(a5)
                    move.w   6(a5), $3A(a5)
                    st       $34(a5)
                    st       $6C(a5)
                    st       $68(a5)
lbC00AF30:          rts

EGGSHK_OFFS:        dc.w     0, 0, -1, -1, 0, 1, 0, 1, 0, 0, 1, -1, 0, 0, 1, 1, 0, 0
                    dc.w     -1, -1, 0, 0, -1, 1, 0, 0, 0, -1, 0, 0, 1, 1, 0, 0
                    dc.w     -1, 1, 0, 0, -1, 0, 0, 0, 1, 1, 0, -1, 0, 1, 0, 0, 1
                    dc.w     -1, 0, 0, -1, -1, 0, 0, 1, 0

MEGGCHICK:          addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00AFC0
                    sub.l    d2, a4
                    move.b   (a4), d7
                    sf       $1C(a5)
lbC00AFC0:          move.b   d7, $1F(a5)
                    move.l   $30(a5), d0
                    moveq    #0, d2
                    move.w   6(a5), d2
                    ext.l    d2
                    add.l    d2, d0
                    move.l   d0, $30(a5)
                    asr.l    #4, d0
                    move.w   d0, 2(a5)
                    move.w   4(a5), d1
                    beq      lbC00AFFA
                    subq.w   #8, d1
                    bsr      SPR_WALLCHK
                    addq.w   #8, d1
                    tst.b    WALLED
                    beq      lbC00AFFA
                    neg.w    6(a5)
lbC00AFFA:          addq.w   #8, d1
                    bsr      SPR_LANDCHK
                    tst.b    GROUNDED
                    beq      lbC00B034
                    move.w   d1, 4(a5)
                    tst.b    $12(a5)
                    bne      lbC00B03A
                    cmp.w    10(a5), d0
                    bmi      lbC00B028
                    cmp.w    12(a5), d0
                    bgt      lbC00B02E
                    rts

lbC00B028:          addq.w   #1, 6(a5)
                    rts

lbC00B02E:          subq.w   #1, 6(a5)
                    rts

lbC00B034:          neg.w    6(a5)
                    rts

lbC00B03A:          move.w   ZOOL_X, d2
                    cmp.w    d2, d0
                    bpl      lbC00B066
                    bmi      lbC00B04C
                    rts

lbC00B04C:          addq.w   #2, 6(a5)
                    cmp.w    #64, 6(a5)
                    ble      lbC00B086
                    move.w   #64, 6(a5)
                    bra      lbC00B086

lbC00B064:          rts

lbC00B066:          subq.w   #2, 6(a5)
                    cmp.w    #-64, 6(a5)
                    bpl      lbC00B07A
                    move.w   #-64, 6(a5)
lbC00B07A:          cmp.w    10(a5), d0
                    bpl.s    lbC00B064
                    addq.w   #4, 6(a5)
                    rts

lbC00B086:          cmp.w    12(a5), d0
                    ble.s    lbC00B064
                    subq.w   #4, 6(a5)
                    rts

CEGGC_CTRL:         cmp.b    #2, (a5)
                    bne      lbC00B0A0
                    st       $38(a5)
                    st       (a5)
lbC00B0A0:          tst.w    6(a5)
                    ble      lbC00B0AE
                    add.w    #$13, $1E(a5)
lbC00B0AE:          tst.b    $35(a5)
                    bne      lbC00B0DE
                    tst.b    $24(a5)
                    beq      lbC00B0DC
                    tst.b    $38(a5)
                    beq      lbC00B0DC
                    st       (a5)
                    sf       $38(a5)
                    st       $26(a5)
                    move.b   #8, $35(a5)
                    move.b   #1, $12(a5)
lbC00B0DC:          rts

lbC00B0DE:          subq.b   #1, $35(a5)
                    bne.s    lbC00B0DC
                    move.b   #2, $26(a5)
                    rts

MEGGHELMET:         move.w   -$36(a5), 2(a5)
                    move.w   -$34(a5), 4(a5)
                    subq.w   #2, 2(a5)
                    subq.w   #2, 4(a5)
                    rts

CEGGH_CTRL:         tst.b    -$38(a5)
                    bne      lbC00B10C
                    sf       (a5)
lbC00B10C:          rts

MGANNET:            addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00B12E
                    sub.l    d2, a4
                    move.b   (a4), d7
                    move.b   #5, $1D(a5)
                    sf       $1C(a5)
                    sf       $12(a5)
lbC00B12E:          move.b   d7, $1F(a5)
                    tst.b    $12(a5)
                    bne      lbC00B1B2
                    move.w   2(a5), d0
                    add.w    6(a5), d0
                    move.w   d0, 2(a5)
                    move.w   4(a5), d1
                    addq.w   #8, d1
                    bsr      SPR_LANDCHK
                    move.w   d1, 4(a5)
                    cmp.w    10(a5), d0
                    bmi      lbC00B166
                    cmp.w    12(a5), d0
                    bgt      lbC00B18E
                    rts

lbC00B166:          cmp.b    #$21, $1C(a5)
                    beq      lbC00B1AA
                    cmp.b    #$1C, $1C(a5)
                    bmi      lbC00B17C
                    rts

lbC00B17C:          clr.w    6(a5)
                    move.b   #$1C, $1C(a5)
                    move.w   #$27, $1E(a5)
                    rts

lbC00B18E:          cmp.b    #$21, $1C(a5)
                    beq      lbC00B1A2
                    cmp.b    #$1C, $1C(a5)
                    bmi.s    lbC00B17C
                    rts

lbC00B1A2:          move.w   #-1, 6(a5)
                    rts

lbC00B1AA:          move.w   #1, 6(a5)
                    rts

lbC00B1B2:          rts

CGANNET_CTRL:       tst.b    $12(a5)
                    bne      lbC00B216
                    move.w   ZOOL_X, d2
                    sub.w    2(a5), d2
                    bpl      lbC00B1D6
                    tst.w    6(a5)
                    bmi      _RANDOM7
                    bra      lbC00B1DE

lbC00B1D6:          tst.w    6(a5)
                    bgt      _RANDOM7
lbC00B1DE:          cmp.w    #$27, $1E(a5)
                    beq      lbC00B1F6
                    tst.w    6(a5)
                    ble      lbC00B1F6
                    add.w    #$23, $1E(a5)
lbC00B1F6:          rts

_RANDOM7:           bsr      RANDOM
                    and.w    #$7FFF, d0
                    cmp.w    #$7D0, d0
                    bpl.s    lbC00B1DE
                    st       $12(a5)
                    move.b   #6, $1D(a5)
                    sf       $1C(a5)
                    bra.s    lbC00B1DE

lbC00B216:          cmp.b    #$10, $1C(a5)
                    bne.s    lbC00B1DE
                    bsr      GANN_FIRE
                    bne.s    lbC00B1DE
                    move.l   a5, -(sp)
                    lea      EPOP_FX, a5
                    jsr      ADD_SFX
                    move.l   (sp)+, a5
                    bra.s    lbC00B1DE

GANN_FIRE:          move.w   SEED, d0
                    and.w    #3, d0
                    add.w    #$3C, d0
                    move.l   EGG_BOMBS, a6
                    sub.l    STAGE_SPRS, a6
                    add.l    #SPR_DATA, a6
                    tst.b    (a6)
                    bne      lbC00B2A0
                    move.w   d0, $1E(a6)
                    move.w   2(a5), 2(a6)
                    addq.w   #4, 2(a6)
                    move.w   4(a5), 4(a6)
                    subq.w   #4, 4(a6)
                    st       (a6)
                    move.w   #$FFB0, 8(a6)
                    move.w   #4, 6(a6)
                    clr.w    $28(a6)
                    tst.w    6(a5)
                    bpl      lbC00B336
                    sub.w    #$10, 2(a6)
                    move.w   #$FFFC, 6(a6)
                    move.w   #4, ccr
                    rts

lbC00B2A0:          lea      $30(a6), a6
                    tst.b    (a6)
                    bne      lbC00B2EE
                    move.w   d0, $1E(a6)
                    move.w   2(a5), 2(a6)
                    addq.w   #4, 2(a6)
                    move.w   4(a5), 4(a6)
                    subq.w   #4, 4(a6)
                    st       (a6)
                    move.w   #-80, 8(a6)
                    move.w   #4, 6(a6)
                    clr.w    $28(a6)
                    tst.w    6(a5)
                    bpl      lbC00B336
                    sub.w    #$10, 2(a6)
                    move.w   #-4, 6(a6)
                    move.w   #4, ccr
                    rts

lbC00B2EE:          lea      $30(a6), a6
                    tst.b    (a6)
                    bne      lbC00B33A
                    move.w   d0, $1E(a6)
                    move.w   2(a5), 2(a6)
                    addq.w   #4, 2(a6)
                    move.w   4(a5), 4(a6)
                    subq.w   #4, 4(a6)
                    st       (a6)
                    move.w   #-80, 8(a6)
                    move.w   #4, 6(a6)
                    clr.w    $28(a6)
                    tst.w    6(a5)
                    bpl      lbC00B336
                    sub.w    #$10, 2(a6)
                    move.w   #-4, 6(a6)
lbC00B336:          move.w   #4, ccr
lbC00B33A:          rts

MBOUNCER:           tst.b    $1D(a5)
                    beq      lbC00B364
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bmi      lbC00B35A
                    addq.b   #1, $1C(a5)
                    move.b   d7, $1F(a5)
                    bra      lbC00B364

lbC00B35A:          sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), $1F(a5)
lbC00B364:          move.w   2(a5), d0
                    add.w    6(a5), d0
                    move.w   d0, 2(a5)
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    btst     #1, $13(a5)
                    beq      lbC00B3DC
                    move.w   8(a5), d2
                    addq.w   #4, d2
                    cmp.w    #$80, d2
                    ble      lbC00B394
                    move.w   #$80, d2
lbC00B394:          move.w   d2, 8(a5)
                    asr.w    #4, d2
                    add.w    d2, d1
                    move.w   d1, 4(a5)
                    subq.w   #8, d1
                    bsr      SPR_WALLCHK
                    tst.b    WALLED
                    bne      lbC00B3DC
                    addq.w   #8, d1
                    tst.w    8(a5)
                    ble      lbC00B3DA
                    bsr      SPR_LANDCHK
                    tst.b    GROUNDED
                    beq      lbC00B3DA
                    move.w   d1, 4(a5)
                    neg.w    8(a5)
                    add.w    #$20, 8(a5)
                    bpl      lbC00B3DC
lbC00B3DA:          rts

lbC00B3DC:          sf       (a5)
                    rts

CNO_CTRL:           rts

MWALLPATROL:        addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00B3F8
                    sub.l    d2, a4
                    move.b   (a4), d7
                    sf       $1C(a5)
lbC00B3F8:          move.b   d7, $1F(a5)
                    move.l   $30(a5), d1
                    moveq    #0, d2
                    move.w   8(a5), d2
                    ext.l    d2
                    add.l    d2, d1
                    move.l   d1, $30(a5)
                    asr.l    #4, d1
                    move.w   d1, 4(a5)
                    cmp.w    10(a5), d1
                    bmi      lbC00B426
                    cmp.w    12(a5), d1
                    bpl      lbC00B42C
                    rts

lbC00B426:          addq.w   #1, 8(a5)
                    rts

lbC00B42C:          subq.w   #1, 8(a5)
                    rts

MGRNDPATROL:        addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00B448
                    sub.l    d2, a4
                    move.b   (a4), d7
                    sf       $1C(a5)
lbC00B448:          move.b   d7, $1F(a5)
                    move.l   $30(a5), d1
                    moveq    #0, d2
                    move.w   6(a5), d2
                    ext.l    d2
                    add.l    d2, d1
                    move.l   d1, $30(a5)
                    asr.l    #4, d1
                    move.w   d1, 2(a5)
                    cmp.w    10(a5), d1
                    bmi      lbC00B476
                    cmp.w    12(a5), d1
                    bpl      lbC00B47C
                    rts

lbC00B476:          addq.w   #1, 6(a5)
                    rts

lbC00B47C:          subq.w   #1, 6(a5)
                    rts

MFLYBIRD:           tst.b    $12(a5)
                    beq      lbC00B594
                    cmp.b    #2, $12(a5)
                    bmi      lbC00B49A
                    beq      lbC00B5C6
                    rts

lbC00B49A:          move.b   #9, $1D(a5)
                    addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00B4B6
                    sub.l    d2, a4
                    move.b   (a4), d7
                    sf       $1C(a5)
lbC00B4B6:          move.b   d7, $1F(a5)
                    move.w   ZOOL_X, d2
                    move.w   ZOOL_Y, d3
                    sub.w    4(a5), d3
                    bmi      lbC00B4E6
                    tst.w    8(a5)
                    bmi      lbC00B4E6
                    bsr      RANDOM
                    and.w    #$7FFF, d0
                    cmp.w    #1000, d0
                    bmi      lbC00B68C
lbC00B4E6:          move.l   $40(a5), d1
                    moveq    #0, d2
                    move.w   8(a5), d2
                    ext.l    d2
                    add.l    d2, d1
                    move.l   d1, $40(a5)
                    asr.l    #4, d1
                    move.w   d1, 4(a5)
                    move.l   $30(a5), d0
                    moveq    #0, d2
                    move.w   6(a5), d2
                    ext.l    d2
                    add.l    d2, d0
                    move.l   d0, $30(a5)
                    asr.l    #4, d0
                    move.w   d0, 2(a5)
                    cmp.w    10(a5), d0
                    bmi      lbC00B52A
                    cmp.w    12(a5), d0
                    bpl      lbC00B542
                    bra      lbC00B556

lbC00B52A:          addq.w   #4, 6(a5)
                    cmp.w    #64, 6(a5)
                    ble      lbC00B556
                    move.w   #64, 6(a5)
                    bra      lbC00B556

lbC00B542:          subq.w   #4, 6(a5)
                    cmp.w    #-64, 6(a5)
                    bgt      lbC00B556
                    move.w   #-64, 6(a5)
lbC00B556:          cmp.w    $3C(a5), d1
                    bmi      lbC00B568
                    cmp.w    $3E(a5), d1
                    bpl      lbC00B57E
                    rts

lbC00B568:          addq.w   #4, 8(a5)
                    cmp.w    #64, 8(a5)
                    ble      lbC00B592
                    move.w   #64, 8(a5)
                    rts

lbC00B57E:          subq.w   #4, 8(a5)
                    cmp.w    #-64, 8(a5)
                    bgt      lbC00B592
                    move.w   #-64, 8(a5)
lbC00B592:          rts

lbC00B594:          addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00B5AA
                    sub.l    d2, a4
                    move.b   (a4), d7
                    sf       $1C(a5)
lbC00B5AA:          move.b   d7, $1F(a5)
                    subq.w   #1, $34(a5)
                    bne.s    lbC00B592
                    move.b   #9, $1D(a5)
                    sf       $1C(a5)
                    move.b   #1, $12(a5)
                    rts

lbC00B5C6:          move.w   #14, $1E(a5)
                    move.l   $40(a5), d1
                    moveq    #0, d2
                    move.w   8(a5), d2
                    ext.l    d2
                    add.l    d2, d1
                    move.l   d1, $40(a5)
                    asr.l    #4, d1
                    move.w   d1, 4(a5)
                    move.l   $30(a5), d0
                    moveq    #0, d2
                    move.w   6(a5), d2
                    ext.l    d2
                    add.l    d2, d0
                    move.l   d0, $30(a5)
                    asr.l    #4, d0
                    move.w   d0, 2(a5)
                    addq.w   #4, 8(a5)
                    cmp.w    #$60, 8(a5)
                    ble      lbC00B610
                    move.w   #$60, 8(a5)
lbC00B610:          tst.w    6(a5)
                    bmi      lbC00B630
                    addq.w   #4, 6(a5)
                    cmp.w    #$60, 6(a5)
                    ble      _SPR_LANDCHK
                    move.w   #$60, 6(a5)
                    bra      _SPR_LANDCHK

lbC00B630:          subq.w   #4, 6(a5)
                    cmp.w    #-96, 6(a5)
                    bpl      _SPR_LANDCHK
                    move.w   #-96, 6(a5)
_SPR_LANDCHK:       bsr      SPR_LANDCHK
                    tst.b    GROUNDED
                    beq      lbC00B592
                    move.w   d1, 4(a5)
                    move.w   #15, $1E(a5)
                    move.b   #9, $1D(a5)
                    move.b   #$11, $1C(a5)
                    move.w   #-16, 8(a5)
                    move.b   #1, $12(a5)
                    tst.w    6(a5)
                    bmi      lbC00B684
                    move.w   #-32, 6(a5)
                    rts

lbC00B684:          move.w   #$20, 6(a5)
                    rts

lbC00B68C:          sub.w    2(a5), d2
                    bmi      lbC00B6A0
                    tst.w    6(a5)
                    ble      lbC00B4E6
                    bra      lbC00B6A8

lbC00B6A0:          tst.w    6(a5)
                    bpl      lbC00B4E6
lbC00B6A8:          move.b   #2, $12(a5)
                    bra      lbC00B5C6

CFLYB_CTRL:         tst.w    6(a5)
                    bgt      lbC00B6BC
                    rts

lbC00B6BC:          add.w    #$23, $1E(a5)
                    rts

CFLYB2_CTRL:        tst.w    6(a5)
                    bgt      lbC00B6D0
                    bra      lbC00B6D6

lbC00B6D0:          add.w    #$23, $1E(a5)
lbC00B6D6:          move.w   2(a5), d0
                    move.w   4(a5), d1
                    bsr      ONSCR_CHK
                    beq      lbC00B6E8
                    rts

lbC00B6E8:          sf       (a5)
                    rts

MFLYBIRD2:          move.b   #9, $1D(a5)
MFLYBIRD3:          addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00B708
                    sub.l    d2, a4
                    move.b   (a4), d7
                    sf       $1C(a5)
lbC00B708:          move.b   d7, $1F(a5)
                    move.l   $40(a5), d1
                    moveq    #0, d2
                    move.w   8(a5), d2
                    ext.l    d2
                    add.l    d2, d1
                    move.l   d1, $40(a5)
                    asr.l    #4, d1
                    move.w   d1, 4(a5)
                    move.l   $30(a5), d0
                    moveq    #0, d2
                    move.w   6(a5), d2
                    ext.l    d2
                    add.l    d2, d0
                    move.l   d0, $30(a5)
                    asr.l    #4, d0
                    move.w   d0, 2(a5)
                    cmp.w    $3C(a5), d1
                    bmi      lbC00B74E
                    cmp.w    $3E(a5), d1
                    bpl      lbC00B764
                    rts

lbC00B74E:          addq.w   #4, 8(a5)
                    cmp.w    #64, 8(a5)
                    ble      lbC00B778
                    move.w   #64, 8(a5)
                    rts

lbC00B764:          subq.w   #4, 8(a5)
                    cmp.w    #-64, 8(a5)
                    bgt      lbC00B778
                    move.w   #-64, 8(a5)
lbC00B778:          rts

MSPITTER:           tst.b    $12(a5)
                    beq      lbC00B790
                    cmp.b    #2, $12(a5)
                    bne      lbC00B7B4
                    bra      lbC00B860

lbC00B790:          subq.w   #1, 10(a5)
                    bmi      lbC00B79A
                    rts

lbC00B79A:          st       $12(a5)
                    move.w   #$60, 10(a5)
                    st       $30(a5)
                    move.b   #11, $1D(a5)
                    sf       $1C(a5)
                    rts

lbC00B7B4:          subq.w   #1, 10(a5)
                    beq      lbC00B834
                    btst     #4, 11(a5)
                    beq      lbC00B7CE
                    st       $30(a5)
                    bra      lbC00B818

lbC00B7CE:          sf       $30(a5)
                    bsr      RANDOM
                    move.w   d0, d2
                    and.w    #7, d2
                    moveq    #-$60, d3
                    asr.w    #8, d0
                    move.w   d0, d7
                    and.w    #3, d7
                    add.w    #$3C, d7
                    move.w   2(a5), d0
                    move.w   4(a5), d1
                    add.w    #10, d0
                    subq.w   #5, d1
                    subq.w   #4, d2
                    bne      _SPIT_FIRE0
                    addq.w   #4, d2
_SPIT_FIRE0:        bsr      SPIT_FIRE
                    bne      lbC00B818
                    move.l   a5, -(sp)
                    lea      EPOP_FX, a5
                    jsr      ADD_SFX
                    move.l   (sp)+, a5
lbC00B818:          addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00B82E
                    subq.b   #2, $1C(a5)
                    move.b   #$21, d7
lbC00B82E:          move.b   d7, $1F(a5)
                    rts

lbC00B834:          move.b   #2, $12(a5)
                    move.w   #$20, $1E(a5)
                    move.w   #5, 10(a5)
                    rts

lbC00B848:          sf       $12(a5)
                    move.w   #$1E, $1E(a5)
                    bsr      RANDOM
                    and.w    #$FF, d0
                    move.w   d0, 10(a5)
                    rts

lbC00B860:          subq.w   #1, 10(a5)
                    bmi.s    lbC00B848
                    rts

CSPIT_CTRL:         rts

SPIT_FIRE:          move.l   EGG_BOMBS, a6
                    sub.l    STAGE_SPRS, a6
                    add.l    #SPR_DATA, a6
                    tst.b    (a6)
                    bne      lbC00B8A2
                    move.w   d7, $1E(a6)
                    move.w   d0, 2(a6)
                    move.w   d1, 4(a6)
                    clr.w    $28(a6)
                    st       (a6)
                    move.w   d3, 8(a6)
                    move.w   d2, 6(a6)
                    move.w   #4, ccr
                    rts

lbC00B8A2:          lea      $30(a6), a6
                    tst.b    (a6)
                    bne      lbC00B8CC
                    move.w   d7, $1E(a6)
                    move.w   d0, 2(a6)
                    move.w   d1, 4(a6)
                    clr.w    $28(a6)
                    st       (a6)
                    move.w   d3, 8(a6)
                    move.w   d2, 6(a6)
                    move.w   #4, ccr
                    rts

lbC00B8CC:          lea      $30(a6), a6
                    tst.b    (a6)
                    bne      lbC00B8F4
                    move.w   d7, $1E(a6)
                    move.w   d0, 2(a6)
                    move.w   d1, 4(a6)
                    clr.w    $28(a6)
                    st       (a6)
                    move.w   d3, 8(a6)
                    move.w   d2, 6(a6)
                    move.w   #4, ccr
lbC00B8F4:          rts

SPIT_FIRE5:         bsr      SPIT_FIRE
                    bne      lbC00B900
                    rts

lbC00B900:          lea      $30(a6), a6
                    tst.b    (a6)
                    bne      lbC00B92A
                    move.w   d7, $1E(a6)
                    move.w   d0, 2(a6)
                    move.w   d1, 4(a6)
                    clr.w    $28(a6)
                    st       (a6)
                    move.w   d3, 8(a6)
                    move.w   d2, 6(a6)
                    move.w   #4, ccr
                    rts

lbC00B92A:          lea      $30(a6), a6
                    tst.b    (a6)
                    bne      lbC00B952
                    move.w   d7, $1E(a6)
                    move.w   d0, 2(a6)
                    move.w   d1, 4(a6)
                    clr.w    $28(a6)
                    st       (a6)
                    move.w   d3, 8(a6)
                    move.w   d2, 6(a6)
                    move.w   #4, ccr
lbC00B952:          rts

MNO_MOVE:           rts

MSWAN_BEAK:         tst.b    INAIR
                    bne      lbC00B9B4
                    move.w   ZOOL_X, d2
                    add.w    #$10, d2
                    sub.w    2(a5), d2
                    bmi      lbC00B9B4
                    cmp.w    #$100, d2
                    bpl      lbC00B9B4
                    move.w   ZOOL_Y, d3
                    add.w    #$50, d3
                    sub.w    4(a5), d3
                    bmi      lbC00B9B4
                    cmp.w    #$C0, d3
                    bpl      lbC00B9B4
                    tst.b    12(a5)
                    bne      lbC00B9B8
                    move.w   #$19, -12(a5)
                    move.w   #$25, $1E(a5)
                    move.w   #$28, 10(a5)
                    st       12(a5)
                    rts

lbC00B9B4:          sf       12(a5)
lbC00B9B8:          tst.w    10(a5)
                    beq      lbC00B9CE
                    subq.w   #1, 10(a5)
                    bne      lbC00B9D0
                    move.w   #$24, $1E(a5)
lbC00B9CE:          rts

lbC00B9D0:          move.w   #$19, -12(a5)
                    rts

MSWAN_EYE:          addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00B9EE
                    sf       $1C(a5)
                    move.b   #$19, d7
lbC00B9EE:          move.b   d7, $1F(a5)
                    rts

MPEACOCK:           tst.b    INAIR
                    bne      lbC00BA4C
                    move.w   ZOOL_X, d2
                    add.w    #$60, d2
                    sub.w    2(a5), d2
                    bmi      lbC00BA4C
                    cmp.w    #$E0, d2
                    bpl      lbC00BA4C
                    move.w   ZOOL_Y, d3
                    add.w    #$50, d3
                    sub.w    4(a5), d3
                    bmi      lbC00BA4C
                    cmp.w    #$50, d3
                    bpl      lbC00BA4C
                    tst.b    12(a5)
                    bne      lbC00BA50
                    move.w   #$1B, $1E(a5)
                    move.w   #$28, 10(a5)
                    st       12(a5)
                    rts

lbC00BA4C:          sf       12(a5)
lbC00BA50:          tst.w    10(a5)
                    beq      lbC00BA66
                    subq.w   #1, 10(a5)
                    bne      lbC00BA66
                    move.w   #$26, $1E(a5)
lbC00BA66:          rts

MNESTIES:           addq.b   #1, $1C(a5)
                    bsr      GET_SPR
                    move.b   (a4), d7
                    bpl      lbC00BA7E
                    sf       $1C(a5)
                    sub.l    d2, a4
                    move.b   (a4), d7
lbC00BA7E:          move.b   d7, $1F(a5)
                    rts

ONSCR_CHK:          move.w   XPOS, d2
                    sub.w    #128, d2
                    sub.w    d0, d2
                    bpl      lbC00BAC0
                    cmp.w    #-576, d2
                    bmi      lbC00BAC0
                    move.w   MAP_LINE, d2
                    sub.w    #92, d2
                    sub.w    d1, d2
                    bpl      lbC00BAC0
                    cmp.w    #-440, d2
                    bmi      lbC00BAC0
                    bset     #1, 19(a5)
                    move.w   #0, ccr
                    rts

lbC00BAC0:          bclr     #1, 19(a5)
                    move.w   #4, ccr
                    rts

GET_SPR:            lea      ANIMS_TAB, a4
                    moveq    #0, d2
                    move.b   29(a5), d2
                    lsl.w    #2, d2
                    move.l   0(a4,d2.w), a4
                    moveq    #0, d2
                    move.b   28(a5), d2
                    lea      0(a4,d2.w), a4
                    rts

SPR_LANDCHK:        bsr      CHECK_TILET
                    tst.b    CUSTOM_ON
                    bne      lbC00BB62
                    tst.b    d7
                    beq      lbC00BB5A
                    cmp.b    #4, d7
                    beq      lbC00BB5A
                    cmp.b    #10, d7
                    beq      lbC00BB5A
                    cmp.b    #$16, d7
                    beq      lbC00BB5A
                    cmp.b    #$47, d7
                    bmi      lbC00BB26
                    cmp.b    #$4D, d7
                    ble      lbC00BB5A
lbC00BB26:          cmp.b    #11, d7
                    beq      lbC00BB5A
                    cmp.b    #$16, d7
                    beq      lbC00BB5A
                    cmp.b    #12, d7
                    beq      lbC00BB5A
                    cmp.b    #$12, d7
                    beq      lbC00BB5A
                    cmp.b    #$13, d7
                    beq      lbC00BB5A
                    and.w    #$FFF0, d1
                    move.b   d7, GROUNDED
                    rts

lbC00BB5A:          sf       GROUNDED
                    rts

lbC00BB62:          st       GROUNDED
                    ext.w    d7
                    add.w    d7, d1
                    rts

SPR_WALLCHK:        tst.w    6(a5)
                    beq      lbC00BBF4
                    move.w   d0, -(sp)
                    bsr      CHECK_TILE2
                    tst.b    CUSTOM_ON
                    bne      lbC00BBF6
                    tst.b    d7
                    beq      lbC00BBF6
                    cmp.b    #4, d7
                    beq      lbC00BBF6
                    cmp.b    #10, d7
                    beq      lbC00BBF6
                    cmp.b    #$16, d7
                    beq      lbC00BBF6
                    cmp.b    #$47, d7
                    bmi      lbC00BBB4
                    cmp.b    #$4D, d7
                    ble      lbC00BBF6
lbC00BBB4:          cmp.b    #11, d7
                    beq      lbC00BBF6
                    cmp.b    #12, d7
                    beq      lbC00BBF6
                    cmp.b    #$16, d7
                    beq      lbC00BBF6
                    cmp.b    #3, d7
                    beq      lbC00BBF6
                    cmp.b    #5, d7
                    beq      lbC00BBF6
                    cmp.b    #$12, d7
                    beq      lbC00BBF6
                    cmp.b    #$13, d7
                    beq      lbC00BBF6
                    st       WALLED
                    move.w   (sp)+, d0
lbC00BBF4:          rts

lbC00BBF6:          sf       WALLED
                    move.w   (sp)+, d0
                    rts

RANDOM:             move.l   a0, -(sp)
                    lea      SEED, a0
                    addq.l   #1, 8(a0)
                    move.l   (a0), d0
                    add.l    4(a0), d0
                    eor.l    d0, 6(a0)
                    move.w   $DFF006, d1
                    eor.w    d1, d0
                    swap     d0
                    neg.w    d1
                    eor.w    d1, d0
                    add.l    8(a0), d0
                    move.l   d0, (a0)
                    roxr.w   (a0)+
                    roxl.w   (a0)+
                    roxr.w   (a0)+
                    roxl.w   (a0)+
                    move.l   (sp)+, a0
                    rts

SCROLL_BUFF:        move.w   XSCROLL, -(sp)
                    move.w   YSCROLL, -(sp)
                    move.w   XSCROLL, d0
                    add.w    d0, XPOS
                    bsr      SCROLL_C
                    move.w   (sp)+, YSCROLL
                    move.w   (sp)+, XSCROLL
                    tst.b    ANDYFRAME
                    beq      lbC00BCA2
                    move.w   OLD_XSCR, d0
                    move.w   OLD_YSCR, d1
                    move.w   XSCROLL, OLD_XSCR
                    move.w   YSCROLL, OLD_YSCR
                    add.w    d0, XSCROLL
                    add.w    d1, YSCROLL
                    move.l   #UNDRAW_TABB, UNDRAW_PTR
                    bra      SCROLL_B

lbC00BCA2:          move.w   OLD_XSCR, d0
                    move.w   OLD_YSCR, d1
                    move.w   XSCROLL, OLD_XSCR
                    move.w   YSCROLL, OLD_YSCR
                    add.w    d0, XSCROLL
                    add.w    d1, YSCROLL
                    move.l   #UNDRAW_TABA, UNDRAW_PTR
                    bra      SCROLL_A

SCROLL_C:           lea      SCROLL, a0
                    lea      SCROLLC, a1
                    movem.l  (a1), d0-d4
                    movem.l  d0-d4, (a0)
                    bsr      SCRO
                    lea      SCROLL, a1
                    lea      SCROLLC, a0
                    movem.l  (a1), d0-d4
                    movem.l  d0-d4, (a0)
                    rts

SCROLL_B:           lea      SCROLL, a0
                    lea      SCROLLB, a1
                    movem.l  (a1), d0-d4
                    movem.l  d0-d4, (a0)
                    bsr      SCRO
                    lea      SCROLL, a1
                    lea      SCROLLB, a0
                    movem.l  (a1), d0-d4
                    movem.l  d0-d4, (a0)
                    rts

SCROLL_A:           lea      SCROLL, a0
                    lea      SCROLLA, a1
                    movem.l  (a1), d0-d4
                    movem.l  d0-d4, (a0)
                    bsr      SCRO
                    lea      SCROLL, a1
                    lea      SCROLLA, a0
                    movem.l  (a1), d0-d4
                    movem.l  d0-d4, (a0)
                    rts

SCRO:               move.l   SCROLL, a3
                    lea      (a3), a4
                    add.l    #$CC00, a3
                    tst.w    XSCROLL
                    beq      lbC00BD8E
                    bmi      _SCRO_RIGHT
                    bsr      SCRO_LEFT
                    bra      lbC00BD8E

_SCRO_RIGHT:        bsr      SCRO_RIGHT
lbC00BD8E:          tst.w    YSCROLL
                    beq      lbC00BDA0
                    bmi      SCRO_DOWN
                    bra      SCRO_UP

lbC00BDA0:          rts

DUMP_ROW:           move.w   #$14, d7
                    lea      $DFF000, a5
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00BDC0
                    bra      lbC00BDE2

lbC00BDC0:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00BDC0
                    tst.b    $DFF002
lbC00BDE2:          move.w   #0, $64(a5)
                    move.w   #$2E, $66(a5)
                    move.w   #-1, $44(a5)
                    move.w   #-1, $46(a5)
                    move.w   #0, $42(a5)
                    move.w   #$9F0, $40(a5)
lbC00BE06:          tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00BE1A
                    bra      lbC00BE3C

lbC00BE1A:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00BE1A
                    tst.b    $DFF002
lbC00BE3C:          moveq    #0, d0
                    move.w   (a0)+, d0
                    lsl.l    #7, d0
                    move.l   TILES, a2
                    add.l    d0, a2
                    move.l   a2, $50(a5)
                    move.l   a1, $54(a5)
                    move.w   #$1001, $58(a5)
                    addq.l   #2, a1
                    dbra     d7, lbC00BE06
                    rts

DUMP_COL:           move.w   #$10, d7
                    moveq    #0, d2
                    move.w   MAP_WIDTH, d2
                    lsl.w    #1, d2
                    lea      $DFF000, a5
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00BE88
                    bra      lbC00BEAA

lbC00BE88:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00BE88
                    tst.b    $DFF002
lbC00BEAA:          move.w   #0, $64(a5)
                    move.w   #$2E, $66(a5)
                    move.w   #-1, $44(a5)
                    move.w   #-1, $46(a5)
                    move.w   #0, $42(a5)
                    move.w   #$9F0, $40(a5)
lbC00BECE:          tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00BEE2
                    bra      lbC00BF04

lbC00BEE2:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00BEE2
                    tst.b    $DFF002
lbC00BF04:          moveq    #0, d0
                    move.w   (a0), d0
                    lsl.l    #7, d0
                    move.l   TILES, a2
                    add.l    d0, a2
                    cmp.l    a3, a1
                    bmi      lbC00BF1E
                    sub.l    #$CC00, a1
lbC00BF1E:          move.l   a2, $50(a5)
                    move.l   a1, $54(a5)
                    move.w   #$1001, $58(a5)
                    add.l    d2, a0
                    lea      $C00(a1), a1
                    dbra     d7, lbC00BECE
                    rts

SCRO_UP:            move.w   YSCROLL, d0
                    cmp.w    #$10, d0
                    ble      lbC00BF56
                    sub.w    #$10, YSCROLL
                    moveq    #$10, d0
                    bsr      lbC00BF56
                    bra.s    SCRO_UP

lbC00BF56:          add.w    d0, YSCRO
                    add.w    d0, MAP_LINE
                    add.b    d0, YREM
                    mulu     #$C0, d0
                    add.l    d0, BUFF_PTR
                    cmp.l    BUFF_PTR, a3
                    bgt      lbC00BF8E
                    sub.l    #$CC00, BUFF_PTR
                    sub.w    #$110, YSCRO
lbC00BF8E:          bclr     #4, YREM
                    bne      lbC00BF9C
                    rts

lbC00BF9C:          move.l   MAP_PTR, a0
                    moveq    #0, d0
                    move.w   MAP_WIDTH, d0
                    lsl.w    #1, d0
                    add.l    d0, a0
                    move.l   a0, MAP_PTR
                    mulu     #$10, d0
                    add.l    d0, a0
                    move.l   SCROLL, a1
                    move.w   YSCRO, d0
                    sub.w    #$10, d0
                    bpl      lbC00BFD2
                    add.w    #$110, d0
lbC00BFD2:          and.w    #$FFF0, d0
                    mulu     #$C0, d0
                    add.l    d0, a1
                    bra      DUMP_ROW

SCRO_DOWN:          move.w   YSCROLL, d0
                    cmp.w    #-16, d0
                    bpl      lbC00BFFE
                    add.w    #$10, YSCROLL
                    moveq    #-$10, d0
                    bsr      lbC00BFFE
                    bra.s    SCRO_DOWN

lbC00BFFE:          add.w    d0, YSCRO
                    add.w    d0, MAP_LINE
                    add.b    d0, YREM
                    muls     #$C0, d0
                    add.l    d0, BUFF_PTR
                    cmp.l    BUFF_PTR, a4
                    ble      lbC00C036
                    add.l    #$CC00, BUFF_PTR
                    add.w    #$110, YSCRO
lbC00C036:          tst.b    YREM
                    bmi      lbC00C042
                    rts

lbC00C042:          add.b    #$10, YREM
                    move.l   MAP_PTR, a0
                    moveq    #0, d0
                    move.w   MAP_WIDTH, d0
                    lsl.w    #1, d0
                    sub.l    d0, a0
                    move.l   a0, MAP_PTR
                    move.l   SCROLL, a1
                    move.w   YSCRO, d0
                    and.w    #$FFF0, d0
                    mulu     #$C0, d0
                    add.l    d0, a1
                    bra      DUMP_ROW

SCRO_LEFT:          move.w   XSCROLL, d0
                    cmp.w    #$10, d0
                    ble      lbC00C09A
                    sub.w    #$10, XSCROLL
                    moveq    #$10, d0
                    bsr      lbC00C09A
                    bra.s    SCRO_LEFT

lbC00C09A:          sub.b    d0, XREM
                    bmi      lbC00C0A6
                    rts

lbC00C0A6:          add.b    #$10, XREM
                    addq.l   #2, SCROLL
                    addq.l   #2, BUFF_PTR
                    move.l   MAP_PTR, a0
                    addq.l   #2, a0
                    move.l   a0, MAP_PTR
                    lea      $28(a0), a0
                    move.l   SCROLL, a1
                    move.w   YSCRO, d0
                    and.w    #$FFF0, d0
                    mulu     #$C0, d0
                    add.l    d0, a1
                    lea      $28(a1), a1
                    addq.l   #2, a3
                    addq.l   #2, a4
                    bra      DUMP_COL

SCRO_RIGHT:         move.w   XSCROLL, d0
                    cmp.w    #-16, d0
                    bpl      lbC00C10C
                    add.w    #$10, XSCROLL
                    moveq    #-$10, d0
                    bsr      lbC00C10C
                    bra.s    SCRO_RIGHT

lbC00C10C:          sub.b    d0, XREM
                    bclr     #4, XREM
                    bne      lbC00C120
                    rts

lbC00C120:          subq.l   #2, SCROLL
                    subq.l   #2, BUFF_PTR
                    move.l   MAP_PTR, a0
                    subq.l   #2, a0
                    move.l   a0, MAP_PTR
                    move.l   SCROLL, a1
                    move.w   YSCRO, d0
                    and.w    #$FFF0, d0
                    mulu     #$C0, d0
                    add.l    d0, a1
                    subq.l   #2, a3
                    subq.l   #2, a4
                    bra      DUMP_COL

SCRO_NOW:           lea      BPL0_H, a0
                    move.l   BUFF_PTR, d0
                    move.w   #3, d1
lbC00C168:          move.w   d0, 6(a0)
                    swap     d0
                    move.w   d0, 2(a0)
                    swap     d0
                    add.l    #$30, d0
                    addq.l   #8, a0
                    dbra     d1, lbC00C168
                    lea      RESET_PTRS, a0
                    move.l   SCROLL, d0
                    move.w   #3, d1
lbC00C190:          move.w   d0, 6(a0)
                    swap     d0
                    move.w   d0, 2(a0)
                    swap     d0
                    add.l    #$30, d0
                    addq.l   #8, a0
                    dbra     d1, lbC00C190
                    move.b   XREM, d0
                    and.w    #15, d0
                    move.w   d0, HORZ_OFF
                    move.w   #$110, d7
                    sub.w    YSCRO, d7
                    cmp.w    #$100, d7
                    bpl      lbC00C1F0
                    add.w    #$3B, d7
                    btst     #8, d7
                    beq      lbC00C1E6
                    subq.b   #1, d7
                    bmi      lbC00C1F8
                    move.b   #1, RESET_POS
                    rts

lbC00C1E6:          sf       RESET_POS
                    subq.b   #1, d7
                    rts

lbC00C1F0:          st       RESET_POS
                    rts

lbC00C1F8:          move.b   #2, RESET_POS
                    rts

; ZOOL 2 animated title
SCRO_NOW3:          clr.b    JOYSTICK_MODE
                    lea      SCRO_PTRS3, a0
                    move.l   BUFF_PTR, d0
                    move.w   #4-1, d1
lbC00C212:          move.w   d0, 6(a0)
                    swap     d0
                    move.w   d0, 2(a0)
                    swap     d0
                    add.l    #$30, d0
                    addq.l   #8, a0
                    dbra     d1, lbC00C212
                    lea      RESET_PTRS, a0
                    move.l   SCROLL, d0
                    move.w   #3, d1
lbC00C23A:          move.w   d0, 6(a0)
                    swap     d0
                    move.w   d0, 2(a0)
                    swap     d0
                    add.l    #$30, d0
                    addq.l   #8, a0
                    dbra     d1, lbC00C23A
                    move.b   XREM, d0
                    and.w    #15, d0
                    move.w   d0, HORZ_OFF
                    move.w   #$110, d7
                    sub.w    YSCRO, d7
                    cmp.w    #$100, d7
                    bpl      lbC00C29A
                    add.w    #$3B, d7
                    btst     #8, d7
                    beq      lbC00C290
                    subq.b   #1, d7
                    bmi      lbC00C2A2
                    move.b   #1, RESET_POS
                    rts

lbC00C290:          sf       RESET_POS
                    subq.b   #1, d7
                    rts

lbC00C29A:          st       RESET_POS
                    rts

lbC00C2A2:          move.b   #2, RESET_POS
                    rts

SCRO2_NOW:          move.w   PARA_XSCRO, d0
                    beq      lbC00C32C
                    bpl      lbC00C2EC
                    add.w    PARA_XREM, d0
                    bpl      lbC00C326
lbC00C2C4:          subq.l   #2, PLAYF_BPL1
                    subq.l   #2, PLAYF_BPL2
                    subq.l   #2, PLAYF_BPL3
                    subq.l   #2, PLAYF_BPL4
                    add.w    #$10, d0
                    bmi.s    lbC00C2C4
                    move.w   d0, PARA_XREM
                    bra      lbC00C32C

lbC00C2EC:          add.w    PARA_XREM, d0
                    cmp.w    #$10, d0
                    bmi      lbC00C326
lbC00C2FA:          addq.l   #2, PLAYF_BPL1
                    addq.l   #2, PLAYF_BPL2
                    addq.l   #2, PLAYF_BPL3
                    addq.l   #2, PLAYF_BPL4
                    sub.w    #$10, d0
                    cmp.w    #$10, d0
                    bpl.s    lbC00C2FA
                    move.w   d0, PARA_XREM
                    bra      lbC00C32C

lbC00C326:          move.w   d0, PARA_XREM
lbC00C32C:          move.w   PARA_YSCRO, d0
                    beq      lbC00C356
                    move.w   PARA_WIDTH, d1
                    muls     d0, d1
                    add.l    d1, PLAYF_BPL1
                    add.l    d1, PLAYF_BPL2
                    add.l    d1, PLAYF_BPL3
                    add.l    d1, PLAYF_BPL4
lbC00C356:          lea      BPL4_H, a0
                    move.w   PLAYF_BPL1, 2(a0)
                    move.w   PLAYF_BPL1+2, 6(a0)
                    move.w   PLAYF_BPL2, 10(a0)
                    move.w   PLAYF_BPL2+2, 14(a0)
                    move.w   PLAYF_BPL3, $12(a0)
                    move.w   PLAYF_BPL3+2, $16(a0)
                    move.w   PLAYF_BPL4, $1A(a0)
                    move.w   PLAYF_BPL4+2, $1E(a0)
                    move.w   PARA_XREM, d0
                    and.w    #15, d0
                    neg.w    d0
                    add.w    #15, d0
                    lsl.w    #4, d0
                    or.w     HORZ_OFF, d0
                    move.w   d0, HORZ_SCRO
                    rts

DOCOL:              move.l   DOCOL_RTN, a0
                    jmp      (a0)

DOCOL_BAND3:        lea      WRITE_COP3, a0
                    move.w   RASTER_BEG, d1
                    bpl      lbC00C3EA
                    subq.w   #2, COLBAND_PTR
                    add.w    #$10, d1
                    add.w    #$10, RASTER_BEG
                    bra      lbC00C400

lbC00C3EA:          bclr     #4, d1
                    beq      lbC00C400
                    sub.w    #$10, RASTER_BEG
                    addq.w   #2, COLBAND_PTR
lbC00C400:          lea      USED_BAND1, a1
                    move.w   COLBAND_PTR, d0
                    lea      0(a1,d0.w), a1
                    move.b   #$2C, d2
                    sub.b    d1, d2
                    move.w   #$FFFE, d3
                    move.l   #$19C0000, d4
                    move.b   #$10, d5
                    move.w   #$11, d0
                    and.w    #$FF, d7
                    and.w    #$FF, d2
                    sf       OVERFLAG
                    sf       RESETFLAG
                    tst.b    RESET_POS
                    bmi      lbC00C55E
                    beq      lbC00C4DE
                    cmp.b    #1, RESET_POS
                    beq      lbC00C48A
lbC00C456:          move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
                    add.b    d5, d2
                    bcc      lbC00C47E
                    bsr      COPYOVER255
                    move.l   d4, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
                    st       OVERFLAG
lbC00C47E:          dbra     d0, lbC00C456
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C48A:          tst.b    OVERFLAG
                    beq      lbC00C4B6
                    tst.b    RESETFLAG
                    bne      lbC00C4B6
                    cmp.w    d2, d7
                    bgt      lbC00C4B6
                    move.b   d7, (a0)+
                    move.b   #$DF, (a0)+
                    move.w   d3, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
lbC00C4B6:          move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
                    add.b    d5, d2
                    bcc      lbC00C4D2
                    bsr      COPYOVER255
                    st       OVERFLAG
lbC00C4D2:          dbra     d0, lbC00C48A
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C4DE:          sf       d6
lbC00C4E0:          tst.b    OVERFLAG
                    bne      lbC00C536
                    tst.b    RESETFLAG
                    bne      lbC00C536
                    cmp.w    d2, d7
                    ble      lbC00C524
                    cmp.w    #$F0, d2
                    bmi      lbC00C536
                    move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
                    move.b   d7, (a0)+
                    move.b   #$DF, (a0)+
                    move.w   d3, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
                    bra      lbC00C542

lbC00C524:          move.b   d7, (a0)+
                    move.b   #$DF, (a0)+
                    move.w   d3, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
lbC00C536:          move.b   d2, (a0)+
                    move.b   #1, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
lbC00C542:          add.b    d5, d2
                    bcc      lbC00C552
                    bsr      COPYOVER255
                    st       OVERFLAG
lbC00C552:          dbra     d0, lbC00C4E0
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C55E:          st       RESETFLAG
lbC00C564:          move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
                    add.b    d5, d2
                    bcc      lbC00C580
                    bsr      COPYOVER255
                    st       OVERFLAG
lbC00C580:          dbra     d0, lbC00C564
                    move.l   #$FFFFFFFE, (a0)+
                    rts

DOCOL_BAND2:        lea      WRITE_COP, a0
                    sf       TOPPAN_DONE
                    sf       BOTPAN_DONE
                    move.w   RASTER_BEG, d1
                    bpl      lbC00C5BE
                    subq.w   #2, COLBAND_PTR
                    add.w    #$10, d1
                    add.w    #$10, RASTER_BEG
                    bra      lbC00C5D4

lbC00C5BE:          bclr     #4, d1
                    beq      lbC00C5D4
                    sub.w    #$10, RASTER_BEG
                    addq.w   #2, COLBAND_PTR
lbC00C5D4:          lea      USED_BAND1, a1
                    lea      USED_BAND2, a4
                    move.w   COLBAND_PTR, d0
                    lea      0(a1,d0.w), a1
                    lea      0(a4,d0.w), a4
                    move.b   #$3B, d2
                    move.w   #$10, d0
                    sub.b    d1, d2
                    move.w   #$FFFE, d3
                    move.l   #$1820000, d4
                    move.l   #$18E0000, d6
                    move.b   #$10, d5
                    and.w    #$FF, d7
                    and.w    #$FF, d2
                    sf       OVERFLAG
                    sf       RESETFLAG
                    tst.b    RESET_POS
                    bmi      lbC00C76A
                    beq      lbC00C6DA
                    cmp.b    #1, RESET_POS
                    beq      lbC00C67A
lbC00C63A:          move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.w   (a4)+, d6
                    move.l   d4, (a0)+
                    move.l   d6, (a0)+
                    add.b    d5, d2
                    bcc      lbC00C66E
                    bsr      COPYOVER255
                    move.l   d4, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
                    st       OVERFLAG
lbC00C66E:          dbra     d0, lbC00C63A
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C67A:          tst.b    OVERFLAG
                    beq      lbC00C6AE
                    tst.b    RESETFLAG
                    bne      lbC00C6AE
                    cmp.w    d2, d7
                    bgt      lbC00C6AE
                    move.b   d7, (a0)+
                    move.b   #$DF, (a0)+
                    move.w   d3, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
lbC00C6AE:          move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.w   (a4)+, d6
                    move.l   d4, (a0)+
                    move.l   d6, (a0)+
                    add.b    d5, d2
                    bcc      lbC00C6CE
                    bsr      COPYOVER255
                    st       OVERFLAG
lbC00C6CE:          dbra     d0, lbC00C67A
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C6DA:          sf       d6
lbC00C6DC:          tst.b    OVERFLAG
                    bne      lbC00C73E
                    tst.b    RESETFLAG
                    bne      lbC00C73E
                    cmp.w    d2, d7
                    ble      lbC00C72C
                    cmp.w    #$F0, d2
                    bmi      lbC00C73E
                    move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.w   (a4)+, d6
                    move.l   d4, (a0)+
                    move.l   d6, (a0)+
                    move.b   d7, (a0)+
                    move.b   #$DF, (a0)+
                    move.w   d3, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
                    bra      lbC00C74E

lbC00C72C:          move.b   d7, (a0)+
                    move.b   #$DF, (a0)+
                    move.w   d3, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
lbC00C73E:          move.b   d2, (a0)+
                    move.b   #1, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.w   (a4)+, d6
                    move.l   d4, (a0)+
                    move.l   d6, (a0)+
lbC00C74E:          add.b    d5, d2
                    bcc      lbC00C75E
                    bsr      COPYOVER255
                    st       OVERFLAG
lbC00C75E:          dbra     d0, lbC00C6DC
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C76A:          st       RESETFLAG
lbC00C770:          move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.w   (a4)+, d6
                    move.l   d4, (a0)+
                    move.l   d6, (a0)+
                    add.b    d5, d2
                    bcc      lbC00C798
                    bsr      COPYOVER255
                    st       OVERFLAG
lbC00C798:          dbra     d0, lbC00C770
                    move.l   #$FFFFFFFE, (a0)+
                    rts

DOCOL_BANDS:        lea      WRITE_COP, a0
                    sf       TOPPAN_DONE
                    sf       BOTPAN_DONE
                    move.w   RASTER_BEG, d1
                    bpl      lbC00C7DA
                    subq.w   #2, COLBAND_PTR
                    add.w    #$10, d1
                    add.w    #$10, RASTER_BEG
                    bra      lbC00C7F0

lbC00C7DA:          bclr     #4, d1
                    beq      lbC00C7F0
                    sub.w    #$10, RASTER_BEG
                    addq.w   #2, COLBAND_PTR
lbC00C7F0:          lea      USED_BAND1, a1
                    move.w   COLBAND_PTR, d0
                    lea      0(a1,d0.w), a1
                    move.b   #$3C, d2
                    sub.b    d1, d2
                    move.w   #$FFFE, d3
                    move.l   #$1820000, d4
                    tst.b    BONUS_CS
                    beq      lbC00C820
                    move.l   #$18E0000, d4
lbC00C820:          move.b   #$10, d5
                    move.w   #$11, d0
                    and.w    #$FF, d7
                    and.w    #$FF, d2
                    sf       OVERFLAG
                    sf       RESETFLAG
                    tst.b    RESET_POS
                    bmi      lbC00C976
                    beq      lbC00C8EE
                    cmp.b    #1, RESET_POS
                    beq      lbC00C892
lbC00C856:          bsr      lbC00C9AC
                    bsr      lbC00C9AE
                    move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
                    add.b    d5, d2
                    bcc      lbC00C886
                    bsr      COPYOVER255
                    move.l   d4, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
                    st       OVERFLAG
lbC00C886:          dbra     d0, lbC00C856
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C892:          bsr      lbC00C9AC
                    bsr      lbC00C9AE
                    tst.b    OVERFLAG
                    beq      lbC00C8C6
                    tst.b    RESETFLAG
                    bne      lbC00C8C6
                    cmp.w    d2, d7
                    bgt      lbC00C8C6
                    move.b   d7, (a0)+
                    move.b   #$DF, (a0)+
                    move.w   d3, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
lbC00C8C6:          move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
                    add.b    d5, d2
                    bcc      lbC00C8E2
                    bsr      COPYOVER255
                    st       OVERFLAG
lbC00C8E2:          dbra     d0, lbC00C892
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C8EE:          sf       d6
lbC00C8F0:          bsr      lbC00C9AC
                    bsr      lbC00C9AE
                    tst.b    OVERFLAG
                    bne      lbC00C94E
                    tst.b    RESETFLAG
                    bne      lbC00C94E
                    cmp.w    d2, d7
                    ble      lbC00C93C
                    cmp.w    #$F0, d2
                    bmi      lbC00C94E
                    move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
                    move.b   d7, (a0)+
                    move.b   #$DF, (a0)+
                    move.w   d3, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
                    bra      lbC00C95A

lbC00C93C:          move.b   d7, (a0)+
                    move.b   #$DF, (a0)+
                    move.w   d3, (a0)+
                    bsr      COPYRESET
                    st       RESETFLAG
lbC00C94E:          move.b   d2, (a0)+
                    move.b   #1, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
lbC00C95A:          add.b    d5, d2
                    bcc      lbC00C96A
                    bsr      COPYOVER255
                    st       OVERFLAG
lbC00C96A:          dbra     d0, lbC00C8F0
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C976:          st       RESETFLAG
lbC00C97C:          bsr      lbC00C9AC
                    bsr      lbC00C9AE
                    move.b   d2, (a0)+
                    move.b   #$2B, (a0)+
                    move.w   d3, (a0)+
                    move.w   (a1)+, d4
                    move.l   d4, (a0)+
                    add.b    d5, d2
                    bcc      lbC00C9A0
                    bsr      COPYOVER255
                    st       OVERFLAG
lbC00C9A0:          dbra     d0, lbC00C97C
                    move.l   #$FFFFFFFE, (a0)+
                    rts

lbC00C9AC:          rts

lbC00C9AE:          rts

COPYOVER255:        move.l   END_COLS, (a0)+
                    move.l   d4, (a0)+
                    move.l   d4, (a0)+
                    rts

COPYOV255_2:        move.l   END_COLS, (a0)+
                    move.l   d4, (a0)+
                    move.l   d4, (a0)+
                    rts

COPYRESET:          lea      RESET_PTRS, a6
                    move.l   (a6)+, (a0)+
                    move.l   (a6)+, (a0)+
                    move.l   (a6)+, (a0)+
                    move.l   (a6)+, (a0)+
                    move.l   (a6)+, (a0)+
                    move.l   (a6)+, (a0)+
                    move.l   (a6)+, (a0)+
                    move.l   (a6)+, (a0)+
                    rts

DUMPSPRITE:         move.w   YCOORD, -(sp)
                    move.w   XCOORD, d5
                    moveq    #0, d6
                    move.b   XREM, d6
                    sub.w    d6, d5
                    add.w    #$40, d5
                    bmi      NODRAW
                    move.w   d5, d6
                    and.w    #15, d6
                    cmp.w    #$180, d5
                    bpl      NODRAW
                    lea      SPR_TAB, a2
                    lea      SPRM_TAB, a1
                    move.w   SPRITE, d7
                    lsl.w    #2, d7
                    move.l   0(a1,d7.w), a1
                    move.l   0(a2,d7.w), a2
                    add.l    MASK_AD, a1
                    add.l    SPRITE_AD, a2
LOADSPR:            move.w   (a2)+, d7
                    ble      DRAW_ERR
                    cmp.w    #$41, d7
                    bpl      DRAW_ERR
                    sub.w    SPRLNS_OFF, d7
                    move.w   (a2)+, d1
                    move.w   d1, SPR_YADD
                    move.w   (a2)+, d0
                    beq      XCHK16
                    cmp.w    #2, d0
                    bmi      XCHK32
                    beq      XCHK48
CONTXDR:            addq.w   #2, d0
                    move.w   (a2)+, MASK_TYPE
                    move.w   d0, d3
                    subq.w   #1, d3
                    bsr      CALC_SPR
                    beq      NODRAW
                    tst.w    MASK_TYPE
                    bgt      SINGMASK
                    bsr      BLIT_SPR
                    tst.b    DBL_BLIT
                    beq      NODRAW
                    lea      (a3), a0
                    lea      (a4), a1
                    lea      (a6), a2
                    lsl.w    #8, d2
                    add.w    d3, d2
                    move.w   d2, d7
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00CAAE
                    bra      lbC00CAD0

lbC00CAAE:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00CAAE
                    tst.b    $DFF002
lbC00CAD0:          lea      $DFF000, a5
                    bsr      NEXTBLIT
NODRAW:             move.w   (sp)+, YCOORD
                    rts

XCHK48:             cmp.w    #$10, d5
                    bpl      CONTXDR
                    move.w   (sp)+, YCOORD
                    rts

XCHK32:             cmp.w    #$20, d5
                    bpl      CONTXDR
                    move.w   (sp)+, YCOORD
                    rts

XCHK16:             cmp.w    #$30, d5
                    bpl      CONTXDR
                    move.w   (sp)+, YCOORD
                    rts

SINGMASK:           bsr      SBLIT_SPR
                    tst.b    DBL_BLIT
                    beq      lbC00CB72
                    lea      (a3), a0
                    lea      (a4), a1
                    lea      (a6), a2
                    lsl.w    #6, d2
                    move.w   d2, d7
                    lsl.w    #2, d7
                    add.w    d3, d7
                    add.w    d3, d2
                    move.w   d2, d1
                    lea      $DFF000, a5
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00CB4C
                    bra      _SNEXTBLIT

lbC00CB4C:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00CB4C
                    tst.b    $DFF002
_SNEXTBLIT:         bsr      SNEXTBLIT
lbC00CB72:          move.w   (sp)+, YCOORD
                    rts

DRAW_ERR:           bra      NODRAW

CALC_SPR:           add.w    YCOORD, d1
                    move.w   d1, YCOORD
                    sub.w    SPRCLIP_T, d1
                    bpl      lbC00CBC6
                    tst.b    P_DRAW
                    bne      PCALC_SPR
                    add.w    d7, d1
                    ble      lbC00CCB4
                    sub.w    d1, d7
                    mulu     d3, d7
                    tst.w    MASK_TYPE
                    bgt      lbC00CCB8
                    lsl.l    #3, d7
                    add.l    d7, a1
                    add.l    d7, a2
lbC00CBB8:          move.w   d1, d7
                    moveq    #0, d1
                    clr.w    YCOORD
                    bra      lbC00CC1E

lbC00CBC6:          tst.b    P_DRAW
                    beq      lbC00CC08
                    move.w   d1, d4
                    sub.w    #$110, d4
                    bpl      lbC00CCB4
                    add.w    d7, d4
                    cmp.w    #-16, d4
                    bmi      lbC00CC08
                    move.w   d3, -(sp)
                    move.w   YSCRO, d3
                    and.w    #15, d3
                    add.w    d3, d4
                    ble      lbC00CC02
                    sub.w    d4, d7
                    bgt      lbC00CC02
                    move.w   (sp)+, d3
                    bra      lbC00CCB4

lbC00CC02:          move.w   (sp)+, d3
                    bra      lbC00CC1E

lbC00CC08:          move.w   d1, d4
                    cmp.w    #$100, d4
                    bpl      lbC00CCB4
                    add.w    d7, d4
                    sub.w    #$100, d4
                    ble      lbC00CC1E
                    sub.w    d4, d7
lbC00CC1E:          add.w    SPRCLIP_T, d1
                    move.l   BUFF_PTR, a0
                    subq.l   #6, a0
                    mulu     #$C0, d1
                    lsr.w    #3, d5
                    and.l    #$FFFE, d5
                    move.w   d5, XOFF
                    add.l    d5, d1
                    add.l    d1, a0
                    move.w   #$30, d5
                    move.w   d3, SPR_MOD
                    move.w   d0, d3
                    lsl.w    #1, d0
                    sub.w    d0, d5
                    bsr      CALC_BOT
                    lsl.w    #8, d7
                    add.w    d3, d7
                    ror.w    #4, d6
                    tst.b    LIGHT_SPR
                    bne      lbC00CCA4
                    tst.b    BLACK_SPR
                    bne      lbC00CC94
                    tst.b    WHITE_SPR
                    bne      lbC00CC84
                    move.w   #$FCA, d4
                    move.w   #$FCA, d1
                    rts

lbC00CC84:          move.w   #$B7A, d4
                    clr.b    WHITE_SPR
                    move.w   #$B7A, d1
                    rts

lbC00CC94:          move.w   #$B0A, d4
                    clr.b    BLACK_SPR
                    move.w   #$B0A, d1
                    rts

lbC00CCA4:          move.w   #$BFA, d4
                    clr.b    LIGHT_SPR
                    move.w   #$BFA, d1
                    rts

lbC00CCB4:          moveq    #0, d7
                    rts

lbC00CCB8:          lsl.l    #1, d7
                    add.l    d7, a1
                    lsl.l    #2, d7
                    add.l    d7, a2
                    bra      lbC00CBB8

PCALC_SPR:          move.w   YSCRO, d4
                    and.w    #15, d4
                    add.w    d4, d1
                    add.w    d7, d1
                    ble      lbC00CDB0
                    sub.w    d1, d7
                    bpl      lbC00CCE6
                    add.w    d1, d7
                    sub.w    d7, d1
                    sub.w    d4, d1
                    bra      lbC00CCFC

lbC00CCE6:          mulu     d3, d7
                    tst.w    MASK_TYPE
                    bgt      lbC00CD76
                    add.l    d7, a1
                    add.l    d7, a2
lbC00CCF6:          move.w   d1, d7
                    move.w   d4, d1
                    neg.w    d1
lbC00CCFC:          add.w    SPRCLIP_T, d1
                    move.l   BUFF_PTR, a0
                    subq.l   #6, a0
                    muls     #$C0, d1
                    lsr.w    #3, d5
                    and.l    #$FFFE, d5
                    move.w   d5, XOFF
                    add.l    d5, d1
                    add.l    d1, a0
                    move.l   SCROLL, a5
                    subq.l   #6, a5
                    cmp.l    a5, a0
                    bpl      lbC00CD34
                    add.l    #$CC00, a0
lbC00CD34:          move.w   #$30, d5
                    move.w   d3, SPR_MOD
                    move.w   d0, d3
                    lsl.w    #1, d0
                    sub.w    d0, d5
                    bsr      PCALC_BOT
                    lsl.w    #8, d7
                    add.w    d3, d7
                    ror.w    #4, d6
                    tst.b    LIGHT_SPR
                    bne      lbC00CDA0
                    tst.b    BLACK_SPR
                    bne      lbC00CD90
                    tst.b    WHITE_SPR
                    bne      lbC00CD80
                    move.w   #$FCA, d4
                    move.w   #$FCA, d1
                    rts

lbC00CD76:          add.l    d7, a2
                    lsr.l    #2, d7
                    add.l    d7, a1
                    bra      lbC00CCF6

lbC00CD80:          move.w   #$B7A, d4
                    clr.b    WHITE_SPR
                    move.w   #$B7A, d1
                    rts

lbC00CD90:          move.w   #$B0A, d4
                    clr.b    BLACK_SPR
                    move.w   #$B0A, d1
                    rts

lbC00CDA0:          move.w   #$BFA, d4
                    clr.b    LIGHT_SPR
                    move.w   #$BFA, d1
                    rts

lbC00CDB0:          moveq    #0, d7
                    rts

PCALC_BOT:          move.w   #$110, d0
                    sub.w    YSCRO, d0
                    sub.w    YCOORD, d0
                    sub.w    d4, d0
                    ble      lbC00CE0C
                    sub.w    d7, d0
                    bpl      lbC00CE2A
                    move.b   #1, DBL_BLIT
                    move.w   d0, d2
                    add.w    d0, d7
                    neg.w    d2
                    move.w   d7, d0
                    mulu     SPR_MOD, d0
                    lsl.w    #3, d0
                    lea      0(a1,d0.w), a4
                    tst.w    MASK_TYPE
                    bgt      lbC00CE32
lbC00CDF6:          lea      0(a2,d0.w), a6
                    move.l   SCROLL, a3
                    move.w   XOFF, d0
                    add.l    d0, a3
                    subq.w   #6, a3
                    rts

lbC00CE0C:          neg.w    d0
                    mulu     #$C0, d0
                    move.l   SCROLL, a0
                    add.w    XOFF, d0
                    add.l    d0, a0
                    subq.w   #6, a0
                    clr.b    DBL_BLIT
                    rts

lbC00CE2A:          clr.b    DBL_BLIT
                    rts

lbC00CE32:          lsr.w    #2, d0
                    bra.s    lbC00CDF6

CALC_BOT:           move.w   #$110, d0
                    sub.w    YSCRO, d0
                    sub.w    YCOORD, d0
                    ble      lbC00CE8C
                    sub.w    d7, d0
                    bpl      lbC00CEAA
                    move.b   #1, DBL_BLIT
                    move.w   d0, d2
                    add.w    d0, d7
                    neg.w    d2
                    move.w   d7, d0
                    mulu     SPR_MOD, d0
                    lsl.w    #3, d0
                    lea      0(a2,d0.w), a6
                    tst.w    MASK_TYPE
                    bgt      lbC00CEB2
lbC00CE76:          lea      0(a1,d0.w), a4
                    move.l   SCROLL, a3
                    move.w   XOFF, d0
                    add.l    d0, a3
                    subq.w   #6, a3
                    rts

lbC00CE8C:          neg.w    d0
                    mulu     #$C0, d0
                    move.l   SCROLL, a0
                    add.w    XOFF, d0
                    add.l    d0, a0
                    subq.w   #6, a0
                    clr.b    DBL_BLIT
                    rts

lbC00CEAA:          clr.b    DBL_BLIT
                    rts

lbC00CEB2:          lsr.w    #2, d0
                    bra.s    lbC00CE76

BLIT_SPR:           tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00CECA
                    bra      lbC00CEEC

lbC00CECA:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00CECA
                    tst.b    $DFF002
lbC00CEEC:          lea      $DFF000, a5
                    move.w   #$fffe, $62(a5)
                    move.w   #$fffe, $64(a5)
BLIT2_SPR:          move.w   #$FFFF, $44(a5)
                    move.w   #$0000, $46(a5)
                    move.w   d5, $60(a5)
                    move.w   d5, $66(a5)
                    move.w   d6, $42(a5)
                    or.w     d6, d4
                    move.w   d4, $40(a5)
NEXTBLIT:           move.l   a1, $50(a5)
                    move.l   a2, $4C(a5)
                    move.l   a0, $48(a5)
                    move.l   a0, $54(a5)
                    move.w   d7, d0
                    lsr.w    #6, d0
                    move.w   d7, $58(a5)
                    tst.b    P_DRAW
                    bne      lbC00CF60
                    move.l   UNDRAW_PTR, a1
                    move.l   a0, (a1)+
                    move.w   d7, (a1)+
                    move.w   d5, (a1)+
                    move.l   a1, UNDRAW_PTR
                    tst.b    ANDYFRAME
                    beq      lbC00CF62
                    addq.w   #1, SPRITES_CNTB
lbC00CF60:          rts

lbC00CF62:          addq.w   #1, SPRITES_CNTA
                    rts

UNDRAW:             move.w   SPRITES_CNTA, d0
                    lea      UNDRAW_TABA, a6
                    tst.b    ANDYFRAME
                    beq      lbC00CF8C
                    move.w   SPRITES_CNTB, d0
                    lea      UNDRAW_TABB, a6
lbC00CF8C:          cmp.w    MAX_UNDS, d0
                    ble      lbC00CF9C
                    move.w   d0, MAX_UNDS
lbC00CF9C:          subq.w   #1, d0
                    bmi      lbC00CFD0
lbC00CFA2:          move.l   (a6)+, d1
                    move.w   (a6)+, d7
                    move.w   (a6)+, d5
                    move.l   d1, a1
                    sub.l    SCROLL, d1
                    move.l   SCROLLC, a0
                    add.l    d1, a0
                    bsr      UNBLIT_SPR
                    dbra     d0, lbC00CFA2
                    tst.b    ANDYFRAME
                    bne      lbC00CFD2
                    clr.w    SPRITES_CNTA
lbC00CFD0:          rts

lbC00CFD2:          clr.w    SPRITES_CNTB
                    rts

UNBLIT_SPR:         tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00CFEE
                    bra      lbC00D010

lbC00CFEE:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00CFEE
                    tst.b    $DFF002
lbC00D010:          lea      $DFF000, a5
                    move.w   #$ffff, $44(a5)
                    move.w   #$ffff, $46(a5)
                    move.w   d5, $64(a5)
                    move.w   d5, $66(a5)
                    move.w   #0, $42(a5)
                    move.w   #$9F0, $40(a5)
                    move.l   a0, $50(a5)
                    move.l   a1, $54(a5)
                    move.w   d7, $58(a5)
                    rts

SBLIT_SPR:          tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00D058
                    bra      lbC00D07A

lbC00D058:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00D058
                    tst.b    $DFF002
lbC00D07A:          lea      $DFF000, a5
                    move.w   #$FFFF, $44(a5)
                    move.w   #$0000, $46(a5)
                    move.w   SPR_MOD, d0
                    lsl.w    #2, d0
                    add.w    SPR_MOD, d0
                    add.w    SPR_MOD, d0
                    subq.w   #2, d0
                    move.w   d5, d1
                    add.w    #$90, d1
                    move.w   d0, $62(a5)
                    move.w   #-2, $64(a5)
                    move.w   d1, $60(a5)
                    move.w   d1, $66(a5)
                    move.w   d6, $42(a5)
                    or.w     d6, d4
                    move.w   d4, $40(a5)
                    move.w   d7, d1
                    lsr.w    #2, d1
                    and.w    #$FFC0, d1
                    move.w   d7, d4
                    and.w    #$3F, d4
                    or.w     d4, d1
SNEXTBLIT:          move.w   SPR_MOD, d0
                    lsl.w    #1, d0
                    move.l   a1, $50(a5)
                    move.l   a2, $4C(a5)
                    move.l   a0, $48(a5)
                    move.l   a0, $54(a5)
                    move.w   d1, $58(a5)
                    lea      0(a2,d0.w), a2
                    lea      $30(a0), a0
BP2:                tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00D10C
                    bra      lbC00D12E

lbC00D10C:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00D10C
                    tst.b    $DFF002
lbC00D12E:          move.l   a1, $50(a5)
                    move.l   a2, $4C(a5)
                    move.l   a0, $48(a5)
                    move.l   a0, $54(a5)
                    move.w   d1, $58(a5)
                    lea      0(a2,d0.w), a2
                    lea      $30(a0), a0
BP3:                tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00D15E
                    bra      lbC00D180

lbC00D15E:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00D15E
                    tst.b    $DFF002
lbC00D180:          move.l   a1, $50(a5)
                    move.l   a2, $4C(a5)
                    move.l   a0, $48(a5)
                    move.l   a0, $54(a5)
                    move.w   d1, $58(a5)
                    lea      0(a2,d0.w), a2
                    lea      $30(a0), a0
BP4:                tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC00D1B0
                    bra      lbC00D1D2

lbC00D1B0:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC00D1B0
                    tst.b    $DFF002
lbC00D1D2:          move.l   a1, $50(a5)
                    move.l   a2, $4C(a5)
                    move.l   a0, $48(a5)
                    move.l   a0, $54(a5)
                    move.w   d1, $58(a5)
                    lea      0(a2,d0.w), a2
                    lea      -$90(a0), a0
                    lsl.w    #2, d0
                    neg.w    d0
                    lea      0(a2,d0.w), a2
                    tst.b    P_DRAW
                    bne      lbC00D222
                    move.l   UNDRAW_PTR, a1
                    move.l   a0, (a1)+
                    move.w   d7, (a1)+
                    move.w   d5, (a1)+
                    move.l   a1, UNDRAW_PTR
                    tst.b    ANDYFRAME
                    beq      lbC00D224
                    addq.w   #1, SPRITES_CNTB
lbC00D222:          rts

lbC00D224:          addq.w   #1, SPRITES_CNTA
                    rts

CONV_COL7:          lea      SPR_TAB, a1
lbC00D232:          move.l   SPRITE_AD, a0
                    move.l   (a1)+, d0
                    bmi      lbC00D2B6
                    add.l    d0, a0
                    tst.b    MENTB_MASK
                    beq      lbC00D252
                    cmp.l    MENTB_AD, a0
                    bmi.s    lbC00D232
lbC00D252:          move.w   (a0), d7
                    move.w   4(a0), d0
                    move.w   d0, d3
                    move.w   d0, d1
                    addq.w   #1, d1
                    lsl.w    #1, d1
                    addq.l   #8, a0
                    subq.w   #1, d7
lbC00D264:          lea      (a0), a3
lbC00D266:          lea      (a0), a2
                    move.w   (a0), d6
                    lea      0(a0,d1.w), a0
                    move.w   (a0), d5
                    and.w    d5, d6
                    lea      0(a0,d1.w), a0
                    move.w   (a0), d5
                    and.w    d5, d6
                    lea      0(a0,d1.w), a0
                    move.w   (a0), d5
                    not.w    d5
                    and.w    d5, d6
                    not.w    d6
                    lea      (a2), a0
                    and.w    d6, (a0)
                    lea      0(a0,d1.w), a0
                    and.w    d6, (a0)
                    lea      0(a0,d1.w), a0
                    and.w    d6, (a0)
                    lea      0(a0,d1.w), a0
                    and.w    d6, (a0)
                    lea      2(a2), a0
                    dbra     d0, lbC00D266
                    move.w   d3, d0
                    move.w   d1, d2
                    lsl.w    #2, d2
                    lea      0(a3,d2.w), a0
                    dbra     d7, lbC00D264
                    bra      lbC00D232

lbC00D2B6:          rts

CONV_COL6:          lea      SPR_TAB, a1
lbC00D2BE:          move.l   SPRITE_AD, a0
                    move.l   (a1)+, d0
                    bmi      lbC00D34E
                    add.l    d0, a0
                    tst.b    MENTB_MASK
                    beq      lbC00D2E8
                    tst.b    MENTB_MASK
                    beq      lbC00D2E8
                    cmp.l    MENTB_AD, a0
                    bmi.s    lbC00D2BE
lbC00D2E8:          move.w   (a0), d7
                    move.w   4(a0), d0
                    move.w   d0, d3
                    move.w   d0, d1
                    addq.w   #1, d1
                    lsl.w    #1, d1
                    addq.l   #8, a0
                    subq.w   #1, d7
lbC00D2FA:          lea      (a0), a3
lbC00D2FC:          lea      (a0), a2
                    move.w   (a0), d6
                    not.w    d6
                    lea      0(a0,d1.w), a0
                    move.w   (a0), d5
                    and.w    d5, d6
                    lea      0(a0,d1.w), a0
                    move.w   (a0), d5
                    and.w    d5, d6
                    lea      0(a0,d1.w), a0
                    move.w   (a0), d5
                    not.w    d5
                    and.w    d5, d6
                    not.w    d6
                    lea      (a2), a0
                    and.w    d6, (a0)
                    lea      0(a0,d1.w), a0
                    and.w    d6, (a0)
                    lea      0(a0,d1.w), a0
                    and.w    d6, (a0)
                    lea      0(a0,d1.w), a0
                    and.w    d6, (a0)
                    lea      2(a2), a0
                    dbra     d0, lbC00D2FC
                    move.w   d3, d0
                    move.w   d1, d2
                    lsl.w    #2, d2
                    lea      0(a3,d2.w), a0
                    dbra     d7, lbC00D2FA
                    bra      lbC00D2BE

lbC00D34E:          rts

MENTB_MASK:         dcb.b    2, 0

SPRITE_TAB:         move.l   SPRITE_AD, a0
                    move.l   MASK_AD, a3
                    lea      SPRM_TAB, a2
                    lea      (a0), a4
                    add.l    #$17250, a4
                    lea      SPR_TAB, a1
                    clr.l    d5
                    clr.l    d6
                    tst.b    MENTB_MASK
                    beq      lbC00D3A4
lbC00D380:          cmp.l    MENTB_AD, a0
                    bpl      lbC00D3A4
                    addq.l   #4, a1
                    addq.l   #4, a2
                    move.l   (a1), d6
                    move.l   SPRITE_AD, a0
                    add.l    d6, a0
                    move.l   (a2), d5
                    move.l   MASK_AD, a3
                    add.l    d5, a3
                    bra.s    lbC00D380

lbC00D3A4:          move.l   d6, (a1)+
                    move.l   d5, (a2)+
                    move.w   (a0), d7
                    beq      lbC00D3DA
                    bsr      lbC00D418
                    move.w   4(a0), d0
                    addq.w   #1, d0
                    mulu     #8, d0
                    mulu     d7, d0
                    tst.b    SAVE_SPACE
                    beq      lbC00D3CE
                    cmp.l    a4, a0
                    bmi      lbC00D40A
lbC00D3CE:          add.l    d0, d5
                    add.l    d0, a3
lbC00D3D2:          addq.l   #8, d0
                    add.l    d0, a0
                    add.l    d0, d6
                    bra.s    lbC00D3A4

lbC00D3DA:          move.l   #-1, -4(a1)
                    tst.b    BONUS_CS
                    bne      lbC00D408
                    tst.b    MENTB_MASK
                    beq      lbC00D408
                    jsr      CONV_COL6
                    jsr      CONV_COL7
                    sf       MENTB_MASK
lbC00D408:          rts

lbC00D40A:          move.l   d0, d1
                    lsr.l    #2, d1
                    add.l    d1, d5
                    move.w   #1, 6(a0)
                    bra.s    lbC00D3D2

lbC00D418:          tst.b    SAVE_SPACE
                    beq      lbC00D42C
                    bra      lbC00D4AA

                    cmp.l    a4, a0
                    bmi      lbC00D4AA
lbC00D42C:          move.l   a3, -(sp)
                    move.w   d7, d0
                    subq.w   #1, d0
                    move.l   a0, -(sp)
                    move.w   4(a0), d1
                    move.w   d1, TEMPW3
                    move.w   d1, d2
                    move.w   d1, d4
                    addq.l   #1, d4
                    lsl.w    #3, d4
                    and.l    #$FFFF, d4
                    lsl.w    #1, d2
                    addq.l   #2, d2
                    addq.w   #8, a0
lbC00D452:          move.l   a0, -(sp)
                    move.l   a3, -(sp)
lbC00D456:          move.w   (a0), d3
                    lea      0(a0,d2.w), a0
                    or.w     (a0), d3
                    lea      0(a0,d2.w), a0
                    or.w     (a0), d3
                    lea      0(a0,d2.w), a0
                    or.w     (a0), d3
                    lea      0(a0,d2.w), a0
                    sub.l    d4, a0
                    addq.l   #2, a0
                    move.w   d3, (a3)
                    lea      0(a3,d2.w), a3
                    move.w   d3, (a3)
                    lea      0(a3,d2.w), a3
                    move.w   d3, (a3)
                    lea      0(a3,d2.w), a3
                    move.w   d3, (a3)
                    lea      0(a3,d2.w), a3
                    sub.l    d4, a3
                    addq.l   #2, a3
                    dbra     d1, lbC00D456
                    move.w   TEMPW3, d1
                    move.l   (sp)+, a3
                    move.l   (sp)+, a0
                    add.l    d4, a0
                    add.l    d4, a3
                    dbra     d0, lbC00D452
                    move.l   (sp)+, a0
                    move.l   (sp)+, a3
                    rts

lbC00D4AA:          move.w   d7, d0
                    subq.w   #1, d0
                    move.l   a0, -(sp)
                    move.w   4(a0), d1
                    move.w   d1, TEMPW3
                    move.w   d1, d2
                    move.w   d1, d4
                    addq.l   #1, d4
                    lsl.w    #3, d4
                    and.l    #$FFFF, d4
                    lsl.w    #1, d2
                    addq.l   #2, d2
                    addq.w   #8, a0
lbC00D4CE:          move.l   a0, -(sp)
lbC00D4D0:          move.w   (a0), d3
                    lea      0(a0,d2.w), a0
                    or.w     (a0), d3
                    lea      0(a0,d2.w), a0
                    or.w     (a0), d3
                    lea      0(a0,d2.w), a0
                    or.w     (a0), d3
                    lea      0(a0,d2.w), a0
                    sub.l    d4, a0
                    addq.l   #2, a0
                    move.w   d3, (a3)+
                    dbra     d1, lbC00D4D0
                    move.w   TEMPW3, d1
                    move.l   (sp)+, a0
                    add.l    d4, a0
                    dbra     d0, lbC00D4CE
                    move.l   (sp)+, a0
                    rts

INIT_CHIP:          move.l   #INTRO_SPSTART, SPRITE_AD
                    move.l   #BOT_CHIP, MASK_AD
                    add.l    #$117BA, MASK_AD
                    lea      SPSAVE, a0
lbC00D528:          clr.l    (a0)+
                    cmp.l    #END_OF_DATA, a0
                    bmi.s    lbC00D528
                    rts

INIT_GAME:          bsr      INIT_GVARS
                    bsr      INIT_PANEL
                    rts

INIT_GVARS:         clr.w    ZOONB_GOT
                    clr.w    END_OF_STG
                    sf       ZOOL_DIES
                    sf       BEG_GAME
                    sf       PLAYER_ON
                    lea      HISCR_TAB, a0
                    move.l   (a0), HISCORE
                    clr.l    SCORE
                    btst     #1, CHEAT+1
                    bne      lbC00D5B0
                    btst     #0, CHEAT+1
                    bne      lbC00D5BA
                    move.w   #4, LIVES
                    move.b   DIFFICULTY, d7
                    beq      lbC00D5AE
                    move.w   #2, LIVES
                    tst.b    d7
                    bpl      lbC00D5AE
                    move.w   #6, LIVES
lbC00D5AE:          rts

lbC00D5B0:          move.w   #20, LIVES
                    rts

lbC00D5BA:          move.w   #10, LIVES
                    rts

INIT_LEVEL:         bsr      SET_MBPNTRS
                    bsr      INIT_ZOOL
                    bsr      CLEAR_TABS
                    sf       BRICKS_ON
                    sf       END_FIGHT
                    sf       DOUBLE_COP
                    move.l   A_MBLK_SPRS, MENT_SPDATA
                    move.l   A_END_MB, END_MENT
                    move.b   LEVEL_NUM, d0
                    cmp.b    #1, d0
                    bmi      BB3_BEACS                  ; EGGS
                    beq      BULB_LEVEL                 ; BULBS
                    cmp.b    #3, d0
                    bmi      TOOT_LEVEL                 ; EGYPT
                    beq      SNAKE_LEVEL                ; DICES/SNAKES
                    cmp.b    #5, d0
                    bmi      ICES_LEVEL                 ; CANDIES/ICE
                    beq      LAST_LEVEL                 ; CUBES
                    bra      PAPE_LEVEL                 ; PAPER

CLEAR_TABS:         lea      PERM_TAB, a1
lbC00D62A:          clr.w    (a1)+
                    cmp.l    #END_PERMS, a1
                    bne.s    lbC00D62A
                    lea      BACKSP_TAB, a1
lbC00D63A:          clr.w    (a1)+
                    cmp.l    #BACKSP_LNS, a1
                    bne.s    lbC00D63A
                    lea      BRICKS_TAB, a1
lbC00D64A:          clr.w    (a1)+
                    cmp.l    #END_BRICKS, a1
                    bne.s    lbC00D64A
                    lea      TILEFX_TAB, a1
lbC00D65A:          clr.w    (a1)+
                    cmp.l    #END_TILEFX, a1
                    bne.s    lbC00D65A
                    lea      ARCHEX_TAB, a1
lbC00D66A:          clr.w    (a1)+
                    cmp.l    #DEL_ARCH, a1
                    bne.s    lbC00D66A
                    lea      TOKEN_TAB, a1
lbC00D67A:          clr.w    (a1)+
                    cmp.l    #TOKENS_ON, a1
                    bne.s    lbC00D67A
                    lea      SHOT_TAB, a1
lbC00D68A:          clr.w    (a1)+
                    cmp.l    #Y_CENTRE, a1
                    bne.s    lbC00D68A
                    lea      LOLL_TAB, a1
lbC00D69A:          clr.w    (a1)+
                    cmp.l    #LAST_LOLL, a1
                    bne.s    lbC00D69A
                    lea      BUTT1_TAB, a1
lbC00D6AA:          sf       (a1)
                    lea      14(a1), a1
                    cmp.l    #BLK_TRIES, a1
                    bne.s    lbC00D6AA
                    lea      TORCH_TAB1, a1
                    lea      TRESET_TAB1, a0
lbC00D6C4:          move.w   (a0)+, (a1)+
                    cmp.l    #END_TRES, a0
                    bne.s    lbC00D6C4
                    clr.w    TOKENS_ON
                    rts

SET_MBPNTRS:        lea      MBSIZE_TAB, a0
                    move.b   LEVEL_NUM, d7
                    ext.w    d7
                    lsl.w    #2, d7
                    move.l   0(a0,d7.w), a0
                    lea      END_MB, a1
                    cmp.b    #5, LEVEL_NUM
                    bne      lbC00D702
                    lea      lbL09F234, a1
lbC00D702:          move.l   a1, A_MBLK_SPRS
                    add.l    (a0)+, a1
                    move.l   a1, A_END_MB
                    move.l   a1, A_MBLK_SP2
                    add.l    (a0)+, a1
                    move.l   a1, A_END_MB2
                    move.l   a1, A_BON_MAP
                    add.l    (a0)+, a1
                    move.l   a1, A_BON_REF

                    add.l    (a0)+, a1
                    move.l   a1, A_SNK_M1
                    add.l    (a0)+, a1
                    move.l   a1, A_SNK_R1
                    add.l    (a0)+, a1
                    move.l   a1, A_SNK_M2
                    add.l    (a0)+, a1
                    move.l   a1, A_SNK_R2
                    add.l    (a0)+, a1
                    move.l   a1, A_SNK_M3
                    add.l    (a0)+, a1
                    move.l   a1, A_SNK_R3
                    add.l    (a0)+, a1
                    move.l   a1, A_END_ADD
                    rts

MBSIZE_TAB:         dc.l     BDSIZE_TAB
                    dc.l     BBSIZE_TAB
                    dc.l     TCSIZE_TAB
                    dc.l     SNSIZE_TAB
                    dc.l     ICSIZE_TAB
                    dc.l     LSSIZE_TAB
                    dc.l     PPSIZE_TAB
BDSIZE_TAB:         dc.l     28242, 0, 648, 360, 0, 0, 0, 0, 0, 0
BBSIZE_TAB:         dc.l     23362, 0, 648, 360, 0, 0, 0, 0, 0, 0
TCSIZE_TAB:         dc.l     18346, 0, 648, 360, 0, 0, 0, 0, 0, 0
SNSIZE_TAB:         dc.l     14114, 0, 648, 360, 938, 468, 1154, 778, 1078, 0
ICSIZE_TAB:         dc.l     10354, 0, 648, 360, 0, 0, 0, 0, 0, 0
LSSIZE_TAB:         dc.l     24138, 28242, 648, 360, 0, 0, 0, 0, 0, 0
PPSIZE_TAB:         dc.l     28242, 0, 648, 360, 0, 0, 0, 0, 0, 0

SET_PERC:           move.l   REF_MAP, a0
                    move.w   MAP_WIDTH, d0
                    mulu     MAP_HEIGHT, d0
                    clr.w    d1
                    subq.l   #1, d0
lbC00D8B0:          move.b   (a0)+, d7
                    cmp.b    #11, d7
                    beq      lbC00D8D2
                    cmp.b    #12, d7
                    beq      lbC00D8D6
                    cmp.b    #$16, d7
                    beq      lbC00D8D2
lbC00D8CA:          subq.l   #1, d0
                    bne.s    lbC00D8B0
                    bra      lbC00D8DA

lbC00D8D2:          addq.w   #1, d1
                    bra.s    lbC00D8CA

lbC00D8D6:          addq.w   #2, d1
                    bra.s    lbC00D8CA

lbC00D8DA:          move.l   #$7BC0, d2
                    cmp.b    #4, LEVEL_NUM
                    bne      lbC00D8FE
                    cmp.b    #1, STAGE_NUM
                    bne      lbC00D8FE
                    move.l   #$C918, d2
lbC00D8FE:          cmp.b    #2, LEVEL_NUM
                    bne      lbC00D920
                    cmp.b    #2, STAGE_NUM
                    bne      lbC00D920
                    move.l   #$875A, d2
                    bra      lbC00D984

lbC00D920:          tst.b    LEVEL_NUM
                    bne      lbC00D940
                    cmp.b    #1, STAGE_NUM
                    bne      lbC00D940
                    move.l   #$9AB0, d2
                    bra      lbC00D984

lbC00D940:          cmp.b    #1, LEVEL_NUM
                    bne      lbC00D962
                    cmp.b    #2, STAGE_NUM
                    bne      lbC00D962
                    move.l   #$9AB0, d2
                    bra      lbC00D984

lbC00D962:          cmp.b    #3, LEVEL_NUM
                    bne      lbC00D984
                    cmp.b    #2, STAGE_NUM
                    bne      lbC00D984
                    move.l   #$9AB0, d2
                    bra      lbC00D984

lbC00D984:          divu     d1, d2
                    move.w   d2, PERC_ADD
                    move.b   DIFFICULTY, d7
                    ext.w    d7
                    neg.w    d7
                    lsl.w    #1, d7
                    move.w   PERC_ADD, d6
                    lsr.w    #4, d6
                    addq.w   #1, d6
                    muls     d7, d6
                    add.w    d6, PERC_ADD
                    btst     #4, CHEAT+1
                    beq      lbC00D9BE
                    ifeq TRAINER
                    move.w   #$6300, PERCENT
                    else
                    move.w   #$6363, PERCENT
                    endif
lbC00D9BE:          rts

INIT_LDATA:         jsr      LOAD_SPRS
                    clr.w    COLBAND_PTR
                    sf       SCROLL_OFF
                    sf       MESSAGE_ON
                    ifeq TRAINER
                    clr.w    PERCENT
                    else
                    move.w   #$6363, PERCENT
                    endif
                    sf       BADDY_ON
                    st       REVIVE
                    jsr      RESTART1
                    bsr      LOAD_BEACS
                    jsr      RESET_EBAR
                    rts

CLEAR_SNKS:         lea      SNAKE_TAB, a0
lbC00DA02:          clr.w    (a0)+
                    cmp.l    #SPARK_AN, a0
                    bne.s    lbC00DA02
                    rts

LOAD_BEACS:         lea      BEACONS_TAB, a1
                    move.l   BEG_BEACS, a0
                    move.w   BEACONS, d7
                    subq.w   #1, d7
                    bmi      lbC00DA3E
lbC00DA26:          st       (a1)+
                    sf       (a1)+
                    move.w   (a0)+, d0
                    lsl.w    #4, d0
                    move.w   d0, (a1)+
                    move.w   (a0)+, d1
                    subq.w   #2, d1
                    lsl.w    #4, d1
                    move.w   d1, (a1)+
                    clr.w    (a1)+
                    dbra     d7, lbC00DA26
lbC00DA3E:          cmp.l    #BEACONS, a1
                    beq      lbC00DA4E
                    sf       (a1)
                    addq.l   #8, a1
                    bra.s    lbC00DA3E

lbC00DA4E:          rts

INIT_ZOOL:          clr.w    X_FORCE
                    clr.w    Y_FORCE
                    clr.w    ZOOL_XDIS
                    clr.w    ZOOL_YDIS
                    st       ZOOL_FACE
                    move.w   #$31, ZOOL_SPR
                    clr.w    ZOOL_MOVE
                    clr.b    INAIR
                    st       REVIVE
                    move.w   #$80, ZOOL_SCRY
                    move.w   #$A0, ZOOL_SCRX
                    move.w   #$A0, PULLBACK
                    move.b   #$50, ZOOL_HIT
                    sf       ZOOL_DIES
                    st       MAINGUY_ON
                    move.w   #4, X_FRICTION
                    lea      SMART_TAB, a0
                    sf       (a0)
                    sf       $10(a0)
                    sf       $20(a0)
                    clr.w    SMARTS
                    sf       BIGB_POW
                    sf       SHADE_ON
                    clr.w    SHIELD_ON
                    rts

PAPE_LEVEL:         move.l   #lbL071D60, CUST_ATTRBS
                    move.l   #LEV_MAPS, CUST_REFS
                    move.l   #PAPER_PAL, LEV_PAL
                    move.l   #PC7_1, COLBAND
                    move.l   #PC7_2, COLBAND2
                    move.w   #8, LBEAM_SPRS
                    move.w   #$CA, SMASH1L
                    move.w   #$CF, SMASH1R
                    move.w   #$4A, SMASH2L
                    move.w   #$1EF, SMASH2R
                    move.w   #$14, END_PERMS
                    move.w   #$11, SMASH2_SPR
                    move.w   #-16, SMASH_XOFF
                    move.w   #-16, SMASH_YOFF
                    move.w   #$9A, LEVEL_SPRS
                    move.w   #$8E, ARCHEX_SPR
                    move.w   #$8A, TOKEN_SPR
                    move.w   #$3E80, END_X
                    move.w   #5, FILLTILE_SPR
                    move.w   #6, FILLTILE_SP2
                    move.l   #DOCOL_BAND2, DOCOL_RTN
                    move.w   #0, FILL_TILE1
                    move.w   #$CC, FILL_TILE2
                    move.w   #$11, SPLAT_ANIM
                    move.b   #9, END_TILEFX
                    move.b   #10, ROOFEX_ANIM
                    move.b   #15, PLATEX_ANIM
                    move.b   #3, HEART_RES
                    move.w   #$A7, BONUS_SP
                    st       DOUBLE_COP
                    cmp.b    #1, STAGE_NUM
                    bgt      lbC00DDD4
                    beq      lbC00DCE2
                    move.w   #0, TORCHES
                    move.w   #0, SPR_CNT
                    move.w   #10, FIXED_SPRS
                    move.l   #PP1_SPRS, STAGE_SPRS
                    move.l   #NOZONES, ZONE_TAB
                    move.l   #SPR_DATA, FIXED_BANK
                    move.w   #1, STARTX
                    move.w   #25, STARTY
                    move.w   #$E0, ZOOL_SCRX
                    move.w   #$10E0, ENDICON_X
                    move.w   #$150, ENDICON_Y
                    move.w   #$1A0, ENDIC_FLR
                    st       ENDICON_ON
                    move.l   #EGG_BOMB_S_BB, EGG_BOMBS
                    move.l   #EXP_BOMB_S_BB, EXP_BOMBS
                    move.l   #BB1_FIXSPRS, ENEMY_BULLS
                    move.l   #BB2_SPRS, HOME_BULLS
                    move.l   #lbL071D82, CURRENT_MAP
                    move.l   #lbL07750A, REF_MAP
                    move.w   #$180, END_YB
                    st       NO_BADDY
                    move.w   #300, TIME
                    move.w   #300, START_TIME
                    move.l   #PP1_BEACS, BEG_BEACS
                    move.w   #3, BEACONS
                    move.w   #-336, ZOONB_X
                    move.w   #$2B0, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00DCE2:          move.w   #0, TORCHES
                    move.w   #0, SPR_CNT
                    move.w   #11, FIXED_SPRS
                    move.l   #PP2_SPRS, STAGE_SPRS
                    move.l   #NOZONES, ZONE_TAB
                    move.l   #SPR_DATA, FIXED_BANK
                    move.w   #1, STARTX
                    move.w   #10, STARTY
                    move.w   #$E0, ZOOL_SCRX
                    move.w   #$10E0, ENDICON_X
                    move.w   #$C0, ENDICON_Y
                    move.w   #$110, ENDIC_FLR
                    st       ENDICON_ON
                    move.l   #EGG_BOMB_S_BB, EGG_BOMBS
                    move.l   #EXP_BOMB_S_BB, EXP_BOMBS
                    move.l   #BB1_FIXSPRS, ENEMY_BULLS
                    move.l   #BB2_SPRS, HOME_BULLS
                    move.l   #lbL07A0CA, CURRENT_MAP
                    move.l   #lbL07F852, REF_MAP
                    move.w   #$F0, END_YB
                    st       NO_BADDY
                    move.w   #300, TIME
                    move.w   #300, START_TIME
                    move.l   #PP2_BEACS, BEG_BEACS
                    move.w   #2, BEACONS
                    move.w   #-336, ZOONB_X
                    move.w   #$2B0, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00DDD4:          move.w   #0, TORCHES
                    move.w   #0, SPR_CNT
                    move.w   #9, FIXED_SPRS
                    move.l   #PP3_SPRS, STAGE_SPRS
                    move.l   #NOZONES, ZONE_TAB
                    move.l   #SPR_DATA, FIXED_BANK
                    move.w   #1, STARTX
                    move.w   #5, STARTY
                    move.w   #$E0, ZOOL_SCRX
                    move.w   #$10E0, ENDICON_X
                    move.w   #$1B0, ENDICON_Y
                    move.w   #$210, ENDIC_FLR
                    st       ENDICON_ON
                    move.l   #EGG_BOMB_S_BB, EGG_BOMBS
                    move.l   #EXP_BOMB_S_BB, EXP_BOMBS
                    move.l   #BB1_FIXSPRS, ENEMY_BULLS
                    move.l   #BB2_SPRS, HOME_BULLS
                    move.l   #lbL082412, CURRENT_MAP
                    move.l   #lbL087B9A, REF_MAP
                    move.w   #$1F0, END_YB
                    st       NO_BADDY
                    move.w   #300, TIME
                    move.w   #300, START_TIME
                    move.l   #PP3_BEACS, BEG_BEACS
                    move.w   #3, BEACONS
                    move.w   #-336, ZOONB_X
                    move.w   #$2B0, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

PP1_BEACS:          dc.w     $4F, $17, $99, $1B, $C6, $13
PP2_BEACS:          dc.w     $6C, $20, $D2, $15
PP3_BEACS:          dc.w     $55, $17, $A2, 15, $D6, $23

BULB_LEVEL:         move.l   #lbL071FE0, CUST_ATTRBS
                    move.l   #LEV_MAPS, CUST_REFS
                    move.w   #2, STARTX
                    move.w   #$15, STARTY
                    move.w   #$1EA, LBEAM_TILE
                    move.l   #BULB_PAL, LEV_PAL
                    move.l   #PC2_1, COLBAND
                    move.l   #PC2_2, COLBAND2
                    move.w   #8, LBEAM_SPRS
                    move.w   #$CA, SMASH1L
                    move.w   #$CF, SMASH1R
                    move.w   #$4A, SMASH2L
                    move.w   #$1EF, SMASH2R
                    move.w   #$14, END_PERMS
                    move.w   #$11, SMASH2_SPR
                    move.w   #-16, SMASH_XOFF
                    move.w   #-16, SMASH_YOFF
                    move.w   #$A7, LEVEL_SPRS
                    move.w   #$9C, ARCHEX_SPR
                    move.w   #$98, TOKEN_SPR
                    move.w   #$3E80, END_X
                    move.w   #$12, FILLTILE_SPR
                    move.w   #$13, FILLTILE_SP2
                    move.l   #DOCOL_BAND2, DOCOL_RTN
                    move.w   #0, FILL_TILE1
                    move.w   #$CC, FILL_TILE2
                    move.w   #8, SPLAT_ANIM
                    move.b   #9, END_TILEFX
                    move.b   #10, ROOFEX_ANIM
                    move.b   #15, PLATEX_ANIM
                    move.b   #3, HEART_RES
                    move.w   #$A7, BONUS_SP
                    st       DOUBLE_COP
                    cmp.b    #1, STAGE_NUM
                    bgt      lbC00E208
                    beq      lbC00E104
                    move.w   #6, TORCHES
                    move.w   #54, SPR_CNT
                    move.w   #19, FIXED_SPRS
                    move.l   #BB1_SPRS, STAGE_SPRS
                    move.l   #BB1_ZONES, ZONE_TAB
                    move.l   #lbW01ACE4, FIXED_BANK
                    move.w   #$B4, STARTX
                    move.w   #$64, STARTY
                    move.w   #$E0, ZOOL_SCRX
                    move.l   #TORCH_TAB1, TORCH_PTR
                    move.w   #$BD0, ENDICON_X
                    move.w   #$30, ENDICON_Y
                    move.w   #$80, ENDIC_FLR
                    st       ENDICON_ON
                    move.l   #EGG_BOMB_S_BB, EGG_BOMBS
                    move.l   #EXP_BOMB_S_BB, EXP_BOMBS
                    move.l   #BB1_FIXSPRS, ENEMY_BULLS
                    move.l   #BB2_SPRS, HOME_BULLS
                    move.l   #lbL07202A, CURRENT_MAP
                    move.l   #lbL07DBB2, REF_MAP
                    move.w   #$60, END_YB
                    st       NO_BADDY
                    move.w   #300, TIME
                    move.w   #300, START_TIME
                    move.l   #BB1_BEACS, BEG_BEACS
                    move.w   #1, BEACONS
                    move.w   #$150, ZOONB_X
                    move.w   #$2B0, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00E104:          move.w   #8, TORCHES
                    move.w   #53, SPR_CNT
                    move.w   #8, FIXED_SPRS
                    move.l   #BB2_SPRS, STAGE_SPRS
                    move.l   #BB2_ZONES, ZONE_TAB
                    move.l   #lbL01AC90, FIXED_BANK
                    move.w   #1, STARTX
                    move.w   #$64, STARTY
                    move.w   #$60, ZOOL_SCRX
                    move.w   #$C0, ZOOL_SCRY
                    move.l   #TORCH_TAB2, TORCH_PTR
                    move.w   #$7E0, ENDICON_X
                    move.w   #$2B0, ENDICON_Y
                    move.w   #$300, ENDIC_FLR
                    st       ENDICON_ON
                    move.l   #EGG_BOMB_S2_BB, EGG_BOMBS
                    move.l   #EXP_BOMB_S2_BB, EXP_BOMBS
                    move.l   #BB2_FIXSPRS, ENEMY_BULLS
                    move.l   #BB3_SPRS, HOME_BULLS
                    move.l   #lbL083972, CURRENT_MAP
                    move.l   #lbL08F4FA, REF_MAP
                    move.w   #$2E0, END_YB
                    st       NO_BADDY
                    move.w   #300, TIME
                    move.w   #300, START_TIME
                    move.l   #BB2_BEACS, BEG_BEACS
                    move.w   #1, BEACONS
                    move.w   #$380, ZOONB_X
                    move.w   #$290, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00E208:          move.w   #9, TORCHES
                    move.w   #48, SPR_CNT
                    move.w   #11, FIXED_SPRS
                    move.l   #BB3_SPRS, STAGE_SPRS
                    move.l   #BB3_ZONES, ZONE_TAB
                    move.l   #lbL01ABC0, FIXED_BANK
                    move.w   #1, STARTX
                    move.w   #$47, STARTY
                    move.l   #TORCH_TAB3, TORCH_PTR
                    move.l   #EGG_BOMB_S3_BB, EGG_BOMBS
                    move.l   #EXP_BOMB_S3_BB, EXP_BOMBS
                    move.l   #BB3_FIXSPRS, ENEMY_BULLS
                    move.l   #TC1_SPRS, HOME_BULLS
                    move.l   #lbL0952BA, CURRENT_MAP
                    move.l   #lbL0A0E42, REF_MAP
                    move.w   #$D40, ENDICON_X
                    move.w   #$4A0, ENDICON_Y
                    move.w   #$4E0, ENDIC_FLR
                    move.w   #300, TIME
                    move.w   #300, START_TIME
                    sf       NO_BADDY
                    sf       ZOOL_FACE
                    move.l   #BB3_BEACS, BEG_BEACS
                    clr.w    BEACONS
                    move.w   #$6B0, ZOONB_X
                    move.w   #$6B0, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    bra      BULB_BADVAR

BULB_BADVAR:        move.w   #$500, BADDY_Y
                    move.w   #$FE0, BADDY_X
                    clr.w    BADDY_YDIS
                    move.w   #-96, BADDY_XDIS
                    move.b   #0, BADDY_ACT
                    move.w   #$2D, BADDY_EN
                    move.w   #$A7, BADDY_SPRS
                    move.w   #$16, BADDY_SPR
                    sf       BADDY_DEAD
                    move.w   #-175, MESSAGE_SPR
                    move.w   #$70, MESSAGE_X
                    move.w   #$5A, MESSAGE_Y
                    move.w   #$B30, END_X
                    move.w   #$440, END_YT
                    move.w   #$490, END_YB
                    move.l   #BULB_BADDY, BADDY_RTN
                    move.l   #BULB_BANIM, BANIMS_TAB
                    move.l   #BULBM_FRMS, BADDY_FRMS
                    move.w   #$40, BADDY_WID
                    move.w   #$58, BADDY_HGT
                    rts

BB1_BEACS:          dc.w     $95, $43
BB2_BEACS:          dc.w     $5B, 10

BB3_BEACS:          move.l   #lbL072290, CUST_ATTRBS
                    move.l   #LEV_MAPS, CUST_REFS
                    move.l   #BIRD_PAL, LEV_PAL
                    move.l   #PC1_2, COLBAND2
                    move.l   #PC1_1, COLBAND
                    st       DOUBLE_COP
                    move.w   #$CA, SMASH1L
                    move.w   #$CF, SMASH1R
                    move.w   #$4A, SMASH2L
                    move.w   #$1EF, SMASH2R
                    move.w   #$10, END_PERMS
                    move.w   #$11, SMASH2_SPR
                    move.w   #0, SMASH_XOFF
                    move.w   #0, SMASH_YOFF
                    move.w   #$A4, LEVEL_SPRS
                    move.w   #$99, ARCHEX_SPR
                    move.w   #$95, TOKEN_SPR
                    move.w   #$3E80, END_X
                    move.l   #DOCOL_BAND2, DOCOL_RTN
                    move.w   #0, FILL_TILE1
                    move.w   #$CC, FILL_TILE2
                    move.w   #6, FILLTILE_SPR
                    move.w   #6, FILLTILE_SP2
                    move.w   #5, SPLAT_ANIM
                    move.b   #4, END_TILEFX
                    move.b   #0, PLATEX_ANIM
                    move.b   #3, ROOFEX_ANIM
                    move.b   #3, HEART_RES
                    move.w   #$A4, BONUS_SP
                    cmp.b    #1, STAGE_NUM
                    bgt      lbC00E656
                    beq      lbC00E576
                    move.w   #66, SPR_CNT
                    move.l   #BD1_SPRS, STAGE_SPRS
                    move.l   #BD1_ZONES, ZONE_TAB
                    move.w   #6, FIXED_SPRS
                    move.l   #lbW01AF1C, FIXED_BANK
                    move.w   #5, STARTX
                    move.w   #$15, STARTY
                    st       NO_BADDY
                    move.w   #$18B0, ENDICON_X
                    move.w   #$2C0, ENDICON_Y
                    move.w   #$310, ENDIC_FLR
                    st       ENDICON_ON
                    move.w   #$17B0, END_X
                    move.w   #0, END_YT
                    move.w   #$2E0, END_YB
                    move.l   #lbL072306, CURRENT_MAP
                    move.l   #lbL07DE8E, REF_MAP
                    move.l   #EGG_BOMB_S, EGG_BOMBS
                    move.w   #300, TIME
                    move.w   #300, START_TIME
                    move.l   #BD1_BEACS, BEG_BEACS
                    move.w   #3, BEACONS
                    move.w   #$670, ZOONB_X
                    move.w   #$E0, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00E576:          move.w   #66, SPR_CNT
                    move.l   #BD2_ZONES, ZONE_TAB
                    move.l   #BD2_SPRS, STAGE_SPRS
                    move.w   #12, FIXED_SPRS
                    move.l   #lbW01AE5E, FIXED_BANK
                    move.w   #1, STARTX
                    move.w   #$28, STARTY
                    move.w   #$1860, ENDICON_X
                    move.w   #$E0, ENDICON_Y
                    move.w   #$160, ENDIC_FLR
                    addq.w   #8, ZOOL_SCRY
                    st       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    st       NO_BADDY
                    move.w   #$17B0, END_X
                    move.w   #10, END_YT
                    move.w   #$120, END_YB
                    move.l   #lbL083C4E, CURRENT_MAP
                    move.l   #lbL08F7D6, REF_MAP
                    move.l   #BD2_FIXSPRS, EGG_BOMBS
                    move.w   #300, TIME
                    move.w   #300, START_TIME
                    move.l   #BD2_BEACS, BEG_BEACS
                    move.w   #6, BEACONS
                    move.w   #$15B0, ZOONB_X
                    move.w   #$2B0, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00E656:          move.w   #68, SPR_CNT
                    move.l   #BD3_SPRS, STAGE_SPRS
                    move.l   #BD3_ZONES, ZONE_TAB
                    move.w   #$13, FIXED_SPRS
                    move.l   #lbW01AF1E, FIXED_BANK
                    move.w   #4, STARTX
                    move.w   #$3D, STARTY
                    sf       NO_BADDY
                    move.l   #lbL095596, CURRENT_MAP
                    move.l   #lbL0A111E, REF_MAP
                    move.l   #EGG_BOMB_S3, EGG_BOMBS
                    move.w   #$110, ENDIC_FLR
                    sf       ENDICON_ON
                    sf       NO_BADDY
                    move.w   #300, TIME
                    move.w   #300, START_TIME
                    move.l   #BD3_BEACS, BEG_BEACS
                    move.w   #5, BEACONS
                    move.w   #$900, ZOONB_X
                    move.w   #$280, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    bra      BIRD_BADVAR

BIRD_BADVAR:        move.w   #-64, BADDY_Y
                    move.w   #$1360, BADDY_X
                    clr.w    BADDY_YDIS
                    move.b   #0, BADDY_ACT
                    move.w   #$46, BADDY_EN
                    move.w   #$A4, BADDY_SPRS
                    move.w   #2, BADDY_SPR
                    sf       BADDY_DEAD
                    move.w   #-175, MESSAGE_SPR
                    move.w   #$70, MESSAGE_X
                    move.w   #$5A, MESSAGE_Y
                    move.w   #$12B0, END_X
                    move.w   #0, END_YT
                    move.w   #$B0, END_YB
                    move.l   #BIRD_BADDY, BADDY_RTN
                    move.l   #BIRD_BANIM, BANIMS_TAB
                    move.l   #BIRDM_FRMS, BADDY_FRMS
                    move.w   #$40, BADDY_WID
                    move.w   #$58, BADDY_HGT
                    rts

BD1_BEACS:          dc.w     $6B, $2B, $67, $1A, $117, $34
BD2_BEACS:          dc.w     $65, $32, $2B, 12, $97, $38, $79, 10, $12F, $18, $142, $39
BD3_BEACS:          dc.w     $20, $1D, $80, $38, $BB, $1C, $90, $46, $116, $46

RESET_PILLS:        lea      RESET_COLS, a0
                    lea      COLUMN1_TAB, a1
lbC00E7EA:          move.w   (a0)+, (a1)+
                    cmp.l    #END_COLS, a0
                    bne.s    lbC00E7EA
                    rts

TOOT_LEVEL:         move.l   #lbL072000, CUST_ATTRBS
                    move.l   #LEV_MAPS, CUST_REFS
                    move.l   #TOOT_PAL, LEV_PAL
                    move.l   #DOCOL_BAND2, DOCOL_RTN
                    move.l   #PC6_2, COLBAND2
                    move.l   #PC6_1, COLBAND
                    st       DOUBLE_COP
                    move.w   #8, LBEAM_SPRS
                    move.w   #0, SMASH1L
                    move.w   #0, SMASH1R
                    move.w   #0, SMASH2L
                    move.w   #0, SMASH2R
                    move.w   #8, END_PERMS
                    move.w   #0, SMASH2_SPR
                    move.w   #0, SMASH_XOFF
                    move.w   #-16, SMASH_YOFF
                    move.w   #$AA, LEVEL_SPRS
                    move.w   #$90, ARCHEX_SPR
                    move.w   #$8C, TOKEN_SPR
                    move.w   #$3E80, END_X
                    move.w   #7, FILLTILE_SPR
                    move.w   #0, FILLTILE_SP2
                    move.w   #0, FILL_TILE1
                    move.w   #0, FILL_TILE2
                    move.w   #11, SPLAT_ANIM
                    move.b   #12, END_TILEFX
                    move.b   #13, ROOFEX_ANIM
                    move.b   #15, PLATEX_ANIM
                    move.w   #$16B, SECRET_TILE
                    move.w   #$18, SECRET_SPR
                    move.w   #$8F, SECHINT_SPR
                    move.b   #$BC, COLUMN_REF
                    move.w   #$14D, LBEAM_TILE
                    move.l   #TOOT_LAVA, LAVA_TAB
                    move.w   #$17, LAVA_EXPSPR
                    move.b   #3, HEART_RES
                    move.w   #$AA, BONUS_SP
                    bsr      RESET_PILLS
                    cmp.b    #1, STAGE_NUM
                    bgt      lbC00EB52
                    beq      lbC00EA48
                    move.w   #4, COLUMNS
                    move.l   #COLUMN1_TAB, COLUMN_PTR
                    move.w   #39, SPR_CNT
                    move.w   #10, FIXED_SPRS
                    move.l   #TC1_SPRS, STAGE_SPRS
                    move.l   #TC1_ZONES, ZONE_TAB
                    move.l   #lbL01AA2C, FIXED_BANK
                    move.w   #1, STARTX
                    move.w   #$64, STARTY
                    st       NO_BADDY
                    move.w   #300, START_TIME
                    move.w   #300, TIME
                    move.w   #$C50, ENDICON_X
                    move.w   #$4B0, ENDICON_Y
                    move.w   #$4E0, ENDIC_FLR
                    st       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    move.w   #$B30, END_X
                    move.w   #$43, END_YT
                    move.w   #$4B0, END_YB
                    move.l   #lbL07204C, CURRENT_MAP
                    move.l   #lbL07DBD4, REF_MAP
                    move.l   #EGG_BOMB_S_TC, EGG_BOMBS
                    move.l   #EGG_BOMB_S_TC, EXP_BOMBS
                    move.l   #EGG_BOMB_S_TC, ENEMY_BULLS
                    move.l   #EGG_BOMB_S_TC, HOME_BULLS
                    move.l   #TC1_BEACS, BEG_BEACS
                    move.w   #4, BEACONS
                    move.w   #$780, ZOONB_X
                    move.w   #$40, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00EA48:          move.w   #10, COLUMNS
                    move.l   #COLUMN2_TAB, COLUMN_PTR
                    move.w   #46, SPR_CNT
                    move.w   #4, FIXED_SPRS
                    move.l   #TC2_SPRS, STAGE_SPRS
                    move.l   #TC2_ZONES, ZONE_TAB
                    move.l   #lbL01ABD8, FIXED_BANK
                    move.w   #$2D, STARTX
                    move.w   #$64, STARTY
                    st       NO_BADDY
                    move.w   #300, START_TIME
                    move.w   #300, TIME
                    move.w   #$9C0, ENDICON_X
                    move.w   #$2B0, ENDICON_Y
                    move.w   #$300, ENDIC_FLR
                    st       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    move.w   #$8C0, END_X
                    move.w   #$24, END_YT
                    move.w   #$2C0, END_YB
                    move.l   #lbL083994, CURRENT_MAP
                    move.l   #lbL08F51C, REF_MAP
                    move.l   #EGG_BOMB_S2_TC, EGG_BOMBS
                    move.l   #EGG_BOMB_S2_TC, EXP_BOMBS
                    move.l   #EGG_BOMB_S2_TC, ENEMY_BULLS
                    move.l   #EGG_BOMB_S2_TC, HOME_BULLS
                    move.l   #TC2_BEACS, BEG_BEACS
                    move.w   #3, BEACONS
                    move.w   #$8C0, ZOONB_X
                    move.w   #$2C0, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00EB52:          move.w   #5, COLUMNS
                    move.l   #COLUMN3_TAB, COLUMN_PTR
                    move.w   #44, SPR_CNT
                    move.w   #4, FIXED_SPRS
                    move.l   #TC3_SPRS, STAGE_SPRS
                    move.l   #TC3_ZONES, ZONE_TAB
                    move.l   #lbL01AB94, FIXED_BANK
                    move.w   #2, STARTX
                    move.w   #$69, STARTY
                    sf       NO_BADDY
                    move.w   #260, START_TIME
                    move.w   #260, TIME
                    sf       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    move.w   #$B30, END_X
                    move.w   #$640, END_YT
                    move.w   #$6C0, END_YB
                    move.l   #lbL0952DC, CURRENT_MAP
                    move.l   #lbL0A0E64, REF_MAP
                    move.l   #EGG_BOMB_S3_TC, EGG_BOMBS
                    move.l   #EGG_BOMB_S3_TC, EXP_BOMBS
                    move.l   #EGG_BOMB_S3_TC, ENEMY_BULLS
                    move.l   #EGG_BOMB_S3_TC, HOME_BULLS
                    move.l   #TC3_BEACS, BEG_BEACS
                    move.w   #2, BEACONS
                    move.w   #$2B0, ZOONB_X
                    move.w   #$40, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    bra      TOOT_BADVAR

TOOT_BADVAR:        move.w   #$6E0, BADDY_Y
                    move.l   #$37000, BADDY_Y2
                    move.w   #$CD0, BADDY_X
                    move.w   #$100, BADDY_YDIS
                    move.w   #-16, BADDY_XDIS
                    move.b   #0, BADDY_ACT
                    move.w   #$28, BADDY_EN
                    move.w   #$AA, BADDY_SPRS
                    move.w   #0, BADDY_SPR
                    sf       BADDY_DEAD
                    move.w   #-175, MESSAGE_SPR
                    move.w   #$70, MESSAGE_X
                    move.w   #$5A, MESSAGE_Y
                    move.l   #TOOT_BADDY, BADDY_RTN
                    move.l   #TOOT_BANIM, BANIMS_TAB
                    move.l   #TOOTM_FRMS, BADDY_FRMS
                    move.w   #$40, BADDY_WID
                    move.w   #$58, BADDY_HGT
                    rts

TC1_BEACS:          dc.w     $3F, $40, $52, $2D, $A8, $3C, $43, $5F
TC2_BEACS:          dc.w     $B3, $60, $55, $4E, $56, $1F
TC3_BEACS:          dc.w     12, $3E, $A1, $27

SNAKE_LEVEL:        jsr      CLEAR_SNKS
                    move.l   #lbL071D60, CUST_ATTRBS
                    move.l   #LEV_MAPS, CUST_REFS
                    move.l   #SNAKE_PAL, LEV_PAL
                    move.l   #DOCOL_BAND2, DOCOL_RTN
                    move.l   #PC3_2, COLBAND2
                    move.l   #PC3_1, COLBAND
                    st       DOUBLE_COP
                    move.w   #8, LBEAM_SPRS
                    move.w   #0, SMASH1L
                    move.w   #0, SMASH1R
                    move.w   #0, SMASH2L
                    move.w   #0, SMASH2R
                    move.w   #$6D, END_PERMS
                    move.w   #$6E, SMASH2_SPR
                    move.w   #0, SMASH_XOFF
                    move.w   #-16, SMASH_YOFF
                    move.w   #$D6, LEVEL_SPRS
                    move.w   #$90, ARCHEX_SPR
                    move.w   #$8C, TOKEN_SPR
                    move.w   #$3E80, END_X
                    move.w   #6, FILLTILE_SPR
                    move.w   #0, FILLTILE_SP2
                    move.w   #0, FILL_TILE1
                    move.w   #0, FILL_TILE2
                    move.w   #11, SPLAT_ANIM
                    move.b   #13, END_TILEFX
                    move.b   #13, ROOFEX_ANIM
                    move.b   #15, PLATEX_ANIM
                    move.b   #3, HEART_RES
                    st       BRICKS_ON
                    move.w   #$146, BRICK_TILE
                    move.w   #$51, BRICKF_SPR
                    move.w   #$52, BRICKL_SPR
                    move.w   #15, LAVA_EXPSPR
                    move.l   #SNAKE_LAVA, LAVA_TAB
                    move.l   #SNAKE_R, SNAKE_REF
                    move.l   #SNAKE_M, SNAKE_MAP
                    move.w   #$A0, BONUS_SP
                    st       SNAKE_CHK
                    sf       SNAKE_ON
                    cmp.b    #1, STAGE_NUM
                    bgt      lbC00F12A
                    beq      lbC00EFC6
                    move.w   #58, SPR_CNT
                    move.w   #4, FIXED_SPRS
                    move.l   #SN1_SPRS, STAGE_SPRS
                    move.l   #SN1_ZONES, ZONE_TAB
                    move.l   #lbL01AED0, FIXED_BANK
                    move.w   #5, STARTX
                    move.w   #$1E, STARTY
                    sf       BADDY_ON
                    st       NO_BADDY
                    move.w   #300, START_TIME
                    move.w   #300, TIME
                    move.w   #$D80, ENDICON_X
                    move.w   #$260, ENDICON_Y
                    move.w   #$2B0, ENDIC_FLR
                    st       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    move.w   #$CF0, END_X
                    move.w   #$1E, END_YT
                    move.w   #$260, END_YB
                    move.l   #lbL071D82, CURRENT_MAP
                    move.l   #lbL07D90A, REF_MAP
                    move.l   #SN1_FIXSPRS, EGG_BOMBS
                    move.l   #SN1_FIXSPRS, EXP_BOMBS
                    move.l   #SN1_FIXSPRS, ENEMY_BULLS
                    move.l   #HOMING_BLS_SN, HOME_BULLS
                    st       SNAKE_STAT
                    st       SNAKE_CHK
                    move.l   #SNAKEB1_TAB, END_BRICKS
                    move.w   #10, SNAKE_BLNKS
                    move.l   #SNAKE_1, LAST_GONE
                    move.l   #END_SN1, LAST_SNAKE
                    move.l   #SN1_BEACS, BEG_BEACS
                    move.w   #2, BEACONS
                    
                    ; looks like it's needed for the moving snake
                    movem.l  d0-d7/a0-a6, -(sp)
                    move.l   A_SNK_R1, a0
                    move.l   #SNAKE_R, a1
                    jsr      UNMANGLE
                    movem.l  (sp)+, d0-d7/a0-a6

                    movem.l  d0-d7/a0-a6, -(sp)
                    move.l   A_SNK_M1, a0
                    move.l   #SNAKE_M, a1
                    jsr      UNMANGLE
                    movem.l  (sp)+, d0-d7/a0-a6
                    
                    move.w   #$1010, ZOONB_X
                    move.w   #$80, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00EFC6:          move.w   #0, SPR_CNT
                    move.w   #12, FIXED_SPRS
                    move.l   #SN2_SPRS, STAGE_SPRS
                    move.l   #SN2_ZONES, ZONE_TAB
                    move.l   #SPR_DATA, FIXED_BANK
                    move.w   #5, STARTX
                    move.w   #$17, STARTY
                    sf       BADDY_ON
                    st       NO_BADDY
                    move.w   #180, START_TIME
                    move.w   #180, TIME
                    move.w   #$1DB0, ENDICON_X
                    move.w   #$1D0, ENDICON_Y
                    move.w   #$220, ENDIC_FLR
                    st       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    move.w   #$B30, END_X
                    move.w   #$15, END_YT
                    move.w   #$1D0, END_YB
                    move.l   #lbL0836CA, CURRENT_MAP
                    move.l   #lbL08F252, REF_MAP
                    move.l   #EGG_BOMB_S2_SN, EGG_BOMBS
                    move.l   #EGG_BOMB_S2_SN, EXP_BOMBS
                    move.l   #EGG_BOMB_S2_SN, ENEMY_BULLS
                    move.l   #EGG_BOMB_S2_SN, HOME_BULLS
                    st       SNAKE_STAT
                    st       SNAKE_CHK
                    move.l   #SNAKEB2_TAB, END_BRICKS
                    move.w   #11, SNAKE_BLNKS
                    move.l   #END_SN1, LAST_GONE
                    move.l   #END_SN2, LAST_SNAKE
                    move.l   #SN1_BEACS, BEG_BEACS
                    move.w   #0, BEACONS
                    
                    movem.l  d0-d7/a0-a6, -(sp)
                    move.l   A_SNK_R2, a0
                    move.l   #SNAKE_R, a1
                    jsr      UNMANGLE
                    movem.l  (sp)+, d0-d7/a0-a6

                    movem.l  d0-d7/a0-a6, -(sp)
                    move.l   A_SNK_M2, a0
                    move.l   #SNAKE_M, a1
                    jsr      UNMANGLE
                    movem.l  (sp)+, d0-d7/a0-a6
                    
                    move.w   #$2430, ZOONB_X
                    move.w   #$180, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00F12A:          move.w   #49, SPR_CNT
                    move.w   #4, FIXED_SPRS
                    move.l   #SN3_SPRS, STAGE_SPRS
                    move.l   #SN3_ZONES, ZONE_TAB
                    move.l   #lbL01AC6C, FIXED_BANK
                    move.w   #1, STARTX
                    move.w   #$1E, STARTY
                    sf       BADDY_ON
                    sf       NO_BADDY
                    move.w   #180, START_TIME
                    move.w   #180, TIME
                    sf       ENDICON_ON
                    move.w   #$1170, END_X
                    move.w   #3, END_YT
                    move.w   #$C0, END_YB
                    move.l   #lbL095012, CURRENT_MAP
                    move.l   #lbL0A0B9A, REF_MAP
                    move.l   #EGG_BOMB_S3_SN, EGG_BOMBS
                    move.l   #EGG_BOMB_S3_SN, EXP_BOMBS
                    move.l   #EGG_BOMB_S3_SN, ENEMY_BULLS
                    move.l   #SN3_FIXSPRS, HOME_BULLS
                    st       SNAKE_STAT
                    st       SNAKE_CHK
                    move.l   #SNAKEB3_TAB, END_BRICKS
                    move.w   #$15, SNAKE_BLNKS
                    move.l   #END_SN2, LAST_GONE
                    move.l   #END_SN3, LAST_SNAKE
                    move.l   #SN2_BEACS, BEG_BEACS
                    move.w   #3, BEACONS
                    
                    movem.l  d0-d7/a0-a6, -(sp)
                    move.l   A_SNK_R3, a0
                    move.l   #SNAKE_R, a1
                    jsr      UNMANGLE
                    movem.l  (sp)+, d0-d7/a0-a6

                    movem.l  d0-d7/a0-a6, -(sp)
                    move.l   A_SNK_M3, a0
                    move.l   #SNAKE_M, a1
                    jsr      UNMANGLE
                    movem.l  (sp)+, d0-d7/a0-a6
                    
                    move.w   #$770, ZOONB_X
                    move.w   #$2D0, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    bra      SNAK_BADVAR

SNAK_BADVAR:        move.w   #$140, BADDY_Y
                    move.l   #$A000, BADDY_Y2
                    move.w   #$1310, BADDY_X
                    move.w   #0, BADDY_YDIS
                    move.w   #-96, BADDY_XDIS
                    move.b   #0, BADDY_ACT
                    move.w   #$28, BADDY_EN
                    move.w   #$A0, BADDY_SPRS
                    move.w   #0, BADDY_SPR
                    sf       BADDY_DEAD
                    move.w   #-175, MESSAGE_SPR
                    move.w   #$70, MESSAGE_X
                    move.w   #$5A, MESSAGE_Y
                    move.l   #SNK_BADDY, BADDY_RTN
                    move.l   #SNKM_FRMS, BADDY_FRMS
                    move.l   #SNK_BANIM, BANIMS_TAB
                    move.w   #$30, BADDY_WID
                    move.w   #$30, BADDY_HGT
                    rts

SN1_BEACS:          dc.w     $D9, $1D, $16C, $29
SN2_BEACS:          dc.w     $6E, $24, $101, $49, $4C, $24

ICES_LEVEL:         move.l   #lbL071FB0, CUST_ATTRBS
                    move.l   #LEV_MAPS, CUST_REFS
                    move.l   #ICES_PAL, LEV_PAL
                    move.l   #DOCOL_BAND2, DOCOL_RTN
                    move.l   #PC4_2, COLBAND
                    move.l   #PC4_1, COLBAND2
                    st       DOUBLE_COP
                    move.w   #8, LBEAM_SPRS
                    move.w   #$66, SMASH1L
                    move.w   #$69, SMASH1R
                    move.w   #$6A, SMASH2L
                    move.w   #$6D, SMASH2R
                    move.w   #8, END_PERMS
                    move.w   #9, SMASH2_SPR
                    move.w   #0, SMASH_XOFF
                    move.w   #-16, SMASH_YOFF
                    move.w   #$A8, LEVEL_SPRS
                    move.w   #$91, ARCHEX_SPR
                    move.w   #$8D, TOKEN_SPR
                    move.w   #$3E80, END_X
                    move.w   #6, FILLTILE_SPR
                    move.w   #7, FILLTILE_SP2
                    move.w   #0, FILL_TILE1
                    move.w   #$86, FILL_TILE2
                    move.w   #14, SPLAT_ANIM
                    move.b   #12, END_TILEFX
                    move.b   #13, ROOFEX_ANIM
                    move.b   #15, PLATEX_ANIM
                    move.b   #3, HEART_RES
                    move.w   #$146, BRICK_TILE
                    move.w   #$51, BRICKF_SPR
                    move.w   #$52, BRICKL_SPR
                    move.w   #15, LAVA_EXPSPR
                    move.l   #SNAKE_LAVA, LAVA_TAB
                    move.w   #$A8, BONUS_SP
                    cmp.b    #1, STAGE_NUM
                    bmi      lbC00F458
                    beq      lbC00F56A
                    bra      lbC00F67C

lbC00F458:          move.w   #59, SPR_CNT
                    move.w   #0, FIXED_SPRS
                    move.l   #IC1_SPRS, STAGE_SPRS
                    move.l   #IC1_ZONES, ZONE_TAB
                    move.l   #lbL01AEC0, FIXED_BANK
                    move.w   #1, STARTX
                    move.w   #$3D, STARTY
                    sf       BADDY_ON
                    st       NO_BADDY
                    move.w   #300, START_TIME
                    move.w   #300, TIME
                    move.w   #$1270, ENDICON_X
                    move.w   #$50, ENDICON_Y
                    move.w   #$A0, ENDIC_FLR
                    st       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    move.w   #$1170, END_X
                    move.w   #0, END_YT
                    move.w   #$60, END_YB
                    move.l   #lbL071FF8, CURRENT_MAP
                    move.l   #lbL07DB80, REF_MAP
                    move.l   #IC2_SPRS, EGG_BOMBS
                    move.l   #IC2_SPRS, EXP_BOMBS
                    move.l   #IC2_SPRS, ENEMY_BULLS
                    move.l   #IC2_SPRS, HOME_BULLS
                    move.l   #BUTT1_TAB, LAST_LOLL
                    move.l   #BUTT2_TAB, LAST_BUTT
                    move.l   #IC1_BEACS, BEG_BEACS
                    move.w   #4, BEACONS
                    move.w   #$1280, ZOONB_X
                    move.w   #$140, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00F56A:          move.w   #73, SPR_CNT
                    move.w   #0, FIXED_SPRS
                    move.l   #IC2_SPRS, STAGE_SPRS
                    move.l   #IC2_ZONES, ZONE_TAB
                    move.l   #lbL01B1BC, FIXED_BANK
                    move.w   #0, STARTX
                    move.w   #$2D, STARTY
                    sf       BADDY_ON
                    st       NO_BADDY
                    move.w   #300, START_TIME
                    move.w   #300, TIME
                    move.w   #$18A0, ENDICON_X
                    move.w   #$30, ENDICON_Y
                    move.w   #$80, ENDIC_FLR
                    st       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    move.w   #$17B0, END_X
                    move.w   #0, END_YT
                    move.w   #$60, END_YB
                    move.l   #lbL083940, CURRENT_MAP
                    move.l   #lbL08F4C8, REF_MAP
                    move.l   #IC2_SPRS, EGG_BOMBS
                    move.l   #IC2_SPRS, EXP_BOMBS
                    move.l   #IC2_SPRS, ENEMY_BULLS
                    move.l   #IC2_SPRS, HOME_BULLS
                    move.l   #BUTT2_TAB, LAST_LOLL
                    move.l   #BUTT3_TAB, LAST_BUTT
                    move.l   #IC2_BEACS, BEG_BEACS
                    move.w   #2, BEACONS
                    move.w   #$1410, ZOONB_X
                    move.w   #$90, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    rts

lbC00F67C:          move.w   #36, SPR_CNT
                    move.w   #0, FIXED_SPRS
                    move.l   #IC3_SPRS, STAGE_SPRS
                    move.l   #IC3_ZONES, ZONE_TAB
                    move.l   #lbL01A9A8, FIXED_BANK
                    move.w   #$46, STARTX
                    move.w   #$7E, STARTY
                    sf       BADDY_ON
                    sf       NO_BADDY
                    move.w   #300, START_TIME
                    move.w   #300, TIME
                    move.w   #$D0, ENDIC_FLR
                    move.w   #$80, ENDICON_X
                    move.w   #0, ENDICON_Y
                    sf       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    move.w   #0, END_X
                    move.w   #-1, END_YT
                    move.w   #$70, END_YB
                    move.l   #lbL095288, CURRENT_MAP
                    move.l   #lbL09B500, REF_MAP
                    move.l   #IC2_SPRS, EGG_BOMBS
                    move.l   #IC2_SPRS, EXP_BOMBS
                    move.l   #IC2_SPRS, ENEMY_BULLS
                    move.l   #IC2_SPRS, HOME_BULLS
                    move.l   #BUTT3_TAB, LAST_LOLL
                    move.l   #BLK_TRIES, LAST_BUTT
                    move.l   #IC3_BEACS, BEG_BEACS
                    move.w   #2, BEACONS
                    move.w   #$50, ZOONB_X
                    move.w   #$180, ZOONB_Y
                    move.w   #-6, ZOONB_YDIS
                    bra      ICES_BADVAR

ICES_BADVAR:        move.w   #-96, BADDY_Y
                    move.w   #$90, BADDY_X
                    move.w   #0, BADDY_YDIS
                    move.w   #0, BADDY_XDIS
                    move.b   #0, BADDY_ACT
                    clr.w    BADDY_FRM
                    move.w   #$A8, BADDY_SPRS
                    move.w   #0, BADDY_SPR
                    sf       BADDY_DEAD
                    move.w   #-175, MESSAGE_SPR
                    move.w   #$70, MESSAGE_X
                    move.w   #$5A, MESSAGE_Y
                    move.l   #ICES_BADDY, BADDY_RTN
                    sf       BADDY_ON
                    sf       NO_BADDY
                    lea      ICEBAD_TAB, a0
                    move.w   #$1A, d7
lbC00F80C:          clr.l    (a0)+
                    clr.l    (a0)+
                    clr.w    (a0)+
                    dbra     d7, lbC00F80C
                    clr.w    ICECUBES
                    rts

IC1_BEACS:          dc.w     6, $11, $D3, $1E, $5D, $4B, $E9, $32
IC2_BEACS:          dc.w     $16E, $37, $145, 15
IC3_BEACS:          dc.w     $55, $5A, $1D, $28

LAST_LEVEL:         move.l   #lbL071F60, CUST_ATTRBS
                    move.l   #LEV_MAPS, CUST_REFS
                    move.l   #LAST_PAL, LEV_PAL
                    move.l   #PC5_1, COLBAND2
                    move.l   #PC5_2, COLBAND
                    move.w   #8, LBEAM_SPRS
                    move.w   #$12C, SMASH1L
                    move.w   #$69, SMASH1R
                    move.w   #$1A2, SMASH2L
                    move.w   #$6D, SMASH2R
                    move.w   #8, END_PERMS
                    move.w   #9, SMASH2_SPR
                    move.w   #0, SMASH_XOFF
                    move.w   #-16, SMASH_YOFF
                    move.w   #$A0, LEVEL_SPRS
                    move.w   #$91, ARCHEX_SPR
                    move.w   #$8D, TOKEN_SPR
                    move.w   #$3E80, END_X
                    move.w   #6, FILLTILE_SPR
                    move.w   #-34, FILLTILE_SP2
                    move.l   #DOCOL_BAND2, DOCOL_RTN
                    st       DOUBLE_COP
                    move.w   #0, FILL_TILE1
                    move.w   #$86, FILL_TILE2
                    move.w   #14, SPLAT_ANIM
                    move.b   #12, END_TILEFX
                    move.b   #13, ROOFEX_ANIM
                    move.b   #15, PLATEX_ANIM
                    move.b   #3, HEART_RES
                    move.w   #$146, BRICK_TILE
                    move.w   #$51, BRICKF_SPR
                    move.w   #$52, BRICKL_SPR
                    move.w   #15, LAVA_EXPSPR
                    move.l   #SNAKE_LAVA, LAVA_TAB
                    cmp.b    #1, STAGE_NUM
                    bmi      lbC00F966

lbC00F966:          move.w   #38, SPR_CNT
                    move.w   #0, FIXED_SPRS
                    move.l   #LS1_SPRS, STAGE_SPRS
                    move.l   #LS1_ZONES, ZONE_TAB
                    move.l   #lbL01AA94, FIXED_BANK
                    move.w   #12, STARTX
                    move.w   #$F5, STARTY
                    move.w   #300, START_TIME
                    move.w   #300, TIME
                    move.w   #$4F0, ENDICON_X
                    move.w   #0, ENDICON_Y
                    move.w   #$130, ENDIC_FLR
                    sf       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    move.w   #$450, END_X
                    move.w   #6, END_YT
                    move.w   #$D0, END_YB
                    move.l   #lbL071FA0, CURRENT_MAP
                    move.l   #lbL07D678, REF_MAP
                    move.l   #PP1_SPRS, EGG_BOMBS
                    move.l   #PP1_SPRS, EXP_BOMBS
                    move.l   #PP1_SPRS, ENEMY_BULLS
                    move.l   #PP1_SPRS, HOME_BULLS
                    move.l   #IC1_BEACS, BEG_BEACS
                    move.w   #0, BEACONS
                    move.w   #$1270, ZOONB_X
                    move.w   #$140, ZOONB_Y
                    bra      LAST_BADVAR

LAST_BADVAR:        move.w   #$3C00, FIST1_X
                    move.w   #$6000, FIST2_X
                    move.w   #$1100, FIST1_Y
                    move.w   #$1100, FIST2_Y
                    move.w   #$480, SHRINK_X
                    move.w   #$C0, SHRINK_Y
                    clr.w    FIST1XDIS
                    clr.w    FIST2XDIS
                    clr.w    FIST1YDIS
                    clr.w    FIST2YDIS
                    move.w   #$23, BADDY_EN
                    move.w   #0, FIST1_ACT
                    move.w   #0, FIST2_ACT
                    move.w   #$88, BADDY_SPRS
                    clr.w    BADDY_SPR
                    move.w   #$32, BADDY_FRM
                    move.b   #7, LASTLOOK
                    sf       BADDY_DEAD
                    move.w   #-175, MESSAGE_SPR
                    move.w   #$70, MESSAGE_X
                    move.w   #$5A, MESSAGE_Y
                    move.l   #LAST_BADDY, BADDY_RTN
                    sf       BADDY_ON
                    sf       NO_BADDY
                    lea      CSEG_TAB, a0
                    clr.l    (a0)+
                    clr.l    (a0)+
                    clr.l    (a0)+
                    clr.l    (a0)+
                    clr.l    (a0)+
                    clr.l    (a0)+
                    clr.l    (a0)+
                    rts

LS1_BEACS:          dc.w     6, $11, $D3, $1E, $5D, $4B, $E9, $32

LEVPALSET:          move.l   LEV_PAL, a0
                    jsr      GAMEPALET
                    move.l   LEV_PAL, a0
                    lea      $20(a0), a0
                    bsr      PARAPALET
                    rts

GAMEPALET:          lea      LEVCOLS, a1
lbC00FB4C:          move.w   (a0)+, (a1)
                    lsl.w    (a1)
                    addq.l   #4, a1
                    cmp.l    #USED_CBANK, a1
                    bne.s    lbC00FB4C
                    rts

PARAPALET:          lea      PARACOLS, a1
lbC00FB62:          move.w   (a0)+, d0
                    add.w    d0, d0
                    bsr.w    LOWER_COLOR                        ; ****
                    move.w   d0, (a1)
                    addq.l   #4, a1
                    cmp.l    #FPARACOLS, a1
                    bne.s    lbC00FB62
                    rts

INIT_FDON:          move.w   #1, FADECOP_CNT
                    move.w   COLBAND_PTR, FADBAND_PTR
INIT_FADE:          st       FADECOL_ON
                    move.w   #1, FADE_CNT
                    rts

FADE_OFF:           tst.w    FADE_CNT
                    beq      lbC00FC56
                    tst.b    FADECOL_ON
                    bne      FADE_ON
                    tst.b    GAME_FADE
                    bne      MAIN_FADOFF
                    subq.w   #1, FADE_CNT
                    bne      lbC00FBC2
                    sf       GAME_FADE
lbC00FBC2:          cmp.w    #12, FADE_CNT
                    bne      lbC00FBE0
                    move.w   COLBAND_PTR, FADBAND_PTR
                    move.w   #$1F, FADECOP_CNT
lbC00FBE0:          move.w   FADE_CNT, d7
                    
                    move.l   LEV_PAL, a0
                    lea      $DFF180, a1
                    move.w   #$1420, $DFF106
                    move.w   #15, d3
lbC00FBFE:          move.w   (a0)+, d0
                    add.w    d0, d0
                    move.l   MASK_PAL, a2
                    bsr      ONECOL
                    dbra     d3, lbC00FBFE
                    
                    lea      PANEL_PAL, a0
                    lea      $DFF1A0, a1
                    move.w   #15, d3
lbC00FC20:          move.w   (a0)+, d0
                    add.w    d0, d0
                    move.l   MASK_PAL, a2
                    bsr      ONECOL
                    dbra     d3, lbC00FC20
                    
                    move.l   LEV_PAL, a0
                    lea      SCRCOLS, a1
                    move.w   #15, d3
lbC00FC42:          move.w   (a0)+, d0
                    move.l   MASK_PAL, a2
                    add.w    d0, d0
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FC42
lbC00FC56:          rts

FADE_ON:            tst.b    GAME_FADE
                    bne      MAIN_FADE
                    cmp.w    #$20, FADE_CNT
                    beq      lbC00FCE6
                    addq.w   #1, FADE_CNT
                    move.w   FADE_CNT, d7
                    move.l   LEV_PAL, a0
                    lea      $DFF180, a1
                    move.w   #$1420, $DFF106
                    move.w   #15, d3
lbC00FC92:          move.w   (a0)+, d0
                    lsl.w    #1, d0
                    move.l   MASK_PAL, a2
                    bsr      ONECOL
                    dbra     d3, lbC00FC92
                    lea      PANEL_PAL, a0
                    lea      $DFF1A0, a1
                    move.w   #15, d3
lbC00FCB4:          move.w   (a0)+, d0
                    lsl.w    #1, d0
                    bsr      ONECOL
                    dbra     d3, lbC00FCB4
                    move.l   LEV_PAL, a0
                    lea      SCRCOLS, a1
                    move.l   MASK_PAL, a2
                    move.w   #15, d3
lbC00FCD6:          move.w   (a0)+, d0
                    lsl.w    #1, d0
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FCD6
                    rts

lbC00FCE6:          clr.w    FADE_CNT
                    rts

MAIN_FADE:          cmp.w    #32, FADE_CNT
                    beq      lbC00FD8A
                    addq.w   #1, FADE_CNT
                    move.w   FADE_CNT, d7
                    move.l   LEV_PAL, a0
                    lea      LEVCOLS, a1
                    move.l   MASK_PAL, a2
                    move.w   #16-1, d3
lbC00FD1C:          move.w   (a0)+, d0
                    add.w    d0, d0
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FD1C

                    lea      PANEL_PAL, a0
                    lea      PANELCOLS, a1
                    move.w   #12-1, d3
lbC00FD3A:          move.w   (a0)+, d0
                    add.w    d0, d0
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FD3A

                    lea      HSP_PAL, a0
                    lea      lbW048BA0, a1
                    move.w   #16-1, d3
lbC00FD58:          move.w   (a0)+, d0
                    add.w    d0, d0
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FD58

                    move.l   LEV_PAL, a0
                    lea      $20(a0), a0
                    lea      PARACOLS, a1
                    move.w   #8-1, d3
lbC00FD7A:          move.w   (a0)+, d0
                    add.w    d0, d0                        ; *****
                    bsr.w    LOWER_COLOR
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FD7A
                    rts

lbC00FD8A:          clr.w    FADE_CNT
                    rts

MAIN_FADOFF:        subq.w   #1, FADE_CNT
                    cmp.w    #12, FADE_CNT
                    bne      lbC00FDB6
                    move.w   COLBAND_PTR, FADBAND_PTR
                    move.w   #$1F, FADECOP_CNT
lbC00FDB6:          move.w   FADE_CNT, d7
                    
                    move.l   LEV_PAL, a0
                    lea      LEVCOLS, a1
                    move.l   MASK_PAL, a2
                    move.w   #15, d3
lbC00FDD2:          move.w   (a0)+, d0
                    add.w    d0, d0
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FDD2
                    
                    lea      PANEL_PAL, a0
                    lea      PANELCOLS, a1
                    move.w   #11, d3
lbC00FDF0:          move.w   (a0)+, d0
                    add.w    d0, d0
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FDF0
                    
                    lea      HSP_PAL, a0
                    lea      lbW048BA0, a1
                    move.w   #15, d3
lbC00FE0E:          move.w   (a0)+, d0
                    add.w    d0, d0
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FE0E
                    
                    move.l   LEV_PAL, a0
                    lea      $20(a0), a0
                    lea      PARACOLS, a1
                    move.w   #8-1, d3
lbC00FE30:          move.w   (a0)+, d0
                    add.w    d0, d0                        ; *****
                    bsr.w    LOWER_COLOR
                    bsr      ONECOL
                    addq.l   #2, a1
                    dbra     d3, lbC00FE30
                    rts

                    clr.w    FADE_CNT
                    rts

RESET_COP:          bsr      FIX_COLBAND
                    tst.b    DOUBLE_COP
                    beq      lbC00FE5A
                    bsr      FIX_COLBND2
lbC00FE5A:          rts

LOWER_COLOR:        movem.w   d1/d2, -(a7)
                    move.w    d0, d1
                    move.w    d0, d2
                    and.w     #$f00, d0
                    and.w     #$f0, d1
                    and.w     #$f, d2
                    sub.w     #$300, d0
                    bge.b     LOWER_R
                    clr.w     d0
LOWER_R:
                    sub.w     #$30, d1
                    bge.b     LOWER_G
                    clr.w     d1
LOWER_G:
                    subq.w    #3, d2
                    bge.b     LOWER_B
                    clr.w     d2
LOWER_B:
                    or.w      d1, d0
                    or.w      d2, d0
                    movem.w   (a7)+, d1/d2
                    rts

FIX_COLBAND:        lea      USED_BAND1, a0
                    move.l   COLBAND, a1
                    move.w   #$8F, d7
lbC00FE6C:          move.w   (a1)+, d0
                    bsr.b    LOWER_COLOR                        ; ****
                    move.w   d0, (a0)+
                    dbra     d7, lbC00FE6C
                    rts

FIX_COLBND2:        lea      USED_BAND2, a0
                    move.l   COLBAND2, a1
                    move.w   #$8F, d7
lbC00FE84:          move.w   (a1)+, d0
                    bsr.b    LOWER_COLOR                        ; ****
                    move.w   d0, (a0)+
                    dbra     d7, lbC00FE84
                    rts

FADE_COP:           tst.w    FADECOP_CNT
                    beq      lbC00FEE4
                    tst.b    FADECOL_ON
                    bne      FADE_COPO
                    subq.w   #1, FADECOP_CNT
                    tst.b    DOUBLE_COP
                    beq      lbC00FEB4
                    bsr      FADE_COP2
lbC00FEB4:          move.w   FADECOP_CNT, d7
                    move.l   COLBAND, a0
                    lea      USED_BAND1, a1
                    move.w   FADBAND_PTR, d0
                    subq.w   #4, d0
                    lea      0(a0,d0.w), a0
                    lea      0(a1,d0.w), a1
                    move.w   #$13, d3
lbC00FEDA:          move.w   (a0)+, d0
                    bsr.w    LOWER_COLOR                    ; ****
                    bsr      ONECOL
                    dbra     d3, lbC00FEDA
lbC00FEE4:          rts

FADE_COP2:          move.w   FADECOP_CNT, d7
                    move.l   COLBAND2, a0
                    lea      USED_BAND2, a1
                    move.w   FADBAND_PTR, d0
                    subq.w   #4, d0
                    lea      0(a1,d0.w), a1
                    lea      0(a0,d0.w), a0
                    move.w   #$13, d3
lbC00FF0C:          move.w   (a0)+, d0
                    bsr.w    LOWER_COLOR                    ; ****
                    bsr      ONECOL
                    dbra     d3, lbC00FF0C
                    rts

FADE_COPO:          cmp.w    #$20, FADECOP_CNT
                    beq      lbC00FF70
                    addq.w   #1, FADECOP_CNT
                    tst.b    DOUBLE_COP
                    beq      lbC00FF36
                    bsr.s    FADE_COP2
lbC00FF36:          move.w   FADECOP_CNT, d7
                    lea      USED_BAND1, a1
                    move.l   COLBAND, a0
                    move.w   FADBAND_PTR, d0
                    subq.w   #4, d0
                    lea      0(a0,d0.w), a0
                    lea      0(a1,d0.w), a1
                    move.w   #$13, d3
lbC00FF5C:          move.w   (a0)+, d0
                    bsr.w    LOWER_COLOR                    ; ****
                    bsr      ONECOL
                    dbra     d3, lbC00FF5C
                    move.w   #$8080, $DFF096
                    rts

lbC00FF70:          clr.w    FADECOP_CNT
                    rts

ONECOL:             move.w   d0, d1
                    move.w   d0, d2
                    and.w    #$F00, d0
                    and.w    #$F0, d1
                    and.w    #15, d2
                    lsr.w    #5, d0
                    mulu     d7, d0
                    and.w    #$F00, d0
                    mulu     d7, d1
                    lsr.w    #5, d1
                    and.w    #$F0, d1
                    mulu     d7, d2
                    lsr.w    #5, d2
                    or.w     d2, d1
                    or.w     d1, d0
                    tst.l    MASK_PAL
                    beq      lbC00FFB8
                    tst.w    (a2)
                    bpl      lbC00FFB6
                    addq.l   #2, a1
                    addq.l   #2, a2
                    rts

lbC00FFB6:          and.w    (a2)+, d0
lbC00FFB8:          move.w   d0, (a1)+
                    rts

INIT_SCRO:          move.l   #BUFF1_START, SCROLLA
                    move.l   #BUFF1_START, BUFF_PTR_A
                    clr.w    YSCRO_A
                    clr.w    XREM_A
                    clr.w    YREM_A
                    clr.w    MAP_LINE_A
                    move.l   CURRENT_MAP, a0
                    move.w   2(a0), MAP_WIDTH
                    move.w   6(a0), MAP_HEIGHT
                    addq.l   #8, a0
                    move.l   a0, MAP_PTR_A
                    move.l   #BUFF2_START, SCROLLB
                    move.l   #BUFF2_START, BUFF_PTR_B
                    clr.w    YSCRO_B
                    clr.w    XREM_B
                    clr.w    YREM_B
                    clr.w    MAP_LINE_B
                    move.l   a0, MAP_PTR_B
                    move.l   #BUFF3_START, SCROLLC
                    move.l   #BUFF3_START, BUFF_PTR_C
                    clr.w    YSCRO_C
                    clr.w    XREM_C
                    clr.w    YREM_C
                    clr.w    MAP_LINE_C
                    move.l   a0, MAP_PTR_C
                    clr.w    OLD_XSCR
                    clr.w    OLD_YSCR
                    move.w   MAP_WIDTH, d0
                    lsl.w    #4, d0
                    sub.w    #$150, d0
                    move.w   d0, RIGHT_MARG
                    move.w   MAP_HEIGHT, d0
                    lsl.w    #4, d0
                    sub.w    #$110, d0
                    move.w   d0, BOT_MARG
                    clr.w    XPOS
                    rts

SETUPSCRO:          jsr      CONV_PLAYF
                    move.w   STARTY, d1
                    move.w   STARTX, d0
                    add.w    #$15, d0
                    lsl.w    #4, d0
                    lsl.w    #4, d1
                    move.w   d0, XSCROLL
                    move.w   d1, YSCROLL
                    move.w   d0, XBEGSCROLL
                    move.w   d1, YBEGSCROLL
                    jsr      SCROLL_BUFF
                    eor.b    #1, ANDYFRAME
                    clr.w    XSCROLL
                    clr.w    YSCROLL
                    jsr      SCROLL_BUFF
                    eor.b    #1, ANDYFRAME
                    clr.w    YSCROLL
                    move.w   #-336, XSCROLL
                    jsr      SCROLL_BUFF
                    eor.b    #1, ANDYFRAME
                    clr.w    XSCROLL
                    clr.w    YSCROLL
                    jsr      SCROLL_BUFF
                    rts

INIT_SCRNS:         move.l   #BUFF1_START, PHYSADR
                    move.l   #BUFF2_START, LOGADR
                    rts

CLR_SCRN:           tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC010156
                    bra      lbC010178

lbC010156:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC010156
                    tst.b    $DFF002
lbC010178:          lea      $DFF000, a5
                    move.w   #$900, $40(a5)
                    clr.w    $42(a5)
                    move.w   #0, $66(a5)
                    move.w   #-2, $64(a5)
                    move.l   a0, $54(a5)
                    move.l   #CLEAR, $50(a5)
                    move.w   #$8818, $58(a5)
                    move.l   #CLEAR, $50(a5)
                    add.w    #$6600, a0
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC0101C6
                    bra      lbC0101E8

lbC0101C6:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC0101C6
                    tst.b    $DFF002
lbC0101E8:          move.l   a0, $54(a5)
                    move.w   #$8818, $58(a5)
                    rts

COP_LOC:            dc.l     COPPER_GAME

; Keyboard interrupt
KBI:                move.l   d1, -(a7)
                    move.b   d0, d1
                    lsl.b    #1, d1
                    not.b    d1
                    move.b   d1, KEYREC
                    move.b   d0, KEYVALUE
                    bpl.s    KEY_REPEAT
                    sf       KEYREC
                    sf       KEYVALUE
KEY_REPEAT:         move.l   (a7)+, d1
                    moveq    #0, d0
                    rts

; Vertical blank interrupt
VBI:                movem.l  d2-d7/a2/a3/a4, -(sp)
                    addq.w   #1, SWITCH_CNT
                    move.l   COP_LOC, $DFF080
                    bsr      DO_AUDIO
                    bsr      FADE_OFF
                    bsr      FADE_COP
                    movem.l  (sp)+, d2-d7/a2/a3/a4
                    moveq    #0, d0
                    rts

DO_AUDIO:           tst.b    MUSIC_ON
                    bne      MUSIC_CALL
                    cmp.b    #2, AUDIO
                    beq      SFX_GO
                    tst.b    AUDIO
                    bmi      SFX_GO
                    rts

MUSIC_CALL:         bmi      _MT_MUSIC
                    jmp      MT_MUSIC

_MT_MUSIC:          jsr      MT_MUSIC
                    subq.w   #1, MUSIC_VOL
                    move.w   MUSIC_VOL, d7
                    ble      lbC01029A
                    move.w   d7, $DFF0A8
                    move.w   d7, $DFF0B8
                    move.w   d7, $DFF0C8
                    move.w   d7, $DFF0D8
                    rts

lbC01029A:          clr.b    MUSIC_ON
                    move.l   CUR_VBR, a0
                    move.l   #NULL_IRQ, $78(a0)
                    jmp      MT_END

SFX_GO:             jmp      PRO_SFX

JPF_BUTTON_BLUE     equ      $800000
JPF_BUTTON_RED      equ      $400000
JPF_BUTTON_YELLOW   equ      $200000
JPF_BUTTON_GREEN    equ      $100000
JPF_BUTTON_FORWARD  equ      $80000
JPF_BUTTON_REVERSE  equ      $40000
JPF_BUTTON_PLAY     equ      $20000
JPF_BUTTON_UP       equ      $8
JPF_BUTTON_DOWN     equ      $4
JPF_BUTTON_LEFT     equ      $2
JPF_BUTTON_RIGHT    equ      $1

; done by ASMAN
CIAB_GAMEPORT0      equ      6       ; gameport 0,  pin 6 (fire button*)
CIAB_GAMEPORT1      equ      7       ; gameport 1,  pin 6 (fire button*)

;in d0 == 0 - port 0
;      == 1 - port 1
READ_JOYSTICK:      movem.l d1-a1, -(a7)

                    lea     $dff000, a0
                    lea     $bfe001, a1
                    moveq   #CIAB_GAMEPORT0, d3  ; red button ( port 0 )
                    moveq   #10, d4              ; blue button ( port 0 )
                    move.w  #$f600, d5           ; for potgo port 0
                    moveq   #$A, d6
            
                    tst.l   d0
                    beq.b   .direction
            
                    moveq   #CIAB_GAMEPORT1, d3  ; red port 1
                    moveq   #14, d4              ; blue port 1 
                    move.w  #$6f00, d5           ; for potgo port 1
                    moveq   #$C, d6              ; port 1
        
.direction:         moveq   #0, d7
                    move.w  (a0,d6.w), d0        ; get joystick direction
            
                    move.w  d0, d1
                    lsr.w   #1, d1
                    eor.w   d0, d1
            
                    btst    #8, d1               ; check joystick up
                    sne     d7
                    add.w   d7, d7
            
                    btst    #0, d1               ; check joystick down
                    sne     d7
                    add.w   d7, d7
            
                    btst    #9, d0               ; check joystick left
                    sne     d7
                    add.w   d7, d7
            
                    btst    #1, d0               ; check joystick right
                    sne d7
                    add.w   d7, d7
            
                    swap    d7

                    ; two buttons
                    cmp.b   #2, JOYSTICK_MODE
                    beq.b   NO_GEN_BUTTON
                    btst    d4, $16(a0)          ; check button blue
                    seq     d7
NO_GEN_BUTTON:
                    add.w   d7, d7
            
                    btst    d3, (a1)             ; check button red
                    seq     d7
                    add.w   d7, d7
            
                    and.w   #$300, d7
                    asr.l   #2, d7
                    swap    d7
                    asr.w   #6, d7
            
                    ; read buttons from CD32 pad
                    bset    d3, $200(a1)         ; set bit to out at ciapra
                    bclr    d3, (a1)             ; clr bit to in at ciapra
                    move.w  d5, $34(a0)
                    moveq   #0, d0
                    moveq   #8-1, d1
                    bra.b   .in

.loop               tst.b   (a1)
                    tst.b   (a1)
.in                 tst.b   (a1)
                    tst.b   (a1)
                    tst.b   (a1)
                    tst.b   (a1)
                    tst.b   (a1)
                    tst.b   (a1)
                    move.w  $16(a0), d2
                    bset    d3, (a1)
                    bclr    d3, (a1)
                    btst    d4, d2
                    bne.b   .next
                    bset    d1, d0
.next               dbf     d1, .loop
                    bclr    d3, $200(a1)     ;set bit to in at ciapra
                    swap    d0
                    or.l    d7, d0
                    move.w  #$ffff, $34(a0)
                    movem.l (a7)+, d1-a1
                    rts

SMART_JOY:          dc.b     0
                    even

READ_JOY:           sf       SPINNING
                    sf       JOYPOS
                    
                    moveq    #1, d0
                    bsr.w    READ_JOYSTICK
                    
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_UP, d1
                    beq.b    lbC16E5FA
                    cmp.b    #3, JOYSTICK_MODE               ; one button mode: always up
                    beq.b    UP_ONE_BUTTON
                    cmp.w    #ZOOL_GRIPPING, ZOOL_MOVE       ; zool is not climbing
                    bne.b    lbC16E5FA
UP_ONE_BUTTON:      bset     #0, JOYPOS                      ; joy up
lbC16E5FA:          
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_DOWN, d1
                    beq.b    lbC16E60E
                    bset     #1, JOYPOS                      ; joy down
lbC16E60E:
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_LEFT, d1
                    beq.b    lbC16E622
                    bset     #2, JOYPOS                      ; joy left
lbC16E622:
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_RIGHT, d1
                    beq.b    lbC16E636
                    bset     #3, JOYPOS                      ; joy right
lbC16E636:
                    cmp.b    #2, JOYSTICK_MODE               ; genesis pad
                    bne.b    JOY_RED
                    btst     #14, $dff016
                    bne.b    JOY_RED
                    cmp.w    #ZOOL_GRIPPING, ZOOL_MOVE       ; zool is climbing
                    beq.b    JOY_RED
                    bset     #0, JOYPOS                      ; jump
JOY_RED:
                    cmp.b    #1, JOYSTICK_MODE               ; cd32 pad
                    beq.b    ONE_BUTTON_FIRE
                    btst     #7, $BFE001
                    bne.b    ONE_BUTTON_FIRE
                    bset     #7, JOYPOS
                    btst     #0, JOYPOS
                    sne      SPINNING
ONE_BUTTON_FIRE:
                    cmp.b    #1, JOYSTICK_MODE
                    bgt.b    lbC16E64A
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_GREEN, d1
                    beq.b    lbC16E64A
                    bset     #7, JOYPOS
lbC16E64A:
                    cmp.b    #1, JOYSTICK_MODE
                    bne.b    RELEASE_BLUE
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_RED, d1
                    beq.b    lbC16E66A
                    cmp.w    #ZOOL_GRIPPING, ZOOL_MOVE      ; zool is climbing
                    beq.b    lbC16E66A
                    bset     #0, JOYPOS                     ; jump
lbC16E66A:
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_BLUE, d1
                    beq.b    lbC16E67E
                    tst.b    SMART_JOY
                    bne.b    RELEASE_BLUE
                    move.b   #$95, KEYREC                   ; trigger the smart bomb
                    st.b     SMART_JOY
                    bra.w    RELEASE_BLUE
lbC16E67E:          tst.b    SMART_JOY
                    beq.b    RELEASE_BLUE
                    sf.b     SMART_JOY                      ; one at a time
                    sf.b     OLDKEY
                    sf.b     KEYREC
RELEASE_BLUE:

                    cmp.b    #1, JOYSTICK_MODE
                    bne.b    lbC16E6D0
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_PLAY, d1
                    beq.b    lbC16E692
                    move.b   #$CD, KEYREC
lbC16E692:          
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_FORWARD, d1
                    beq.b    lbC16E6B2
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_REVERSE, d1
                    beq.b    lbC16E6B2
                    move.b   #$75, KEYREC
lbC16E6B2:          
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_GREEN, d1
                    beq.b    lbC16E6D0
                    move.l   d0, d1
                    and.l    #JPF_BUTTON_RED, d1
                    beq.b    lbC16E6D0
                    st       SPINNING
lbC16E6D0:          
                    move.b   KEYREC,d0
                    cmp.b    #$FD,d0
                    beq.b    lbC0103A2
                    cmp.b    #$EB,d0
                    beq.b    lbC0103B0
                    cmp.b    #$9D,d0
                    beq.b    lbC0103BE
                    cmp.b    #$B3,d0
                    beq.b    lbC0103CC
                    cmp.b    #$DF,d0
                    beq.b    lbC0103DA
                    cmp.b    #$BF,d0
                    beq.b    lbC0103E8
                    cmp.b    #$D1,d0
                    beq.b    lbC0103F6
                    cmp.b    #$CF,d0
                    beq.b    lbC010404
                    cmp.b    #1, JOYSTICK_MODE
                    ble.b    SPACEBAR_SMARTB
                    cmp.b    #$7f,d0
                    bne.b    CONVERT_SMART
                    move.b   #$95, KEYREC                   ; smart bomb
CONVERT_SMART:      rts
SPACEBAR_SMARTB:    cmp.b    #$7F,d0
                    beq      lbC010412
                    rts

lbC0103A2:          move.b   #5,JOYPOS
                    rts

lbC0103B0:          move.b   #9,JOYPOS
                    rts

lbC0103BE:          move.b   #6,JOYPOS
                    rts

lbC0103CC:          move.b   #10,JOYPOS
                    rts

lbC0103DA:          move.b   #1,JOYPOS
                    rts

lbC0103E8:          move.b   #2,JOYPOS
                    rts

lbC0103F6:          move.b   #4,JOYPOS
                    rts

lbC010404:          move.b   #8,JOYPOS
                    rts

lbC010412:          move.b   #$80,JOYPOS
                    rts

SEL_JOYSTICK_MODE:  dc.b     0
                    even
JOYSTICK_MODE:      dc.b     0
                    even
SPINNING:           dc.b     0
                    even

PRO_BADDY:          tst.b    NO_BADDY
                    bne      lbC0104CC
                    tst.b    BADDY_ON
                    bmi      lbC0104CC
                    beq      lbC010476
                    subq.b   #1, BADDY_ON
                    cmp.b    #1, BADDY_ON
                    beq      lbC0104CE
                    bgt      lbC0104CC
                    addq.b   #1, BADDY_ON
                    move.l   BADDY_RTN, a0
                    jmp      (a0)

lbC010476:          cmp.b    #99, PERCENT
                    bne      lbC0104CC
                    move.w   MAP_LINE, d1
                    move.w   XPOS, d0
                    sub.w    END_X, d0
                    bmi      lbC0104CC
                    cmp.w    #$10, d0
                    bpl      lbC0104CC
                    cmp.w    END_YT, d1
                    bmi      lbC0104CC
                    cmp.w    END_YB, d1
                    bpl      lbC0104CC
                    st       SCROLL_OFF
                    tst.b    NO_BADDY
                    bne      lbC0104CC
                    move.b   #$64, BADDY_ON
lbC0104CC:          rts

lbC0104CE:          tst.b    AUDIO
                    ble      lbC0104F2
                    ;clr.w    MUSICON
                    ;jsr      KILLAUDIO
                    ;moveq    #9, d0
                    ;clr.w    MUSICON
                    ;jsr      PLAYTRACK
lbC0104F2:          sf       BADDY_HIT
                    lea      BADETS_TAB, a0
                    move.w   #2, d7
lbC010502:          clr.l    (a0)+
                    clr.l    (a0)+
                    clr.w    (a0)+
                    dbra     d7, lbC010502
                    lea      SMART_TAB, a0
                    sf       (a0)
                    sf       $10(a0)
                    sf       $20(a0)
                    clr.w    SMARTS
                    st       END_FIGHT
                    move.l   MENTB_AD, a1
                    move.l   MENT_SPDATA, a0
                    move.l   END_MENT, a2
lbC01053A:          move.l   (a0)+, (a1)+
                    cmp.l    a2, a0
                    bmi.s    lbC01053A
                    sf       SAVE_SPACE
                    st       MENTB_MASK
                    bsr      SPRITE_TAB
                    jsr      REMEMBER
                    cmp.b    #5, LEVEL_NUM
                    bne      lbC01057A
                    move.l   #NOZONES, ZONE_TAB
                    clr.w    SPR_CNT
                    clr.w    FIXED_SPRS
                    rts

lbC01057A:          move.w   #$28, SLIDE_SND
                    cmp.b    #1, LEVEL_NUM
                    beq      lbC0104CC
                    move.w   #$14, SLIDE_SND
                    rts

LAST_BADDY:         bsr      COL_CSEGS
                    bsr      MOVE_FISTS
                    bsr      SHRINK
                    rts

SHRINK:             tst.w    BADDY_EN
                    bpl      lbC01071C
                    tst.b    ANDYFRAME
                    beq      lbC010628
                    move.w   SHRINK_X, d0
                    move.w   #$110, d1
                    move.w   #$12, d7
                    add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    jsr      ADD_PERM
                    sub.w    #$40, d1
                    jsr      ADD_PERM
                    sub.w    #$10, d1
                    jsr      ADD_PERM
                    neg.w    d0
                    add.w    #$9DF, d0
                    jsr      ADD_PERM
                    add.w    #$10, d1
                    jsr      ADD_PERM
                    add.w    #$40, d1
                    jsr      ADD_PERM
                    addq.w   #1, SHRINK_X
                    cmp.w    #$4D1, SHRINK_X
                    ble      lbC01071C
                    subq.w   #1, SHRINK_X
                    rts

lbC010628:          move.w   #$13, d7
                    add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    move.w   SHRINK_Y, d1
                    move.w   #$480, d0
                    jsr      ADD_PERM
                    add.w    #$40, d0
                    jsr      ADD_PERM
                    add.w    #$40, d0
                    jsr      ADD_PERM
                    add.w    #$20, d0
                    jsr      ADD_PERM
                    addq.w   #1, SHRINK_Y
                    cmp.w    #$114, SHRINK_Y
                    ble      lbC01071C
                    bsr      CLEAR_PERMS
                    move.w   #$61, SPRITE
                    move.w   #$4D0, d0
                    move.w   #$110, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.l   BUFF_PTR, -(sp)
                    move.l   SCROLL, -(sp)
                    move.l   BUFF_PTR_C, BUFF_PTR
                    move.l   SCROLLC, SCROLL
                    st       P_DRAW
                    jsr      DUMPSPRITE
                    add.w    #$20, YCOORD
                    st       P_DRAW
                    jsr      DUMPSPRITE
                    add.w    #$20, XCOORD
                    st       P_DRAW
                    jsr      DUMPSPRITE
                    sub.w    #$20, YCOORD
                    st       P_DRAW
                    jsr      DUMPSPRITE
                    move.l   (sp)+, SCROLL
                    move.l   (sp)+, BUFF_PTR
                    bra      START_SHAKE

lbC01071C:          rts

START_SHAKE:        move.w   #$150, BADDY_Y
                    move.w   #$4F0, BADDY_X
                    clr.w    BADDY_YDIS
                    move.b   #1, BADDY_ACT
                    move.w   #$32, BADDY_FRM
                    move.w   #$46, BADDY_EN
                    move.w   #2, BADDY_SPR
                    sf       BADDY_DEAD
                    sf       BADDY_HIT
                    clr.w    SEGMENTS
                    st       END_FIGHT
                    move.b   #1, BADDY_ON
                    sf       NO_BADDY
                    move.w   #-175, MESSAGE_SPR
                    move.w   #$70, MESSAGE_X
                    move.w   #$5A, MESSAGE_Y
                    move.l   #BIRD_BADDY, BADDY_RTN
                    move.l   #BIRD_BANIM, BANIMS_TAB
                    move.l   #BIRDM_FRMS, BADDY_FRMS
                    move.w   #$40, BADDY_WID
                    move.w   #$58, BADDY_HGT
                    move.b   #6, LEVEL_NUM
                    move.l   MENTB_AD, a1
                    move.l   A_MBLK_SP2, a0
                    move.l   A_END_MB2, a2
lbC0107DA:          move.l   (a0)+, (a1)+
                    cmp.l    a2, a0
                    bmi.s    lbC0107DA
                    sf       SAVE_SPACE
                    st       MENTB_MASK
                    bsr      SPRITE_TAB
                    rts

CLEAR_PERMS:        lea      PERM_TAB, a0
lbC0107F8:          clr.w    (a0)
                    addq.l   #8, a0
                    cmp.l    #END_PERMS, a0
                    bne.s    lbC0107F8
                    rts

MOVE_FISTS:         lea      FIST_X, a0
                    move.w   FIST1_X, (a0)+
                    move.w   FIST1_Y, (a0)+
                    move.w   FIST1XDIS, (a0)+
                    move.w   FIST1YDIS, (a0)+
                    move.w   FIST1_ACT, (a0)+
                    sf       FIST_NUM
                    bsr      COL_FIST
                    bsr      GO_FIST
                    lea      FIST_X, a0
                    move.w   (a0)+, FIST1_X
                    move.w   (a0)+, FIST1_Y
                    move.w   (a0)+, FIST1XDIS
                    move.w   (a0)+, FIST1YDIS
                    move.w   (a0)+, FIST1_ACT
                    lea      FIST_X, a0
                    move.w   FIST2_X, (a0)+
                    move.w   FIST2_Y, (a0)+
                    move.w   FIST2XDIS, (a0)+
                    move.w   FIST2YDIS, (a0)+
                    move.w   FIST2_ACT, (a0)+
                    st       FIST_NUM
                    bsr      COL_FIST
                    bsr      GO_FIST
                    lea      FIST_X, a0
                    move.w   (a0)+, FIST2_X
                    move.w   (a0)+, FIST2_Y
                    move.w   (a0)+, FIST2XDIS
                    move.w   (a0)+, FIST2YDIS
                    move.w   (a0)+, FIST2_ACT
                    rts

COL_FIST:           move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    move.w   FIST_X, d2
                    move.w   FIST_Y, d3
                    lsr.w    #4, d2
                    lsr.w    #4, d3
                    sub.w    d2, d0
                    bmi      lbC010910
                    cmp.w    #$48, d0
                    bpl      lbC010910
                    sub.w    d3, d1
                    bmi      lbC010910
                    cmp.w    #$48, d1
                    bpl      lbC010910
                    tst.b    ZOOL_HIT
                    bne      lbC010910
                    tst.b    SHADE_ON
                    bne      lbC010910
                    tst.w    SHIELD_ON
                    bne      lbC010910
                    jsr      ZOOL_DAMAGE
lbC010910:          rts

GO_FIST:            tst.w    BADDY_EN
                    bmi      lbC0109AA
                    tst.w    FIST_ACT
                    beq      lbC010B52
                    cmp.w    #2, FIST_ACT
                    bmi      _RANDOM
                    beq      lbC01093A
                    bra      lbC010ADA

lbC01093A:          move.w   FISTXDIS, d0
                    add.w    FIST_X, d0
                    move.w   d0, FIST_X
                    move.w   FISTYDIS, d1
                    add.w    FIST_Y, d1
                    tst.w    FISTYDIS
                    bpl      lbC010970
                    addq.w   #4, FISTYDIS
                    move.w   d1, FIST_Y
                    rts

lbC010970:          add.w    #12, FISTYDIS
                    tst.w    BADDY_EN
                    beq      lbC0109AC
                    move.w   d1, FIST_Y
                    cmp.w    #$1100, d1
                    ble      lbC0109AA
                    lea      LAND_FX, a5
                    jsr      ADD_SFX
                    move.w   #$1100, FIST_Y
                    subq.w   #1, FIST_ACT
lbC0109AA:          rts

lbC0109AC:          move.w   d1, FIST_Y
                    cmp.w    #$1700, d1
                    bmi.s    lbC0109AA
                    move.w   #$1700, FIST_Y
                    tst.b    FIST_NUM
                    beq.s    lbC0109AA
                    cmp.w    #$1700, FIST1_Y
                    bmi.s    lbC0109AA
                    move.w   #-1, BADDY_EN
                    move.b   #12, LASTLOOK
                    lea      BADHIT_FX, a5
                    jsr      ADD_SFX
                    rts

_RANDOM:            jsr      RANDOM
                    move.b   SEED, d7
                    and.b    #$7F, d7
                    cmp.b    #$7A, d7
                    bmi.s    lbC0109AA
                    cmp.b    #$7A, d7
                    beq      lbC010AA6
                    cmp.b    #$7B, d7
                    beq      lbC010AA6
lbC010A16:          addq.w   #1, FIST_ACT
                    move.w   #-88, d1
                    move.w   SEED+2, d7
                    and.w    #$1F, d7
                    sub.w    d7, d1
                    move.w   d1, FISTYDIS
                    btst     #1, SEED+1
                    beq      lbC010A68
                    tst.b    FIST_NUM
                    beq      lbC010A58
                    cmp.w    #$5400, FIST_X
                    bmi      lbC010A9C
                    bra      lbC010A92

lbC010A58:          cmp.w    #$4A00, FIST_X
                    bmi      lbC010A9C
                    bra      lbC010A92

lbC010A68:          tst.b    FIST_NUM
                    beq      lbC010A82
                    cmp.w    #$5200, FIST_X
                    bpl      lbC010A92
                    bra      lbC010A9C

lbC010A82:          cmp.w    #$4800, FIST_X
                    bpl      lbC010A92
                    bra      lbC010A9C

lbC010A92:          move.w   #-16, FISTXDIS
                    rts

lbC010A9C:          move.w   #$10, FISTXDIS
                    rts

lbC010AA6:          tst.b    FIST_NUM
                    beq      lbC010A16
                    cmp.w    #$1100, FIST1_Y
                    bne      lbC010A16
                    addq.w   #2, FIST_ACT
                    addq.w   #2, FIST1_ACT
                    move.w   #$10, FISTXDIS
                    move.w   #-16, FIST1XDIS
                    rts

lbC010ADA:          tst.b    FIST_NUM
                    beq      lbC0109AA
                    move.w   FISTXDIS, d0
                    add.w    FIST_X, d0
                    cmp.w    #$5800, d0
                    bmi      lbC010AFC
                    move.w   #$5800, d0
lbC010AFC:          move.w   d0, FIST_X
                    cmp.w    #$4300, FIST1_X
                    ble      lbC010B34
                    move.w   FIST1XDIS, d0
                    add.w    FIST1_X, d0
                    cmp.w    #$4300, d0
                    bpl      lbC010B2C
                    move.w   #$4300, FIST1_X
                    rts

lbC010B2C:          move.w   d0, FIST1_X
                    rts

lbC010B34:          move.w   #$30, FIST1XDIS
                    move.w   #-48, FISTXDIS
                    clr.w    FIST1_ACT
                    clr.w    FIST_ACT
                    rts

lbC010B52:          tst.b    FIST_NUM
                    bmi      lbC010BA2
                    move.w   FISTXDIS, d0
                    add.w    FIST_X, d0
                    cmp.w    #$4B00, d0
                    bmi      lbC010B92
                    move.w   #$4B00, FIST_X
                    clr.w    FISTXDIS
                    addq.w   #1, FIST_ACT
                    lea      LAND_FX, a5
                    jsr      ADD_SFX
                    rts

lbC010B92:          add.w    #12, FISTXDIS
                    move.w   d0, FIST_X
                    rts

lbC010BA2:          move.w   FISTXDIS, d0
                    add.w    FIST_X, d0
                    cmp.w    #$4F00, d0
                    bpl      lbC010BCC
                    move.w   #$4F00, FIST_X
                    clr.w    FISTXDIS
                    addq.w   #1, FIST_ACT
                    rts

lbC010BCC:          sub.w    #12, FISTXDIS
                    move.w   d0, FIST_X
                    rts

COL_CSEGS:          tst.b    INAIR
                    beq      lbC010C66
                    tst.w    Y_FORCE
                    bpl      lbC010C66
                    move.w   ZOOL_Y, d1
                    cmp.w    #$DE, d1
                    bpl      lbC010C66
                    move.w   ZOOL_X, d0
                    sub.w    #$490, d0
                    bmi      lbC010C66
                    lsr.w    #5, d0
                    cmp.w    #7, d0
                    bpl      lbC010C66
                    lsl.w    #1, d0
                    lea      CSEG_TAB, a0
                    lea      0(a0,d0.w), a0
                    cmp.w    #5, (a0)
                    beq      lbC010C66
                    subq.w   #1, BADDY_EN
                    clr.w    Y_FORCE
                    addq.w   #1, (a0)
                    move.w   BADDY_SPRS, d7
                    add.w    (a0), d7
                    add.w    #10, d7
                    sub.w    BACKFX_SPRS, d7
                    lsl.w    #4, d0
                    add.w    #$480, d0
                    move.w   #$A0, d1
                    jsr      ADD_PERM
                    lea      BREAK_FX, a5
                    jsr      ADD_SFX
lbC010C66:          rts

DRAW_LASTBAD:       bsr      DRAW_BLOOK
                    bsr      DRAW_FISTS
                    rts

DRAW_FISTS:         tst.w    BADDY_EN
                    bmi      lbC010D0A
                    move.w   FIST1_X, d0
                    move.w   FIST1_Y, d1
                    asr.w    #4, d0
                    asr.w    #4, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   #6, d7
                    add.w    BADDY_SPRS, d7
                    tst.w    d0
                    bmi      lbC010CB6
                    addq.w   #1, d7
lbC010CB6:          move.w   d7, SPRITE
                    jsr      DUMPSPRITE
                    move.w   FIST2_X, d0
                    move.w   FIST2_Y, d1
                    asr.w    #4, d0
                    asr.w    #4, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   #8, d7
                    add.w    BADDY_SPRS, d7
                    cmp.w    #$100, d0
                    bpl      lbC010CFE
                    addq.w   #1, d7
lbC010CFE:          move.w   d7, SPRITE
                    jmp      DUMPSPRITE

lbC010D0A:          rts

DRAW_BLOOK:         tst.w    BADDY_EN
                    bmi      lbC010E00
                    cmp.b    #1, LASTLOOK
                    bgt      lbC010D9A
                    move.w   ZOOL_X, d0
                    sub.w    #$4E0, d0
                    bmi      lbC010D86
                    cmp.w    #$40, d0
                    bmi      lbC010D70
                    tst.b    LASTLOOK
                    beq      lbC010DFE
                    sf       LASTLOOK
                    moveq    #2, d7
lbC010D4A:          add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    move.w   #$4B0, d0
                    move.w   #$F0, d1
                    jsr      ADD_PERM
                    add.w    #$40, d0
                    addq.w   #3, d7
                    jmp      ADD_PERM

lbC010D70:          tst.b    LASTLOOK
                    bgt      lbC010DFE
                    move.b   #1, LASTLOOK
                    moveq    #0, d7
                    bra.s    lbC010D4A

lbC010D86:          tst.b    LASTLOOK
                    bmi      lbC010DFE
                    st       LASTLOOK
                    moveq    #1, d7
                    bra.s    lbC010D4A

lbC010D9A:          subq.w   #1, BADDY_FRM
                    bpl      lbC010DFE
                    tst.w    BADDY_SPR
                    beq      lbC010DC6
                    clr.w    BADDY_SPR
                    move.w   #14, BADDY_FRM
                    subq.b   #1, LASTLOOK
                    moveq    #0, d7
                    bra.s    lbC010D4A

lbC010DC6:          move.w   #1, BADDY_SPR
                    move.w   #7, BADDY_FRM
                    moveq    #$10, d7
                    add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    move.w   #$4B0, d0
                    move.w   #$F0, d1
                    jsr      ADD_PERM
                    add.w    #$40, d0
                    addq.w   #1, d7
                    jmp      ADD_PERM

lbC010DFE:          rts

lbC010E00:          cmp.b    #4, LASTLOOK
                    beq.s    lbC010DFE
                    subq.b   #1, LASTLOOK
                    cmp.b    #4, LASTLOOK
                    beq      lbC010E28
                    cmp.b    #11, LASTLOOK
                    beq.s    lbC010DC6
                    rts

lbC010E28:          move.w   #$14, d7
                    add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    move.w   #$4B0, d0
                    move.w   #$100, d1
                    jsr      ADD_PERM
                    add.w    #$40, d0
                    jmp      ADD_PERM

ICES_BADDY:         cmp.b    #1, BADDY_ACT
                    bmi      lbC010E72
                    beq      lbC010EF2
                    cmp.b    #3, BADDY_ACT
                    bmi      lbC010FAA
                    beq      lbC010FFE
lbC010E70:          rts

lbC010E72:          move.w   BADDY_X, d0
                    move.w   BADDY_Y, d1
                    move.w   0, d7
                    add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    jsr      ADD_BACKSP
                    sub.w    #$20, d0
                    sub.w    #$10, d1
                    move.w   #10, d7
                    add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    jsr      ADD_BACKSP
                    addq.w   #2, d7
                    add.w    #$60, d0
                    jsr      ADD_BACKSP
                    addq.w   #1, BADDY_Y
                    move.w   MAP_LINE, d1
                    add.w    #15, d1
                    cmp.w    BADDY_Y, d1
                    bne.s    lbC010E70
                    addq.b   #1, BADDY_ACT
                    move.w   #$FA, BADDY_FRM
                    lea      COLPOW_FX, a5
                    jsr      ADD_SFX
                    rts

lbC010EF2:          move.w   BADDY_X, d0
                    move.w   BADDY_Y, d1
                    move.w   ZOOL_X, d2
                    sub.w    #$20, d2
                    sub.w    d0, d2
                    bpl      lbC010F22
                    move.w   0, d7
                    cmp.w    #-40, d2
                    bpl      lbC010F32
                    move.w   #2, d7
                    bra      lbC010F32

lbC010F22:          move.w   0, d7
                    cmp.w    #$28, d2
                    bmi      lbC010F32
                    move.w   #1, d7
lbC010F32:          add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    jsr      ADD_BACKSP
                    sub.w    #$20, d0
                    sub.w    #$10, d1
                    move.w   #10, d7
                    add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    jsr      ADD_BACKSP
                    addq.w   #2, d7
                    add.w    #$60, d0
                    jsr      ADD_BACKSP
                    subq.w   #1, BADDY_FRM
                    bne      lbC010E70
                    addq.b   #1, BADDY_ACT
                    move.w   #1, ICECUBES
                    lea      ICEBAD_TAB, a0
                    move.b   #$10, (a0)+
                    move.b   #8, (a0)+
                    move.w   BADDY_X, (a0)+
                    move.w   BADDY_Y, d0
                    lsl.w    #4, d0
                    move.w   d0, (a0)+
                    clr.w    (a0)+
                    clr.w    (a0)+
                    rts

lbC010FAA:          bsr      lbC010FFE
                    move.w   BADDY_X, d0
                    move.w   BADDY_Y, d1
                    sub.w    #$20, d0
                    sub.w    #$10, d1
                    move.w   #11, d7
                    add.w    BADDY_SPRS, d7
                    sub.w    BACKFX_SPRS, d7
                    jsr      ADD_BACKSP
                    add.w    #$60, d0
                    addq.w   #2, d7
                    jsr      ADD_BACKSP
                    subq.w   #1, BADDY_Y
                    cmp.w    #-32, BADDY_Y
                    bne      lbC010E70
                    addq.b   #1, BADDY_ACT
                    rts

lbC010FFE:          lea      ICEBAD_TAB, a0
lbC011004:          tst.b    (a0)
                    beq      lbC0110C0
                    moveq    #0, d2
                    move.b   1(a0), d2
                    lsl.w    #3, d2
                    move.w   d2, d3
                    move.w   2(a0), d0
                    add.w    6(a0), d0
                    sub.w    XPOS, d0
                    bpl      lbC01102C
                    clr.w    d0
                    neg.w    6(a0)
lbC01102C:          neg.w    d2
                    add.w    #$140, d2
                    cmp.w    d2, d0
                    ble      lbC01103E
                    move.w   d2, d0
                    neg.w    6(a0)
lbC01103E:          add.w    XPOS, d0
                    move.w   d0, 2(a0)
                    move.w   d3, d2
                    move.w   4(a0), d4
                    add.w    8(a0), d4
                    move.w   d4, d1
                    asr.w    #4, d1
                    neg.w    d2
                    add.w    #$F0, d2
                    cmp.w    d2, d1
                    ble      lbC0110AC
                    lea      LAND_FX, a5
                    cmp.b    #8, 1(a0)
                    bpl      _ADD_SFX5
                    lea      BRICK_FX, a5
                    cmp.b    #4, 1(a0)
                    beq      _ADD_SFX5
                    lea      FSTEP_FX, a5
_ADD_SFX5:          jsr      ADD_SFX
                    move.w   d2, d4
                    lsl.w    #4, d4
                    move.w   d3, d2
                    lsr.w    #1, d2
                    sub.w    #$60, d2
                    move.w   d2, 8(a0)
                    tst.w    6(a0)
                    bne      lbC0110AC
                    move.w   #-2, 6(a0)
lbC0110AC:          move.w   d4, 4(a0)
                    addq.w   #2, 8(a0)
                    move.w   d4, d1
                    asr.w    #4, d1
                    bsr      ICE_COLBULL
                    bsr      ICE_COLS
lbC0110C0:          lea      10(a0), a0
                    cmp.l    #CSEG_TAB, a0
                    bne      lbC011004
                    rts

ICE_COLS:           tst.b    ZOOL_HIT
                    bne      lbC011146
                    tst.w    SHIELD_ON
                    bne      lbC011146
                    move.w   ZOOL_X, d2
                    move.w   ZOOL_Y, d3
                    moveq    #0, d4
                    move.b   1(a0), d4
                    lsl.w    #3, d4
                    add.w    #$20, d4
                    add.w    #8, d2
                    sub.w    d0, d2
                    bmi      lbC011146
                    cmp.w    d4, d2
                    bpl      lbC011146
                    add.w    #$10, d3
                    sub.w    d1, d3
                    bmi      lbC011146
                    cmp.w    d4, d3
                    bpl      lbC011146
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
                    move.b   #$50, ZOOL_HIT
                    lea      ZLDAMA_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX6
                    lea      ZZDAMA_FX, a5
_ADD_SFX6:          jsr      ADD_SFX
lbC011146:          rts

ICE_COLBULL:        lea      SHOT_TAB, a4
lbC01114E:          tst.b    (a4)
                    beq      lbC011210
                    move.w   2(a4), d2
                    move.w   4(a4), d3
                    move.w   d2, -(sp)
                    move.w   d3, -(sp)
                    moveq    #0, d4
                    move.b   1(a0), d4
                    lsl.w    #3, d4
                    add.w    #$10, d4
                    addq.w   #8, d2
                    sub.w    d0, d2
                    bmi      lbC01120E
                    cmp.w    d4, d2
                    bpl      lbC01120E
                    addq.w   #8, d3
                    sub.w    d1, d3
                    bmi      lbC01120E
                    cmp.w    d4, d3
                    bpl      lbC01120E
                    subq.b   #1, (a0)
                    bne      lbC0111DE
                    moveq    #0, d5
                    move.b   1(a0), d5
                    lsr.b    #1, d5
                    move.b   d5, 1(a0)
                    beq      lbC01121E
                    addq.w   #2, ICECUBES
                    lea      EDIE_FX, a5
                    jsr      ADD_SFX
                    move.b   d5, (a0)
                    clr.w    6(a0)
                    move.w   #-80, 8(a0)
                    move.w   d0, d2
                    move.w   d1, d3
                    lsr.w    #1, d5
                    subq.w   #3, d5
                    cmp.w    #-1, d5
                    bmi      _ADD_ICES
                    move.w   #-2, d5
_ADD_ICES:          bsr      ADD_ICES
                    neg.w    d5
                    bsr      ADD_ICES
                    bra      lbC0111E0

lbC0111DE:          neg.b    (a0)
lbC0111E0:          move.w   (sp)+, d3
                    move.w   (sp)+, d2
                    move.w   d0, -(sp)
                    move.w   d1, -(sp)
                    move.w   d2, d0
                    move.w   d3, d1
                    move.w   #$400, d7
                    clr.w    d2
                    move.w   SPLAT_ANIM, d3
                    move.l   a0, -(sp)
                    jsr      ADD_FXPIX
                    move.l   (sp)+, a0
                    move.w   (sp)+, d1
                    move.w   (sp)+, d0
                    tst.b    (a4)
                    bpl      lbC011210
                    sf       (a4)
lbC01120E:          addq.l   #4, sp
lbC011210:          addq.l   #8, a4
                    cmp.l    #lbL01F788, a4
                    bne      lbC01114E
                    rts

lbC01121E:          sf       (a0)
                    subq.w   #1, ICECUBES
                    bne.s    lbC0111DE
                    move.w   2(a0), d2
                    move.w   4(a0), d3
                    asr.w    #4, d3
                    subq.w   #4, d2
                    subq.w   #4, d3
                    move.w   d2, ENDICON_X
                    move.w   d3, ENDICON_Y
                    st       ENDICON_ON
                    clr.w    ENDIC_YDIS
                    bra.s    lbC0111DE

ADD_ICES:           lea      ICEBAD_TAB, a5
lbC011256:          tst.b    (a5)
                    beq      lbC011262
                    lea      10(a5), a5
                    bra.s    lbC011256

lbC011262:          move.w   d2, -(sp)
                    move.w   d3, -(sp)
                    lsr.w    #2, d4
                    move.b   1(a0), 1(a5)
                    move.b   1(a0), (a5)
                    add.w    d4, d2
                    add.w    d4, d3
                    lsl.w    #4, d3
                    move.w   d2, 2(a5)
                    move.w   d3, 4(a5)
                    move.w   d5, 6(a5)
                    move.w   #-64, 8(a5)
                    move.w   (sp)+, d3
                    move.w   (sp)+, d2
                    rts

DRAW_ICEBAD:        lea      ICEBAD_TAB, a0
lbC011296:          tst.b    (a0)
                    beq      lbC011350
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    asr.w    #4, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    moveq    #0, d2
                    move.b   1(a0), d2
                    lsl.w    #2, d2
                    add.w    d2, d0
                    add.w    XPOS, d0
                    sub.w    ZOOL_X, d0
                    bpl      lbC0112E8
                    cmp.w    #-32, d0
                    bmi      lbC0112F6
lbC0112E0:          move.w   #0, d7
                    bra      lbC0112FA

lbC0112E8:          cmp.w    #$20, d0
                    bmi.s    lbC0112E0
                    move.w   #2, d7
                    bra      lbC0112FA

lbC0112F6:          move.w   #1, d7
lbC0112FA:          cmp.b    #4, 1(a0)
                    bgt      lbC011326
                    beq      lbC011322
                    cmp.b    #1, 1(a0)
                    bgt      lbC01131A
                    move.w   #9, d7
                    bra      lbC011326

lbC01131A:          add.w    #6, d7
                    bra      lbC011326

lbC011322:          add.w    #3, d7
lbC011326:          add.w    BADDY_SPRS, d7
                    move.w   d7, SPRITE
                    move.l   a0, -(sp)
                    tst.b    (a0)
                    bpl      _DUMPSPRITE8
                    neg.b    (a0)
                    st       LIGHT_SPR
_DUMPSPRITE8:       jsr      DUMPSPRITE
                    sf       LIGHT_SPR
                    move.l   (sp)+, a0
lbC011350:          lea      10(a0), a0
                    cmp.l    #CSEG_TAB, a0
                    bne      lbC011296
                    rts

SNK_BADDY:          cmp.b    #1, BADDY_ACT
                    bmi      lbC011386
                    beq      _GET_BADSPR
                    cmp.b    #3, BADDY_ACT
                    bmi      lbC011434
                    beq      lbC011502
                    bra      lbC0116F0

lbC011386:          subq.w   #1, SLIDE_SND
                    bpl      lbC0113A4
                    move.w   #8, SLIDE_SND
                    lea      SLIDE_FX, a5
                    jsr      ADD_SFX
lbC0113A4:          move.w   BADDY_XDIS, d0
                    asr.w    #4, d0
                    add.w    d0, BADDY_X
                    cmp.w    #$123A, BADDY_X
                    bpl      lbC0113E0
                    addq.w   #8, BADDY_XDIS
                    bmi      lbC0113E0
                    addq.b   #1, BADDY_ACT
                    clr.w    BADDY_XDIS
                    clr.w    BADDY_AN
                    clr.w    BADDY_FRM
lbC0113E0:          rts

_GET_BADSPR:        bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    bpl      lbC01142C
                    lea      COLPOW_FX, a5
                    jsr      ADD_SFX
                    clr.w    BADDY_FRM
                    move.w   #0, BADDY_SPR
                    move.w   #9, SEGMENTS
                    move.b   #2, BADDY_ACT
                    move.w   #$32, CIRC_CNT
                    move.w   #4, BADDY_EN
                    rts

lbC01142C:          addq.w   #1, BADDY_FRM
                    rts

lbC011434:          subq.w   #1, CIRC_CNT
                    beq      lbC011484
                    lea      SNBAD_XYS, a0
                    lea      SNK_SEGCIRC, a1
                    move.w   SEGMENTS, d7
lbC011450:          move.w   (a0), d0
                    move.w   2(a0), d1
                    cmp.w    (a1)+, d0
                    beq      lbC011468
                    bpl      lbC011466
                    addq.w   #1, d0
                    bra      lbC011468

lbC011466:          subq.w   #1, d0
lbC011468:          cmp.w    (a1)+, d1
                    beq      lbC01147A
                    bpl      lbC011478
                    addq.w   #1, d1
                    bra      lbC01147A

lbC011478:          subq.w   #1, d1
lbC01147A:          move.w   d0, (a0)+
                    move.w   d1, (a0)+
                    dbra     d7, lbC011450
                    rts

lbC011484:          lea      SNMB_PATH, a4
                    move.w   #$4F, d2
                    move.w   #0, d5
                    move.w   #0, d6
                    move.w   #$DC, TEMPW
lbC01149E:          move.w   TEMPW, d7
                    cmp.w    #$168, d7
                    bmi      _GET_XY
                    sub.w    #$168, TEMPW
                    sub.w    #$168, d7
_GET_XY:            jsr      GET_XY
                    add.w    #$1210, d0
                    add.w    #$100, d1
                    move.w   d0, (a4)+
                    move.w   d1, (a4)+
                    addq.w   #4, TEMPW
                    dbra     d2, lbC01149E
                    clr.w    BADDY_PATH
                    addq.b   #1, BADDY_ACT
                    clr.w    BADDY_YDIS
                    clr.w    BADDY_XDIS
                    move.l   #$12080, BADDY_LX
                    move.l   #$1200, BADDY_LY
                    rts

lbC011502:          tst.w    BADDY_EN
                    bpl      lbC01151E
                    subq.w   #1, SEGMENTS
                    beq      lbC01169C
                    move.w   #4, BADDY_EN
lbC01151E:          subq.w   #1, SLIDE_SND
                    bpl      lbC011548
                    lea      HISS_FX, a5
                    jsr      ADD_SFX
                    move.w   SEED, d0
                    and.w    #$7F, d0
                    add.w    #14, d0
                    move.w   d0, SLIDE_SND
lbC011548:          move.l   BADDY_LX, d0
                    move.l   BADDY_LY, d1
                    lsr.l    #4, d0
                    asr.l    #4, d1
                    add.w    #$10, d0
                    add.w    #$10, d1
                    move.w   d0, BADDY_X
                    move.w   d1, BADDY_Y
                    bsr      COL_BADDY
                    move.l   BADDY_LX, d0
                    lsr.l    #4, d0
                    add.w    #$18, d0
                    cmp.w    ZOOL_X, d0
                    bpl      lbC0115A4
                    addq.w   #4, BADDY_XDIS
                    cmp.w    #$40, BADDY_XDIS
                    ble      lbC0115BE
                    move.w   #$40, BADDY_XDIS
                    bra      lbC0115BE

lbC0115A4:          subq.w   #4, BADDY_XDIS
                    cmp.w    #-64, BADDY_XDIS
                    bpl      lbC0115BE
                    move.w   #-64, BADDY_XDIS
lbC0115BE:          move.l   BADDY_LY, d0
                    asr.w    #4, d0
                    add.w    #$18, d0
                    move.w   ZOOL_Y, d1
                    bpl      lbC0115D6
                    clr.w    d1
lbC0115D6:          cmp.w    d1, d0
                    bpl      lbC0115FA
                    addq.w   #4, BADDY_YDIS
                    cmp.w    #$40, BADDY_YDIS
                    ble      lbC011614
                    move.w   #$40, BADDY_YDIS
                    bra      lbC011614

lbC0115FA:          subq.w   #4, BADDY_YDIS
                    cmp.w    #-64, BADDY_YDIS
                    bpl      lbC011614
                    move.w   #-64, BADDY_YDIS
lbC011614:          move.w   BADDY_XDIS, d0
                    ext.l    d0
                    add.l    BADDY_LX, d0
                    move.l   d0, BADDY_LX
                    lsr.l    #4, d0
                    move.w   BADDY_YDIS, d1
                    ext.l    d1
                    add.l    BADDY_LY, d1
                    move.l   d1, BADDY_LY
                    lsr.l    #4, d1
                    lea      SNMB_PATH, a0
                    move.w   BADDY_PATH, d7
                    lsl.w    #2, d7
                    lea      0(a0,d7.w), a0
                    lea      SNBAD_XYS, a1
                    move.w   d0, (a1)+
                    move.w   d1, (a1)+
                    move.w   d0, (a0)
                    move.w   d1, 2(a0)
                    move.w   #7, d2
                    addq.w   #1, BADDY_PATH
                    cmp.w    #$50, BADDY_PATH
                    bne      lbC01167E
                    clr.w    BADDY_PATH
lbC01167E:          lea      -$20(a0), a0
                    cmp.l    #SNMB_PATH, a0
                    bpl      lbC011690
                    lea      $140(a0), a0
lbC011690:          move.w   (a0), (a1)+
                    move.w   2(a0), (a1)+
                    dbra     d2, lbC01167E
                    rts

lbC01169C:          addq.b   #1, BADDY_ACT
                    move.w   #-64, BADDY_YDIS
                    move.w   #$20, BADDY_XDIS
                    move.w   BADDY_X, ENDICON_X
                    move.w   BADDY_X, ENDICON_Y
                    move.w   #$120, ENDIC_FLR
                    move.w   #-64, ENDIC_YDIS
                    st       ENDICON_ON
                    addq.w   #1, SEGMENTS
                    lea      EDIE_FX, a5
                    jsr      ADD_SFX
                    rts

lbC0116F0:          move.l   BADDY_LX, d0
                    move.l   BADDY_LY, d1
                    lsr.l    #4, d0
                    asr.l    #4, d1
                    add.w    #$10, d0
                    add.w    #$10, d1
                    lea      SNBAD_XYS, a0
                    move.w   d0, (a0)
                    move.w   d1, 2(a0)
                    tst.w    BADDY_YDIS
                    ble      lbC01174A
                    cmp.w    #$120, d1
                    bmi      lbC01174A
                    move.w   #$120, 2(a0)
                    move.w   #-64, BADDY_YDIS
                    cmp.w    #$1300, (a0)
                    bmi      lbC0113E0
                    sf       BADDY_ON
                    st       NO_BADDY
                    rts

lbC01174A:          addq.w   #4, BADDY_YDIS
                    move.w   BADDY_XDIS, d0
                    ext.l    d0
                    add.l    BADDY_LX, d0
                    move.l   d0, BADDY_LX
                    lsr.l    #4, d0
                    move.w   BADDY_YDIS, d1
                    ext.l    d1
                    add.l    BADDY_LY, d1
                    move.l   d1, BADDY_LY
                    rts

TOOT_BADDY:         tst.b    BADDY_ACT
                    beq      lbC01183E
                    bsr      COL_BADDY
                    cmp.b    #1, BADDY_ACT
                    beq      lbC011B50
                    cmp.b    #3, BADDY_ACT
                    bmi      lbC0118CE
                    beq      lbC01197E
                    cmp.b    #5, BADDY_ACT
                    bmi      lbC011912
                    beq      lbC0117FC
                    subq.w   #1, BADDY_Y
                    move.w   BADDY_Y, d0
                    sub.w    MAP_LINE, d0
                    bpl      lbC0117DA
                    sf       BADDY_ON
                    st       NO_BADDY
                    rts

lbC0117DA:          cmp.w    #$40, d0
                    bpl      lbC011910
                    st       ENDICON_ON
                    move.w   BADDY_X, ENDICON_X
                    sub.w    #$10, ENDICON_X
                    rts

lbC0117FC:          addq.w   #1, BADDY_FRM
                    cmp.w    #2, BADDY_FRM
                    bne      _GET_BADSPR0
                    lea      EDIE_FX, a5
                    jsr      ADD_SFX
_GET_BADSPR0:       bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    bpl      lbC01183C
                    move.w   #9, BADDY_SPR
                    clr.w    BADDY_FRM
                    addq.b   #1, BADDY_ACT
lbC01183C:          rts

lbC01183E:          clr.w    BADDY_AN
                    addq.w   #1, BADDY_FRM
                    bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    bpl      lbC011866
                    clr.w    BADDY_FRM
                    move.w   #0, BADDY_SPR
lbC011866:          subq.w   #1, BADDY_X
                    cmp.w    #$C40, BADDY_X
                    bne      lbC011884
                    lea      COLPOW_FX, a5
                    jsr      ADD_SFX
lbC011884:          subq.w   #2, BADDY_YDIS
                    move.w   BADDY_YDIS, d0
                    ext.l    d0
                    cmp.w    #-512, d0
                    ble      _CONV_FLOOR
                    add.l    d0, BADDY_Y2
                    move.l   BADDY_Y2, d0
                    lsr.l    #7, d0
                    move.w   d0, BADDY_Y
                    rts

_CONV_FLOOR:        bsr      CONV_FLOOR
lbC0118B4:          clr.w    BADDY_YDIS
                    move.b   #2, BADDY_ACT
                    move.w   #3, BADDY_SPR
                    bra      lbC011A92

lbC0118CE:          move.w   PILL_X, d0
                    add.w    XPOS, d0
                    cmp.w    BADDY_X, d0
                    beq      lbC0118FA
                    bmi      lbC0118F2
                    addq.w   #4, BADDY_X
                    bra      lbC0118F8

lbC0118F2:          subq.w   #4, BADDY_X
lbC0118F8:          rts

lbC0118FA:          move.b   #4, BADDY_ACT
                    move.w   #10, BADDY_FRM
                    clr.w    BADDY_YDIS
lbC011910:          rts

lbC011912:          subq.w   #1, BADDY_FRM
                    bne.s    lbC011910
                    cmp.w    #4, BADDY_SPR
                    beq      lbC01196C
                    tst.w    BADDY_YDIS
                    bne      lbC011958
                    move.w   #3, BADDY_SPR
                    move.b   #3, BADDY_ACT
                    clr.w    BADDY_YDIS
                    move.w   BADDY_Y, d0
                    lsl.w    #4, d0
                    ext.l    d0
                    move.l   d0, BADDY_Y2
                    rts

lbC011958:          move.w   BADDY_Y, d0
                    lsl.w    #4, d0
                    ext.l    d0
                    move.l   d0, BADDY_Y2
                    bra      lbC0118B4

lbC01196C:          move.b   #1, BADDY_ACT
                    move.w   #5, BADDY_SPR
                    rts

lbC01197E:          addq.w   #8, BADDY_YDIS
                    move.w   BADDY_YDIS, d1
                    cmp.w    #$100, d1
                    ble      lbC01199C
                    move.w   #$100, d1
                    move.w   d1, BADDY_YDIS
lbC01199C:          ext.l    d1
                    add.l    d1, BADDY_Y2
                    move.l   BADDY_Y2, d1
                    lsr.l    #4, d1
                    move.w   d1, BADDY_Y
                    move.w   BADDY_X, d0
                    lsr.w    #4, d0
                    lsr.w    #4, d1
                    move.l   REF_MAP, a0
                    mulu     MAP_WIDTH, d1
                    add.w    d0, d1
                    lea      0(a0,d1.w), a0
                    cmp.b    #6, (a0)
                    beq      lbC0119E0
                    cmp.b    #$11, (a0)
                    beq      lbC0119EA
                    rts

lbC0119E0:          tst.b    PILL_HIT
                    beq      lbC011A5C
lbC0119EA:          and.w    #$FFF0, BADDY_Y
                    and.l    #$FF00, BADDY_Y2
                    move.b   #4, BADDY_ACT
                    clr.w    BADDY_YDIS
                    move.w   #$1E, BADDY_FRM
                    move.w   #4, BADDY_SPR
                    sf       PILL_HIT
                    tst.w    BADDY_EN
                    bpl      lbC011910
                    move.b   #5, BADDY_ACT
                    move.w   #1, BADDY_AN
                    clr.w    BADDY_FRM
                    move.w   #6, BADDY_SPR
                    move.w   BADDY_Y, ENDIC_FLR
                    sub.w    #$20, ENDIC_FLR
                    rts

lbC011A5C:          move.w   #4, BADDY_SPR
                    st       PILL_HIT
                    move.w   BADDY_X, d0
                    sub.w    #$10, d0
                    move.w   BADDY_Y, d1
                    move.w   #$200, d7
                    moveq    #2, d2
                    move.b   ROOFEX_ANIM, d3
                    move.w   FILLTILE_SPR, d4
                    jmp      ADD_TILEFX

lbC011A92:          lea      PILLS, a0
                    move.w   (a0), d7
                    addq.l   #4, a0
                    cmp.w    (a0), d7
                    bpl      lbC011AA4
                    move.w   (a0), d7
lbC011AA4:          addq.l   #4, a0
                    cmp.w    (a0), d7
                    bpl      lbC011AAE
                    move.w   (a0), d7
lbC011AAE:          addq.l   #4, a0
                    cmp.w    (a0), d7
                    bpl      lbC011AB8
                    move.w   (a0), d7
lbC011AB8:          lea      PILL_RES, a1
                    clr.l    (a1)
                    lea      -12(a0), a0
                    clr.w    d6
                    cmp.w    (a0), d7
                    bne      lbC011AD0
                    st       (a1)
                    addq.w   #1, d6
lbC011AD0:          addq.l   #4, a0
                    cmp.w    (a0), d7
                    bne      lbC011ADE
                    st       1(a1)
                    addq.w   #1, d6
lbC011ADE:          addq.l   #4, a0
                    cmp.w    (a0), d7
                    bne      lbC011AEC
                    st       2(a1)
                    addq.w   #1, d6
lbC011AEC:          addq.l   #4, a0
                    cmp.w    (a0), d7
                    bne      _RANDOM2
                    st       3(a1)
                    addq.w   #1, d6
_RANDOM2:           bsr      RANDOM
                    lea      PILL_RES, a1
                    tst.w    d6
                    beq      lbC011B40
                    move.b   SEED+3, d0
                    and.w    #3, d0
lbC011B14:          tst.b    0(a1,d0.w)
                    bne      lbC011B24
                    subq.w   #1, d0
                    bpl.s    lbC011B14
                    moveq    #3, d0
                    bra.s    lbC011B14

lbC011B24:          move.w   d0, PILL_CRUSH
                    lea      PILLS, a0
                    lsl.w    #2, d0
                    subq.w   #1, 0(a0,d0.w)
                    move.w   2(a0,d0.w), PILL_X
                    rts

lbC011B40:          move.w   #$FFFF, PILL_CRUSH
                    move.w   #$100, PILL_X
lbC011B50:          subq.w   #4, BADDY_Y
                    move.w   BADDY_Y, d0
                    sub.w    MAP_LINE, d0
                    cmp.w    #$20, d0
                    bne      lbC011910
                    move.b   #4, BADDY_ACT
                    move.w   #3, BADDY_SPR
                    move.w   #$14, BADDY_FRM
                    move.w   #$FFFF, BADDY_YDIS
                    rts

CONV_FLOOR:         move.l   REF_MAP, a0
                    move.w   MAP_WIDTH, d7
                    mulu     #$71, d7
                    add.w    #$B4, d7
                    lea      0(a0,d7.w), a0
                    move.w   #$13, d7
lbC011BA8:          cmp.b    #2, (a0)+
                    bne      lbC011BB6
                    move.b   #7, -1(a0)
lbC011BB6:          dbra     d7, lbC011BA8
                    rts

BULB_BADDY:         bsr      PRO_MBGEN
                    bsr      PRO_MBOLTS
                    cmp.b    #1, BADDY_ACT
                    bmi      lbC011BF4
                    beq      _GET_BADSPR1
                    bsr      COL_BADDY
                    cmp.b    #3, BADDY_ACT
                    bmi      _GET_BADSPR2
                    beq      lbC011D02
                    bra      lbC011E70

lbC011BF4:          subq.w   #1, SLIDE_SND
                    bpl      lbC011C12
                    move.w   #8, SLIDE_SND
                    lea      SLIDE_FX, a5
                    jsr      ADD_SFX
lbC011C12:          move.w   BADDY_XDIS, d0
                    asr.w    #4, d0
                    add.w    d0, BADDY_X
                    cmp.w    #$C00, BADDY_X
                    bpl      lbC011C4E
                    addq.w   #8, BADDY_XDIS
                    bmi      lbC011C4E
                    addq.b   #1, BADDY_ACT
                    clr.w    BADDY_XDIS
                    clr.w    BADDY_AN
                    clr.w    BADDY_FRM
lbC011C4E:          rts

_GET_BADSPR1:       bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    bpl      lbC011C8C
                    lea      COLPOW_FX, a5
                    jsr      ADD_SFX
                    clr.w    BADDY_FRM
                    move.w   #15, BADDY_SPR
                    addq.w   #1, BADDY_AN
                    addq.b   #1, BADDY_ACT
                    clr.w    BADDY_FRM
                    rts

lbC011C8C:          addq.w   #1, BADDY_FRM
                    rts

_GET_BADSPR2:       bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    bmi      lbC011CC0
                    cmp.w    #$10, BADDY_SPR
                    bne      lbC011CB8
                    tst.w    BADDY_EN
                    bmi      lbC011E44
lbC011CB8:          addq.w   #1, BADDY_FRM
                    rts

lbC011CC0:          clr.w    BADDY_FRM
                    move.w   #$12, BADDY_SPR
                    addq.w   #1, BADDY_AN
                    addq.b   #1, BADDY_ACT
                    move.w   #$FF80, BADDY_YDIS
                    move.w   #$FFFE, BADDY_XDIS
                    move.w   ZOOL_X, d0
                    cmp.w    BADDY_X, d0
                    bmi      lbC011D00
                    neg.w    BADDY_XDIS
lbC011D00:          rts

lbC011D02:          move.w   BADDY_X, d0
                    add.w    BADDY_XDIS, d0
                    move.w   d0, BADDY_X
                    sub.w    XPOS, d0
                    bpl      lbC011D36
                    or.w     #15, BADDY_X
                    addq.w   #1, BADDY_X
                    neg.w    BADDY_XDIS
                    bra      lbC011D4C

lbC011D36:          cmp.w    #$140, d0
                    bmi      lbC011D4C
                    neg.w    BADDY_XDIS
                    and.w    #$FFF0, BADDY_X
lbC011D4C:          move.w   BADDY_YDIS, d1
                    addq.w   #4, d1
                    move.w   d1, BADDY_YDIS
                    asr.w    #4, d1
                    add.w    d1, BADDY_Y
                    move.w   BADDY_Y, d1
                    tst.b    BADDY_DEAD
                    beq      lbC011D96
                    cmp.w    #$130, d1
                    bmi      _GET_BADSPR3
                    subq.w   #1, BADDY_FRM
                    cmp.w    #$1B0, d1
                    bmi      _GET_BADSPR3
                    sf       BADDY_ON
                    st       NO_BADDY
                    rts

lbC011D96:          cmp.w    #$500, d1
                    bmi      _GET_BADSPR3
                    and.w    #$FFF0, d1
                    move.w   d1, BADDY_Y
                    lea      BOING_FX, a5
                    jsr      ADD_SFX
                    subq.b   #1, BADDY_ACT
                    clr.w    BADDY_FRM
                    subq.w   #1, BADDY_AN
                    move.w   #$10, BADDY_SPR
                    rts

_GET_BADSPR3:       bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    cmp.w    #$1E, BADDY_FRM
                    beq      lbC011F20
lbC011DE6:          addq.w   #1, BADDY_FRM
                    tst.b    BADDY_DEAD
                    beq      lbC011E3A
                    tst.b    ENDICON_ON
                    bne      lbC011C4E
                    tst.w    END_OF_STG
                    bne      lbC011C4E
                    st       ENDICON_ON
                    move.w   BADDY_X, d0
                    move.w   BADDY_Y, d1
                    move.w   #$FFD0, ENDIC_YDIS
                    sub.w    #$66, d1
                    sub.w    #$10, d0
                    move.w   d0, ENDICON_X
                    move.w   d1, ENDICON_Y
                    rts

lbC011E3A:          cmp.w    #$25, BADDY_SPR
                    rts

lbC011E44:          addq.b   #2, BADDY_ACT
                    move.w   #$FFD0, BADDY_YDIS
                    move.w   #$17, BADDY_SPR
                    move.w   #$64, BADDY_FRM
                    lea      EDIE_FX, a5
                    jsr      ADD_SFX
                    rts

lbC011E70:          tst.w    BADDY_FRM
                    bmi      lbC011EB4
                    subq.w   #1, BADDY_FRM
                    bpl      lbC011C4E
                    st       ENDICON_ON
                    move.w   BADDY_X, d0
                    move.w   BADDY_Y, d1
                    move.w   #$FFD0, ENDIC_YDIS
                    sub.w    #$66, d1
                    sub.w    #$10, d0
                    move.w   d0, ENDICON_X
                    move.w   d1, ENDICON_Y
                    rts

lbC011EB4:          move.w   #$18, BADDY_SPR
                    addq.w   #2, BADDY_X
                    cmp.w    #$CC0, BADDY_X
                    bpl      lbC011F12
                    move.w   BADDY_YDIS, d1
                    addq.w   #4, d1
                    move.w   d1, BADDY_YDIS
                    asr.w    #4, d1
                    add.w    d1, BADDY_Y
                    move.w   BADDY_Y, d1
                    cmp.w    #$500, d1
                    bmi      lbC011C4E
                    lea      FSTEP_FX, a5
                    jsr      ADD_SFX
                    move.w   #$FFD0, BADDY_YDIS
                    and.w    #$FFF0, d1
                    move.w   d1, BADDY_Y
                    rts

lbC011F12:          st       NO_BADDY
                    sf       BADDY_ON
                    rts

lbC011F20:          move.w   ZOOL_SCRX, d0
                    cmp.w    #$50, d0
                    bmi      lbC011F56
                    cmp.w    #$A0, d0
                    bmi      lbC011F4E
                    cmp.w    #$F0, d0
                    bmi      lbC011F46
                    move.w   #$C40, d0
                    bra      lbC011F5A

lbC011F46:          move.w   #$BF0, d0
                    bra      lbC011F5A

lbC011F4E:          move.w   #$B90, d0
                    bra      lbC011F5A

lbC011F56:          move.w   #$B40, d0
lbC011F5A:          lea      SPARK_FX, a5
                    jsr      ADD_SFX
                    move.b   #$14, MBGENER_ON
                    move.b   #$46, MBBOLTS_ON
                    move.w   d0, MBGENER_X
                    add.w    #13, d0
                    move.w   d0, MBBOLT1_X
                    move.w   d0, MBBOLT2_X
                    move.w   d0, MBBOLT3_X
                    move.w   #$460, MBBOLT_Y
                    bra      lbC011DE6

PRO_MBGEN:          tst.b    MBGENER_ON
                    beq      lbC011FC8
                    subq.b   #1, MBGENER_ON
                    move.w   #$450, d1
                    move.w   MBGENER_X, d0
                    move.w   #$BB, d7
                    sub.w    BACKFX_SPRS, d7
                    jmp      ADD_BACKSP

lbC011FC8:          rts

PRO_MBOLTS:         tst.b    MBBOLTS_ON
                    beq      lbC0120D2
                    subq.b   #1, MBBOLTS_ON
                    addq.w   #4, MBBOLT_Y
                    move.w   MBBOLT_Y, d1
                    subq.w   #1, MBBOLT1_X
                    addq.w   #1, MBBOLT3_X
                    move.w   MBBOLT1_X, d0
                    move.w   MBBOLT_FRM, d7
                    lea      MBBOLT_AN, a0
                    move.w   0(a0,d7.w), d7
                    bpl      lbC012016
                    move.w   #$FFFE, MBBOLT_FRM
                    clr.w    d7
lbC012016:          addq.w   #2, MBBOLT_FRM
                    add.w    #$BC, d7
                    sub.w    BACKFX_SPRS, d7
                    jsr      ADD_BACKSP
                    move.w   MBBOLT2_X, d0
                    jsr      ADD_BACKSP
                    move.w   MBBOLT3_X, d0
                    jsr      ADD_BACKSP
                    tst.b    ZOOL_HIT
                    bne      lbC0120D2
                    tst.w    SHIELD_ON
                    bne      lbC0120D2
                    tst.b    SHADE_ON
                    bne      lbC0120D2
                    move.w   ZOOL_Y, d3
                    add.w    #$10, d3
                    sub.w    MBBOLT_Y, d3
                    bmi      lbC0120D2
                    cmp.w    #$28, d3
                    bpl      lbC0120D2
                    move.w   ZOOL_X, d2
                    add.w    #13, d2
                    sub.w    MBBOLT1_X, d2
                    bmi      lbC01209A
                    cmp.w    #$26, d2
                    bmi      lbC0120D4
lbC01209A:          move.w   ZOOL_X, d2
                    add.w    #13, d2
                    sub.w    MBBOLT2_X, d2
                    bmi      lbC0120B6
                    cmp.w    #$26, d2
                    bmi      lbC0120D4
lbC0120B6:          move.w   ZOOL_X, d2
                    add.w    #13, d2
                    sub.w    MBBOLT3_X, d2
                    bmi      lbC0120D2
                    cmp.w    #$26, d2
                    bmi      lbC0120D4
lbC0120D2:          rts

lbC0120D4:          move.b   #$28, ZOOL_HIT
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
                    lea      ZLDAMA_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX7
                    lea      ZZDAMA_FX, a5
_ADD_SFX7:          jsr      ADD_SFX
                    rts

BIRD_BADDY:         bsr      BD_BADDY
                    bra      PRO_BADETS

BD_BADDY:           cmp.b    #1, BADDY_ACT
                    beq      lbC0122AA
                    bmi      lbC012248
                    cmp.b    #3, BADDY_ACT
                    bmi      _GET_BADSPR4
                    beq      _GET_BADSPR5
                    bsr      COL_BADDY
                    cmp.b    #5, BADDY_ACT
                    bmi      _GET_BADSPR6
                    beq      _GET_BADSPR7
                    cmp.b    #7, BADDY_ACT
                    bmi      lbC01240A
                    beq      _GET_BADSPR8
                    subq.w   #1, BADDY_FRM
                    bpl      lbC0122A8
                    cmp.b    #6, LEVEL_NUM
                    bne      lbC01220C
                    st       BADDY_DEAD
                    st       NO_BADDY
                    sf       BADDY_ON
                    move.b   #9, BADDY_ACT
                    move.w   #10, END_OF_STG
                    move.w   BADDY_X, d0
                    move.w   BADDY_Y, d1
                    sub.w    #$10, d0
                    sub.w    #$18, d1
                    cmp.b    #6, LEVEL_NUM
                    beq      lbC0121D8
                    move.w   #$29, d2
                    jsr      NOTOK
                    addq.w   #8, d0
                    subq.w   #8, d1
                    move.w   #$29, d2
                    jsr      NOTOK
                    sub.w    #$14, d0
                    move.w   #$29, d2
                    jsr      NOTOK
                    lea      EDIE_FX, a5
                    jsr      ADD_SFX
                    rts

lbC0121D8:          move.w   #$11, d2
                    jsr      NOTOK
                    addq.w   #8, d0
                    subq.w   #8, d1
                    move.w   #$11, d2
                    jsr      NOTOK
                    sub.w    #$14, d0
                    move.w   #$11, d2
                    jsr      NOTOK
                    lea      EDIE_FX, a5
                    jsr      ADD_SFX
                    rts

lbC01220C:          st       BADDY_DEAD
                    move.w   #$2E, BADDY_SPR
                    move.w   #0, BADDY_FRM
                    move.w   #4, BADDY_AN
                    move.b   #6, BADDY_ACT
                    st       MESSAGE_ON
                    clr.w    BADDY_XDIS
                    move.w   #$FF90, BADDY_YDIS
                    rts

lbC012248:          move.w   BADDY_YDIS, d1
                    addq.w   #8, d1
                    move.w   d1, BADDY_YDIS
                    asr.w    #4, d1
                    add.w    d1, BADDY_Y
                    move.w   BADDY_Y, d1
                    cmp.w    #$130, d1
                    bmi      lbC0122A8
                    lea      LAND_FX, a5
                    jsr      ADD_SFX
                    and.w    #$FFF0, d1
                    move.w   d1, BADDY_Y
                    neg.w    BADDY_YDIS
                    add.w    #$30, BADDY_YDIS
                    bmi      lbC0122A8
                    clr.w    BADDY_YDIS
                    addq.b   #1, BADDY_ACT
                    move.w   #$32, BADDY_FRM
lbC0122A8:          rts

lbC0122AA:          subq.w   #1, BADDY_FRM
                    bne.s    lbC0122A8
                    addq.b   #1, BADDY_ACT
                    clr.w    BADDY_AN
                    rts

_GET_BADSPR4:       bsr      GET_BADSPR
                    tst.w    (a4)
                    bne      lbC0122DE
                    tst.w    2(a4)
                    beq      lbC0122DE
                    lea      LANDTOK_FX, a5
                    jsr      ADD_SFX
lbC0122DE:          move.w   (a4), BADDY_SPR
                    bpl      lbC012304
                    move.w   #3, BADDY_SPR
                    addq.b   #1, BADDY_ACT
                    clr.w    BADDY_FRM
                    addq.w   #1, BADDY_AN
                    rts

lbC012304:          addq.w   #1, BADDY_FRM
                    rts

_GET_BADSPR5:       bsr      GET_BADSPR
                    cmp.w    #$13, (a4)
                    beq      lbC01233E
                    move.w   (a4), BADDY_SPR
                    bpl      lbC012354
                    clr.w    BADDY_FRM
                    move.w   #$13, BADDY_SPR
                    addq.w   #1, BADDY_AN
                    addq.b   #1, BADDY_ACT
                    rts

lbC01233E:          cmp.w    #$12, -2(a4)
                    bne      lbC012354
                    lea      COLPOW_FX, a5
                    jsr      ADD_SFX
lbC012354:          addq.w   #1, BADDY_FRM
                    rts

_GET_BADSPR6:       bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    bpl      lbC0123A0
                    clr.w    BADDY_FRM
                    bsr      RANDOM
                    bchg     #4, SEED
                    beq      lbC01238A
                    bchg     #3, SEED
                    beq.s    _GET_BADSPR6
lbC01238A:          move.w   #$1C, BADDY_SPR
                    addq.w   #1, BADDY_AN
                    addq.b   #1, BADDY_ACT
                    rts

lbC0123A0:          tst.w    BADDY_EN
                    bmi      lbC0125A6
                    addq.w   #1, BADDY_FRM
                    rts

_GET_BADSPR7:       bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    bpl      lbC012402
                    clr.w    BADDY_FRM
                    move.w   #$20, BADDY_SPR
                    addq.w   #1, BADDY_AN
                    addq.b   #1, BADDY_ACT
                    move.w   #$FF90, BADDY_YDIS
                    move.w   #$FFFE, BADDY_XDIS
                    move.w   ZOOL_X, d0
                    cmp.w    BADDY_X, d0
                    bmi      lbC012400
                    neg.w    BADDY_XDIS
lbC012400:          rts

lbC012402:          addq.w   #1, BADDY_FRM
                    rts

lbC01240A:          move.w   BADDY_X, d0
                    add.w    BADDY_XDIS, d0
                    move.w   d0, BADDY_X
                    sub.w    XPOS, d0
                    bpl      lbC01243E
                    or.w     #15, BADDY_X
                    addq.w   #1, BADDY_X
                    neg.w    BADDY_XDIS
                    bra      lbC012454

lbC01243E:          cmp.w    #$140, d0
                    bmi      lbC012454
                    neg.w    BADDY_XDIS
                    and.w    #$FFF0, BADDY_X
lbC012454:          move.w   BADDY_YDIS, d1
                    addq.w   #4, d1
                    move.w   d1, BADDY_YDIS
                    asr.w    #4, d1
                    add.w    d1, BADDY_Y
                    move.w   BADDY_Y, d1
                    tst.b    BADDY_DEAD
                    beq      lbC0124DA
                    cmp.b    #6, LEVEL_NUM
                    beq      lbC0124AA
                    cmp.w    #$130, d1
                    bmi      _GET_BADSPR9
                    subq.w   #1, BADDY_FRM
                    cmp.w    #$1B0, d1
                    bmi      _GET_BADSPR9
                    sf       BADDY_ON
                    st       NO_BADDY
                    rts

lbC0124AA:          cmp.w    #$150, d1
                    bmi      _GET_BADSPR9
                    subq.w   #1, BADDY_FRM
                    cmp.w    #$1D0, d1
                    bmi      _GET_BADSPR9
                    sf       BADDY_ON
                    st       NO_BADDY
                    rts

lbC0124CE:          cmp.w    #$150, d1
                    bmi      _GET_BADSPR9
                    bra      lbC0124EC

lbC0124DA:          cmp.b    #6, LEVEL_NUM
                    beq.s    lbC0124CE
                    cmp.w    #$130, d1
                    bmi      _GET_BADSPR9
lbC0124EC:          and.w    #$FFF0, d1
                    move.w   d1, BADDY_Y
                    addq.b   #1, BADDY_ACT
                    lea      LAND_FX, a5
                    jsr      ADD_SFX
_GET_BADSPR9:       bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    addq.w   #1, BADDY_FRM
                    tst.b    BADDY_DEAD
                    beq      lbC012566
                    tst.b    ENDICON_ON
                    bne      lbC0122A8
                    tst.w    END_OF_STG
                    bne      lbC0122A8
                    st       ENDICON_ON
                    move.w   BADDY_X, d0
                    move.w   BADDY_Y, d1
                    move.w   #$FFD0, ENDIC_YDIS
                    sub.w    #$66, d1
                    sub.w    #$10, d0
                    move.w   d0, ENDICON_X
                    move.w   d1, ENDICON_Y
                    rts

lbC012566:          cmp.w    #$25, BADDY_SPR
                    beq      ADD_BADET
                    rts

_GET_BADSPR8:       bsr      GET_BADSPR
                    move.w   (a4), BADDY_SPR
                    bpl      lbC01259E
                    clr.w    BADDY_FRM
                    move.w   #$2D, BADDY_SPR
                    subq.w   #2, BADDY_AN
                    subq.b   #3, BADDY_ACT
                    rts

lbC01259E:          addq.w   #1, BADDY_FRM
                    rts

lbC0125A6:          move.b   #8, BADDY_ACT
                    move.w   #$2E, BADDY_SPR
                    move.w   #$78, BADDY_FRM
                    move.b   #$7F, BADDY_HIT
                    lea      EDIE_FX, a5
                    jsr      ADD_SFX
                    rts

ADD_BADET:          lea      BADETS_TAB, a6
lbC0125DA:          tst.b    (a6)
                    beq      lbC0125EE
                    lea      10(a6), a6
                    cmp.l    #BIRD_BANIM, a6
                    bne.s    lbC0125DA
                    rts

lbC0125EE:          lea      POP_FX, a5
                    jsr      ADD_SFX
                    st       (a6)+
                    move.b   #$1A, (a6)+
                    move.w   BADDY_X, (a6)
                    subq.w   #8, (a6)+
                    move.w   BADDY_Y, (a6)
                    sub.w    #$5A, (a6)+
                    bsr      RANDOM
                    move.w   SEED, d7
                    and.w    #7, d7
                    lea      BADET_LNCH, a5
                    lsl.w    #2, d7
                    move.w   0(a5,d7.w), (a6)+
                    move.w   2(a5,d7.w), (a6)+
                    rts

PRO_BADETS:         lea      BADETS_TAB, a0
                    subq.b   #1, BADET_AN
                    bpl      lbC01265C
                    eor.b    #1, 1(a0)
                    eor.b    #1, 11(a0)
                    eor.b    #1, $15(a0)
                    move.b   #3, BADET_AN
lbC01265C:          tst.b    (a0)
                    beq      lbC012756
                    move.w   6(a0), d0
                    add.w    d0, 2(a0)
                    move.w   2(a0), d0
                    move.w   d0, d2
                    sub.w    XPOS, d0
                    cmp.w    #-16, d0
                    ble      lbC01277E
                    cmp.w    #$140, d0
                    bpl      lbC01277E
                    move.w   8(a0), d0
                    asr.w    #4, d0
                    add.w    d0, 4(a0)
                    addq.w   #4, 8(a0)
                    move.w   4(a0), d3
                    cmp.b    #6, LEVEL_NUM
                    beq      lbC012766
                    cmp.w    #$120, d3
                    bmi      lbC0126B8
                    move.w   #-64, 8(a0)
                    and.w    #$FFF0, 4(a0)
lbC0126B8:          add.w    #$24, d2
                    sub.w    ZOOL_X, d2
                    bmi      lbC012756
                    cmp.w    #$20, d2
                    bpl      lbC012756
                    add.w    #$20, d3
                    sub.w    ZOOL_Y, d3
                    bmi      lbC012756
                    cmp.w    #$30, d3
                    bpl      lbC012756
                    tst.b    ZOOL_HIT
                    bne      lbC012756
                    tst.b    SHADE_ON
                    bne      lbC012756
                    tst.w    SHIELD_ON
                    bne      lbC012756
                    lea      ZLDAMA_FX, a5
                    tst.b    ITS_ZOOL
                    bne      _ADD_SFX8
                    lea      ZZDAMA_FX, a5
_ADD_SFX8:          jsr      ADD_SFX
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
                    move.b   #$50, ZOOL_HIT
                    move.l   a0, -(sp)
                    jsr      INIT_JUMP
                    move.l   (sp)+, a0
                    move.w   #$FF90, Y_FORCE
                    move.w   #$FFC0, X_FORCE
                    tst.w    6(a0)
                    bmi      lbC012756
                    move.w   #$40, X_FORCE
lbC012756:          lea      10(a0), a0
                    cmp.l    #BIRD_BANIM, a0
                    bne      lbC01265C
                    rts

lbC012766:          cmp.w    #$140, d3
                    bmi      lbC0126B8
                    move.w   #-64, 8(a0)
                    and.w    #$FFF0, 4(a0)
                    bra      lbC0126B8

lbC01277E:          sf       (a0)
                    bra.s    lbC012756

DRAW_BADETS:        lea      BADETS_TAB, a0
                    cmp.b    #8, BADDY_ACT
                    bpl      lbC0127E0
lbC012794:          tst.b    (a0)
                    beq      lbC0127D4
                    move.w   2(a0), d0
                    sub.w    XPOS, d0
                    move.w   4(a0), d1
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    moveq    #0, d7
                    move.b   1(a0), d7
                    add.w    BADDY_SPRS, d7
                    move.w   d7, SPRITE
                    move.l   a0, -(sp)
                    bsr      DUMPSPRITE
                    move.l   (sp)+, a0
lbC0127D4:          lea      10(a0), a0
                    cmp.l    #BIRD_BANIM, a0
                    bne.s    lbC012794
lbC0127E0:          rts

COL_BADDY:          tst.w    BADDY_EN
                    bmi      lbC0128FE
                    bsr      BULL_BADDY
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sub.w    #$30, d0
                    sub.w    BADDY_X, d0
                    bpl      lbC0128FE
                    move.w   BADDY_WID, d2
                    neg.w    d2
                    cmp.w    d2, d0
                    bmi      lbC0128FE
                    sub.w    BADDY_Y, d1
                    bpl      lbC0128FE
                    move.w   BADDY_HGT, d2
                    neg.w    d2
                    sub.w    d2, d1
                    bmi      lbC0128FE
                    tst.w    Y_FORCE
                    ble      lbC0128B6
                    cmp.w    #$10, d1
                    bpl      lbC0128B6
                    tst.b    BADDY_HIT
                    bne      lbC0128FE
                    jsr      INIT_JUMP
                    lea      BADHIT_FX, a5
                    jsr      ADD_SFX
                    move.w   #$FF80, Y_FORCE
                    move.b   #$64, BADDY_HIT
                    subq.w   #5, BADDY_EN
                    move.w   ZOOL_X, d0
                    move.w   ZOOL_Y, d1
                    sub.w    #$1C, d0
                    add.w    #$10, d1
                    move.b   #2, d2
                    cmp.b    #7, LEVEL_NUM
                    beq      lbC0128A4
                    cmp.b    #5, LEVEL_NUM
                    bpl      lbC0128FE
lbC0128A4:          move.w   #$400, d7
                    clr.w    d2
                    move.w   SPLAT_ANIM, d3
                    jmp      ADD_FXPIX

lbC0128B6:          tst.b    BADDY_HIT
                    bne      lbC0128FE
                    tst.b    ZOOL_HIT
                    bne      lbC0128FE
                    tst.w    SHIELD_ON
                    bne      lbC0128FE
                    ifeq TRAINER
                    subq.w   #1, ENERGY
                    endif
                    move.b   #$50, ZOOL_HIT
                    lea      ZZDAMA_FX, a5
                    tst.b    ITS_ZOOL
                    beq      _ADD_SFX9
                    lea      ZLDAMA_FX, a5
_ADD_SFX9:          jsr      ADD_SFX
lbC0128FE:          rts

BULL_BADDY:         move.w   BADDY_X, d2
                    move.w   BADDY_Y, d3
                    sub.w    #$18, d2
                    cmp.b    #3, LEVEL_NUM
                    beq      lbC012924
                    sub.w    #$50, d3
                    bra      lbC012928

lbC012924:          sub.w    #$10, d3
lbC012928:          lea      SHOT_TAB, a4
lbC01292E:          tst.b    (a4)
                    beq      lbC0129FE
                    move.w   2(a4), d0
                    move.w   4(a4), d1
                    sub.w    d2, d0
                    bmi      lbC0129FE
                    move.w   BADDY_WID, d7
                    sub.w    #$10, d7
                    cmp.w    d7, d0
                    bpl      lbC0129FE
                    sub.w    d3, d1
                    bmi      lbC0129FE
                    move.w   BADDY_HGT, d7
                    sub.w    #$18, d7
                    cmp.w    d7, d1
                    bpl      lbC0129FE
                    move.b   #2, d2
                    move.w   2(a4), d0
                    move.w   4(a4), d1
                    cmp.b    #7, LEVEL_NUM
                    beq      lbC01298C
                    cmp.b    #5, LEVEL_NUM
                    bpl      lbC01299E
lbC01298C:          move.w   #$400, d7
                    clr.w    d2
                    move.w   SPLAT_ANIM, d3
                    jsr      ADD_FXPIX
lbC01299E:          tst.b    (a4)
                    bpl      lbC012A0C
                    sf       (a4)
                    lea      BADHIT_FX, a5
                    jsr      ADD_SFX
                    subq.w   #1, BADDY_EN
                    move.b   #4, BADDY_HIT
                    cmp.b    #2, LEVEL_NUM
                    beq      lbC012A0A
                    move.w   BADDY_X, d0
                    sub.w    XPOS, d0
                    cmp.w    #$130, d0
                    bpl      lbC012A0A
                    cmp.w    #$10, d0
                    bmi      lbC012A0A
                    addq.w   #4, BADDY_X
                    tst.w    6(a4)
                    bpl      lbC0129FC
                    subq.w   #8, BADDY_X
lbC0129FC:          rts

lbC0129FE:          addq.l   #8, a4
                    cmp.l    #lbL01F788, a4
                    bne      lbC01292E
lbC012A0A:          rts

lbC012A0C:          subq.w   #8, BADDY_EN
                    move.b   #$1E, BADDY_HIT
                    lea      BADHIT_FX, a5
                    jsr      ADD_SFX
                    cmp.b    #2, LEVEL_NUM
                    beq.s    lbC012A0A
                    add.w    #12, BADDY_X
                    sf       (a4)
                    tst.w    6(a4)
                    bpl.s    lbC0129FC
                    sub.w    #$18, BADDY_X
                    rts

DRAW_BADDY:         tst.b    END_FIGHT
                    beq      lbC012B16
                    tst.b    BADDY_ON
                    beq      lbC012B06
                    cmp.b    #4, LEVEL_NUM
                    beq      DRAW_ICEBAD
                    cmp.b    #5, LEVEL_NUM
                    beq      DRAW_LASTBAD
                    tst.b    BADDY_HIT
                    beq      lbC012A86
                    subq.b   #1, BADDY_HIT
lbC012A86:          tst.w    SEGMENTS
                    bne      lbC012B18
                    move.w   BADDY_X, d0
                    move.w   BADDY_Y, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.l   BADDY_FRMS, a0
                    move.w   BADDY_SPR, d7
                    lsl.w    #2, d7
                    move.l   0(a0,d7.w), a0
lbC012ABA:          move.w   (a0)+, d7
                    bmi      lbC012B06
                    move.w   d0, -(sp)
                    move.w   d1, -(sp)
                    add.w    (a0)+, d0
                    add.w    (a0)+, d1
                    move.l   a0, -(sp)
                    add.w    BADDY_SPRS, d7
                    move.w   d7, SPRITE
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    btst     #0, BADDY_HIT
                    beq      _DUMPSPRITE1
                    st       LIGHT_SPR
_DUMPSPRITE1:       bsr      DUMPSPRITE
                    sf       LIGHT_SPR
                    move.l   (sp)+, a0
                    move.w   (sp)+, d1
                    move.w   (sp)+, d0
                    bra.s    lbC012ABA

lbC012B06:          cmp.b    #2, LEVEL_NUM
                    beq      DRAW_TOOTMB
                    bra      DRAW_BADETS

lbC012B16:          rts

lbC012B18:          move.w   SEGMENTS, TEMPW
                    lea      SNBAD_XYS+4, a0
                    addq.w   #1, BADDY_FRM
                    cmp.w    #$10, BADDY_FRM
                    bne      lbC012B40
                    clr.w    BADDY_FRM
lbC012B40:          move.w   BADDY_FRM, d2
lbC012B46:          subq.w   #1, TEMPW
                    beq      lbC012BB2
                    move.w   d2, d7
                    lsr.w    #2, d7
                    add.w    BADDY_SPRS, d7
                    add.w    #9, d7
                    move.w   d7, SPRITE
                    move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.l   a0, -(sp)
                    move.w   d2, -(sp)
                    btst     #0, BADDY_HIT
                    beq      _DUMPSPRITE2
                    st       LIGHT_SPR
_DUMPSPRITE2:       jsr      DUMPSPRITE
                    sf       LIGHT_SPR
                    move.w   (sp)+, d2
                    move.l   (sp)+, a0
                    subq.w   #2, d2
                    bpl      lbC012BB0
                    add.w    #$10, d2
lbC012BB0:          bra.s    lbC012B46

lbC012BB2:          move.w   #$11, d7
                    add.w    BADDY_SPRS, d7
                    move.w   d7, SPRITE
                    lea      SNBAD_XYS, a0
                    move.w   (a0)+, d0
                    move.w   (a0)+, d1
                    sub.w    XPOS, d0
                    sub.w    MAP_LINE, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    btst     #0, BADDY_HIT
                    beq      _DUMPSPRITE3
                    st       LIGHT_SPR
_DUMPSPRITE3:       jsr      DUMPSPRITE
                    sf       LIGHT_SPR
                    rts

DRAW_TOOTMB:        bsr      DRAW_CHAIN
                    rts

DRAW_CHAIN:         move.w   BADDY_Y, d1
                    move.w   BADDY_X, d0
                    tst.b    BADDY_ACT
                    beq      lbC012C2C
                    sub.w    #8, d0
                    sub.w    #$60, d1
                    bra      lbC012C34

lbC012C2C:          sub.w    #$98, d1
                    sub.w    #$20, d0
lbC012C34:          sub.w    MAP_LINE, d1
                    sub.w    XPOS, d0
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   #5, d7
                    add.w    BADDY_SPRS, d7
                    move.w   d7, SPRITE
lbC012C5C:          cmp.w    #$FFF0, YCOORD
                    ble      lbC012C76
                    bsr      DUMPSPRITE
                    sub.w    #$20, YCOORD
                    bra.s    lbC012C5C

lbC012C76:          rts

DRAW_MESS:          tst.b    MESSAGE_ON
                    beq      lbC012CC0
                    move.w   MESSAGE_X, XCOORD
                    move.w   MESSAGE_Y, YCOORD
                    move.w   MESSAGE_SPR, SPRITE
                    bpl      DUMPSPRITE
                    neg.w    SPRITE
                    bsr      DUMPSPRITE
                    add.w    #$40, XCOORD
                    addq.w   #1, SPRITE
                    bra      DUMPSPRITE

lbC012CC0:          rts

GET_BADSPR:         move.l   BANIMS_TAB, a4
                    move.w   BADDY_AN, d2
                    lsl.w    #2, d2
                    move.l   0(a4,d2.w), a4
                    move.w   BADDY_FRM, d2
                    lsl.w    #1, d2
                    lea      0(a4,d2.w), a4
                    rts

NEWPALET:           lea      $DFF180, a1
                    move.w   #$1420, $DFF106
                    move.l   a2, -(sp)
                    move.l   a3, -(sp)
                    lea      LEVCOLS, a2
                    lea      SCRCOLS, a3
                    move.w   #15, d7
lbC012DB4:          move.w   (a0)+, d6
                    lsl.w    #1, d6
                    move.w   d6, (a1)+
                    move.w   d6, (a2)+
                    move.w   d6, (a3)+
                    addq.l   #2, a2
                    addq.l   #2, a3
                    dbra     d7, lbC012DB4
                    move.l   (sp)+, a3
                    move.l   (sp)+, a2
                    rts

UPPERPALET:         lea      $DFF1A0, a1
                    move.w   #15, d7
lbC012DD6:          move.w   (a0)+, d6
                    lsl.w    #1, d6
                    move.w   d6, (a1)+
                    dbra     d7, lbC012DD6
                    rts

CLRPALET:           move.w   SWITCH_CNT, d0
lbC012DE8:          cmp.w    SWITCH_CNT, d0
                    beq.s    lbC012DE8
                    move.w   #$C8, d7
lbC012DF4:          dbra     d7, lbC012DF4
                    lea      BLACK_PAL, a0
                    jsr      NEWPALET
                    lea      BLACK_PAL, a0
                    jsr      UPPERPALET
                    move.w   #0, lbB048BB4
                    move.w   #0, lbB048BB8
                    move.w   #0, lbB048BBC
                    move.w   #0, lbB048BC4
                    move.w   #0, lbB048BC8
                    move.w   #0, lbB048BCC
                    move.w   SWITCH_CNT, d0
lbC012E46:          cmp.w    SWITCH_CNT, d0
                    beq.s    lbC012E46
                    move.w   #$C8, d7
lbC012E52:          dbra     d7, lbC012E52
                    move.w   #$100, $DFF096
                    rts

PRO_ZONES:          move.w   XPOS, d0
                    move.w   MAP_LINE, d1
                    asr.w    #4, d0
                    asr.w    #4, d1
                    move.l   ZONE_TAB, a5
lbC012E76:          move.l   (a5)+, d7                   ; zone pointer
                    beq      END_PRO_ZONES
                    move.l   d7, a0
                    cmp.w    (a0)+, d0                   ; check zone's boundaries
                    bmi.s    lbC012E76
                    cmp.w    (a0)+, d1
                    bmi.s    lbC012E76
                    cmp.w    (a0)+, d0
                    bpl.s    lbC012E76
                    cmp.w    (a0)+, d1
                    bpl.s    lbC012E76
                    move.l   (a0)+, d0                  ; LS1_SPRS
                    lea      SPR_DATA, a1               ; (block with a copy of some data from enemydat)
                    sub.l    STAGE_SPRS, d0             ; PP1_SPRS PP2_SPRS PP3_SPRS
                    add.l    d0, a1
                    move.l   a1, SPRITE_BANK
                    move.w   (a0)+, SPR_CNT
                    rts

END_PRO_ZONES:      clr.w    SPR_CNT
                    move.l   #SPR_DATA, SPRITE_BANK
                    rts

PRINT_PANEL:        bsr      PRINT_SCORE
                    bsr      PRINT_NRG
                    bsr      PRINT_LIVES
                    bsr      PRINT_TIME
                    bsr      PRINT_PERC
                    bsr      PRINT_EXIT
                    rts

INIT_PANEL:         move.l   #$FFFFFFFF, OLDSCORE
                    move.w   #$FFFF, OLDEX_DIR
                    move.w   #$FFFF, OLDLIVES
                    move.w   #$FFFF, OLDPERC
                    move.w   #$FFFF, OLDTIME
                    move.w   #$FFFF, OLDENERGY
                    st       REVIVE
                    st       HIGH_RES
                    move.l   SCORE, -(sp)
                    move.l   HISCORE, SCORE
                    bsr      PRINT_SCORE
                    move.l   (sp)+, SCORE
                    move.l   #$FFFFFFFF, OLDSCORE
                    clr.w    ENERGY
                    clr.w    TIME_CNT
                    addq.w   #1, TIME
                    move.w   #$FFFF, BLINK_CNT
                    rts

PRINT_EXIT:         bsr      POINT_EXIT
                    move.w   EXIT_DIR, d0
                    cmp.w    OLDEX_DIR, d0
                    beq      lbC012F98
                    move.w   d0, OLDEX_DIR
                    lea      TOP_PANEL, a1
                    lea      lbL02E70C, a0
                    lsl.w    #7, d0
                    lea      0(a0,d0.w), a0
                    move.w   #11, d7
lbC012F86:          move.w   (a0)+, $6A(a1)
                    move.w   (a0)+, $6E(a1)
                    lea      $94(a1), a1
                    addq.l   #4, a0
                    dbra     d7, lbC012F86
lbC012F98:          rts

POINT_EXIT:         cmp.w    #25, TIME_CNT
                    bne      lbC012FE4
                    move.w   ENDICON_X, d0
                    move.w   ENDICON_Y, d1
                    add.w    #15, d0
                    sub.w    ZOOL_X, d0
                    bmi      lbC01304E
                    sub.w    ZOOL_Y, d1
                    ble      lbC013034
                    cmp.w    d0, d1
                    beq      lbC012FE6
                    bpl      lbC01302C
                    asr.w    #1, d0
                    cmp.w    d0, d1
                    bpl      lbC012FE6
lbC012FDC:          move.w   #6, EXIT_DIR
lbC012FE4:          rts

lbC012FE6:          move.w   #5, EXIT_DIR
                    rts

lbC012FF0:          move.w   #4, EXIT_DIR
                    rts

lbC012FFA:          move.w   #3, EXIT_DIR
                    rts

lbC013004:          move.w   #2, EXIT_DIR
                    rts

lbC01300E:          move.w   #1, EXIT_DIR
                    rts

lbC013018:          move.w   #0, EXIT_DIR
                    rts

lbC013022:          move.w   #7, EXIT_DIR
                    rts

lbC01302C:          asr.w    #1, d1
                    cmp.w    d0, d1
                    bpl.s    lbC012FF0
                    bra.s    lbC012FE6

lbC013034:          neg.w    d1
                    cmp.w    d0, d1
                    beq.s    lbC013022
                    bpl      lbC013046
                    asr.w    #1, d0
                    cmp.w    d0, d1
                    bpl.s    lbC013022
                    bra.s    lbC012FDC

lbC013046:          asr.w    #1, d1
                    cmp.w    d0, d1
                    bpl.s    lbC013018
                    bra.s    lbC013022

lbC01304E:          sub.w    ZOOL_Y, d1
                    ble      lbC013072
                    neg.w    d0
                    cmp.w    d0, d1
                    beq.s    lbC012FFA
                    bpl      lbC01306A
                    asr.w    #1, d0
                    cmp.w    d0, d1
                    bpl.s    lbC012FFA
                    bra.s    lbC013004

lbC01306A:          asr.w    #1, d1
                    cmp.w    d0, d1
                    bpl.s    lbC012FF0
                    bra.s    lbC012FFA

lbC013072:          neg.w    d0
                    neg.w    d1
                    cmp.w    d0, d1
                    beq.s    lbC01300E
                    bpl      lbC013088
                    asr.w    #1, d0
                    cmp.w    d0, d1
                    bpl.s    lbC01300E
                    bra      lbC013004

lbC013088:          asr.w    #1, d1
                    cmp.w    d0, d1
                    bpl.s    lbC013018
                    bra      lbC01300E

PRINT_LIVES:        move.w   LIVES, d0
                    bpl      lbC01309E
                    clr.w    d0
lbC01309E:          cmp.w    OLDLIVES, d0
                    beq      lbC013110
                    move.w   d0, OLDLIVES
                    lea      DEC_NUMS+16, a1
                    and.l    #$FF, d0
                    bsr      CONVTODEC
                    lea      DECIMAL, a0
                    lea      TOP_PANEL, a1
                    move.b   (a0)+, d0
                    lea      lbW02E20C, a2
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a3
                    move.b   (a0)+, d0
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a4
                    move.w   #11, d7
lbC0130EC:          move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $8E(a1)
                    move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $92(a1)
                    addq.w   #4, a3
                    addq.w   #4, a4
                    lea      $94(a1), a1
                    dbra     d7, lbC0130EC
lbC013110:          rts

PRINT_PERC:         move.b   PERCENT, d0
                    cmp.b    #$64, d0
                    bmi      lbC01312C
                    move.b   #99, PERCENT
                    move.b   #99, d0
lbC01312C:          cmp.b    OLDPERC, d0
                    beq      lbC01319E
                    move.b   d0, OLDPERC
                    lea      DEC_NUMS+16, a1
                    and.l    #$FF, d0
                    bsr      CONVTODEC
                    lea      DECIMAL, a0
                    lea      TOP_PANEL, a1
                    move.b   (a0)+, d0
                    lea      lbW02E20C, a2
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a3
                    move.b   (a0)+, d0
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a4
                    move.w   #11, d7
lbC01317A:          move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $2E(a1)
                    move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $32(a1)
                    addq.w   #4, a3
                    addq.w   #4, a4
                    lea      $94(a1), a1
                    dbra     d7, lbC01317A
lbC01319E:          rts

PRINT_TIME:         tst.w    END_OF_STG
                    bgt      lbC0131DA
                    subq.w   #1, TIME_CNT
                    bpl      lbC0132A0
                    move.w   #49, TIME_CNT
                    move.w   #2, BLINK_CNT
                    btst     #5, CHEAT+1
                    bne      lbC0131DA
                    ifeq TRAINER
                    subq.w   #1, TIME
                    bmi      lbC0132A2
                    endif
lbC0131DA:          tst.b    BADDY_ON
                    bne      lbC0132A0
                    move.w   TIME, d0
                    cmp.w    OLDTIME, d0
                    beq      lbC0132A0
                    tst.w    END_OF_STG
                    bne      lbC01320A
                    lea      HEART_FX, a5
                    jsr      ADD_SFX
lbC01320A:          move.w   d0, OLDTIME
                    lea      DEC_NUMS+12, a1
                    and.l    #$FFFF, d0
                    bsr      CONVTODEC
                    lea      DECIMAL, a0
                    lea      TOP_PANEL, a1
                    move.b   (a0)+, d0
                    lea      lbW02E20C, a2
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a3
                    move.b   (a0)+, d0
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a4
                    move.w   #11, d7
lbC01324E:          move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $76(a1)
                    move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $7A(a1)
                    addq.w   #4, a3
                    addq.w   #4, a4
                    lea      $94(a1), a1
                    dbra     d7, lbC01324E
                    lea      TOP_PANEL, a1
                    move.b   (a0)+, d0
                    lea      lbW02E20C, a2
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a3
                    move.w   #11, d7
lbC01328E:          move.w   (a3)+, $82(a1)
                    move.w   (a3)+, $86(a1)
                    addq.w   #4, a3
                    lea      $94(a1), a1
                    dbra     d7, lbC01328E
lbC0132A0:          rts

lbC0132A2:          clr.w    TIME
                    tst.w    END_OF_STG
                    beq      lbC0132B4
                    rts

lbC0132B4:          subq.w   #1, LIVES                      ; ****
                    clr.w    ENERGY
                    move.w   START_TIME, TIME
                    st       ZOOL_DIES
                    cmp.l    #NOZONES, ZONE_TAB
                    beq      lbC0132F6
                    move.w   ZOOL_X, d0
                    sub.w    #$10, d0
                    move.w   ZOOL_Y, d1
                    clr.w    d2
                    jsr      NOTOK
lbC0132F6:          lea      SMART_FX, a5
                    jmp      ADD_SFX

                    rts

PRINT_NRG:          tst.b    REVIVE
                    beq      lbC01332E
                    addq.w   #1, ENERGY
                    cmp.w    #4, ENERGY
                    ble      lbC01332E
                    move.w   #4, ENERGY
                    sf       REVIVE
lbC01332E:          tst.b    ON_BONUS
                    beq      lbC013340
                    move.w   #0, d0
                    bra      lbC01338A

lbC013340:          tst.w    BLINK_CNT
                    bmi      lbC013384
                    subq.w   #1, BLINK_CNT
                    beq      lbC013384
                    eor.b    #$66, lbB048BB4+1
                    eor.b    #$AA, lbB048BB8+1
                    eor.b    #$EE, lbB048BBC+1
                    eor.b    #$66, lbB048BC4+1
                    eor.b    #$AA, lbB048BC8+1
                    eor.b    #$EE, lbB048BCC+1
lbC013384:          move.w   ENERGY, d0
lbC01338A:          cmp.w    OLDENERGY, d0
                    beq      lbC0133D6
                    move.w   d0, OLDENERGY
                    bmi      lbC013478
                    lea      lbW048DF6, a1
                    lea      lbL02EB0C, a0
                    cmp.w    #4, d0
                    bpl      lbC013462
                    cmp.w    #2, d0
                    bgt      lbC013434
                    beq      lbC013406
                    tst.w    d0
                    bne      lbC0133D8
                    moveq    #5, d7
lbC0133C6:          clr.w    $3A(a1)
                    clr.w    $3E(a1)
                    lea      $94(a1), a1
                    dbra     d7, lbC0133C6
lbC0133D6:          rts

lbC0133D8:          moveq    #5, d7
lbC0133DA:          move.w   (a0)+, $3A(a1)
                    move.w   (a0)+, $3E(a1)
                    addq.l   #4, a0
                    lea      $94(a1), a1
                    dbra     d7, lbC0133DA
                    moveq    #5, d7
                    lea      lbW048DF6, a1
lbC0133F4:          clr.w    $46(a1)
                    clr.w    $4A(a1)
                    lea      $94(a1), a1
                    dbra     d7, lbC0133F4
                    rts

lbC013406:          moveq    #5, d7
lbC013408:          move.w   (a0)+, $46(a1)
                    move.w   (a0)+, $4A(a1)
                    addq.l   #4, a0
                    lea      $94(a1), a1
                    dbra     d7, lbC013408
                    moveq    #5, d7
                    lea      lbW048DF6, a1
lbC013422:          clr.w    $52(a1)
                    clr.w    $56(a1)
                    lea      $94(a1), a1
                    dbra     d7, lbC013422
                    rts

lbC013434:          moveq    #5, d7
lbC013436:          move.w   (a0)+, $52(a1)
                    move.w   (a0)+, $56(a1)
                    addq.l   #4, a0
                    lea      $94(a1), a1
                    dbra     d7, lbC013436
                    moveq    #5, d7
                    lea      lbW048DF6, a1
lbC013450:          clr.w    $5E(a1)
                    clr.w    $62(a1)
                    lea      $94(a1), a1
                    dbra     d7, lbC013450
                    rts

lbC013462:          moveq    #5, d7
lbC013464:          move.w   (a0)+, $5E(a1)
                    move.w   (a0)+, $62(a1)
                    addq.l   #4, a0
                    lea      $94(a1), a1
                    dbra     d7, lbC013464
                    rts

lbC013478:          tst.w    END_OF_STG
                    beq      lbC013484
                    rts

lbC013484:          tst.w    END_OF_STG
                    bne      lbC0134D4
                    ifeq TRAINER
                    subq.w   #1, LIVES
                    endif
                    st       ZOOL_DIES
                    clr.w    ENERGY
                    cmp.l    #NOZONES, ZONE_TAB
                    beq      lbC0134C6
                    move.w   ZOOL_X, d0
                    sub.w    #$10, d0
                    move.w   ZOOL_Y, d1
                    clr.w    d2
                    jsr      NOTOK
lbC0134C6:          lea      SMART_FX, a5
                    jmp      ADD_SFX

lbC0134D4:          clr.w    ENERGY
                    rts

PRINT_SCORE:        tst.w    END_OF_STG
                    bgt      lbC0135F2
lbC0134E6:          move.l   SCORE, d0
                    cmp.l    OLDSCORE, d0
                    beq      lbC0135F0
                    move.l   d0, OLDSCORE
                    lea      DEC_NUMS, a1
                    bsr      CONVTODEC
                    lea      DECIMAL, a0
                    lea      TOP_PANEL, a1
                    move.b   (a0)+, d0
                    lea      lbW02E20C, a2
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a3
                    move.b   (a0)+, d0
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a4
                    move.w   #11, d7
lbC013534:          move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, 10(a1)
                    move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, 14(a1)
                    addq.w   #4, a3
                    addq.w   #4, a4
                    lea      $94(a1), a1
                    dbra     d7, lbC013534
                    lea      TOP_PANEL, a1
                    move.b   (a0)+, d0
                    lea      lbW02E20C, a2
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a3
                    move.b   (a0)+, d0
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a4
                    move.w   #11, d7
lbC013580:          move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $16(a1)
                    move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $1A(a1)
                    addq.w   #4, a3
                    addq.w   #4, a4
                    lea      $94(a1), a1
                    dbra     d7, lbC013580
                    lea      TOP_PANEL, a1
                    move.b   (a0)+, d0
                    lea      lbW02E20C, a2
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a3
                    move.b   (a0)+, d0
                    and.w    #$FF, d0
                    lsl.w    #7, d0
                    lea      0(a2,d0.w), a4
                    move.w   #11, d7
lbC0135CC:          move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $22(a1)
                    move.w   (a3)+, d0
                    move.w   (a4)+, d1
                    lsr.w    #8, d1
                    or.w     d1, d0
                    move.w   d0, $26(a1)
                    addq.w   #4, a3
                    addq.w   #4, a4
                    lea      $94(a1), a1
                    dbra     d7, lbC0135CC
lbC0135F0:          rts

lbC0135F2:          tst.w    TIME
                    beq      lbC0134E6
                    ifeq TRAINER
                    subq.w   #2, TIME
                    bpl      lbC01360C
                    clr.w    TIME
lbC01360C:          
                    endif
                    add.l    #30, SCORE
                    bra      lbC0134E6

CONVTODEC:          lea      DECIMAL, a0
                    clr.l    (a0)
                    clr.w    4(a0)
lbC013626:          moveq    #0, d1
lbC013628:          addq.b   #1, d1
                    sub.l    (a1), d0
                    bpl.s    lbC013628
                    subq.b   #1, d1
                    move.b   d1, (a0)+
                    add.l    (a1), d0
                    beq      lbC01363C
                    addq.l   #4, a1
                    bra.s    lbC013626

lbC01363C:          rts

BONUS_CS:           dcb.b    2, 0

GO_BONUS:           bsr      INIT_BONUS
                    jsr      RESET_EBAR
                    move.l   #DOCOL_BANDS, DOCOL_RTN
                    move.l   #BONUS_PAL, LEV_PAL
                    jsr      VBL
                    st       GAME_FADE
                    tst.b    AUDIO
                    ble      _INIT_FDON
                    jsr      PLAY_MUSIC
_INIT_FDON:         jsr      INIT_FDON
                    move.l   #COPPER_GAME, COP_LOC
_MAIN_BON:          bsr      MAIN_BON
                    cmp.b    #$CD, KEYREC
                    bne      lbC0136CA
_READ_JOY2:         jsr      READ_JOY
                    cmp.b    #$CD, KEYREC
                    beq.s    _READ_JOY2
_READ_JOY3:         jsr      READ_JOY
                    cmp.b    #$CD, KEYREC
                    bne.s    _READ_JOY3
_READ_JOY4:         jsr      READ_JOY
                    cmp.b    #$CD, KEYREC
                    beq.s    _READ_JOY4
lbC0136CA:          tst.w    TILE_CNT
                    beq      lbC0136E0
                    tst.w    BALLS_ON
                    beq      lbC0136E0
                    bra.s    _MAIN_BON

lbC0136E0:          sf       FADECOL_ON
                    sf       BONUS_CS
                    move.w   #$20, FADE_CNT
                    tst.b    MUSIC_ON
                    beq      _WAIT_FDOFF
                    st       MUSIC_ON
_WAIT_FDOFF:        bsr      WAIT_FDOFF
                    move.w   #$3420, USED_CBANK
                    rts

INIT_BONUS:         st       BONUS_CS
                    bsr      DOWNL_SPRS
                    jsr      INIT_SCRNS
                    bsr      DUMP_BTLS
                    move.l   A_BON_MAP, CURRENT_MAP
                    move.l   A_BON_REF, REF_MAP
                    bsr      SETUP_BSCR
                    clr.w    XSCROLL
                    clr.w    YSCROLL
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    clr.w    FASTER
                    bsr      COUNT_TLS
                    move.w   #$880, ZOON_X
                    move.w   #50, TIME
                    move.w   #49, TIME_CNT
                    clr.w    OLDTIME
                    move.w   #1, BALLS_ON
                    sf       DOUBLE_COP
                    move.l   #BONUS_BAND, COLBAND
                    clr.w    COLBAND_PTR
                    jsr      RESET_COP
                    move.w   #$1420, USED_CBANK
                    clr.w    HORZ_SCRO
                    move.w   #1, BON_START
                    lea      BALLS_TAB, a0
                    move.w   #3, d7
                    lea      RBALLS_TAB, a1
lbC0137C8:          move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    move.w   (a1)+, (a0)+
                    dbra     d7, lbC0137C8
                    lea      EYES_TAB, a1
                    move.w   #2, d7
lbC0137E0:          clr.l    (a1)+
                    clr.l    (a1)+
                    dbra     d7, lbC0137E0
                    lea      ICON_TAB, a1
                    move.w   #5, d7
lbC0137F2:          clr.l    (a1)+
                    clr.w    (a1)+
                    dbra     d7, lbC0137F2
                    rts

COUNT_TLS:          move.l   REF_MAP, a0
                    move.w   #15, d1
lbC013806:          move.w   #9, d0
lbC01380A:          cmp.b    #1, (a0)
                    ble      lbC013818
                    addq.w   #1, TILE_CNT
lbC013818:          addq.l   #2, a0
                    dbra     d0, lbC01380A
                    dbra     d1, lbC013806
                    rts

DOWNL_SPRS:         move.l   MENTB_AD, a0
                    move.w   #$4E20, d7
                    jsr      CLEAR_MEM
                    
                    move.l   MENTB_AD, a1
                    lea      BONUS_SPRITES_NAME, a1
                    jsr      LOADFILE

                    sf       SAVE_SPACE
                    st       MENTB_MASK
                    bsr      SPRITE_TAB
                    rts

DUMP_BTLS:          move.l   #BOT_CHIP, TILES
                    move.l   TILES, a0
                    lea      BONUS_TILES_NAME, a1
                    jsr      LOADFILE
                    rts

SETUP_BSCR:         jsr      INIT_SCRO
                    move.l   CURRENT_MAP, a0
                    addq.l   #8, a0
                    lea      lbL14794A, a1
                    move.w   #$10, TEMPW
_DUMP_ROW:          jsr      DUMP_ROW
                    lea      $BD6(a1), a1
                    subq.l   #2, a0
                    subq.w   #1, TEMPW
                    bne.s    _DUMP_ROW
                    move.l   A_BON_MAP, a0
                    addq.w   #8, a0
                    lea      lbL15455A, a1
                    move.w   #$10, TEMPW
_DUMP_ROW0:         jsr      DUMP_ROW
                    lea      $BD6(a1), a1
                    subq.l   #2, a0
                    subq.w   #1, TEMPW
                    bne.s    _DUMP_ROW0
                    move.l   A_BON_MAP, a0
                    addq.w   #8, a0
                    lea      lbL16116A, a1
                    move.w   #$10, TEMPW
_DUMP_ROW1:         jsr      DUMP_ROW
                    lea      $BD6(a1), a1
                    subq.l   #2, a0
                    subq.w   #1, TEMPW
                    bne.s    _DUMP_ROW1
                    rts

MAIN_BON:           jsr      READ_JOY
                    btst     #7, JOYPOS
                    beq      lbC013926
                    clr.w    BON_START
lbC013926:          btst     #7, CHEAT
                    beq      _ADD_EYE
                    cmp.b    #$93, KEYREC
                    bne      _ADD_EYE
                    tst.w    BALLS_ON
                    bne      _ADD_EYE
                    move.w   #1, BALLS_ON
                    move.w   #1, BALLS_TAB
                    move.w   #$FFFE, lbW019FC6
                    clr.w    FASTER
_ADD_EYE:           bsr      ADD_EYE
                    tst.w    BON_START
                    bne      _REMOVE_TLS
                    bsr      PRO_BSPEED
                    bsr      PRO_BALL
                    bsr      PRO_BAT
_REMOVE_TLS:        bsr      REMOVE_TLS
                    eor.b    #1, ANDYFRAME
                    jsr      SCROLL_BUFF
                    bsr      DRAW_BON
                    jsr      VBL
                    jsr      SCRO_NOW
                    jsr      DOCOL
                    rts

DRAW_BON:           jsr      UNDRAW
                    jsr      DRAW_PERMS
                    jsr      DRAW_BACKSP
                    bsr      DRAW_EYES
                    bsr      DRAW_ZOON
                    bsr      DRAW_BALL
                    jsr      DRAW_ARCHEX
                    bsr      DRAW_ICONS
                    bsr      PRINT_PANEL
                    rts

PRO_BALL:           lea      BALLS_TAB, a6
lbC0139DE:          tst.w    (a6)
                    beq      lbC013A28
                    move.w   2(a6), BALL_X
                    move.w   4(a6), BALL_Y
                    move.w   6(a6), BALL_XDIS
                    move.w   8(a6), BALL_YDIS
                    bsr      GOBALL
                    move.w   BALL_X, 2(a6)
                    move.w   BALL_Y, 4(a6)
                    move.w   BALL_XDIS, 6(a6)
                    move.w   BALL_YDIS, 8(a6)
lbC013A28:          lea      10(a6), a6
                    cmp.l    #BALLS_ON, a6
                    bne.s    lbC0139DE
                    rts

GOBALL:             move.w   BALL_XDIS, d0
                    move.w   BALL_YDIS, d1
                    tst.w    d1
                    bmi      lbC013A98
                    move.w   BALL_Y, d2
                    addq.w   #6, d2
                    sub.w    #$D4, d2
                    bmi      lbC013A98
                    cmp.w    #12, d2
                    bpl      lbC013A98
                    move.w   BALL_X, d2
                    move.w   ZOON_X, d3
                    lsr.w    #4, d3
                    sub.w    d3, d2
                    bmi      lbC013A98
                    cmp.w    #$30, d2
                    bpl      lbC013A98
                    lea      SNARL_FX, a5
                    jsr      ADD_SFX
                    neg.w    d1
                    lsr.w    #3, d2
                    lsl.w    #1, d2
                    lea      BOUNCE_TAB, a0
                    move.w   0(a0,d2.w), d0
lbC013A98:          tst.w    d0
                    bmi      lbC013ABC
                    cmp.w    #$13A, BALL_X
                    ble      lbC013AD6
                    neg.w    d0
                    lea      BRICK_FX, a5
                    jsr      ADD_SFX
                    bra      lbC013AD6

lbC013ABC:          cmp.w    #6, BALL_X
                    bpl      lbC013AD6
                    neg.w    d0
                    lea      BRICK_FX, a5
                    jsr      ADD_SFX
lbC013AD6:          tst.w    d1
                    bmi      lbC013B12
                    cmp.w    #$100, BALL_Y
                    ble      lbC013B2C
                    btst     #5, CHEAT
                    beq      lbC013AFA
                    neg.w    d1
                    bra      lbC013B2C

lbC013AFA:          lea      WIMP_FX, a5
                    jsr      ADD_SFX
                    subq.w   #1, BALLS_ON
                    clr.w    (a6)
                    bra      lbC013B2C

lbC013B12:          cmp.w    #6, BALL_Y
                    bpl      lbC013B2C
                    neg.w    d1
                    lea      BRICK_FX, a5
                    jsr      ADD_SFX
lbC013B2C:          move.w   BALL_X, d2
                    move.w   BALL_Y, d3
                    bsr      lbC013B66
                    move.w   BALL_X, d2
                    move.w   BALL_Y, d3
                    bsr      lbC013BAA
                    add.w    d0, BALL_X
                    add.w    d1, BALL_Y
                    move.w   d0, BALL_XDIS
                    move.w   d1, BALL_YDIS
                    rts

lbC013B66:          move.w   #$FFFF, TILE_HIT
                    tst.w    d0
                    bmi      lbC013B82
                    addq.w   #6, d2
                    cmp.w    #$140, d2
                    bpl      lbC013BA8
                    bra      _READ_TILE

lbC013B82:          subq.w   #6, d2
                    bmi      lbC013BA8
_READ_TILE:         bsr      READ_TILE
                    beq      lbC013BA8
                    lea      BRICK_FX, a5
                    jsr      ADD_SFX
                    move.w   d3, TILE_HIT
                    bsr      DEL_TILE
                    neg.w    d0
lbC013BA8:          rts

lbC013BAA:          tst.w    d1
                    bmi      lbC013BB6
                    addq.w   #6, d3
                    bra      _READ_TILE0

lbC013BB6:          subq.w   #6, d3
_READ_TILE0:        bsr      READ_TILE
                    beq      lbC013BDC
                    lea      BRICK_FX, a5
                    jsr      ADD_SFX
                    cmp.w    TILE_HIT, d3
                    beq      lbC013BDA
                    bsr      DEL_TILE
lbC013BDA:          neg.w    d1
lbC013BDC:          rts

REMOVE_TLS:         jsr      RANDOM
                    lea      DEL_TILES, a0
lbC013BEA:          tst.b    (a0)
                    beq      lbC013C88
                    move.w   2(a0), d0
                    move.w   4(a0), d1
                    tst.w    6(a0)
                    beq      _ADD_GONE
                    subq.w   #1, 6(a0)
                    move.w   6(a0), d7
                    lsr.w    #1, d7
                    beq      lbC013C96
                    neg.w    d7
                    add.w    #$1E, d7
                    add.w    BONUS_SP, d7
                    sub.w    BACKFX_SPRS, d7
                    jsr      ADD_BACKSP
                    bra      lbC013C88

_ADD_GONE:          bsr      ADD_GONE
                    lea      BREAK_FX, a5
                    jsr      ADD_SFX
                    subq.w   #1, TILES_LEFT
                    sf       (a0)
                    move.w   FILLTILE_SPR, d7
                    jsr      ADD_PERM
                    add.w    #$10, d0
                    jsr      ADD_PERM
                    clr.w    d2
                    move.l   a0, -(sp)
                    jsr      NOTOK
                    move.l   (sp)+, a0
                    move.b   SEED+2, d7
                    bclr     #7, d7
                    cmp.b    #$64, d7
                    bmi      lbC013C88
                    and.b    #3, d7
                    eor.b    d7, SEED+2
                    sub.w    #$10, d0
                    bsr      ADD_ICON
lbC013C88:          addq.l   #8, a0
                    cmp.l    #BOUNCE_TAB, a0
                    bne      lbC013BEA
                    rts

lbC013C96:          sf       (a0)
                    clr.w    6(a0)
                    rts

ADD_ICON:           lea      ICON_TAB, a6
lbC013CA4:          tst.b    (a6)
                    beq      lbC013CB6
                    addq.l   #6, a6
                    cmp.l    #LAST_ICON, a6
                    bne.s    lbC013CA4
                    rts

lbC013CB6:          move.w   d0, 2(a6)
                    move.w   d1, 4(a6)
                    move.b   d7, 1(a6)
                    st       (a6)
                    lea      COLPOW_FX, a5
                    jsr      ADD_SFX
                    rts

DRAW_ICONS:         lea      ICON_TAB, a6
lbC013CD8:          tst.b    (a6)
                    beq      lbC013D1C
                    move.w   2(a6), XCOORD
                    move.w   4(a6), YCOORD
                    addq.w   #1, 4(a6)
                    cmp.w    #$100, 4(a6)
                    bpl      lbC013D28
                    moveq    #0, d7
                    move.b   1(a6), d7
                    add.w    BONUS_SP, d7
                    add.w    #$1E, d7
                    move.w   d7, SPRITE
                    move.l   a6, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a6
lbC013D1C:          addq.l   #6, a6
                    cmp.l    #LAST_ICON, a6
                    bne.s    lbC013CD8
                    rts

lbC013D28:          sf       (a6)
                    lea      WIMP_FX, a5
                    jsr      ADD_SFX
                    bra.s    lbC013D1C

ADD_EYE:            lea      EYES_TAB, a6
lbC013D3E:          tst.b    (a6)
                    beq      lbC013D50
                    addq.l   #8, a6
                    cmp.l    #LAST_EYE, a6
                    bne.s    lbC013D3E
lbC013D4E:          rts

lbC013D50:          move.w   SEED, d0
                    move.w   d0, d1
                    lsr.w    #7, d1
                    and.w    #$1E, d0
                    cmp.w    #$14, d0
                    bpl.s    lbC013D4E
                    and.w    #15, d1
                    move.w   d1, d2
                    mulu     #$14, d1
                    add.w    d0, d1
                    move.l   REF_MAP, a0
                    cmp.b    #1, 0(a0,d1.w)
                    ble.s    lbC013D4E
                    st       (a6)
                    sf       1(a6)
                    sf       6(a6)
                    move.b   #2, 7(a6)
                    lsl.w    #4, d2
                    move.w   d2, 4(a6)
                    lsl.w    #4, d0
                    move.w   d0, 2(a6)
                    rts

DRAW_EYES:          lea      EYES_TAB, a6
lbC013DA2:          tst.b    (a6)
                    beq      lbC013E8A
                    bsr      lbC013E9C
                    cmp.b    #1, 1(a6)
                    bmi      lbC013DE0
                    beq      lbC013E16
                    cmp.b    #3, 1(a6)
                    bmi      lbC013E4C
                    subq.b   #1, 7(a6)
                    bpl      lbC013E5A
                    move.b   #3, 7(a6)
                    subq.b   #1, 6(a6)
                    bpl      lbC013E5A
                    sf       (a6)
                    bra      lbC013E8A

lbC013DE0:          subq.b   #1, 7(a6)
                    bpl      lbC013E5A
                    move.b   #3, 7(a6)
                    addq.b   #1, 6(a6)
                    cmp.b    #3, 6(a6)
                    bne      lbC013E5A
lbC013DFC:          move.b   #1, 1(a6)
                    move.b   SEED+1, d0
                    and.b    #$1F, d0
                    addq.b   #8, d0
                    move.b   d0, 7(a6)
                    bra      lbC013E5A

lbC013E16:          subq.b   #1, 7(a6)
                    bpl      lbC013E5A
                    btst     #2, SEED+3
                    beq      lbC013E3A
                    move.b   #3, 1(a6)
                    move.b   #1, 7(a6)
                    bra      lbC013E5A

lbC013E3A:          addq.b   #1, 1(a6)
                    move.b   #4, 7(a6)
                    addq.b   #1, 6(a6)
                    bra      lbC013E5A

lbC013E4C:          subq.b   #1, 7(a6)
                    bpl      lbC013E5A
                    subq.b   #1, 6(a6)
                    bra.s    lbC013DFC

lbC013E5A:          move.w   2(a6), XCOORD
                    move.w   4(a6), YCOORD
                    moveq    #0, d7
                    move.b   6(a6), d7
                    add.w    BONUS_SP, d7
                    add.w    #$22, d7
                    move.w   d7, SPRITE
                    move.l   a6, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a6
lbC013E8A:          addq.l   #8, a6
                    cmp.l    #LAST_EYE, a6
                    bne      lbC013DA2
                    bra      CLR_GONET

                    rts

lbC013E9C:          lea      LAST_EYE, a1
lbC013EA2:          tst.w    (a1)
                    bne      lbC013EB4
lbC013EA8:          addq.l   #6, a1
                    cmp.l    #LAST_GONE, a1
                    bne.s    lbC013EA2
                    rts

lbC013EB4:          move.w   2(a1), d0
                    move.w   4(a1), d1
                    cmp.w    2(a6), d0
                    bne.s    lbC013EA8
                    cmp.w    4(a6), d1
                    bne.s    lbC013EA8
                    sf       (a6)
                    rts

CLR_GONET:          lea      LAST_EYE, a4
                    clr.w    (a4)
                    clr.w    6(a4)
                    clr.w    12(a4)
                    clr.w    $12(a4)
                    clr.w    $18(a4)
                    clr.w    $1E(a4)
                    rts

ADD_GONE:           lea      LAST_EYE, a4
lbC013EF0:          tst.w    (a4)
                    beq      lbC013F02
                    addq.l   #6, a4
                    cmp.l    #LAST_GONE, a4
                    bne.s    lbC013EF0
                    rts

lbC013F02:          move.w   d0, 2(a4)
                    move.w   d1, 4(a4)
                    st       (a4)
                    rts

READ_TILE:          move.l   REF_MAP, a0
                    move.w   d2, TILEX
                    move.w   d3, TILEY
                    lsr.w    #4, d2
                    lsr.w    #4, d3
                    mulu     #$14, d3
                    add.w    d2, d3
                    lea      0(a0,d3.w), a0
                    move.b   (a0), d7
                    rts

DEL_TILE:           cmp.b    #1, d7
                    beq      lbC013FBA
                    lea      DEL_TILES, a1
lbC013F40:          tst.b    (a1)
                    beq      lbC013F52
                    addq.l   #8, a1
                    cmp.l    #BOUNCE_TAB, a1
                    bne.s    lbC013F40
                    rts

lbC013F52:          move.w   TILEX, 2(a1)
                    move.w   TILEY, 4(a1)
                    btst     #0, d2
                    beq      lbC013F72
                    sub.w    #$10, 2(a1)
                    subq.l   #1, a0
lbC013F72:          st       (a1)
                    and.w    #$FFF0, 2(a1)
                    and.w    #$FFF0, 4(a1)
                    add.l    #15, SCORE
                    subq.b   #1, d7
                    cmp.b    #1, d7
                    beq      lbC013FA2
                    move.b   d7, (a0)
                    move.b   d7, 1(a0)
                    move.w   #13, 6(a1)
                    rts

lbC013FA2:          st       (a1)
                    clr.w    6(a1)
                    clr.w    (a0)
                    add.l    #$1E, SCORE
                    subq.w   #1, TILE_CNT
lbC013FBA:          rts

PRO_BAT:            move.b   JOYPOS, d7
                    and.w    #12, d7
                    sf       ZOON_FACE
                    btst     #2, d7
                    beq      lbC014008
                    tst.w    ZOON_XDIS
                    ble      lbC013FE4
                    subq.w   #6, ZOON_XDIS
lbC013FE4:          subq.w   #6, ZOON_XDIS
                    st       ZOON_FACE
                    cmp.w    #$FFA8, ZOON_XDIS
                    bpl      lbC014042
                    move.w   #$FFA8, ZOON_XDIS
                    bra      lbC014042

lbC014008:          btst     #3, d7
                    beq      lbC014042
                    tst.w    ZOON_XDIS
                    bpl      lbC014020
                    addq.w   #6, ZOON_XDIS
lbC014020:          addq.w   #6, ZOON_XDIS
                    move.b   #1, ZOON_FACE
                    cmp.w    #$58, ZOON_XDIS
                    ble      lbC014042
                    move.w   #$58, ZOON_XDIS
lbC014042:          move.w   ZOON_XDIS, d0
                    tst.w    d7
                    bne      lbC01407C
                    tst.w    d0
                    beq      lbC01407C
                    bmi      lbC01406C
                    subq.w   #6, ZOON_XDIS
                    bpl      lbC01407C
                    clr.w    ZOON_XDIS
                    bra      lbC01407C

lbC01406C:          addq.w   #6, ZOON_XDIS
                    ble      lbC01407C
                    clr.w    ZOON_XDIS
lbC01407C:          move.w   ZOON_X, d1
                    add.w    d0, d1
                    bpl      lbC014090
                    clr.w    ZOON_XDIS
                    clr.w    d1
lbC014090:          cmp.w    #$1100, d1
                    ble      lbC0140A2
                    move.w   #$1100, d1
                    clr.w    ZOON_XDIS
lbC0140A2:          move.w   d1, ZOON_X
                    asr.w    #4, d1
                    lea      ICON_TAB, a6
lbC0140B0:          tst.b    (a6)
                    beq      lbC014190
                    move.w   2(a6), d0
                    move.w   4(a6), d2
                    sub.w    #$D0, d2
                    bmi      lbC014190
                    cmp.w    #$20, d2
                    bpl      lbC014190
                    add.w    #$1C, d0
                    sub.w    d1, d0
                    bmi      lbC014190
                    cmp.w    #$4C, d0
                    bpl      lbC014190
                    lea      BARK_FX, a5
                    jsr      ADD_SFX
                    sf       (a6)
                    move.b   1(a6), d7
                    cmp.b    #1, d7
                    bmi      lbC014150
                    beq      lbC014146
                    cmp.b    #3, d7
                    bmi      lbC014138
                    tst.w    BALL_YDIS
                    bmi      lbC01411C
                    move.w   #2, BALL_YDIS
                    bra      lbC014124

lbC01411C:          move.w   #$FFFE, BALL_YDIS
lbC014124:          move.w   #500, SLOW_TIME
                    move.w   #$FFFF, d7
                    bsr      SET_BALLSPD
                    bra      lbC014190

lbC014138:          add.l    #$64, SCORE
                    bra      lbC014190

lbC014146:          addq.w   #1, LIVES
                    bra      lbC014190

lbC014150:          cmp.w    #4, BALLS_ON
                    beq.s    lbC014146
                    lea      BALLS_TAB, a5
lbC014160:          tst.w    (a5)
                    bne      lbC014184
                    move.w   #1, (a5)
                    move.w   #$FFFE, 8(a5)
                    move.w   FASTER, d0
                    sub.w    d0, 8(a5)
                    addq.w   #1, BALLS_ON
                    bra      lbC014190

lbC014184:          lea      10(a5), a5
                    cmp.l    #BALLS_ON, a5
                    bne.s    lbC014160
lbC014190:          addq.w   #6, a6
                    cmp.l    #LAST_ICON, a6
                    bne      lbC0140B0
                    rts

PRO_BSPEED:         tst.w    TIME
                    bne      lbC0141DE
                    move.w   #50, TIME
                    move.w   #49, TIME_CNT
                    tst.w    SLOW_TIME
                    bne      lbC0141DE
                    cmp.w    #2, FASTER
                    beq      lbC0141DE
                    addq.w   #1, FASTER
                    move.w   FASTER, d7
                    bsr      SET_BALLSPD
lbC0141DE:          tst.w    SLOW_TIME
                    beq      lbC0141FC
                    subq.w   #1, SLOW_TIME
                    bne      lbC0141FC
                    move.w   FASTER, d7
                    bra      SET_BALLSPD

lbC0141FC:          rts

SET_BALLSPD:        addq.w   #2, d7
                    lea      BALLS_TAB, a4
lbC014206:          tst.w    (a4)
                    beq      lbC014224
                    tst.w    8(a4)
                    bmi      lbC01421C
                    move.w   d7, 8(a4)
                    bra      lbC014224

lbC01421C:          move.w   d7, 8(a4)
                    neg.w    8(a4)
lbC014224:          lea      10(a4), a4
                    cmp.l    #BALLS_ON, a4
                    bne.s    lbC014206
                    rts

DRAW_ZOON:          move.w   ZOON_SPR, d7
                    tst.b    ZOON_FACE
                    beq      lbC0142CE
                    bmi      lbC01428A
                    cmp.w    #9, d7
                    bmi      lbC01427C
                    cmp.w    #$10, d7
                    bgt      lbC01427C
                    subq.w   #1, ZOON_FRM
                    bpl      lbC01433A
                    move.w   #2, ZOON_FRM
                    addq.w   #1, d7
                    cmp.w    #$11, d7
                    bpl      lbC01427C
                    move.w   d7, ZOON_SPR
                    bra      lbC01433A

lbC01427C:          move.w   #9, d7
                    move.w   d7, ZOON_SPR
                    bra      lbC01433A

lbC01428A:          cmp.w    #1, d7
                    bmi      lbC0142C0
                    cmp.w    #8, d7
                    bgt      lbC0142C0
                    subq.w   #1, ZOON_FRM
                    bpl      lbC01433A
                    move.w   #2, ZOON_FRM
                    addq.w   #1, d7
                    cmp.w    #9, d7
                    bpl      lbC0142C0
                    move.w   d7, ZOON_SPR
                    bra      lbC01433A

lbC0142C0:          move.w   #1, d7
                    move.w   d7, ZOON_SPR
                    bra      lbC01433A

lbC0142CE:          cmp.w    #$11, d7
                    beq      lbC014304
                    bmi      lbC0142E4
                    subq.w   #1, ZOON_FRM
                    bpl      lbC01433A
lbC0142E4:          move.w   #$11, d7
                    move.w   d7, ZOON_SPR
                    jsr      RANDOM
                    and.w    #3, d0
                    addq.w   #8, d0
                    move.w   d0, ZOON_FRM
                    bra      lbC01433A

lbC014304:          subq.w   #1, ZOON_FRM
                    bpl      lbC01433A
                    move.w   #4, ZOON_FRM
                    btst     #2, SEED+3
                    beq      lbC014330
                    move.w   #$13, d7
                    move.w   d7, ZOON_SPR
                    bra      lbC01433A

lbC014330:          move.w   #$12, d7
                    move.w   d7, ZOON_SPR
lbC01433A:          move.w   ZOON_X, d0
                    asr.w    #4, d0
                    move.w   d0, XCOORD
                    move.w   #$B0, YCOORD
                    add.w    BONUS_SP, d7
                    move.w   d7, SPRITE
                    jsr      DUMPSPRITE
                    move.w   ZOON_SPR, d7
                    cmp.w    #8, d7
                    ble      lbC0143A6
                    cmp.w    #$16, ZOON_SPKSP
                    bpl      lbC01439A
                    subq.w   #1, ZOON_SPK
                    bpl      lbC0143D8
                    move.w   #1, ZOON_SPK
                    eor.w    #1, ZOON_SPKSP
                    bra      lbC0143D8

lbC01439A:          move.w   #$14, ZOON_SPKSP
                    bra      lbC0143D8

lbC0143A6:          cmp.w    #$16, ZOON_SPKSP
                    bmi      lbC0143D0
                    subq.w   #1, ZOON_SPK
                    bpl      lbC0143D8
                    move.w   #1, ZOON_SPK
                    eor.w    #1, ZOON_SPKSP
                    bra      lbC0143D8

lbC0143D0:          move.w   #$16, ZOON_SPKSP
lbC0143D8:          move.w   ZOON_SPKSP, d7
                    add.w    BONUS_SP, d7
                    move.w   d7, SPRITE
                    jmp      DUMPSPRITE

DRAW_BALL:          lea      BALLS_TAB, a6
lbC0143F6:          tst.w    (a6)
                    beq      lbC014430
                    move.w   2(a6), d0
                    move.w   4(a6), d1
                    subq.w   #6, d0
                    subq.w   #6, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.w   BALL_SPR, d7
                    add.w    BONUS_SP, d7
                    move.w   d7, SPRITE
                    move.l   a6, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a6
lbC014430:          lea      10(a6), a6
                    cmp.l    #BALLS_ON, a6
                    bne.s    lbC0143F6
                    rts

RESTART1:           lea      RESTART_TAB, a0
                    move.w   STARTX, (a0)+
                    move.w   STARTY, (a0)+
                    move.w   ZOOL_SCRX, (a0)+
                    move.w   ZOOL_SCRY, (a0)+
                    move.w   START_TIME, (a0)+
                    move.b   ZOOL_FACE, (a0)+
                    rts

REMEMBER:           move.l   a0, -(sp)
                    move.w   d0, -(sp)
                    move.w   d1, -(sp)
                    lea      RESTART_TAB, a0
                    move.w   XPOS, d0
                    move.w   MAP_LINE, d1
                    lsr.w    #4, d0
                    lsr.w    #4, d1
                    move.w   d0, (a0)+
                    move.w   d1, (a0)+
                    move.w   ZOOL_SCRX, (a0)+
                    move.w   ZOOL_SCRY, (a0)+
                    move.w   START_TIME, (a0)+
                    move.w   (sp)+, d1
                    move.w   (sp)+, d0
                    move.l   (sp)+, a0
                    rts

REMEMBER2:          move.l   a0, -(sp)
                    move.w   d0, -(sp)
                    move.w   d1, -(sp)
                    lea      RESTART_TAB, a0
                    move.w   XPOS, d0
                    move.w   MAP_LINE, d1
                    lsr.w    #4, d0
                    lsr.w    #4, d1
                    subq.w   #1, d1
                    move.w   d0, (a0)+
                    move.w   d1, (a0)+
                    move.w   ZOOL_SCRX, (a0)+
                    move.w   ZOOL_SCRY, (a0)
                    sub.w    #$10, (a0)
                    and.w    #$FFF0, (a0)+
                    move.w   START_TIME, (a0)+
                    move.w   (sp)+, d1
                    move.w   (sp)+, d0
                    move.l   (sp)+, a0
                    rts

SET_SPR_ADS:        move.b   LEVEL_NUM, d7
                    ext.w    d7
                    lsl.w    #4, d7
                    lea      SPR_ADS, a0
                    move.l   0(a0,d7.w), d0
                    move.l   d0, SPRITE_AD
                    move.l   4(a0,d7.w), MASK_AD
                    add.l    d0, MASK_AD
                    add.l    d0, MUSIC_PTR
                    move.l   12(a0,d7.w), MENTB_AD
                    add.l    d0, MENTB_AD
                    move.l   #MASKHERE, MASK_AD
                    move.l   #MUSICHERE, MUSIC_PTR
                    move.l   #MUSICHERE, MUSIC_AD
                    rts

SPR_ADS:            dc.l     SPSTART, $1F3DA
                    dc.l     lbL130400, $18198
                    dc.l     SPSTART, $1FD62
                    dc.l     lbL138994, $19E30
                    dc.l     SPSTART, $1E4CA
                    dc.l     lbL12EF48, $18E30
                    dc.l     SPSTART, $22616
                    dc.l     lbL130836, $18480
                    dc.l     SPSTART, $1F538
                    dc.l     lbL1317E8, $18D78
                    dc.l     SPSTART, $253FC
                    dc.l     lbL138994, $17250, 0, 0, 0, 0
                    dc.l     SPSTART, $1F3DA
                    dc.l     lbL130400, $18198

GET_ZZORZL:         tst.b    PLAYER_ON
                    beq      lbC01461E
                    tst.b    PLAY2_CHAR
                    beq      lbC014604
                    tst.b    ITS_ZOOL
                    beq      lbC01465C
                    sf       ITS_ZOOL
                    st       PSPR_CHANGE
                    bra      lbC01465C

lbC014604:          tst.b    ITS_ZOOL
                    bne      lbC01465C
                    st       ITS_ZOOL
                    st       PSPR_CHANGE
                    bra      lbC01465C

lbC01461E:          tst.b    PLAY1_CHAR
                    beq      lbC014642
                    tst.b    ITS_ZOOL
                    beq      lbC01465C
                    sf       ITS_ZOOL
                    st       PSPR_CHANGE
                    bra      lbC01465C

lbC014642:          tst.b    ITS_ZOOL
                    bne      lbC01465C
                    st       ITS_ZOOL
                    st       PSPR_CHANGE
                    bra      lbC01465C

lbC01465C:          rts

GET_DISK:           tst.b    DISK_IN
                    beq      lbC014680
                    tst.b    LEVEL_NUM
                    beq      GET_DISK1
                    cmp.b    #7, LEVEL_NUM
                    beq      GET_DISK1
                    rts

lbC014680:          cmp.b    #1, LEVEL_NUM
                    beq      lbC0146A4
                    cmp.b    #7, LEVEL_NUM
                    beq      lbC0146A2
                    tst.b    LEVEL_NUM
                    bne      GET_DISK2
lbC0146A2:          rts

lbC0146A4:          tst.b    ON_BONUS
                    beq      GET_DISK2
                    rts

GET_DISK1:          sf       DISK_IN
                    rts

GET_DISK2:          st       DISK_IN
                    rts

LOAD_DSCRN:         jsr      INIT_SCRNS
                    jsr      INIT_SCRO
                    bsr      CLEAR_ALLFX
                    move.l   PHYSADR, a0
                    jsr      CLR_SCRN
                    move.l   LOGADR, a0
                    jsr      CLR_SCRN
                    lea      BUFF3_START, a0
                    jsr      CLR_SCRN
                    
                    bsr      SET_CSCRO
                    move.l   #COPPER2, COP_LOC
                    jsr      VBL
                    move.w   #$8080, $DFF096
                    move.l   #STAGE_PAL, a0
                    jsr      NEWPALET
                    rts

END_SEQ:            st       JUST_ENDED
                    jsr      CLRPALET
                    move.b   #7, LEVEL_NUM
                    jsr      GET_DISK
                    jsr      INIT_SCRNS
                    jsr      INIT_SCRO
                    move.l   #INTRO_SPSTART, SPRITE_AD
                    move.l   #BOT_CHIP, MASK_AD
                    add.l    #$117BA, MASK_AD
                    add.l    #$3034, MASK_AD
                    move.l   MASK_AD, a0
                    add.l    #$1173A, a0
                    move.l   a0, MUSIC_PTR
                    move.l   a0, MUSIC_AD
                    move.l   #BLACK_BAND, COLBAND
                    clr.w    COLBAND_PTR
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    bsr      CLEAR_ALLFX
                    
                    move.l   SPRITE_AD, a0
                    lea      ENDSPR_NAME, a1
                    jsr      LOADFILE
                    
                    move.l   SPRITE_AD, a0
                    add.w    #12336, a0
                    lea      HITEXT_NAME, a1
                    jsr      LOADFILE
                    
                    move.l   SPRITE_AD, a0
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB

                    move.l   MUSIC_PTR, a0
                    lea      INTMUS_NAME, a1
                    jsr      LOADFILE

                    jsr      CLRPALET
                    move.l   #DOCOL_BAND3, DOCOL_RTN
                    move.l   #COPPER2, COP_LOC
                    move.l   #$FFFFFFFE, COL_SCROLL2
                    bsr      CLEAR_UNDS
                    move.w   #$8080, $DFF096

                    move.l   PHYSADR, a0
                    lea      ENDSCR_NAME, a1
                    jsr      LOADFILE

                    move.l   #BUFF3_START, a0
                    lea      ENDSCR_NAME, a1
                    jsr      LOADFILE

                    move.l   PHYSADR, a0
                    move.l   LOGADR, a1
                    jsr      COPY_SCRN
                    bsr      SETSCREEN
                    move.l   #END_PAL, LEV_PAL
                    tst.b    MUSIC_ON
                    bne      lbC014B3A
                    jsr      PLAY_MUSIC
lbC014B3A:          move.w   #$8080, $DFF096
                    jsr      VBL
                    jsr      INIT_FADE
                    clr.w    PAGE
                    move.l   #TEXT1, TEXT_PTR
                    move.w   #0, MARKER_X
                    move.w   #4, MARKER_Y
                    clr.w    WATER_SPR
                    move.b   #1, WATER_AN
                    move.w   #$64, TEMPW
_SCROLL_BUFF:       jsr      SCROLL_BUFF
                    move.l   LOGADR, BUFF_PTR
                    move.l   LOGADR, SCROLL
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    jsr      UNDRAW
                    bsr      DRAW_WATER
                    bsr      FLIP
                    clr.w    SWITCH_CNT
lbC014BBE:          cmp.w    #2, SWITCH_CNT
                    bne.s    lbC014BBE
                    subq.w   #1, TEMPW
                    bne.s    _SCROLL_BUFF
_SCROLL_BUFF0:      jsr      SCROLL_BUFF
                    move.l   LOGADR, BUFF_PTR
                    move.l   LOGADR, SCROLL
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    jsr      UNDRAW
                    bsr      DRAW_TXT
                    bsr      FLIP
                    clr.w    SWITCH_CNT
lbC014C0A:          cmp.w    #2, SWITCH_CNT
                    bne.s    lbC014C0A
                    jsr      READ_JOY
                    btst     #7, JOYPOS
                    beq.s    _SCROLL_BUFF0
                    move.w   #1, MUSIC_VOL
                    st       MUSIC_ON
                    jsr      INS_HISCORE
                    tst.b    INSERT_HSCR
                    beq      lbC014CE0
                    move.l   #NULLCOP, COP_LOC
                    move.l   #$FFFFFFFE, WRITE_COP
                    jsr      CLRPALET
                    jsr      LOAD_INTRO
                    move.l   #COPPER2, COP_LOC
                    move.l   #$FFFFFFFE, COL_SCROLL2
                    jsr      CLRPALET
                    move.l   #NULLCOP, COP_LOC
                    jsr      INIT_SCRO
                    jsr      SCRO_NOW
                    jsr      DOCOL
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    clr.w    OLD_YSCR
                    clr.w    OLD_XSCR
                    jsr      HISCORE_SCR
                    jsr      CLR_ALLSCR
                    jsr      INIT_SCRO
                    jsr      INIT_SCRNS
                    clr.w    XREM
                    clr.w    YSCRO
                    jmp      STOPGAME

lbC014CE0:          move.l   #NULLCOP, COP_LOC
                    move.l   #$fffffffe, WRITE_COP
                    jsr      CLRPALET
                    jsr      LOAD_INTRO
                    move.l   #COPPER2, COP_LOC
                    move.l   #$fffffffe, COL_SCROLL2
                    jsr      CLRPALET
                    move.l   #NULLCOP, COP_LOC
                    jsr      CLR_ALLSCR
                    jsr      INIT_SCRO
                    jsr      INIT_SCRNS
                    clr.w    XREM
                    clr.w    YSCRO
                    jmp      STOPGAME

DRAW_WATER:         st       P_DRAW
                    move.w   WATER_SPR, SPRITE
                    move.w   #$58, XCOORD
                    move.w   #$90, YCOORD
                    jsr      DUMPSPRITE
                    add.w    #$40, XCOORD
                    addq.w   #3, SPRITE
                    jsr      DUMPSPRITE
                    sf       P_DRAW
                    subq.b   #1, WATER_AN
                    bpl      lbC014DB8
                    move.b   #1, WATER_AN
                    addq.w   #1, WATER_SPR
                    cmp.w    #3, WATER_SPR
                    bne      lbC014DB8
                    clr.w    WATER_SPR
lbC014DB8:          rts

DRAW_TXT:           move.l   TEXT_PTR, a5
                    move.w   MARKER_X, d0
                    bmi      lbC014E92
                    move.w   MARKER_Y, d1
                    lsl.w    #3, d0
                    lsl.w    #3, d1
                    move.w   d0, XCOORD
                    move.w   d1, YCOORD
                    move.b   (a5)+, d5
                    beq      lbC014E94
                    bmi      lbC014EAA
                    cmp.b    #$3F, d5
                    bne      lbC014DFA
                    move.w   #13, d5
                    bra      lbC014E44

lbC014DFA:          cmp.b    #$2D, d5
                    bne      lbC014E0A
                    move.w   #11, d5
                    bra      lbC014E44

lbC014E0A:          cmp.b    #$21, d5
                    bne      lbC014E1A
                    move.w   #12, d5
                    bra      lbC014E44

lbC014E1A:          cmp.b    #$20, d5
                    bne      lbC014E2A
                    move.w   #$10, d5
                    bra      lbC014E44

lbC014E2A:          cmp.b    #$2E, d5
                    bne      lbC014E3A
                    move.w   #10, d5
                    bra      lbC014E44

lbC014E3A:          sub.b    #$41, d5
                    add.b    #$11, d5
                    ext.w    d5
lbC014E44:          addq.w   #6, d5
                    move.w   d5, SPRITE
                    st       P_DRAW
                    jsr      DUMPSPRITE
                    move.l   PHYSADR, BUFF_PTR
                    move.l   PHYSADR, SCROLL
                    jsr      DUMPSPRITE
                    move.l   LOGADR, BUFF_PTR
                    move.l   LOGADR, SCROLL
                    addq.l   #1, TEXT_PTR
                    addq.w   #1, MARKER_X
lbC014E92:          rts

lbC014E94:          move.w   #0, MARKER_X
                    addq.w   #3, MARKER_Y
                    addq.l   #1, TEXT_PTR
                    rts

lbC014EAA:          move.w   #2, MARKER_Y
                    clr.w    MARKER_X
                    move.w   #$190, d7
                    jsr      WAIT_SECS
                    addq.w   #1, PAGE
                    cmp.w    #2, PAGE
                    bmi      lbC014EFA
                    beq      lbC014EE2
                    move.w   #$FFFF, MARKER_X
                    rts

lbC014EE2:          move.w   #14, MARKER_Y
                    move.l   #TEXT3, TEXT_PTR
                    bsr      CLR_TEXT
                    rts

lbC014EFA:          move.l   #TEXT2, TEXT_PTR
                    bsr      CLR_TEXT
                    rts

CLR_TEXT:           lea      BUFF3_START, a0
                    move.l   LOGADR, a1
                    jsr      COPY_SCRN
                    jsr      VBL
                    lea      BUFF3_START, a0
                    move.l   PHYSADR, a1
                    jsr      COPY_SCRN
                    rts

TEXT1:              dc.b     ' WELL DONE NINJA! MENTAL BLOCK HAS  ', 0
                    dc.b     '    BEEN VANQUISHED AND THE NTH     ', 0
                    dc.b     '  DIMENSION CAN LIVE IN PEACE ONCE  ', 0
                    dc.b     'MORE. YOU ARE TRULY A WARRIOR OF THE', 0
                    dc.b     '  HIGHEST RANK. TAKE A WELL EARNED  ', 0
                    dc.b     ' REST BUT BE CAREFUL! KROOL IS ALL  ', 0
                    dc.b     '  POWERFUL. I FEAR WE WILL NEED TO  ', 0
                    dc.b     'CALL ON YOUR SERVICES AGAIN SOMEDAY!', 0
                    dc.b     $FF
                    even
TEXT2:              dc.b     '               CREDITS              ', 0
                    dc.b     0
                    dc.b     'ANDY FINDLAY.............PROGRAMMING', 0
                    dc.b     'ED CAMPBELL.................GRAPHICS', 0
                    dc.b     'PAT PHELAN..............MUSIC AND FX', 0
                    dc.b     'NEIL BIGGIN....................MUSIC', 0
                    dc.b     'PETE COOK...................PRODUCER', 0
                    dc.b     'GRAEME ING............CD DEVELOPMENT', 0
                    dc.b     'MARK GLOSSOP..........CD DEVELOPMENT', 0
                    dc.b     'SYD FRANKLIN...........3D ANIMATIONS', 0
                    dc.b     $FF
                    even
TEXT3:              dc.b     '               THE END              ', 0
                    dc.b     $FF
                    even

NEXT_STAGE:         move.w   #$A0, $DFF096
                    move.l   #LAST_ICON, $DFF120
                    move.l   #LAST_ICON, $DFF124
                    move.l   #LAST_ICON, $DFF128
                    move.l   #LAST_ICON, $DFF12C
                    move.l   #LAST_ICON, $DFF130
                    move.l   #LAST_ICON, $DFF134
                    move.l   #LAST_ICON, $DFF138
                    move.l   #LAST_ICON, $DFF13C
                    sf       GAME_FADE
                    st       SCROLL_OFF
                    clr.w    XSCROLL
                    clr.w    YSCROLL
                    cmp.b    #6, LEVEL_NUM
                    bne      _GET_DISK0
                    bsr      END_SEQ

_GET_DISK0:         bsr      GET_DISK
                    jsr      CLR_ALLSCR
                    bsr      GET_ZZORZL
                    jsr      INIT_SCRNS
                    jsr      INIT_SCRO
                    bsr      CLEAR_ALLFX
                    move.w   #$74, LEVNAME_X
                    
                    move.l   LOGADR, a0
                    lea      $1200(a0), a0
                    
                    tst.b    ITS_ZOOL
                    beq      lbC0152E8
                    lea      ZOOL_STAGE_NAME, a1
                    jsr      LOADFILE
                    
                    bra      lbC015308

lbC0152E8:          lea      ZOOZ_STAGE_NAME, a1
                    jsr      LOADFILE

lbC015308:          move.b   #1, ANDYFRAME
                    move.l   SPRITE_AD, a5
                    lea      BOT_CHIP, a6
                    move.l   #$3070, d7
                    jsr      MOVE_MEM
                    move.l   SPRITE_AD, a0
                    move.w   #$3070, d7
                    bsr      CLEAR_MEM
                    lea      STGMUS_NAME, a1
                    lea      lbL13B210, a0
                    jsr      LOADFILE
                    move.l   #lbL13B210, MUSIC_AD
                    ;jsr      INIT_AUDIO
                    
                    lea      STGSPR_NAME1, a1
                    move.l   SPRITE_AD, a0
                    jsr      LOADFILE
                    
                    lea      BON2_NAME1, a1
                    move.l   SPRITE_AD, a0
                    add.w    #12384, a0
                    jsr      LOADFILE
                    
                    move.l   LOGADR, a0
                    lea      BUFF3_START, a1
                    bsr      COPY_SCRN
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    bsr      SETSCREEN
                    bsr      SET_CSCRO
                    move.l   #COPPER2, COP_LOC
                    clr.w    SWITCH_CNT
                    jsr      SYNCRO
                    jsr      CLRPALET
                    move.w   #$32, d7
                    jsr      WAIT_SECS
                    move.l   LOGADR, a0
                    move.l   PHYSADR, a1
                    bsr      COPY_SCRN
                    clr.w    SWITCH_CNT
                    jsr      SYNCRO
                    move.w   #$8080, $DFF096
                    move.l   #STAGE_PAL, LEV_PAL
                    jsr      INIT_FADE
                    jsr      PLAY_MUSIC
                    move.w   #-46, LEVNAME_Y
                    tst.b    ON_BONUS
                    beq      lbC0154C8
                    move.w   #$18, LEVNAME_S
                    bra      lbC0154EE

lbC0154C8:          moveq    #0, d7
                    move.b   LEVEL_NUM, d7
                    cmp.b    #7, d7
                    bne      lbC0154E4
                    move.w   #$16, LEVNAME_S
                    bra      lbC0154EE

lbC0154E4:          lsl.w    #1, d7
                    addq.w   #3, d7
                    move.w   d7, LEVNAME_S
lbC0154EE:          clr.w    BLINK_WAIT
                    move.w   #$14, FOOT_TAP
                    move.w   #$14, ZOONB_WAIT
                    move.w   #0, ZOONB_DRAWN
HERE:               jsr      SCROLL_BUFF
                    move.l   LOGADR, BUFF_PTR
                    jsr      UNDRAW
                    bsr      DRAW_ZEYES
                    bsr      DRAW_ZTAP
                    bsr      DRAW_TEXT
                    bsr      DRAW_ZONBS
                    bsr      FLIP
                    jsr      READ_JOY
                    btst     #7, JOYPOS
                    beq.s    HERE
                    cmp.w    #$60, LEVNAME_Y
                    bne.s    HERE
                    move.l   SPRITE_AD, a6
                    lea      BOT_CHIP, a5
                    move.l   #$3070, d7
                    jsr      MOVE_MEM
                    move.l   #NULLCOP, COP_LOC
                    jsr      CLRPALET
                    move.l   #$FFFFFFFE, WRITE_COP
                    bsr      CLEAR_ALLFX
                    bsr      GET_READY
                    tst.b    ON_BONUS
                    beq      _SET_SPR_ADS
                    st       MUSIC_ON
                    tst.b    STAGE_NUM
                    bne      _LOAD_LSPRS
                    subq.b   #1, LEVEL_NUM
                    bpl      _LOAD_LSPRS0
                    move.b   #7, LEVEL_NUM
_LOAD_LSPRS0:       jsr      LOAD_LSPRS
                    bsr      LOAD_AUDIO
                    addq.b   #1, LEVEL_NUM
                    cmp.b    #8, LEVEL_NUM
                    bne      lbC0155E6
                    sf       LEVEL_NUM
                    bra      lbC0155E6

_LOAD_LSPRS:        jsr      LOAD_LSPRS
                    bsr      LOAD_AUDIO
lbC0155E6:          tst.b    AUDIO
                    bne      lbC0155FA
                    move.w   #50, d7
                    jsr      WAIT_SECS
lbC0155FA:          sf       ON_BONUS
                    move.l   #$FFFFFFFE, WRITE_COP
                    jsr      CLRPALET
                    move.l   #NULLCOP, COP_LOC
                    jsr      CLRPALET
                    clr.w    ZOONB_GOT
                    jsr      GO_BONUS
                    jsr      CLRPALET
                    move.l   #NULLCOP, COP_LOC
                    tst.b    PLAYERS
                    beq      NEXT_STAGE
                    tst.b    STAGE_NUM
                    bne      _INIT_LEVEL
                    move.w   #$FFFF, END_OF_STG
                    move.b   #2, STAGE_NUM
                    subq.b   #1, LEVEL_NUM
                    bpl      lbC015672
                    move.b   #7, LEVEL_NUM
lbC015672:          addq.l   #4, sp
                    jmp      STORE_GUY

_INIT_LEVEL:        jsr      INIT_LEVEL
                    jsr      INIT_LDATA
                    jsr      INIT_SCRO
                    jsr      SET_PERC
                    addq.l   #4, sp
                    clr.w    END_OF_STG
                    jmp      NEW_PLAYER

_SET_SPR_ADS:       bsr      SET_SPR_ADS
                    bsr      SET_MBPNTRS
                    bsr      LOAD_TILES
                    bsr      LOAD_PSPRS
                    bsr      LOAD_MBDATA
                    bsr      LOAD_MAPS
                    bsr      LOAD_ENEMY
                    jsr      REP_CALLS
                    bsr      LOAD_LSPRS
                    sf       MUSIC_ON
                    jsr      LOAD_AUDIO
                    move.l   #$FFFFFFFE, WRITE_COP
                    move.l   #NULLCOP, COP_LOC
                    jsr      CLRPALET
                    rts

LOAD_AUDIO:         sf       MUSIC_ON
                    lea      LEVMUS_TAB, a0
                    move.b   LEVEL_NUM, d7
                    ext.w    d7
                    lsl.w    #2, d7
                    move.l   (a0,d7.w), a1
                    move.l   #MUSICHERE, a0
                    move.l   a0, MUSIC_AD
                    jsr      LOADFILE
 
                    move.l   #BUFF2_START, DISK_BUFFER_ADDR
                    lea      PSAMS_NAME1, a1
                    lea      PSAMS, a0
                    move.b   LEVEL_NUM, d7
                    beq      _START_DISK
                    cmp.b    #2, d7
                    bmi      _START_DISK0
                    beq      _START_DISK1
                    cmp.b    #4, d7
                    bmi      _START_DISK2
                    beq      _START_DISK3
                    rts

_START_DISK3:       move.l   #BUFF2_START, DISK_BUFFER_ADDR
                    lea      END_PSAMS, a0
                    lea      ICEFX_NAME, a1
                    jsr      LOADFILE
                    move.l   #BUFF2_START, DISK_BUFFER_ADDR
                    lea      SNOW_FXAD, a0
                    lea      SNOFX_NAME, a1
                    jsr      LOADFILE
                    rts

_START_DISK2:       move.l   #BUFF2_START, DISK_BUFFER_ADDR
                    lea      HISS_FXAD, a0
                    lea      HISFX_NAME, a1
                    jsr      LOADFILE
                    move.l   #BUFF2_START, DISK_BUFFER_ADDR
                    lea      END_PSAMS, a0
                    lea      LAVFX_NAME, a1
                    jsr      LOADFILE
                    rts

_START_DISK1:       move.l   #BUFF2_START, DISK_BUFFER_ADDR
                    lea      HISS_FXAD, a0
                    lea      PLTFX_NAME, a1
                    jsr      LOADFILE
                    move.l   #BUFF2_START, DISK_BUFFER_ADDR
                    lea      END_PSAMS, a0
                    lea      LAVFX_NAME, a1
                    jsr      LOADFILE
                    rts

_START_DISK0:       move.l   #BUFF2_START, DISK_BUFFER_ADDR
                    lea      END_PSAMS, a0
                    lea      SPKFX_NAME, a1
                    jsr      LOADFILE
                    rts

_START_DISK:        move.l   #BUFF2_START, DISK_BUFFER_ADDR
                    lea      END_PSAMS, a0
                    lea      TWEFX_NAME, a1
                    jsr      LOADFILE
                    rts

LEVMUS_TAB:         dc.l     BIRMUS_NAME
                    dc.l     BULMUS_NAME
                    dc.l     TOOMUS_NAME
                    dc.l     BULMUS_NAME
                    dc.l     SODMUS_NAME
                    dc.l     BULMUS_NAME
                    dc.l     0
                    dc.l     SODMUS_NAME

GET_READY:          move.l   #lbL148548, a0
                    tst.b    PLAYER_ON
                    beq      lbC0159BC
                    lea      READY2_NAME1, a1
                    bra      _LOADFILE

lbC0159BC:          lea      READY_NAME1, a1
_LOADFILE:          jsr      LOADFILE
                    lea      BUFF1_START, a2
                    move.l   a2, PHYSADR
                    bsr      SETSCREEN
                    move.l   PHYSADR, BUFF_PTR
                    jsr      SCRO_NOW
                    clr.w    YSCRO
                    
                    bsr      SET_CSCRO
                    move.l   #COPPER2, COP_LOC
                    jsr      VBL
                    move.w   #$8080, $DFF096
                    move.l   #STAGE_PAL, a0
                    jsr      NEWPALET
                    rts

LOAD_MAPS:          lea      LEV_MAPS, a0
                    lea      LEVMAPS_TAB, a5
                    move.b   LEVEL_NUM, d7
                    ext.w    d7
                    lsl.w    #2, d7
                    move.l   (a5,d7.w), a1
                    jsr      LOADFILE
                    rts

LOAD_TILES:         lea      BOT_CHIP, a0
                    lea      LEVTLS_TAB, a5
                    move.b   LEVEL_NUM, d7
                    ext.w    d7
                    lsl.W    #2, d7
                    move.l   (a5,d7.w), a1
                    jsr      LOADFILE
                    rts

LOAD_LSPRS:         move.l   SPRITE_AD, a0
                    add.l    #94800, a0
                    lea      LEVSPRS_TAB, a5
                    move.b   LEVEL_NUM, d7
                    ext.w    d7
                    lsl.W    #2, d7
                    move.l   (a5,d7.w), a1
                    jsr      LOADFILE
                    rts

LEVSPRS_TAB:        dc.l     BIRSPR_NAME
                    dc.l     BULSPR_NAME
                    dc.l     TOOSPR_NAME
                    dc.l     SNASPR_NAME
                    dc.l     SODSPR_NAME
                    dc.l     LASSPR_NAME
                    dc.l     0
                    dc.l     PAPSPR_NAME

LEVTLS_TAB:         dc.l     BIRTLS_NAME
                    dc.l     BULTLS_NAME
                    dc.l     TOOTLS_NAME
                    dc.l     SNATLS_NAME
                    dc.l     SODTLS_NAME
                    dc.l     LASTLS_NAME
                    dc.l     0
                    dc.l     PAPTLS_NAME

LOAD_PSPRS:         move.l   SPRITE_AD, a0
                    tst.b    ITS_ZOOL
                    bne      lbC015C4E
                    lea      PERM2_NAME1, a1
                    bra      _LOADFILE0

lbC015C4E:          lea      PERM_NAME1, a1
_LOADFILE0:         jsr      LOADFILE
                    rts

LEVMAPS_TAB:        dc.l     BIRMAP_NAME
                    dc.l     BULMAP_NAME
                    dc.l     TOOMAP_NAME
                    dc.l     SNAMAP_NAME
                    dc.l     SODMAP_NAME
                    dc.l     LASMAP_NAME
                    dc.l     0
                    dc.l     PAPMAP_NAME

LOAD_ENEMY:         move.l   #BD1_SPRS, a0
                    lea      ENEMY_NAME1, a1
                    jsr      LOADFILE
                    rts

LOAD_MBDATA:        move.b   LEVEL_NUM, d7
                    beq      lbC015EAC
                    cmp.b    #2, d7
                    bmi      lbC015E72
                    beq      lbC015E38
                    cmp.b    #4, d7
                    bmi      lbC015DFE
                    beq      lbC015DC4
                    cmp.b    #7, d7
                    beq      lbC015EAC
                    
                    move.l   #lbL09F234, a0
                    lea      LASMB_NAME, a1
                    jsr      LOADFILE
                    rts

lbC015DC4:          move.l   A_MBLK_SPRS, a0
                    lea      SODMB_NAME, a1
                    jsr      LOADFILE
                    rts

lbC015DFE:          move.l   A_MBLK_SPRS, a0
                    lea      SNAMB_NAME, a1
                    jsr      LOADFILE
                    rts

lbC015E38:          move.l   A_MBLK_SPRS, a0
                    lea      TOOMB_NAME, a1
                    jsr      LOADFILE
                    rts

lbC015E72:          move.l   A_MBLK_SPRS, a0
                    lea      BULMB_NAME, a1
                    jsr      LOADFILE
                    rts

lbC015EAC:          move.l   A_MBLK_SPRS, a0
                    lea      BIRMB_NAME, a1
                    jsr      LOADFILE
                    rts

CLEAR_ALLFX:        bsr      CLEAR_FX
                    bsr      CLEAR_ARCH
                    bsr      CLEAR_PERMS
                    bsr      CLEAR_UNDS
                    rts

CLEAR_FX:           lea      TILEFX_TAB, a0
lbC015EFE:          sf       (a0)
                    lea      14(a0), a0
                    cmp.l    #END_TILEFX, a0
                    bne.s    lbC015EFE
                    rts

CLEAR_ARCH:         lea      ARCHEX_TAB, a0
lbC015F14:          sf       (a0)
                    lea      $1A(a0), a0
                    cmp.l    #DEL_ARCH, a0
                    bne.s    lbC015F14
                    rts

CLEAR_UNDS:         clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    rts

CLEAR_MEM:          lsr.w    #2, d7
                    subq.w   #1, d7
lbC015F36:          clr.l    (a0)+
                    dbra     d7, lbC015F36
                    rts

MOVE_MEM:           lsr.w    #2, d7
                    subq.w   #1, d7
lbC015F42:          move.l   (a5)+, (a6)+
                    dbra     d7, lbC015F42
                    rts

DRAW_ZONBS:         subq.w   #1, ZOONB_WAIT
                    bpl      lbC015FCE
                    move.w   #$32, ZOONB_WAIT
                    move.w   ZOONB_DRAWN, d7
                    cmp.w    ZOONB_GOT, d7
                    beq      lbC015FCE
                    addq.w   #1, ZOONB_DRAWN
                    mulu     #$30, d7
                    add.w    #$30, d7
                    move.w   d7, YCOORD
                    move.w   #$F8, XCOORD
                    move.w   #$15, SPRITE
                    st       P_DRAW
                    jsr      DUMPSPRITE
                    move.l   BUFF_PTR, -(sp)
                    move.l   PHYSADR, BUFF_PTR
                    jsr      DUMPSPRITE
                    move.l   #BUFF3_START, BUFF_PTR
                    jsr      DUMPSPRITE
                    sf       P_DRAW
                    move.l   (sp)+, BUFF_PTR
lbC015FCE:          rts

DRAW_TEXT:          cmp.w    #$60, LEVNAME_Y
                    beq      lbC015FE2
                    addq.w   #2, LEVNAME_Y
lbC015FE2:          move.w   LEVNAME_X, XCOORD
                    move.w   LEVNAME_Y, YCOORD
                    move.w   LEVNAME_S, SPRITE
                    st       LIGHT_SPR
                    jsr      DUMPSPRITE
                    add.w    #$40, XCOORD
                    addq.w   #1, SPRITE
                    st       LIGHT_SPR
                    jsr      DUMPSPRITE
                    addq.w   #6, XCOORD
                    subq.w   #6, YCOORD
                    jsr      DUMPSPRITE
                    sub.w    #$40, XCOORD
                    subq.w   #1, SPRITE
                    jsr      DUMPSPRITE
                    cmp.w    #$18, LEVNAME_S
                    beq      lbC016094
                    move.b   STAGE_NUM, lbB01F0F5
                    subq.w   #6, XCOORD
                    neg.w    YCOORD
                    add.w    #$D2, YCOORD
                    st       LIGHT_SPR
                    jsr      DUMPSPRITE
                    addq.w   #6, XCOORD
                    subq.w   #6, YCOORD
                    jsr      DUMPSPRITE
lbC016094:          rts

DRAW_ZEYES:         tst.b    ITS_ZOOL
                    beq      lbC016154
                    tst.w    BLINK_WAIT
                    beq      _RANDOM0
                    bmi      lbC0160D4
                    subq.w   #1, BLINK_WAIT
                    move.w   #$48, XCOORD
                    move.w   #$50, YCOORD
                    move.w   #$11, SPRITE
                    jsr      DUMPSPRITE
                    rts

lbC0160D4:          addq.w   #1, BLINK_WAIT
                    move.w   #$48, XCOORD
                    move.w   #$50, YCOORD
                    move.w   #$10, SPRITE
                    jsr      DUMPSPRITE
                    rts

_RANDOM0:           jsr      RANDOM
                    move.w   SEED+2, d0
                    and.w    #$7F, d0
                    cmp.w    #7, d0
                    bgt      lbC016152
                    btst     #6, SEED+2
                    beq      lbC016152
                    btst     #7, SEED+2
                    beq      lbC016152
                    btst     #1, d0
                    bne      lbC01614A
                    move.w   SEED, d0
                    and.w    #$1F, d0
                    add.w    #$14, d0
                    neg.w    d0
                    move.w   d0, BLINK_WAIT
                    rts

lbC01614A:          move.w   #4, BLINK_WAIT
lbC016152:          rts

lbC016154:          tst.w    BLINK_WAIT
                    beq.s    _RANDOM0
                    bmi      lbC016186
                    subq.w   #1, BLINK_WAIT
                    move.w   #$40, XCOORD
                    move.w   #$48, YCOORD
                    move.w   #$13, SPRITE
                    jsr      DUMPSPRITE
                    rts

lbC016186:          addq.w   #1, BLINK_WAIT
                    move.w   #$40, XCOORD
                    move.w   #$48, YCOORD
                    move.w   #$12, SPRITE
                    jsr      DUMPSPRITE
                    rts

DRAW_ZTAP:          tst.b    ITS_ZOOL
                    beq      lbC01621A
                    tst.w    FOOT_TAP
                    bpl      lbC016206
                    addq.w   #1, FOOT_TAP
                    bmi      lbC0161E6
_RANDOM1:           jsr      RANDOM
                    move.w   SEED, d0
                    and.w    #$3F, d0
                    add.w    #15, d0
                    move.w   d0, FOOT_TAP
                    rts

lbC0161E6:          move.w   #$18, XCOORD
                    move.w   #$48, YCOORD
                    move.w   #15, SPRITE
                    jsr      DUMPSPRITE
                    rts

lbC016206:          subq.w   #1, FOOT_TAP
                    bpl      lbC016218
                    move.w   #$FFFB, FOOT_TAP
lbC016218:          rts

lbC01621A:          tst.w    FOOT_TAP
                    bpl.s    lbC016206
                    addq.w   #1, FOOT_TAP
                    bpl.s    _RANDOM1
                    move.w   #$40, XCOORD
                    move.w   #$88, YCOORD
                    move.w   #$14, SPRITE
                    jsr      DUMPSPRITE
                    rts

SETSCREEN:          move.l   PHYSADR, a0
                    subq.l   #2, a0
                    lea      SETSCR_BPLS, a1
                    moveq    #3, d1
lbC01625A:          move.l   a0, d0
                    move.w   d0, 6(a1)
                    swap     d0
                    move.w   d0, 2(a1)
                    addq.l   #8, a1
                    lea      $30(a0), a0
                    dbra     d1, lbC01625A
                    rts

FLIP:               jsr      VBL
                    move.l   PHYSADR, a0
                    move.l   LOGADR, PHYSADR
                    move.l   a0, LOGADR
                    eor.b    #1, ANDYFRAME
                    bsr.s    SETSCREEN
                    rts

WAIT_SECS:          dbra     d7, lbC0162A0
                    rts

lbC0162A0:          move.w   d7, -(sp)
                    jsr      VBL
                    move.w   (sp)+, d7
                    move.w   #$3E8, d6
lbC0162AE:          dbra     d6, lbC0162AE
                    bra.s    WAIT_SECS

FWAIT_SECS:         dbra     d7, lbC0162BA
                    rts

lbC0162BA:          move.w   d7, -(sp)
                    jsr      VBL
                    jsr      READ_JOY
                    btst     #7, JOYPOS
                    beq      lbC0162D8
                    move.w   (sp)+, d7
                    rts

lbC0162D8:          move.w   (sp)+, d7
                    move.w   #$3E8, d6
lbC0162DE:          dbra     d6, lbC0162DE
                    bra.s    FWAIT_SECS

; ---------------------------------
; CHARACTER SELECTION
; ---------------------------------
CHOOSE_SCR:         jsr      INIT_SCRNS
                    jsr      INIT_SCRO
                    bsr      CLEAR_ALLFX
                    move.l   PHYSADR, a0
                    jsr      CLR_SCRN
                    move.l   LOGADR, a0
                    jsr      CLR_SCRN
                    lea      BUFF3_START, a0
                    jsr      CLR_SCRN
                    
                    move.l   PHYSADR, a0
                    lea      $C00(a0), a0
                    lea      CHOOSE_NAME, a1
                    jsr      LOADFILE
                    
                    move.l   LOGADR, a0
                    lea      $C00(a0), a0
                    lea      CHOOSE_NAME, a1
                    jsr      LOADFILE
                    
                    move.l   SPRITE_AD, a0
                    lea      HITEXT_NAME, a1
                    jsr      LOADFILE
                    
                    move.l   PHYSADR, a0
                    lea      BUFF3_START, a1
                    bsr      COPY_SCRN
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    bsr      SETSCREEN
                    move.l   #CHOOSE_BAND, SK_PAL
                    move.l   #CHOOSE_BAND, SK_PAL2
                    move.b   #6, BAND_HGT
                    move.w   #$18C, COL_MOVE
                    move.w   #$180, COL_MOVE2
                    bsr      INIT_CSCRO
                    move.l   #COPPER2, COP_LOC
                    bsr      CLEAR_UNDS
                    move.l   #CHOOSE_PAL, LEV_PAL
                    move.b   #1, ANDYFRAME
                    jsr      VBL
                    move.w   #$8080, $DFF096
                    jsr      INIT_FADE
                    sf       PLAY_CHOOSE
STOPPP:             jsr      SCROLL_BUFF
                    move.l   LOGADR, BUFF_PTR
                    move.l   LOGADR, SCROLL
                    jsr      UNDRAW
                    bsr      DRAW_PLYCHS
                    jsr      READ_JOY
                    bsr      FLIP
                    bsr      PRO_JOYSEL
                    tst.b    PLAY_CHOOSE
                    bmi      lbC01647C
                    bra.s    STOPPP

lbC01647C:          move.w   #$32, d7
                    jsr      WAIT_SECS
                    move.w   #$20, FADE_CNT
                    sf       FADECOL_ON
lbC016494:          tst.w    FADE_CNT
                    bne.s    lbC016494
                    st       MUSIC_ON
                    move.w   #$A0, $DFF096
                    move.l   #LAST_ICON, $DFF120
                    move.l   #LAST_ICON, $DFF124
                    move.l   #LAST_ICON, $DFF128
                    move.l   #LAST_ICON, $DFF12C
                    move.l   #LAST_ICON, $DFF130
                    move.l   #LAST_ICON, $DFF134
                    move.l   #LAST_ICON, $DFF138
                    move.l   #LAST_ICON, $DFF13C
                    jsr      CLRPALET
                    move.l   #NULLCOP, COP_LOC
                    jsr      CHEAT_START
                    rts

DRAW_PLYCHS:        move.w   #$40, XCOORD
                    move.w   #$10, YCOORD
                    lea      PL1_CHS_TXT, a0
                    tst.b    PLAY_CHOOSE
                    beq      WRITE_TEXT
                    lea      PL2_CHS_TXT, a0
                    bra      WRITE_TEXT

PL1_CHS_TXT:        dc.b     'PLAYER ONE CHOOSE...'
                    dc.b     $FF
                    even
PL2_CHS_TXT:        dc.b     'PLAYER TWO CHOOSE...'
                    dc.b     $FF
                    even

PRO_JOYSEL:         btst     #2, JOYPOS
                    beq      lbC016592
                    cmp.w    #$18E, COL_MOVE
                    bne      lbC016592
                    move.w   #$18C, COL_MOVE
                    move.w   #$CCC, $DFF18E
                    bsr      INIT_CSCRO
lbC016592:          btst     #3, JOYPOS
                    beq      lbC0165BE
                    cmp.w    #$18C, COL_MOVE
                    bne      lbC0165BE
                    move.w   #$18E, COL_MOVE
                    move.w   #$CCC, $DFF18C
                    bsr      INIT_CSCRO
lbC0165BE:          btst     #7, JOYPOS
                    beq      lbC0166A4
                    tst.b    PLAYERS
                    beq      lbC016674
                    tst.b    PLAY_CHOOSE
                    beq      lbC01660E
                    cmp.w    #$18C, COL_MOVE
                    beq      lbC0165FC
                    st       PLAY2_CHAR
                    move.w   #$FFF, $DFF18C
                    bra      lbC0166A6

lbC0165FC:          sf       PLAY2_CHAR
                    move.w   #$FFF, $DFF18E
                    bra      lbC0166A6

lbC01660E:          addq.b   #1, PLAY_CHOOSE
                    cmp.w    #$18C, COL_MOVE
                    beq      lbC016632
                    st       PLAY1_CHAR
                    move.w   #$FFF, $DFF18C
                    bra      _READ_JOY5

lbC016632:          sf       PLAY1_CHAR
                    move.w   #$FFF, $DFF18E
_READ_JOY5:         jsr      READ_JOY
                    btst     #7, JOYPOS
                    bne.s    _READ_JOY5
                    cmp.w    #$18E, COL_MOVE
                    beq      lbC016668
                    move.w   #$CCC, $DFF18E
                    bra      lbC0166A4

lbC016668:          move.w   #$CCC, $DFF18C
                    bra      lbC0166A4

lbC016674:          cmp.w    #$18C, COL_MOVE
                    beq      lbC016692
                    st       PLAY1_CHAR
                    move.w   #$FFF, $DFF18C
                    bra      lbC0166A6

lbC016692:          sf       PLAY1_CHAR
                    move.w   #$FFF, $DFF18E
                    bra      lbC0166A6

lbC0166A4:          rts

lbC0166A6:          st       PLAY_CHOOSE
                    rts

INTRO_SCR:          jsr      CLRPALET
                    sf       GAME_FADE
                    jsr      INIT_SCRNS
                    jsr      INIT_SCRO
                    jsr      SETSCREEN
                    move.l   MASK_AD, a0
                    add.l    #71482, a0
                    move.l   a0, MUSIC_PTR
                    move.l   a0, MUSIC_AD
                    move.l   #BLACK_BAND, COLBAND
                    clr.w    COLBAND_PTR
                    jsr      RESET_COP
                    move.w   #$14, MAP_WIDTH
                    move.l   #BOT_CHIP, a0
                    move.l   a0, TILES
                    move.w   #$1F, d7
lbC016712:          clr.l    (a0)+
                    dbra     d7, lbC016712
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    
                    move.l   SPRITE_AD, a0
                    lea      INTSP_NAME, a1
                    jsr      LOADFILE
                    
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    move.l   MUSIC_PTR, a0
                    lea      INTMUS_NAME, a1
                    jsr      LOADFILE
                    jsr      CLRPALET
                    move.l   #DOCOL_BAND3, DOCOL_RTN
                    move.l   #COPPER2, COP_LOC
                    move.l   #$FFFFFFFE, COL_SCROLL2
                    bsr      SETSCREEN
                    bsr      CLEAR_ALLFX
                    bsr      CLR_ALLSCR
                    bsr      CLEAR_UNDS
GOAGAIN:            move.l   #GREM_PAL, LEV_PAL
                    
                    move.l   PHYSADR, a0
                    lea      GREM_NAME, a1
                    jsr      LOADFILE
                    
                    tst.b    MUSIC_ON
                    bne      lbC016824
                    jsr      PLAY_MUSIC
lbC016824:          move.w   #$8080, $DFF096
                    jsr      VBL
                    jsr      INIT_FADE
                    move.w   #$C8, d7
                    jsr      FWAIT_SECS
                    move.w   #$20, FADE_CNT
                    sf       FADECOL_ON
                    bsr      WAIT_FDOFF
                    move.l   #WARP_PAL, LEV_PAL
                    bsr      CLR_ALLSCR
                    
                    move.l   #lbL148B48, a0
                    lea      WARP_NAME, a1
                    jsr      LOADFILE
                    
                    move.l   #lbL155758, a0
                    lea      WARP_NAME, a1
                    jsr      LOADFILE
                    
                    move.l   #lbL162368, a0
                    lea      WARP_NAME, a1
                    jsr      LOADFILE
                    
                    clr.w    XSCROLL
                    clr.w    YSCROLL
                    clr.w    OLD_YSCR
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    jsr      INIT_SCRO
                    jsr      VBL
                    jsr      INIT_FADE
                    bsr      LOGO_APP
                    move.w   #$64, d7
                    jsr      FWAIT_SECS
                    move.w   #$20, FADE_CNT
                    sf       FADECOL_ON
                    bsr      WAIT_FDOFF
                    move.l   #CHUP_PAL, LEV_PAL
                    bsr      CLR_ALLSCR
                    
                    move.l   PHYSADR, a0
                    lea      $C00(a0), a0
                    lea      CHPS_NAME, a1
                    jsr      LOADFILE
                    
                    jsr      VBL
                    jsr      INIT_FADE
                    move.w   #$C8, d7
                    jsr      FWAIT_SECS
                    move.w   #$20, FADE_CNT
                    sf       FADECOL_ON
                    bsr      WAIT_FDOFF
                    move.w   #$32, d7
                    jsr      FWAIT_SECS
TITLE_SCR:          jsr      INIT_SCRO
                    jsr      SCROLL_BUFF
                    move.l   #INTRO_SPSTART, SPRITE_AD
                    move.l   SPRITE_AD, a5
                    lea      SNAKE_R, a6
lbC0169CA:          move.l   (a6)+, (a5)+
                    cmp.l    #SNAKE_R, a6
                    bmi.s    lbC0169CA
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    tst.b    MUSIC_ON
                    bne      lbC016A02
                    jsr      PLAY_MUSIC
lbC016A02:          sf       GAME_FADE
                    sf       P_DRAW
                    sf       STAGE_NUM
                    sf       LEVEL_NUM
                    clr.w    CHEAT_SEQ
                    clr.w    CHEAT_LETT
                    clr.w    MAP_WIDTH
                    clr.w    OLD_YSCR
                    clr.w    XSCROLL
                    clr.w    YSCROLL
                    clr.w    XREM
                    move.l   #BUFF1_START, a0
                    jsr      CLR_SCRN
                    move.l   #BUFF2_START, a0
                    jsr      CLR_SCRN
                    move.l   #BUFF3_START, a0
                    jsr      CLR_SCRN
                    move.l   #BLACK_BAND, COLBAND
                    clr.w    COLBAND_PTR
                    move.l   #DOCOL_BAND3, DOCOL_RTN
                    move.l   #$FFFFFFFE, WRITE_COP3
                    move.l   #COPPER3, COP_LOC
                    move.l   #BUFF3_START, SCROLL
                    move.l   #BUFF3_START, BUFF_PTR
                    bsr      DRAW_ZI
                    move.l   #BUFF2_START, SCROLL
                    move.l   #BUFF2_START, BUFF_PTR
                    bsr      DRAW_ZI
                    move.l   #BUFF1_START, SCROLL
                    move.l   #BUFF1_START, BUFF_PTR
                    bsr      DRAW_ZI
                    move.w   #$8080, $DFF096
                    jsr      VBL
                    move.w   #$8080, $DFF096
                    jsr      SCRO_NOW3
                    jsr      DOCOL
                    move.l   #ZEYES_PAL, LEV_PAL
                    jsr      INIT_FADE
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    clr.w    XSCROLL
                    clr.w    YSCROLL
                    clr.w    OLD_YSCR
                    jsr      INIT_SCRO
                    move.w   #$100, MARKER_Y
                    sf       P_DRAW
lbC016B42:          eor.b    #1, ANDYFRAME
                    jsr      SCROLL_BUFF
                    jsr      UNDRAW
                    bsr      DRAW_ZL
                    jsr      VBL
                    jsr      SCRO_NOW3
                    jsr      DOCOL
                    tst.w    MARKER_Y
                    bmi      lbC016BEE
                    subq.w   #2, MARKER_Y
                    cmp.w    #$64, MARKER_Y
                    bpl.s    lbC016B42
                    addq.w   #2, MARKER_Y
                    eor.b    #1, ANDYFRAME
                    jsr      SCROLL_BUFF
                    jsr      UNDRAW
                    bsr      DRAW_ZL
                    jsr      VBL
                    jsr      SCRO_NOW3
                    jsr      DOCOL
                    eor.b    #1, ANDYFRAME
                    jsr      SCROLL_BUFF
                    bsr      DRAW_ZL
                    jsr      VBL
                    jsr      SCRO_NOW3
                    jsr      DOCOL
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    move.w   #$FFFF, MARKER_Y
lbC016BEE:          move.l   #SPSAVE, MAP_PTR_A
                    move.l   #SPSAVE, MAP_PTR_B
                    move.l   #SPSAVE, MAP_PTR_C
                    clr.w    MAP_WIDTH
                    clr.w    MAP_LINE_A
                    clr.w    MAP_LINE_B
                    clr.w    MAP_LINE_C
                    move.w   #2, YSCROLL
                    move.w   #$3E8, d7
lbC016C30:          dbra     d7, lbC016C30
                    cmp.w    #$50, YSCRO
                    bne      lbC016B42
                    bra      lbC016C48

                    bra      lbC016B42

lbC016C48:          move.w   #$32, d7
                    jsr      FWAIT_SECS
                    move.w   #0, TEMPW
lbC016C5A:          eor.b    #1, ANDYFRAME
                    clr.w    YSCROLL
                    jsr      SCROLL_BUFF
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    jsr      UNDRAW
                    tst.b    ANDYFRAME
                    beq      _VBL
                    move.l   BUFF_PTR_A, BUFF_PTR
                    bsr      DRAW_2BITZ
                    move.l   BUFF_PTR_B, BUFF_PTR
                    bsr      DRAW_2BITZ
_VBL:               jsr      VBL
                    jsr      SCRO_NOW3
                    jsr      DOCOL
                    tst.b    ANDYFRAME
                    beq.s    lbC016C5A
                    addq.w   #1, TEMPW
                    cmp.w    #8, TEMPW
                    bne.s    lbC016C5A
                    eor.b    #1, ANDYFRAME
                    clr.w    YSCROLL
                    jsr      SCROLL_BUFF
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    jsr      UNDRAW
                    bsr      DRAW_COPY
                    jsr      VBL
                    jsr      SCRO_NOW3
                    jsr      DOCOL
                    eor.b    #1, ANDYFRAME
                    jsr      SCROLL_BUFF
                    bsr      DRAW_COPY
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    move.w   #$14, TEMPW
                    move.w   #$258, MARKER_Y
lbC016D3A:          subq.w   #1, MARKER_Y
                    bmi      lbC016DC6
                    jsr      READ_JOY
                    btst     #7, JOYPOS
                    bne      lbC016DE2
                    bsr      PRO_CHEATS
                    eor.b    #1, ANDYFRAME
                    jsr      SCROLL_BUFF
                    jsr      UNDRAW
                    tst.w    TEMPW
                    bmi      lbC016D96
                    subq.w   #1, TEMPW
                    bne      _DRAW_GMESS
                    move.w   #$FFEC, TEMPW
                    bra      lbC016D96

_DRAW_GMESS:        bsr      DRAW_GMESS
                    bra      _VBL0

lbC016D96:          move.w   #$3E8, d7
lbC016D9A:          dbra     d7, lbC016D9A
                    addq.w   #1, TEMPW
                    bne      _VBL0
                    move.w   #$1E, TEMPW
_VBL0:              jsr      VBL
                    jsr      SCRO_NOW3
                    jsr      DOCOL
                    bra      lbC016D3A

lbC016DC6:          sf       INSERT_HSCR
                    sf       FADECOL_ON
                    move.w   #$20, FADE_CNT
                    bsr      WAIT_FDOFF
                    bra      HISCORE_SCR

lbC016DE2:          sf       FADECOL_ON
                    move.w   #$20, FADE_CNT
                    bsr      WAIT_FDOFF
                    bra      OPTIONS_SCR

PRO_CHEATS:         move.w   #$1420, $DFF106
                    move.w   #6, $DFF180
                    move.b   KEYREC, d7
                    cmp.b    OLDKEY, d7
                    beq      lbC016E9A
                    move.b   d7, OLDKEY
                    btst     #0, d7
                    beq      lbC016E9A
                    lea      CHEATS_TAB, a0
                    move.w   CHEAT_SEQ, d0
                    beq      lbC016EAA
                    subq.w   #1, d0
                    lsl.w    #2, d0
                    move.l   0(a0,d0.w), a1
                    move.w   CHEAT_LETT, d1
                    cmp.b    0(a1,d1.w), d7
                    beq      lbC016E8A
                    bsr      lbC016E9C
                    bra      lbC016EAA

lbC016E8A:          addq.w   #1, d1
                    tst.b    0(a1,d1.w)
                    beq      lbC016ECA
                    move.w   d1, CHEAT_LETT
lbC016E9A:          rts

lbC016E9C:          clr.w    CHEAT_LETT
                    clr.w    CHEAT_SEQ
                    rts

lbC016EAA:          move.w   #1, CHEAT_SEQ
                    clr.w    d1
lbC016EB4:          move.l   (a0)+, a1
                    cmp.b    (a1), d7
                    beq.s    lbC016E8A
                    cmp.l    #CHEAT1, a0
                    beq.s    lbC016E9C
                    addq.w   #1, CHEAT_SEQ
                    bra.s    lbC016EB4

lbC016ECA:          move.w   CHEAT_SEQ, d0
                    move.w   #$1420, $DFF106
                    move.w   #$F00, $DFF180
                    subq.w   #1, d0
                    cmp.w    #7, d0
                    bmi      lbC016EFA
                    cmp.w    #12, d0
                    bgt      lbC016EFA
                    and.w    #$E07F, CHEAT
lbC016EFA:          move.w   CHEAT, d1
                    bset     d0, d1
                    move.w   d1, CHEAT
                    bra.s    lbC016E9C

CHEATS_TAB:         dc.l     CHEAT1
                    dc.l     CHEAT2
                    dc.l     CHEAT3
                    dc.l     CHEAT4
                    dc.l     CHEAT5
                    dc.l     CHEAT6
                    dc.l     CHEAT7
                    dc.l     CHEAT8
                    dc.l     CHEAT9
                    dc.l     CHEAT10
                    dc.l     CHEAT11
                    dc.l     CHEAT12
                    dc.l     CHEAT13
                    dc.l     CHEAT14
                    dc.l     CHEAT15
                    dc.l     CHEAT16

                    ;         C   R   E   A   M   O   L   A
CHEAT1:             dc.b     $99, $D9, $DB, $BF, $91, $CF, $AF, $BF, 0
                    ;         V   I   S   I   O   N
CHEAT2:             dc.b     $97, $D1, $BD, $D1, $CF, $93, 0
                    ;         K   I   C   K   A   S   S
CHEAT3:             dc.b     $B1, $D1, $99, $B1, $BF, $BD, $BD, 0
                    ;         N   A   P   O   L   E   O   N
CHEAT4:             dc.b     $93, $BF, $CD, $CF, $AF, $DB, $CF, $93, 0
                    ;         A   L   C   E   N   T   O
CHEAT5:             dc.b     $BF, $AF, $99, $DB, $93, $D7, $CF, 0
                    ;         O   L   D   E   N   E   M   Y
CHEAT6:             dc.b     $CF, $AF, $BB, $DB, $93, $DB, $91, $D5, 0
                    ;         T   O   U   G   H   G   U   Y
CHEAT7:             dc.b     $D7, $CF, $D3, $B7, $B5, $B7, $D3, $D5, 0
                    ;         S   E   S   A   M   E
CHEAT8:             dc.b     $BD, $DB, $BD, $BF, $91, $DB, 0
                    ;         R   O   N   S   O   N
CHEAT9:             dc.b     $D9, $CF, $93, $BD, $CF, $93, 0
                    ;         F   U   N   K   Y   T   U   T
CHEAT10:            dc.b     $B9, $D3, $93, $B1, $D5, $D7, $D3, $D7, 0
                    ;         H   I   S   S   T   E   R   I   A
CHEAT11:            dc.b     $B5, $D1, $BD, $BD, $D7, $DB, $D9, $D1, $BF, 0
                    ;         7   S   L   U   R   P
CHEAT12:            dc.b     $F1, $BD, $AF, $D3, $D9, $CD, 0
                    ;         P   L   U   N   G   E   R
CHEAT13:            dc.b     $CD, $AF, $D3, $93, $B7, $DB, $D9, 0
                    ;         M   A   R   R   O   B   O   N   E
CHEAT14:            dc.b     $91, $BF, $D9, $D9, $CF, $95, $CF, $93, $DB, 0
                    ;         B   U   M   B   L   E   B   E   E
CHEAT15:            dc.b     $95, $D3, $91, $95, $AF, $DB, $95, $DB, $DB, 0
                    ;         W   A   R   P   M   O   D   E
CHEAT16:            dc.b     $DD, $BF, $D9, $CD, $91, $CF, $BB, $DB, 0
                    even

CHEAT_SEQ:          dc.w     0
CHEAT_LETT:         dc.w     0

DRAW_GMESS:         move.w   #64, XCOORD
                    move.w   #156, YCOORD
                    lea      PRESS_MESS, a0
                    bra      WRITE_TEXT

PRESS_MESS:         dc.b     'PRESS FIRE FOR OPTIONS'
                    dc.b     $FF
                    even

DRAW_COPY:          lea      COPY_MESS, a0
                    move.w   #8, XCOORD
                    move.w   #$B0, YCOORD
                    bra      WRITE_TEXT

COPY_MESS:          dc.b     '$ 1993 GREMLIN GRAPHICS SOFTWARE LTD.'
                    dc.b     $FF
                    even

DRAW_2BITZ:         lea      BTAB2, a0
                    move.w   TEMPW, d7
                    lsl.w    #2, d7
                    move.l   0(a0,d7.w), a0
                    jmp      (a0)

BTAB2:              dc.l     BITZ_1
                    dc.l     BITZ_2
                    dc.l     BITZ_3
                    dc.l     BITZ_4
                    dc.l     BITZ_5
                    dc.l     BITZ_6
                    dc.l     BITZ_7
                    dc.l     BITZ_8

BITZ_1:             move.w   #$D0, XCOORD
                    move.w   #$20, YCOORD
                    move.w   #$33, SPRITE
                    jmp      DUMPSPRITE

BITZ_2:             move.w   #$D8, XCOORD
                    move.w   #$10, YCOORD
                    move.w   #$34, SPRITE
                    jmp      DUMPSPRITE

BITZ_3:             move.w   #$B8, XCOORD
                    move.w   #$10, YCOORD
                    move.w   #$35, SPRITE
                    jsr      DUMPSPRITE
                    move.w   #$F8, XCOORD
                    move.w   #$10, YCOORD
                    move.w   #$36, SPRITE
                    jmp      DUMPSPRITE

BITZ_4:             move.w   #$F8, XCOORD
                    move.w   #$18, YCOORD
                    move.w   #$37, SPRITE
                    jsr      DUMPSPRITE
                    move.w   #$E0, XCOORD
                    move.w   #$30, YCOORD
                    move.w   #$38, SPRITE
                    jmp      DUMPSPRITE

BITZ_5:             move.w   #$E8, XCOORD
                    move.w   #$40, YCOORD
                    move.w   #$39, SPRITE
                    jsr      DUMPSPRITE
                    move.w   #$B0, XCOORD
                    move.w   #$50, YCOORD
                    move.w   #$3A, SPRITE
                    jmp      DUMPSPRITE

BITZ_6:             move.w   #$A8, XCOORD
                    move.w   #$58, YCOORD
                    move.w   #$3B, SPRITE
                    jsr      DUMPSPRITE
                    move.w   #$B0, XCOORD
                    move.w   #$58, YCOORD
                    move.w   #$3C, SPRITE
                    jmp      DUMPSPRITE

BITZ_7:             move.w   #$A0, XCOORD
                    move.w   #$58, YCOORD
                    move.w   #$3D, SPRITE
                    jsr      DUMPSPRITE
                    move.w   #$D0, XCOORD
                    move.w   #$58, YCOORD
                    move.w   #$3E, SPRITE
                    jmp      DUMPSPRITE

BITZ_8:             move.w   #$D0, XCOORD
                    move.w   #$60, YCOORD
                    move.w   #$3F, SPRITE
                    jsr      DUMPSPRITE
                    move.w   #$F0, XCOORD
                    move.w   #$60, YCOORD
                    move.w   #$40, SPRITE
                    jmp      DUMPSPRITE

HOLDIT:             bra.s    HOLDIT

DRAW_ZL:            move.w   MARKER_Y, YCOORD
                    bmi      lbC0172F2
                    move.w   #0, XCOORD
                    move.w   #$4E, SPRITE
                    jsr      DUMPSPRITE
                    add.w    #$40, XCOORD
                    addq.w   #1, SPRITE
                    jsr      DUMPSPRITE
                    add.w    #$A0, XCOORD
                    addq.w   #1, SPRITE
                    jsr      DUMPSPRITE
                    add.w    #$40, XCOORD
                    addq.w   #1, SPRITE
                    jsr      DUMPSPRITE
                    clr.w    XCOORD
                    add.w    #$40, YCOORD
                    addq.w   #1, SPRITE
                    jsr      DUMPSPRITE
                    add.w    #$40, XCOORD
                    addq.w   #1, SPRITE
                    jsr      DUMPSPRITE
                    add.w    #$40, XCOORD
                    subq.w   #8, YCOORD
                    addq.w   #1, SPRITE
                    jsr      DUMPSPRITE
                    add.w    #$40, XCOORD
                    addq.w   #1, SPRITE
                    jsr      DUMPSPRITE
                    add.w    #$40, XCOORD
                    addq.w   #1, SPRITE
                    jmp      DUMPSPRITE

lbC0172F2:          rts

DRAW_ZI:            move.w   #$64, YCOORD
                    move.w   #$60, XCOORD
                    move.w   #$31, SPRITE
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC017320
                    bra      _DUMPSPRITE4

lbC017320:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC017320
                    tst.b    $DFF002
_DUMPSPRITE4:       jsr      DUMPSPRITE
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    add.w    #$40, XCOORD
                    move.w   #$32, SPRITE
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC017378
                    bra      _DUMPSPRITE5

lbC017378:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC017378
                    tst.b    $DFF002
_DUMPSPRITE5:       jsr      DUMPSPRITE
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    tst.b    $DFF002
                    btst     #6, $DFF002
                    bne.s    lbC0173C0
                    bra      lbC0173E2

lbC0173C0:          nop
                    nop
                    nop
                    tst.b    $BFE001
                    tst.b    $BFE001
                    btst     #6, $DFF002
                    bne.s    lbC0173C0
                    tst.b    $DFF002
lbC0173E2:          rts

CLR_ALLSCR:         lea      BUFF1_START, a0
                    jsr      CLR_SCRN
                    lea      BUFF2_START, a0
                    jsr      CLR_SCRN
                    lea      BUFF3_START, a0
                    jmp      CLR_SCRN

LOGO_APP:           tst.w    FADE_CNT
                    bne.s    LOGO_APP
                    move.w   #$41, TEMPW
_SCROLL_BUFF1:      jsr      SCROLL_BUFF
                    move.l   LOGADR, BUFF_PTR
                    move.l   LOGADR, SCROLL
                    jsr      UNDRAW
                    bsr      DRAW_LOGO
                    bsr      FLIP
                    tst.b    ANDYFRAME
                    beq.s    _SCROLL_BUFF1
                    addq.w   #1, TEMPW
                    cmp.w    #$4E, TEMPW
                    bne.s    _SCROLL_BUFF1
                    rts

DRAW_LOGO:          move.w   #$70, XCOORD
                    move.w   #$48, YCOORD
                    move.w   TEMPW, SPRITE
                    jsr      DUMPSPRITE
                    rts

WAIT_FDOFF:         tst.w    FADE_CNT
                    bne.s    WAIT_FDOFF
                    rts

; ---------------------------------
; GAME OPTIONS
; ---------------------------------
OPTIONS_SCR:        jsr      INIT_SCRNS
                    jsr      INIT_SCRO
                    clr.w    MARKER_Y
                    bsr      CLEAR_ALLFX
                    move.l   PHYSADR, a0
                    jsr      CLR_SCRN
                    move.l   LOGADR, a0
                    jsr      CLR_SCRN
                    lea      BUFF3_START, a0
                    jsr      CLR_SCRN

                    move.l   SPRITE_AD, a0
                    lea      HITEXT_NAME, a1
                    jsr      LOADFILE

                    move.l   PHYSADR, a0
                    lea      BUFF3_START, a1
                    bsr      COPY_SCRN
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    bsr      SETSCREEN
                    move.l   #COPPER2, COP_LOC
                    move.l   #$FFFFFFFE, COL_SCROLL2
                    bsr      CLEAR_UNDS
                    move.l   #HISC_PAL, LEV_PAL
                    move.b   #1, ANDYFRAME
                    jsr      VBL
                    move.w   #$8080, $DFF096
                    ;sf       MUSIC_TEST
                    sf       SEL_JOYSTICK_MODE
                    sf       JOYSTICK_MODE
                    sf       START_SEL
                    jsr      INIT_FADE
                   
LOOPH:              jsr      STOOSLOW

                    move.l   PHYSADR, a0
                    move.l   LOGADR, PHYSADR
                    move.l   a0, LOGADR

                    bsr      PRO_PNTR
                    move.l   PHYSADR, BUFF_PTR
                    move.l   PHYSADR, SCROLL
                    jsr      SCROLL_BUFF

                    move.l   PHYSADR, BUFF_PTR

                    move.l   PHYSADR, a0
                    jsr      CLR_SCRN

WAIT_BLT:           btst     #14, $DFF002
                    bne.s    WAIT_BLT

                    move.w   #34,  XCOORD
                    move.w   #34+5,  YCOORD
                    lea      OPTIONS_TXT, a0
                    bsr      WRITE_TEXT

                    move.w   #34,  XCOORD
                    move.w   #74+6,  YCOORD
                    lea      SOUNDS_TXT, a0
                    bsr      WRITE_TEXT

                    move.w   #34,  XCOORD
                    move.w   #98+6,  YCOORD
                    lea      DIFFICULTY_TXT, a0
                    bsr      WRITE_TEXT

                    move.w   #34,  XCOORD
                    move.w   #122+6,  YCOORD
                    lea      PLAYERS_TXT, a0
                    bsr      WRITE_TEXT

                    move.w   #34,  XCOORD
                    move.w   #146+6,  YCOORD
                    lea      INPUT_TXT, a0
                    bsr      WRITE_TEXT

                    move.w   #34,  XCOORD
                    move.w   #186+6,  YCOORD
                    lea      START_TXT, a0
                    bsr      WRITE_TEXT

                    bsr      DRAW_OPTS
                    bsr      DRAW_PNTR

                    bsr.w    SETSCREEN
                    
                    tst.b    START_SEL
                    bne      lbC01766A
                    bra.w    LOOPH

lbC01766A:          

WAIT_BLT2:          btst     #14, $DFF002
                    bne.s    WAIT_BLT2

                    move.w   #$20, FADE_CNT
                    sf       FADECOL_ON
                    bsr      WAIT_FDOFF
                    bra      CHOOSE_SCR

PRO_PNTR:           move.w   #ZOOL_GRIPPING, ZOOL_MOVE
                    jsr      READ_JOY
                    clr.w    ZOOL_MOVE
                    btst     #0, JOYPOS
                    beq      lbC0176DE
                    bset     #0, OLDJOY
                    beq      lbC0176D4
                    subq.b   #1, JOY_REPT
                    bpl      lbC0176E6
                    move.b   #2, JOY_REPT
lbC0176BE:          subq.w   #1, MARKER_Y
                    bpl      lbC0176E6
                    move.w   #4, MARKER_Y
                    bra      lbC0176E6

lbC0176D4:          move.b   #$12, JOY_REPT
                    bra.s    lbC0176BE

lbC0176DE:          bclr     #0, OLDJOY
lbC0176E6:          btst     #1, JOYPOS
                    beq      lbC017736
                    bset     #1, OLDJOY
                    beq      lbC01772C
                    subq.b   #1, JOY_REPT
                    bpl      lbC01773E
                    move.b   #2, JOY_REPT
lbC017710:          addq.w   #1, MARKER_Y
                    cmp.w    #5, MARKER_Y
                    bne      lbC01773E
                    clr.w    MARKER_Y
                    bra      lbC01773E

lbC01772C:          move.b   #$12, JOY_REPT
                    bra.s    lbC017710

lbC017736:          bclr     #1, OLDJOY
lbC01773E:          cmp.w    #3, MARKER_Y
                    bne      lbC017756
                    cmp.b    #$CD, KEYREC
                    beq      lbC017762
lbC017756:          btst     #7, JOYPOS
                    beq      lbC017820
lbC017762:          bset     #7, JOYPOS
                    bset     #7, OLDJOY
                    beq      lbC017814
                    subq.b   #1, JOY_REPT
                    bpl      lbC017828
                    move.b   #2, JOY_REPT
lbC017788:          cmp.w    #1, MARKER_Y
                    bmi      lbC0177DA
                    beq      lbC0177BC
                    cmp.w    #3, MARKER_Y
                    bmi      lbC0177B0
                    beq      lbC0177F8
                    st       START_SEL
                    ; Validate the joystick mode
                    move.b   SEL_JOYSTICK_MODE, JOYSTICK_MODE
                    addq.b   #1, JOYSTICK_MODE
                    rts

lbC0177B0:          eor.b    #1, PLAYERS
                    bra      lbC017828

lbC0177BC:          addq.b   #1, DIFFICULTY
                    cmp.b    #2, DIFFICULTY
                    bne      lbC017828
                    move.b   #$FF, DIFFICULTY
                    bra      lbC017828

lbC0177DA:          addq.b   #1, AUDIO
                    cmp.b    #3, AUDIO
                    bne      lbC017828
                    move.b   #$FF, AUDIO
                    bra      lbC017828

lbC0177F8:          
                    addq.b   #1, SEL_JOYSTICK_MODE
                    cmp.b    #3, SEL_JOYSTICK_MODE
                    bne      lbC017828
                    sf       SEL_JOYSTICK_MODE
                    bra      lbC017828

                    ;addq.b   #1, MUSIC_TEST
                    ;cmp.b    #10, MUSIC_TEST
                    ;bne      lbC017828
                    ;sf       MUSIC_TEST
                    ;bra      lbC017828

lbC017814:          move.b   #$12, JOY_REPT
                    bra      lbC017788

lbC017820:          bclr     #7, OLDJOY
lbC017828:          rts

;MUSIC_TEST:         dcb.b    2, 0

DRAW_PNTR:          move.w   MARKER_Y, d1
                    cmp.w    #4, MARKER_Y
                    bne      lbC017842
                    move.w   #5, d1
lbC017842:          lsl.w    #3, d1
                    move.w   d1, d2
                    add.w    d1, d1
                    add.w    d2, d1
                    add.w    #80, d1
                    cmp.w    #4, MARKER_Y
                    bne.b    GAP_START
                    subq.w   #8, d1
GAP_START:
                    move.w   d1, YCOORD
                    move.w   #0, XCOORD
                    move.w   #45, SPRITE
                    jmp      DUMPSPRITE

DRAW_OPTS:          move.w   #$C0, XCOORD
                    move.w   #80, YCOORD
                    tst.b    AUDIO
                    bmi      lbC0178B0
                    beq      lbC0178BE
                    cmp.b    #2, AUDIO
                    bne      lbC0178A2
                    lea      MUSFX_TXT, a0
                    bsr      WRITE_TEXT
                    bra      lbC0178C8

lbC0178A2:          lea      MUSIC_TXT, a0
                    bsr      WRITE_TEXT
                    bra      lbC0178C8

lbC0178B0:          lea      FX_TXT, a0
                    bsr      WRITE_TEXT
                    bra      lbC0178C8

lbC0178BE:          lea      NONE_TXT, a0
                    bsr      WRITE_TEXT
lbC0178C8:          move.w   #$C0, XCOORD
                    move.w   #104, YCOORD
                    tst.b    DIFFICULTY
                    bmi      lbC0178F4
                    beq      lbC017902
                    lea      HARD_TXT, a0
                    bsr      WRITE_TEXT
                    bra      lbC01790C

lbC0178F4:          lea      EASY_TXT, a0
                    bsr      WRITE_TEXT
                    bra      lbC01790C

lbC017902:          lea      NORM_TXT, a0
                    bsr      WRITE_TEXT
lbC01790C:          move.w   #$C0, XCOORD
                    move.w   #128, YCOORD
                    tst.b    PLAYERS
                    beq      lbC017934
                    lea      TWO_TXT, a0
                    bsr      WRITE_TEXT
                    bra      _SELECT_JOYSTICK

lbC017934:          lea      ONE_TXT, a0
                    bsr      WRITE_TEXT

_SELECT_JOYSTICK:   ;bsr      PLAY_MTEST
                    move.w   #$C0, XCOORD
                    move.w   #152, YCOORD
                    lea      JOYSEL_TAB, a0
                    move.b   SEL_JOYSTICK_MODE, d7
                    ext.w    d7
                    lsl.w    #2, d7
                    move.l   0(a0,d7.w), a0
                    bsr      WRITE_TEXT
                    rts

;OLDPLAY:            dcb.b    2, 0
;MUSICON:            dc.w     1

;PLAY_MTEST:         moveq    #0, d0
;                    move.b   MUSIC_TEST, d0
;                    cmp.b    OLDPLAY, d0
;                    bne      lbC017984
;                    rts
;
;lbC017984:          move.w   #0, MUSICON
;                    ;jsr      KILLAUDIO
;                    sf       MUSIC_ON
;                    move.b   d0, OLDPLAY
;                    beq      _PLAY_MUSIC
;                    ;addq.l   #1, d0
;                    ;jsr      PLAYTRACK
;                    rts

;_PLAY_MUSIC:        bsr      PLAY_MUSIC
;                    move.w   #1, MUSICON
;                    rts

JOYSEL_TAB:         dc.l     MT_CD32PAD
                    dc.l     MT_TWOBUTTONS
                    dc.l     MT_ONEBUTTON

MT_CD32PAD:          dc.b     'CD32 PAD', $FF
MT_TWOBUTTONS:       dc.b     'TWO BUTTONS', $FF
MT_ONEBUTTON:        dc.b     'ONE BUTTON', $FF

;MTEST_TAB:          dc.l     MT_TITLE
;                    dc.l     MT_PAPER
;                    dc.l     MT_BIRD
;                    dc.l     MT_BULB
;                    dc.l     MT_TOOT
;                    dc.l     MT_SNAKE
;                    dc.l     MT_ICES
;                    dc.l     MT_LAST
;                    dc.l     MT_ENDLV
;                    dc.l     MT_CONT

;MT_BIRD:            dc.b     'TWO', $FF
;MT_BULB:            dc.b     'THREE', $FF
;MT_TOOT:            dc.b     'FOUR', $FF
;MT_SNAKE:           dc.b     'FIVE', $FF
;MT_ICES:            dc.b     'SIX', $FF
;MT_LAST:            dc.b     'SEVEN', $FF
;MT_ENDLV:           dc.b     'MENTAL', $FF
;MT_CONT:            dc.b     'NEXT STAGE', $FF

OPTIONS_TXT:        dc.b     '           OPTIONS', $FF
SOUNDS_TXT:         dc.b     '     SOUND .......', $FF
DIFFICULTY_TXT:     dc.b     'DIFFICULTY .......', $FF
PLAYERS_TXT:        dc.b     '   PLAYERS .......', $FF
INPUT_TXT:          dc.b     '  JOYSTICK .......', $FF
START_TXT:          dc.b     '            START ', $FF

ONE_TXT:            dc.b     'ONE', $FF
TWO_TXT:            dc.b     'TWO', $FF

MUSIC_TXT:          dc.b     'MUSIC', $FF
MUSFX_TXT:          dc.b     'MUSIC AND FX', $FF
FX_TXT:             dc.b     'FX', $FF

NONE_TXT:           dc.b     'NONE', $FF
EASY_TXT:           dc.b     'EASY', $FF
NORM_TXT:           dc.b     'NORMAL', $FF
HARD_TXT:           dc.b     'HARD', $FF
                    even

WRITE_TEXT:         move.b   (a0)+, d5
                    bmi      lbC017ADA
                    cmp.b    #'$', d5
                    bne      lbC017A66
                    move.w   #'+', d5
                    bra      lbC017AC0

lbC017A66:          cmp.b    #'?', d5
                    bne      lbC017A76
                    move.w   #13, d5
                    bra      lbC017AC0

lbC017A76:          cmp.b    #'-', d5
                    bne      lbC017A86
                    move.w   #11, d5
                    bra      lbC017AC0

lbC017A86:          cmp.b    #'!', d5
                    bne      lbC017A96
                    move.w   #12, d5
                    bra      lbC017AC0

lbC017A96:          cmp.b    #' ', d5
                    bne      lbC017AA6
                    move.w   #16, d5
                    bra      lbC017AC0

lbC017AA6:          cmp.b    #'.', d5
                    bne      lbC017AB6
                    move.w   #10, d5
                    bra      lbC017AC0

lbC017AB6:          sub.b    #'A', d5
                    add.b    #17, d5
                    ext.w    d5
lbC017AC0:          move.w   d5, SPRITE
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
                    addq.w   #8, XCOORD
                    bra      WRITE_TEXT

lbC017ADA:          rts

HISCORE_SCR:        sf       P_DRAW
                    sf       GAME_FADE
                    jsr      INIT_SCRNS
                    jsr      INIT_SCRO
                    bsr      CLEAR_ALLFX
                    move.l   PHYSADR, a0
                    jsr      CLR_SCRN
                    move.l   LOGADR, a0
                    jsr      CLR_SCRN
                    lea      BUFF3_START, a0
                    jsr      CLR_SCRN
                    
                    move.l   PHYSADR, a0
                    lea      ZHISC_NAME, a1
                    jsr      LOADFILE
                    
                    move.l   PHYSADR, a0
                    move.l   LOGADR, a1
                    jsr      COPY_SCRN
                    
                    move.l   SPRITE_AD, a0
                    lea      HITEXT_NAME, a1
                    jsr      LOADFILE
                    
                    move.l   PHYSADR, a0
                    lea      BUFF3_START, a1
                    bsr      COPY_SCRN
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    bsr      SETSCREEN
                    clr.w    SPRITES_CNTA
                    clr.w    SPRITES_CNTB
                    move.l   #COPPER2, COP_LOC
                    move.l   #$FFFFFFFE, COL_SCROLL2
                    jsr      VBL
                    move.w   #$8080, $DFF096
                    clr.w    XREM
                    move.l   #HISC_PAL, LEV_PAL
                    jsr      INIT_SCRO
                    tst.b    INSERT_HSCR
                    beq      JUST_DISP
                    move.l   LOGADR, BUFF_PTR
                    bsr      SETUP_HSCR
                    bsr      DISP_LETTS
                    bsr      FLIP
                    move.l   LOGADR, BUFF_PTR
                    bsr      SETUP_HSCR
                    bsr      DISP_LETTS
                    move.l   #BUFF3_START, BUFF_PTR
                    bsr      DISP_LETTS
                    move.l   LOGADR, BUFF_PTR
                    bsr      CLEAR_UNDS
                    move.b   #1, ANDYFRAME
                    jsr      INIT_FADE
                    tst.b    MUSIC_ON
                    bne      lbC017C80
                    jsr      PLAY_MUSIC
lbC017C80:          clr.w    XREM_A
                    clr.w    XREM_B
                    clr.w    XREM_B
                    clr.w    XREM
lbC017C98:          clr.w    CHARACTER
                    bsr      PRO_MARKER
                    cmp.w    #$64, CHARACTER
                    beq      lbC017CE6
                    clr.w    XSCROLL
                    jsr      SCROLL_BUFF
                    move.l   LOGADR, BUFF_PTR
                    move.l   LOGADR, SCROLL
                    jsr      UNDRAW
                    bsr      DRAW_MARKER
                    bsr      DRAW_CHAR
                    bsr      DRAW_NAME
                    bsr      FLIP
                    bra.s    lbC017C98

lbC017CE6:          move.b   LAST_CHAR, lbB01A18D
                    subq.b   #1, lbB01A18D
                    jsr      SCROLL_BUFF
                    move.l   LOGADR, BUFF_PTR
                    move.l   LOGADR, SCROLL
                    jsr      UNDRAW
                    bsr      DRAW_NAME
                    bsr      FLIP
                    bsr      POS1
                    move.w   #$64, d7
                    jsr      WAIT_SECS
                    move.w   #$20, FADE_CNT
                    sf       FADECOL_ON
                    tst.b    INSERT_HSCR
                    bne      SAVE_HISCORE
GO_TITLE:           
                    move.l   SPRITE_AD, a0
                    lea      INTSP_NAME, a1
                    jsr      LOADFILE
                    
                    sf       SAVE_SPACE
                    jsr      SPRITE_TAB
                    bsr      CLEAR_UNDS
                    move.l   #BOT_CHIP, a0
                    move.l   a0, TILES
                    move.w   #$1F, d7
lbC017DA2:          clr.l    (a0)+
                    dbra     d7, lbC017DA2
                    bra      TITLE_SCR

SAVE_HISCORE:       jsr      WAIT_FDOFF
                    lea      HISCR_TAB, a0
                    lea      HISCORE_NAME, a1
                    moveq    #112, d0
                    jsr      SAVEFILE
                    rts

REAL_RAND:          jsr      RANDOM
                    lea      0, a0
                    move.w   $DFF006, d0
                    move.w   SEED, d1
                    eor.w    d1, d0
                    move.w   #$C8, d7
lbC0181FA:          move.w   (a0)+, d1
                    eor.w    d1, d0
                    eor.w    d0, SEED
                    dbra     d7, lbC0181FA
                    move.w   $DFF006, d1
                    rol.w    d1, d0
                    eor.w    d0, SEED
                    rts

POS1:               move.l   HISC_HERE, a1
                    lea      HISC_NAME, a0
lbC01858C:          move.b   (a0)+, d0
                    cmp.b    #9, d0
                    ble      lbC0185AC
                    cmp.b    #$10, d0
                    ble      lbC0185B6
                    sub.b    #$11, d0
                    add.b    #'A', d0
                    move.b   d0, (a1)+
                    bra      lbC0185FA

lbC0185AC:          add.b    #'0', d0
                    move.b   d0, (a1)+
                    bra      lbC0185FA

lbC0185B6:          cmp.b    #10, d0
                    bne      lbC0185C6
                    move.b   #'.', (a1)+
                    bra      lbC0185FA

lbC0185C6:          cmp.b    #11, d0
                    bne      lbC0185D6
                    move.b   #'-', (a1)+
                    bra      lbC0185FA

lbC0185D6:          cmp.b    #12, d0
                    bne      lbC0185E6
                    move.b   #'!', (a1)+
                    bra      lbC0185FA

lbC0185E6:          cmp.b    #13, d0
                    bne      lbC0185F6
                    move.b   #'?', (a1)+
                    bra      lbC0185FA

lbC0185F6:          move.b   #' ', (a1)+
lbC0185FA:          cmp.l    #ZONE, a0
                    bne.s    lbC01858C
                    rts

COPY_SCRN:          move.w   #$CBF, d7
lbC018608:          move.l   (a0)+, (a1)+
                    move.l   (a0)+, (a1)+
                    move.l   (a0)+, (a1)+
                    move.l   (a0)+, (a1)+
                    dbra     d7, lbC018608
                    rts

INS_HISCORE:        move.l   SCORE, d0
                    lea      HISCR_TAB, a0
                    clr.w    d1
lbC018624:          cmp.l    (a0), d0
                    bgt      lbC01863E
                    lea      14(a0), a0
                    addq.w   #1, d1
                    cmp.w    #8, d1
                    bne.s    lbC018624
                    sf       INSERT_HSCR
                    rts

lbC01863E:          move.w   d1, CURSOR_Y
                    clr.w    CURSOR_X
                    clr.w    MARKER_X
                    clr.w    MARKER_AN
                    move.l   #$A0A0A0A, HISC_NAME
                    move.l   #$A0A0A0A, lbL01A188
                    move.w   #$A0A, lbB01A18C
                    lea      HILAST_NAME, a1
lbC018678:          cmp.w    #7, d1
                    beq      lbC01869E
                    move.l   -14(a1), (a1)
                    move.l   -10(a1), 4(a1)
                    move.l   -6(a1), 8(a1)
                    move.w   -2(a1), 12(a1)
                    lea      -14(a1), a1
                    addq.w   #1, d1
                    bra.s    lbC018678

lbC01869E:          move.l   d0, (a0)+
                    move.l   a0, HISC_HERE
                    move.l   #'....', (a0)+
                    move.l   #'....', (a0)+
                    move.w   #'..', (a0)+
                    st       INSERT_HSCR
                    rts

DRAW_CHAR:          tst.w    CHARACTER
                    beq      lbC018778
                    bmi      lbC018716
                    clr.w    MARKER_AN
lbC0186D2:          lea      HISC_NAME, a0
                    move.w   CHARACTER, d7
                    subq.w   #1, d7
                    move.w   CURSOR_X, d0
                    move.b   d7, 0(a0,d0.w)
                    addq.w   #1, CURSOR_X
                    cmp.w    #10, CURSOR_X
                    bne      lbC018714
                    addq.b   #1, d7
                    tst.w    MARKER_AN
                    bmi      lbC01870E
                    move.b   d7, LAST_CHAR
lbC01870E:          subq.w   #1, CURSOR_X
lbC018714:          rts

lbC018716:          move.w   CURSOR_X, TEMPW
                    move.w   #11, CHARACTER
                    cmp.w    #9, CURSOR_X
                    beq      lbC018758
lbC018734:          bsr.s    lbC0186D2
                    move.b   #11, LAST_CHAR
                    move.w   TEMPW, CURSOR_X
                    subq.w   #1, CURSOR_X
                    bpl.s    lbC018714
                    clr.w    CURSOR_X
                    rts

lbC018758:          cmp.b    #11, LAST_CHAR
                    beq.s    lbC018734
                    bsr      lbC0186D2
                    move.w   #9, CURSOR_X
                    move.b   #11, LAST_CHAR
                    rts

lbC018778:          tst.w    MARKER_AN
                    bmi      lbC0187B6
                    subq.w   #1, MARKER_AN
                    bpl      lbC0187C8
                    move.w   #$FFFA, MARKER_AN
                    move.w   #$11, CHARACTER
                    move.w   CURSOR_X, TEMPW
                    bsr      lbC0186D2
                    move.w   TEMPW, CURSOR_X
                    rts

lbC0187B6:          addq.w   #1, MARKER_AN
                    bmi      lbC018714
                    move.w   #6, MARKER_AN
lbC0187C8:          move.w   CURSOR_X, TEMPW
                    cmp.w    #9, TEMPW
                    bne      lbC0187F8
                    move.b   LAST_CHAR, lbB01A0FB
                    bsr      lbC0186D2
                    move.w   TEMPW, CURSOR_X
                    rts

lbC0187F8:          move.w   #11, CHARACTER
                    bsr      lbC0186D2
                    move.w   TEMPW, CURSOR_X
                    rts

DRAW_NAME:          lea      HISC_NAME, a0
                    move.w   CURSOR_Y, d1
                    addq.w   #3, d1
                    lsl.w    #4, d1
                    move.w   #$60, XCOORD
                    move.w   d1, YCOORD
lbC01882E:          move.b   (a0)+, d7
                    ext.w    d7
                    move.w   d7, SPRITE
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
                    addq.w   #8, XCOORD
                    cmp.l    #ZONE, a0
                    bne.s    lbC01882E
                    rts

PRO_MARKER:         jsr      READ_JOY
                    btst     #2, JOYPOS
                    beq      lbC0188A2
                    bset     #2, OLDJOY
                    beq      lbC018898
                    subq.b   #1, JOY_REPT
                    bpl      lbC0188AA
                    move.b   #2, JOY_REPT
lbC018882:          subq.w   #1, MARKER_X
                    bpl      lbC0188AA
                    move.w   #$20, MARKER_X
                    bra      lbC0188AA

lbC018898:          move.b   #$12, JOY_REPT
                    bra.s    lbC018882

lbC0188A2:          bclr     #2, OLDJOY
lbC0188AA:          btst     #3, JOYPOS
                    beq      lbC0188FA
                    bset     #3, OLDJOY
                    beq      lbC0188F0
                    subq.b   #1, JOY_REPT
                    bpl      lbC018902
                    move.b   #2, JOY_REPT
lbC0188D4:          addq.w   #1, MARKER_X
                    cmp.w    #$21, MARKER_X
                    bne      lbC018902
                    clr.w    MARKER_X
                    bra      lbC018902

lbC0188F0:          move.b   #$12, JOY_REPT
                    bra.s    lbC0188D4

lbC0188FA:          bclr     #3, OLDJOY
lbC018902:          btst     #7, JOYPOS
                    beq      lbC0189B4
                    bset     #7, OLDJOY
                    beq      lbC0189A8
                    subq.b   #1, JOY_REPT
                    bpl      lbC0189BC
                    move.b   #2, JOY_REPT
lbC01892C:          cmp.w    #$20, MARKER_X
                    beq      lbC01899C
                    cmp.w    #$1F, MARKER_X
                    beq      lbC018990
                    cmp.w    #$1A, MARKER_X
                    beq      lbC018970
                    cmp.w    #$1B, MARKER_X
                    bpl      lbC01897C
                    move.w   MARKER_X, d7
                    add.w    #$12, d7
                    move.w   d7, CHARACTER
                    bra      lbC0189BC

lbC018970:          move.w   #$11, CHARACTER
                    bra      lbC0189BC

lbC01897C:          move.w   MARKER_X, d7
                    sub.w    #$10, d7
                    move.w   d7, CHARACTER
                    bra      lbC0189BC

lbC018990:          move.w   #$FFFF, CHARACTER
                    bra      lbC0189BC

lbC01899C:          move.w   #$64, CHARACTER
                    bra      lbC0189BC

lbC0189A8:          move.b   #$12, JOY_REPT
                    bra      lbC01892C

lbC0189B4:          bclr     #7, OLDJOY
lbC0189BC:          rts

DRAW_MARKER:        move.w   MARKER_X, d0
                    lsl.w    #3, d0
                    addq.w   #8, d0
                    move.w   d0, XCOORD
                    move.w   #$C8, YCOORD
                    move.w   #$2C, SPRITE
                    jmp      DUMPSPRITE

DISP_LETTS:         move.w   #$11, SPRITE
                    move.w   #$10, XCOORD
                    move.w   #$D0, YCOORD
_DUMPSPRITE6:       jsr      DUMPSPRITE
                    add.w    #8, XCOORD
                    addq.w   #1, SPRITE
                    cmp.w    #$2B, SPRITE
                    bne.s    _DUMPSPRITE6
                    move.w   #$10, SPRITE
                    jsr      DUMPSPRITE
                    add.w    #8, XCOORD
                    move.w   #10, SPRITE
_DUMPSPRITE7:       jsr      DUMPSPRITE
                    add.w    #8, XCOORD
                    addq.w   #1, SPRITE
                    cmp.w    #$10, SPRITE
                    bne.s    _DUMPSPRITE7
                    rts

JUST_DISP:          move.l   #BUFF1_START, BUFF_PTR
                    bsr      SETUP_HSCR
                    move.l   #BUFF2_START, BUFF_PTR
                    bsr      SETUP_HSCR
                    move.l   #BUFF3_START, BUFF_PTR
                    bsr      SETUP_HSCR
                    bsr      CLEAR_UNDS
                    jsr      VBL
                    move.w   #$8080, $DFF096
                    jsr      INIT_FADE
                    move.w   #$14, TEMPW
                    move.l   #DOCOL_BAND3, DOCOL_RTN
                    move.w   #$190, MARKER_Y
lbC018AB4:          eor.b    #1, ANDYFRAME
                    jsr      SCROLL_BUFF
                    jsr      UNDRAW
                    tst.w    TEMPW
                    bmi      lbC018AF0
                    subq.w   #1, TEMPW
                    bne      _DRAW_FMESS
                    move.w   #$FFEC, TEMPW
                    bra      lbC018AF0

_DRAW_FMESS:        bsr      DRAW_FMESS
                    bra      _VBL1

lbC018AF0:          move.w   #$3E8, d7
lbC018AF4:          dbra     d7, lbC018AF4
                    addq.w   #1, TEMPW
                    bne      _VBL1
                    move.w   #$1E, TEMPW
_VBL1:              jsr      VBL
                    jsr      SCRO_NOW
                    jsr      DOCOL
                    subq.w   #1, MARKER_Y
                    bmi      lbC018B4E
                    jsr      READ_JOY
                    btst     #7, JOYPOS
                    beq      lbC018AB4
                    move.w   #$20, FADE_CNT
                    sf       FADECOL_ON
                    bsr      WAIT_FDOFF
                    bra      OPTIONS_SCR

lbC018B4E:          move.w   #$20, FADE_CNT
                    sf       FADECOL_ON
                    bsr      WAIT_FDOFF
                    bra      GO_TITLE

DRAW_FMESS:         move.w   #$40, XCOORD
                    move.w   #$D8, YCOORD
                    lea      PRESS_MESS, a0
                    bra      WRITE_TEXT

SETUP_HSCR:         lea      HISCR_TAB, a5
                    move.w   #3, d7
lbC018B88:          move.l   (a5)+, d0
                    lea      DEC_NUMS, a1
                    bsr      CONVTODEC
                    lea      DECIMAL, a0
                    move.l   a5, -(sp)
                    move.w   d7, -(sp)
                    move.w   #$1A, d6
                    bsr      DISP_SCORE
                    move.w   (sp), d7
                    move.l   2(sp), a5
                    move.w   #12, d6
                    bsr      DISP_NAME
                    move.w   (sp)+, d7
                    move.l   (sp)+, a5
                    lea      10(a5), a5
                    addq.w   #1, d7
                    tst.b    ANDYFRAME
                    beq      lbC018BDC
                    move.l   #UNDRAW_TABB, UNDRAW_PTR
                    clr.w    SPRITES_CNTB
                    bra      lbC018BEC

lbC018BDC:          move.l   #UNDRAW_TABA, UNDRAW_PTR
                    clr.w    SPRITES_CNTA
lbC018BEC:          cmp.w    #11, d7
                    bne.s    lbC018B88
                    rts

DISP_NAME:          move.w   #10, TEMPW
                    lsl.w    #4, d7
                    move.w   d7, YCOORD
                    lsl.w    #3, d6
                    move.w   d6, XCOORD
lbC018C0C:          move.b   (a5)+, d5
                    cmp.b    #$3F, d5
                    bne      lbC018C1E
                    move.w   #13, d5
                    bra      lbC018C68

lbC018C1E:          cmp.b    #$2D, d5
                    bne      lbC018C2E
                    move.w   #11, d5
                    bra      lbC018C68

lbC018C2E:          cmp.b    #$21, d5
                    bne      lbC018C3E
                    move.w   #12, d5
                    bra      lbC018C68

lbC018C3E:          cmp.b    #$20, d5
                    bne      lbC018C4E
                    move.w   #$10, d5
                    bra      lbC018C68

lbC018C4E:          cmp.b    #$2E, d5
                    bne      lbC018C5E
                    move.w   #10, d5
                    bra      lbC018C68

lbC018C5E:          sub.b    #$41, d5
                    add.b    #$11, d5
                    ext.w    d5
lbC018C68:          move.w   d5, SPRITE
                    move.l   a5, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a5
                    addq.w   #8, XCOORD
                    subq.w   #1, TEMPW
                    bne.s    lbC018C0C
                    rts

DISP_SCORE:         sf       LAST_CHAR
                    move.w   #6, TEMPW
                    lsl.w    #4, d7
                    move.w   d7, YCOORD
                    lsl.w    #3, d6
                    move.w   d6, XCOORD
lbC018CA6:          move.b   (a0)+, d5
                    tst.b    LAST_CHAR
                    bne      lbC018CC0
                    tst.b    d5
                    beq      lbC018CD2
                    move.b   #11, LAST_CHAR
lbC018CC0:          ext.w    d5
                    move.w   d5, SPRITE
                    move.l   a0, -(sp)
                    jsr      DUMPSPRITE
                    move.l   (sp)+, a0
lbC018CD2:          addq.w   #8, XCOORD
                    subq.w   #1, TEMPW
                    bne.s    lbC018CA6
                    rts

FIRST_HISC:         lea      FIRST_HTAB, a0
                    lea      HISCR_TAB, a1
                    move.w   #7, d0
lbC018CF2:          move.l   (a0)+, (a1)+
                    move.l   (a0)+, (a1)+
                    move.l   (a0)+, (a1)+
                    move.w   (a0)+, (a1)+
                    dbra     d0, lbC018CF2
                    rts

FIRST_HTAB:         dc.l     100000
                    dc.b     'TURBO.....'
                    dc.l     70000
                    dc.b     'THE BEAST!'
                    dc.l     60000
                    dc.b     'WEE GEMMS!'
                    dc.l     50000
                    dc.b     'CHRIS.....'
                    dc.l     40000
                    dc.b     'HI-FI MAN!'
                    dc.l     30000
                    dc.b     'JP-TASTIC!'
                    dc.l     20000
                    dc.b     'JOYCE M...'
                    dc.l     10000
                    dc.b     'COL.......'

SET_CSCRO:          move.l   #STAGE_COL, SK_PAL
                    move.l   #STAGE_COL2, SK_PAL2
                    move.b   #6, BAND_HGT
                    move.w   #$18E, COL_MOVE
                    move.w   #$18C, COL_MOVE2
                    bra      INIT_CSCRO

STAGE_COL:          dc.w     0, $F00, 0, 0
STAGE_COL2:         dc.w     0, $F80, $E80, $D80, $C80, $B80, $A80, $980, $880, 0
CHOOSE_BAND:        dc.w     0, 15, 0, $FFFB, 0
DISK_BAND:          dc.w     0, $F0F, 0, $FFFB, 0
BLACK_COLB:         dc.w     0, $FFCE, 0

INIT_CSCRO:         lea      COL_SCROLL2, a0
                    move.l   SK_PAL, a1
                    move.w   (a1), d0
                    move.w   #$4431, d2
lbC018DE8:          move.w   d2, (a0)+
                    move.w   #$FFFE, (a0)+
                    move.w   COL_MOVE, (a0)+
                    move.w   d0, (a0)+
                    move.w   #$180, (a0)+
                    move.w   d0, (a0)+
                    move.w   COL_MOVE2, (a0)+
                    move.w   d0, d1
                    lsr.w    #2, d1
                    and.w    #$F00, d1
                    lsr.w    #4, d1
                    add.w    #$10, d1
                    or.w     d0, d1
                    move.w   d1, (a0)+
                    add.w    BAND_HGT, d2
                    bcs      lbC018E24
                    bsr      COL_BLEND
                    bra.s    lbC018DE8

lbC018E24:          move.l   #$FFFFFFFE, (a0)
                    rts

COL_BLEND:          tst.b    BAND_REP
                    bne      lbC018EC0
                    tst.w    2(a1)
                    bmi      lbC018EA6
                    cmp.w    2(a1), d0
                    beq      lbC018EA6
                    move.w   2(a1), d7
                    move.b   d7, d6
                    move.b   d7, d5
                    and.b    #15, d5
                    lsr.b    #4, d6
                    lsr.w    #8, d7
                    move.b   d0, d4
                    and.b    #15, d4
                    cmp.b    d4, d5
                    beq      lbC018E72
                    bpl      lbC018E6E
                    sub.w    #1, d0
                    bra      lbC018E72

lbC018E6E:          add.w    #1, d0
lbC018E72:          move.b   d0, d4
                    lsr.b    #4, d4
                    cmp.b    d4, d6
                    beq      lbC018E8C
                    bpl      lbC018E88
                    sub.w    #$10, d0
                    bra      lbC018E8C

lbC018E88:          add.w    #$10, d0
lbC018E8C:          move.w   d0, d4
                    lsr.w    #8, d4
                    cmp.b    d4, d7
                    beq      lbC018EA4
                    bpl      lbC018EA0
                    sub.w    #$100, d0
                    rts

lbC018EA0:          add.w    #$100, d0
lbC018EA4:          rts

lbC018EA6:          addq.l   #2, a1
                    move.w   (a1), d0
                    bmi      lbC018EB0
                    rts

lbC018EB0:          move.w   d0, d1
                    addq.l   #2, a1
                    move.w   (a1), d0
                    move.b   #1, BAND_REP
                    rts

lbC018EC0:          addq.w   #1, d1
                    bmi.s    lbC018EA4
                    clr.b    BAND_REP
                    bra      COL_BLEND

PLAY_MAP:           dc.l     PLAY0_MAP
                    dc.l     PLAY0_TLS
                    dc.l     PLAY1_MAP
                    dc.l     PLAY1_TLS
                    dc.l     PLAY2_MAP
                    dc.l     PLAY2_TLS
                    dc.l     PLAY3_MAP
                    dc.l     PLAY3_TLS
                    dc.l     PLAY4_MAP
                    dc.l     PLAY4_TLS
                    dc.l     PLAY5_MAP
                    dc.l     PLAY5_TLS
                    dc.l     PLAY5_MAP
                    dc.l     PLAY5_TLS
                    dc.l     PLAY7_MAP
                    dc.l     PLAY7_TLS

CONV_PLAYF:         lea      PLAY_MAP, a0
                    moveq    #0, d7
                    move.b   LEVEL_NUM, d7
                    lsl.w    #3, d7
                    move.l   4(a0,d7.w), a1
                    move.l   0(a0,d7.w), a0
                    lea      SPEND, a2
                    clr.w    d2
                    clr.w    d3
                    move.l   (a0)+, d0
                    move.w   d0, d7
                    move.w   d0, d5
                    lsl.w    #1, d5
                    move.w   d5, PARA_WIDTH
                    mulu     2(a0), d5
                    lsl.l    #4, d5
                    lea      (a2), a4
                    move.l   a4, PLAYF_BPL1
                    add.l    d5, a4
                    move.l   a4, PLAYF_BPL2
                    add.l    d5, a4
                    move.l   a4, PLAYF_BPL3
                    add.l    d5, a4
                    move.l   a4, PLAYF_BPL4
                    sub.w    #$16, d7
                    lsl.w    #1, d7
                    move.w   d7, PLAY_MOD
                    move.l   (a0)+, d1
lbC018F72:          clr.w    d5
                    lea      (a0), a4
lbC018F76:          move.w   d0, d4
                    subq.w   #1, d4
lbC018F7A:          move.w   (a0)+, d7
                    lsl.w    #7, d7
                    lea      0(a1,d7.w), a3
                    lea      0(a3,d2.w), a3
                    lea      0(a3,d3.w), a3
                    move.w   (a3), (a2)+
                    dbra     d4, lbC018F7A
                    addq.w   #8, d2
                    bclr     #7, d2
                    bne      lbC018FA0
                    sub.l    d0, a0
                    sub.l    d0, a0
                    bra.s    lbC018F76

lbC018FA0:          addq.w   #1, d5
                    cmp.w    d5, d1
                    bne.s    lbC018F76
                    addq.w   #2, d3
                    lea      (a4), a0
                    cmp.w    #6, d3
                    bne.s    lbC018F72
                    clr.l    PRLX_XDIS
                    clr.l    PRLX_YDIS
                    clr.w    PARA_XREM
                    rts

INIT_DUAL:          bsr      SETPANEL
                    lea      COPPER_GAME, a0
                    lea      BPL0_H, a0
                    move.w   #$E0, (a0)
                    move.w   #$E2, 4(a0)
                    move.w   #$E8, 8(a0)
                    move.w   #$EA, 12(a0)
                    move.w   #$F0, $10(a0)
                    move.w   #$F2, $14(a0)
                    move.w   #$F8, $18(a0)
                    move.w   #$FA, $1C(a0)
                    lea      BPL4_H, a0
                    move.w   #$E4, (a0)+
                    move.w   PLAYF_BPL1, (a0)+
                    move.w   #$E6, (a0)+
                    move.w   PLAYF_BPL1+2, (a0)+
                    move.w   #$EC, (a0)+
                    move.w   PLAYF_BPL2, (a0)+
                    move.w   #$EE, (a0)+
                    move.w   PLAYF_BPL2+2, (a0)+
                    move.w   #$F4, (a0)+
                    move.w   PLAYF_BPL3, (a0)+
                    move.w   #$F6, (a0)+
                    move.w   PLAYF_BPL3+2, (a0)+
                    move.w   #$FC, (a0)+
                    move.w   PLAYF_BPL4, (a0)+
                    move.w   #$FE, (a0)+
                    move.w   PLAYF_BPL4+2, (a0)+
                    move.w   #$7401, DUAL_ON
                    move.w   #$3420, USED_CBANK
                    move.w   #$24, lbW048B54
                    move.w   PLAY_MOD, BPLMOD2
                    jsr      CONV_COL7
                    cmp.b    #1, LEVEL_NUM
                    beq      _CONV_COL6
                    cmp.b    #5, LEVEL_NUM
                    beq      _CONV_COL6
                    rts

_CONV_COL6:         jmp      CONV_COL6

SETPANEL:           lea      PANEL, a0
                    lea      PANELBPLS, a1
                    move.w   #3, d7
                    subq.l   #2, a0
lbC019108:          move.l   a0, d0
                    move.w   d0, 6(a1)
                    swap     d0
                    move.w   d0, 2(a1)
                    lea      $30(a0), a0
                    addq.l   #8, a1
                    dbra     d7, lbC019108
                    rts

; ---------------------------------------------------
UNMANGLE:           move.b   (a0)+, d7
                    btst     #7, d7
                    bne      UNMANGLE_BLOCK
                    btst     #6, d7
                    beq      UNMANGLE_STRING
                    btst     #5, d7
                    bne      UNMANGLE_SEQUENCE
UNMANGLE_DIFFERENCE:
                    btst     #4, d7
                    bne      UNMANGLE_D_WORD
UNMANGLE_D_BYTE:    and.w    #15, d7
                    addq.w   #2, d7
                    move.b   -2(a1), d6
                    move.b   -1(a1), d5
                    sub.b    d6, d5
                    move.b   -1(a1), d6
                    add.b    d5, d6
UNMANGLE_NX_D_BYTE: move.b   d6, (a1)+
                    add.b    d5, d6
                    dbra     d7, UNMANGLE_NX_D_BYTE
                    bra      UNMANGLE_CONT

UNMANGLE_D_WORD:    and.w    #15, d7
                    addq.w   #1, d7
                    move.b   -4(a1), d6
                    lsl.w    #8, d6
                    move.b   -3(a1), d6
                    move.b   -2(a1), d5
                    lsl.w    #8, d5
                    move.b   -1(a1), d5
                    sub.w    d6, d5
                    move.b   -2(a1), d6
                    lsl.w    #8, d6
                    move.b   -1(a1), d6
                    add.w    d5, d6
UNMANGLE_NX_D_WORD: move.w   d6, d4
                    move.b   d4, 1(a1)
                    lsr.w    #8, d4
                    move.b   d4, (a1)
                    addq.l   #2, a1
                    add.w    d5, d6
                    dbra     d7, UNMANGLE_NX_D_WORD
                    bra      UNMANGLE_CONT

UNMANGLE_SEQUENCE:  btst     #4, d7
                    bne      UNMANGLE_S_WORD
UNMANGLE_S_BYTE:    and.w    #15, d7
                    addq.w   #2, d7
                    move.b   -1(a1), d6
UNMANGLE_NX_S_BYTE: move.b   d6, (a1)+
                    dbra     d7, UNMANGLE_NX_S_BYTE
                    bra      UNMANGLE_CONT

UNMANGLE_S_WORD:    and.w    #15, d7
                    addq.w   #1, d7
                    move.b   -2(a1), d6
                    lsl.w    #8, d6
                    move.b   -1(a1), d6
UNMANGLE_NX_S_WORD: move.w   d6, d4
                    move.b   d4, 1(a1)
                    lsr.w    #8, d4
                    move.b   d4, (a1)
                    addq.l   #2, a1
                    dbra     d7, UNMANGLE_NX_S_WORD
                    bra      UNMANGLE_CONT

UNMANGLE_STRING:    and.w    #$3F, d7
                    beq      UNMANGLE_TERMINATE
                    subq.w   #1, d7
UNMANGLE_NX_STRING: move.b   (a0)+, (a1)+
                    dbra     d7, UNMANGLE_NX_STRING
                    bra      UNMANGLE_CONT

UNMANGLE_BLOCK:     btst     #6, d7
                    beq      UNMANGLE_SHORT
                    btst     #5, d7
                    beq      UNMANGLE_MEDIUM
UNMANGLE_LONG:      and.w    #$1F, d7
                    lsl.w    #8, d7
                    move.w   d7, d5
                    move.b   (a0)+, d5
                    addq.w   #3, d5
                    moveq    #0, d7
                    move.b   (a0)+, d7
                    addq.w   #5, d7
UNMANGLE_COPY_BLOCK:
                    subq.w   #1, d7
                    neg.w    d5
                    lea      (a1), a2
                    lea      0(a2,d5.w), a2
UNMANGLE_NX_BLK:    move.b   (a2)+, (a1)+
                    dbra     d7, UNMANGLE_NX_BLK
                    bra      UNMANGLE_CONT

UNMANGLE_MEDIUM:    move.b   d7, d6
                    and.w    #$1C, d7
                    and.w    #3, d6
                    lsl.w    #8, d6
                    move.w   d6, d5
                    move.b   (a0)+, d5
                    addq.w   #3, d5
                    lsr.w    #2, d7
                    addq.b   #4, d7
                    and.w    #$FF, d7
                    bra.s    UNMANGLE_COPY_BLOCK

UNMANGLE_SHORT:     and.w    #$3F, d7
                    move.w   d7, d5
                    addq.w   #3, d5
                    moveq    #3, d7
                    bra.s    UNMANGLE_COPY_BLOCK

UNMANGLE_CONT:      bra      UNMANGLE

UNMANGLE_TERMINATE: rts
; ---------------------------------------------------

MT_INIT:            move.l   a0, MT_DATAP
                    lea      MT_MULU(pc), a1
                    move.l   a0, d0
                    add.l    #12, d0
                    moveq    #$1F, d1
                    moveq    #$1E, d3
MT_LOP4:            move.l   d0, (a1)+
                    add.l    d3, d0
                    dbra     d1, MT_LOP4
                    lea      $3B8(a0), a1
                    moveq    #$7F, d0
                    moveq    #0, d1
                    moveq    #0, d2
MT_LOP2:            move.b   (a1)+, d1
                    cmp.b    d2, d1
                    ble.s    MT_LOP
                    move.l   d1, d2
MT_LOP:             dbra     d0, MT_LOP2
                    addq.w   #1, d2
                    asl.l    #8, d2
                    asl.l    #2, d2
                    lea      4(a1,d2.l), a2
                    lea      MT_SAMPLESTARTS(pc), a1
                    lea      $2A(a0), a0
                    moveq    #$1E, d0
MT_LOP3:            clr.l    (a2)
                    move.l   a2, (a1)+
                    moveq    #0, d1
                    move.b   d1, 2(a0)
                    move.w   (a0), d1
                    asl.l    #1, d1
                    add.l    d1, a2
                    add.l    d3, a0
                    dbra     d0, MT_LOP3
                    move.l   $78, $70(a1)
                    or.b     #2, $BFE001
                    move.b   #6, -$84(a1)
                    moveq    #0, d0
                    lea      $DFF000, a0
                    move.w   d0, $A8(a0)
                    move.w   d0, $B8(a0)
                    move.w   d0, $C8(a0)
                    move.w   d0, $D8(a0)
                    move.b   d0, -$80(a1)
                    move.b   d0, -$83(a1)
                    move.w   d0, -$82(a1)
                    rts

MT_END:             moveq    #0, d0
                    lea      $DFF000, a0
                    move.w   d0, $A8(a0)
                    move.w   d0, $B8(a0)
                    move.w   d0, $C8(a0)
                    move.w   d0, $D8(a0)
                    move.w   #15, $DFF096
                    rts

MT_MUSIC:           move.l   MT_DATAP, a0
                    lea      MT_VOICE1(pc), a4
                    addq.b   #1, -$83(a4)
                    move.b   MT_COUNTER(pc), d0
                    cmp.b    MT_SPEED(pc), d0
                    blt      MT_NONEW
                    moveq    #0, d0
                    move.b   d0, -$83(a4)
                    move.w   d0, -$7E(a4)
                    move.l   MT_DATAP, a0
                    lea      $3B8(a0), a2
                    lea      $43C(a0), a0
                    moveq    #0, d1
                    move.b   MT_SONGPOS(pc), d0
                    move.b   0(a2,d0.w), d1
                    lsl.w    #8, d1
                    lsl.w    #2, d1
                    add.w    MT_PATTPOS(pc), d1
                    lea      $DFF0A0, a5
                    lea      MT_SONGPOS(pc), a1
                    lea      MT_PLAYVOICE(pc), a6
                    jsr      (a6)
                    addq.l   #4, d1
                    lea      $DFF0B0, a5
                    lea      MT_VOICE2(pc), a4
                    jsr      (a6)
                    addq.l   #4, d1
                    lea      $DFF0C0, a5
                    lea      MT_VOICE3(pc), a4
                    jsr      (a6)
                    addq.l   #4, d1
                    lea      $DFF0D0, a5
                    lea      MT_VOICE4(pc), a4
                    jsr      (a6)
                    move.w   MT_DMACON(pc), d0
                    beq.s    MT_NODMA
                    lea      $BFD000, a3
                    move.b   #$7F, $D00(a3)
                    move.w   #$2000, $DFF09C
                    move.w   #$A000, $DFF09A
                    move.l   a0,-(a7)
                    move.l   CUR_VBR, a0
                    move.l   #MT_IRQ1, $78(a0)
                    move.l   (a7)+,a0
                    moveq    #0, d0
                    move.b   d0, $E00(a3)
                    move.b   #$A8, $400(a3)
                    move.b   d0, $500(a3)
                    or.w     #$8000, -$D2(a4)
                    move.b   #$11, $E00(a3)
                    move.b   #$81, $D00(a3)
MT_NODMA:           add.w    #$10, -$D6(a4)
                    cmp.w    #$400, -$D6(a4)
                    bne.s    MT_EXIT
MT_NEXT:            clr.w    -$D6(a4)
                    clr.b    -$D3(a4)
                    addq.b   #1, -$D4(a4)
                    and.b    #$7F, -$D4(a4)
                    move.b   -2(a2), d0
                    cmp.b    MT_SONGPOS(pc), d0
                    bne.s    MT_EXIT
                    move.b   -1(a2), -$D4(a4)
MT_EXIT:            tst.b    -$D3(a4)
                    bne.s    MT_NEXT
                    rts

MT_NONEW:           lea      $DFF0A0, a5
                    lea      MT_COM(pc), a6
                    jsr      (a6)
                    lea      MT_VOICE2(pc), a4
                    lea      $DFF0B0, a5
                    jsr      (a6)
                    lea      MT_VOICE3(pc), a4
                    lea      $DFF0C0, a5
                    jsr      (a6)
                    lea      MT_VOICE4(pc), a4
                    lea      $DFF0D0, a5
                    jsr      (a6)
                    tst.b    -211(a4)
                    bne.s    MT_NEXT
                    rts

MT_IRQ1:            tst.b    $BFDD00
                    move.w   MT_DMACON(pc), $DFF096
                    move.l   a0,-(a7)
                    move.l   CUR_VBR, a0
                    move.l   #MT_IRQ2, $78(a0)
                    move.l   (a7)+,a0
                    move.w   #$2000, $DFF09C
                    rte

MT_IRQ2:            tst.b    $BFDD00
                    movem.l  a3/a4, -(sp)
                    lea      MT_VOICE1(pc), a4
                    lea      $DFF000, a3
                    move.l   10(a4), $A0(a3)
                    move.w   14(a4), $A4(a3)
                    move.l   $26(a4), $B0(a3)
                    move.w   $2A(a4), $B4(a3)
                    move.l   $42(a4), $C0(a3)
                    move.w   $46(a4), $C4(a3)
                    move.l   $5E(a4), $D0(a3)
                    move.w   $62(a4), $D4(a3)
                    movem.l  (sp)+, a3/a4
                    move.b   #0, $BFDE00
                    move.b   #$7F, $BFDD00
                    move.l   a0,-(a7)
                    move.l   CUR_VBR, a0
                    move.l   MT_OLDIRQ(pc), $78(a0)
                    move.l   (a7)+,a0
                    move.w   #$2000, $DFF09C
                    move.w   #$2000, $DFF09A
                    rte

MT_PLAYVOICE:       move.l   0(a0,d1.l), (a4)
                    moveq    #0, d2
                    move.b   2(a4), d2
                    lsr.b    #4, d2
                    move.b   (a4), d0
                    and.b    #$F0, d0
                    or.b     d0, d2
                    beq      MT_OLDINSTR
                    asl.w    #2, d2
                    move.l   0(a1,d2.l), 4(a4)
                    move.l   MT_MULU(pc,d2.w), a3
                    move.w   (a3)+, 8(a4)
                    move.w   (a3)+, $12(a4)
                    move.l   4(a4), d0
                    moveq    #0, d3
                    move.w   (a3)+, d3
                    beq      MT_NOLOOP
                    asl.w    #1, d3
                    add.l    d3, d0
                    move.l   d0, 10(a4)
                    move.w   -2(a3), d0
                    add.w    (a3), d0
                    move.w   d0, 8(a4)
                    bra      MT_HEJASVERIGE

MT_MULU:            dcb.l    $20, 0

MT_NOLOOP:          add.l    d3, d0
                    move.l   d0, 10(a4)
MT_HEJASVERIGE:     move.w   (a3), 14(a4)
                    move.w   $12(a4), 8(a5)
MT_OLDINSTR:        move.w   (a4), d3
                    and.w    #$FFF, d3
                    beq      MT_COM2
                    tst.w    8(a4)
                    beq.s    MT_STOPSOUND
                    move.b   2(a4), d0
                    and.b    #15, d0
                    cmp.b    #5, d0
                    beq.s    MT_SETPORT
                    cmp.b    #3, d0
                    beq.s    MT_SETPORT
                    move.w   d3, $10(a4)
                    move.w   $1A(a4), $DFF096
                    clr.b    $19(a4)
                    move.l   4(a4), (a5)
                    move.w   8(a4), 4(a5)
                    move.w   $10(a4), 6(a5)
                    move.w   $1A(a4), d0
                    or.w     d0, $44C(a6)
                    bra      MT_COM2

MT_STOPSOUND:       move.w   $1A(a4), $DFF096
                    bra      MT_COM2

MT_SETPORT:         move.w   (a4), d2
                    and.w    #$FFF, d2
                    move.w   d2, $16(a4)
                    move.w   $10(a4), d0
                    clr.b    $14(a4)
                    cmp.w    d0, d2
                    beq.s    MT_CLRPORT
                    bge      MT_COM2
                    move.b   #1, $14(a4)
                    bra      MT_COM2

MT_CLRPORT:         clr.w    $16(a4)
                    rts

MT_PORT:            moveq    #0, d0
                    move.b   3(a4), d2
                    beq.s    MT_PORT2
                    move.b   d2, $15(a4)
                    move.b   d0, 3(a4)
MT_PORT2:           tst.w    $16(a4)
                    beq.s    MT_RTS
                    move.b   $15(a4), d0
                    tst.b    $14(a4)
                    bne.s    MT_SUB
                    add.w    d0, $10(a4)
                    move.w   $16(a4), d0
                    cmp.w    $10(a4), d0
                    bgt.s    MT_PORTOK
                    move.w   $16(a4), $10(a4)
                    clr.w    $16(a4)
MT_PORTOK:          move.w   $10(a4), 6(a5)
MT_RTS:             rts

MT_SUB:             sub.w    d0, $10(a4)
                    move.w   $16(a4), d0
                    cmp.w    $10(a4), d0
                    blt.s    MT_PORTOK
                    move.w   $16(a4), $10(a4)
                    clr.w    $16(a4)
                    move.w   $10(a4), 6(a5)
                    rts

MT_SIN:             dc.b     0, $18
                    dc.b     $31, $4A, $61, $78, $8D, $A1, $B4, $C5, $D4, $E0, $EB, $F4
                    dc.b     $FA, $FD, $FF, $FD, $FA, $F4, $EB, $E0, $D4, $C5, $B4, $A1
                    dc.b     $8D, $78, $61, $4A, $31, $18

MT_VIB:             move.b   3(a4), d0
                    beq.s    MT_VIB2
                    move.b   d0, $18(a4)
MT_VIB2:            move.b   $19(a4), d0
                    lsr.w    #2, d0
                    and.w    #$1F, d0
                    moveq    #0, d2
                    move.b   MT_SIN(pc,d0.w), d2
                    move.b   $18(a4), d0
                    and.w    #15, d0
                    mulu     d0, d2
                    lsr.w    #7, d2
                    move.w   $10(a4), d0
                    tst.b    $19(a4)
                    bmi.s    MT_VIBSUB
                    add.w    d2, d0
                    bra.s    MT_VIB3

MT_VIBSUB:          sub.w    d2, d0
MT_VIB3:            move.w   d0, 6(a5)
                    move.b   $18(a4), d0
                    lsr.w    #2, d0
                    and.w    #$3C, d0
                    add.b    d0, $19(a4)
lbC01970C:          rts

MT_ARPLIST:         dc.b     0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0
                    dc.b     1, 2, 0, 1, 2, 0, 1

MT_ARP:             moveq    #0, d0
                    move.b   MT_COUNTER(pc), d0
                    move.b   MT_ARPLIST(pc,d0.w), d0
                    beq.s    MT_NORMPER
                    cmp.b    #2, d0
                    beq.s    MT_ARP2
MT_ARP1:            move.b   3(a4), d0
                    lsr.w    #4, d0
                    bra.s    MT_ARPDO

MT_ARP2:            move.b   3(a4), d0
                    and.w    #15, d0
MT_ARPDO:           asl.w    #1, d0
                    move.w   $10(a4), d1
                    lea      MT_PERIODS(pc), a0
MT_ARP3:            cmp.w    (a0)+, d1
                    blt.s    MT_ARP3
                    move.w   -2(a0,d0.w), 6(a5)
                    rts

MT_NORMPER:         move.w   $10(a4), 6(a5)
                    rts

MT_COM:             move.w   2(a4), d0
                    and.w    #$FFF, d0
                    beq.s    MT_NORMPER
                    move.b   2(a4), d0
                    and.b    #15, d0
                    beq.s    MT_ARP
                    cmp.b    #6, d0
                    beq.s    MT_VOLVIB
                    cmp.b    #4, d0
                    beq      MT_VIB
                    cmp.b    #5, d0
                    beq.s    MT_VOLPORT
                    cmp.b    #3, d0
                    beq      MT_PORT
                    cmp.b    #1, d0
                    beq.s    MT_PORTUP
                    cmp.b    #2, d0
                    beq.s    MT_PORTDOWN
                    move.w   $10(a4), 6(a5)
                    cmp.b    #10, d0
                    beq.s    MT_VOLSLIDE
                    rts

MT_PORTUP:          moveq    #0, d0
                    move.b   3(a4), d0
                    sub.w    d0, $10(a4)
                    move.w   $10(a4), d0
                    cmp.w    #$71, d0
                    bpl.s    MT_PORTUP2
                    move.w   #$71, $10(a4)
MT_PORTUP2:         move.w   $10(a4), 6(a5)
                    rts

MT_PORTDOWN:        moveq    #0, d0
                    move.b   3(a4), d0
                    add.w    d0, $10(a4)
                    move.w   $10(a4), d0
                    cmp.w    #$358, d0
                    bmi.s    MT_PORTDOWN2
                    move.w   #$358, $10(a4)
MT_PORTDOWN2:       move.w   $10(a4), 6(a5)
                    rts

MT_VOLVIB:          bsr      MT_VIB2
                    bra.s    MT_VOLSLIDE

MT_VOLPORT:         bsr      MT_PORT2
MT_VOLSLIDE:        moveq    #0, d0
                    move.b   3(a4), d0
                    lsr.b    #4, d0
                    beq.s    MT_VOL3
                    add.b    d0, $13(a4)
                    cmp.b    #$40, $13(a4)
                    bmi.s    MT_VOL2
                    move.b   #$40, $13(a4)
MT_VOL2:            move.w   $12(a4), 8(a5)
                    rts

MT_VOL3:            move.b   3(a4), d0
                    and.b    #15, d0
                    sub.b    d0, $13(a4)
                    bpl.s    MT_VOL4
                    clr.b    $13(a4)
MT_VOL4:            move.w   $12(a4), 8(a5)
                    rts

MT_COM2:            move.b   2(a4), d0
                    and.b    #15, d0
                    beq      MT_RTS
                    cmp.b    #14, d0
                    beq.s    MT_FILTER
                    cmp.b    #13, d0
                    beq.s    MT_PATTBREAK
                    cmp.b    #11, d0
                    beq.s    MT_SONGJMP
                    cmp.b    #12, d0
                    beq.s    MT_SETVOL
                    cmp.b    #15, d0
                    beq.s    MT_SETSPEED
                    rts

MT_FILTER:          move.b   3(a4), d0
                    and.b    #1, d0
                    asl.b    #1, d0
                    and.b    #$FD, $BFE001
                    or.b     d0, $BFE001
                    rts

MT_PATTBREAK:       move.b   #1, $44B(a6)
                    rts

MT_SONGJMP:         move.b   #1, $44B(a6)
                    move.b   3(a4), d0
                    subq.b   #1, d0
                    move.b   d0, $44A(a6)
                    rts

MT_SETVOL:          cmp.b    #$40, 3(a4)
                    bls.s    MT_SV2
                    move.b   #$40, 3(a4)
MT_SV2:             moveq    #0, d0
                    move.b   3(a4), d0
                    move.b   d0, $13(a4)
                    move.w   d0, 8(a5)
                    rts

MT_SETSPEED:        moveq    #0, d0
                    move.b   3(a4), d0
                    cmp.b    #$1F, d0
                    bls.s    MT_SP2
                    moveq    #$1F, d0
MT_SP2:             tst.w    d0
                    bne.s    MT_SP3
                    moveq    #1, d0
MT_SP3:             move.b   d0, $446(a6)
                    rts

MT_PERIODS:         dc.w     $358, $328, $2FA, $2D0, $2A6, $280, $25C, $23A, $21A, $1FC
                    dc.w     $1E0, $1C5, $1AC, $194, $17D, $168, $153, $140, $12E, $11D
                    dc.w     $10D, $FE, $F0, $E2, $D6, $CA, $BE, $B4, $AA, $A0, $97, $8F
                    dc.w     $87, $7F, $78, $71, 0
MT_SPEED:           dc.b     6
MT_COUNTER:         dc.b     0
MT_PATTPOS:         dc.w     0
MT_SONGPOS:         dc.b     0
MT_BREAK:           dc.b     0
MT_DMACON:          dc.w     0
MT_SAMPLESTARTS:    dcb.l    31, 0
MT_VOICE1:          dcb.l    6, 0
                    dc.l     1
MT_VOICE2:          dcb.l    6, 0
                    dc.l     2
MT_VOICE3:          dcb.l    6, 0
                    dc.l     4
MT_VOICE4:          dcb.l    6, 0
                    dc.l     8
MT_OLDIRQ:          dc.l     0
MT_DATAP:           dc.l     0

PLAY_MUSIC:         sf.b     MUSIC_ON
                    move.l   MUSIC_AD, a0
                    jsr      MT_INIT
                    move.b   #1, MUSIC_ON
                    move.w   #64, MUSIC_VOL
                    rts

; a0 = buffer
; a1 = filename
LOADFILE:           movem.l  d0-d7/a0-a6, -(sp)
                    lea      (a0), a5
                    lea      (a1), a0
                    jsr      DOS_LOADFILE
                    movem.l  (sp)+, d0-d7/a0-a6
                    rts

; a0 = buffer
; a1 = filename
LOADFILE_OPT:       movem.l  d1-d7/a0-a6, -(sp)
                    lea      (a0), a5
                    lea      (a1), a0
                    jsr      DOS_LOADFILE_OPT
                    movem.l  (sp)+, d1-d7/a0-a6
                    rts

; a0 = buffer
; a1 = filename
; d0 = length
SAVEFILE:           movem.l  d1-d7/a0-a6, -(sp)
                    lea      (a0), a5
                    lea      (a1), a0
                    jsr      DOS_SAVEFILE
                    movem.l  (sp)+, d1-d7/a0-a6
                    rts

PRO_SFX:            clr.w    d6
                    addq.w   #1, CLOCK
                    move.w   CLOCK, d5
                    bpl      lbC019BA6
                    clr.w    CLOCK
lbC019BA6:          move.w   $DFF002, d7
                    btst     #0, d7
                    beq      lbC019BC0
                    cmp.w    AUD0DUR, d5
                    bmi      lbC019BC0
                    addq.w   #1, d6
lbC019BC0:          btst     #1, d7
                    beq      lbC019BD4
                    cmp.w    AUD1DUR, d5
                    bmi      lbC019BD4
                    addq.w   #2, d6
lbC019BD4:          btst     #2, d7
                    beq      lbC019BE8
                    cmp.w    AUD2DUR, d5
                    bmi      lbC019BE8
                    addq.w   #4, d6
lbC019BE8:          btst     #3, d7
                    beq      lbC019BFC
                    cmp.w    AUD3DUR, d5
                    bmi      lbC019BFC
                    addq.w   #8, d6
lbC019BFC:          move.w   d6, $DFF096
                    rts

ADD_SFX:            cmp.b    #2, AUDIO
                    beq      lbC019C1A
                    tst.b    AUDIO
                    bpl      lbC019CD6
lbC019C1A:          movem.l  d6/d7/a4, -(sp)
                    bsr      GET_CHANNEL
                    beq      lbC019CD8
lbC019C26:          btst     #0, d6
                    beq      lbC019C48
                    move.w   CLOCK, d7
                    add.w    (a5), d7
                    move.w   d7, AUD0DUR
                    moveq    #1, d6
                    lea      $DFF0A0, a4
                    bra      lbC019CAA

lbC019C48:          btst     #1, d6
                    beq      lbC019C6A
                    move.w   CLOCK, d7
                    add.w    (a5), d7
                    move.w   d7, AUD1DUR
                    moveq    #2, d6
                    lea      $DFF0B0, a4
                    bra      lbC019CAA

lbC019C6A:          btst     #2, d6
                    beq      lbC019C8C
                    move.w   CLOCK, d7
                    add.w    (a5), d7
                    move.w   d7, AUD2DUR
                    moveq    #4, d6
                    lea      $DFF0C0, a4
                    bra      lbC019CAA

lbC019C8C:          btst     #3, d6
                    beq      lbC019CD2
                    move.w   CLOCK, d7
                    add.w    (a5), d7
                    move.w   d7, AUD3DUR
                    moveq    #8, d6
                    lea      $DFF0D0, a4
lbC019CAA:          move.w   d6, $DFF096
                    nop
                    nop
                    nop
                    nop
                    move.l   8(a5), (a4)+
                    move.w   2(a5), (a4)+
                    move.w   4(a5), (a4)+
                    move.w   6(a5), (a4)+
                    bset     #15, d6
                    move.w   d6, $DFF096
lbC019CD2:          movem.l  (sp)+, d6/d7/a4
lbC019CD6:          rts

lbC019CD8:          subq.w   #1, FORCE_CHAN
                    bpl      lbC019CEA
                    move.w   #3, FORCE_CHAN
lbC019CEA:          moveq    #0, d6
                    move.w   FORCE_CHAN, d7
                    bset     d7, d6
                    bra      lbC019C26

GET_CHANNEL:        move.w   $DFF002, d6
                    not.w    d6
                    and.w    #15, d6
                    rts

LANDTOK_FX:         dc.w     4, $15A, $1AF, $40
                    dc.l     PSAMS
SPIN_FX:            dc.w     5, $1C7, $1AF, $40
                    dc.l     SPIN_SAM
SKID_FX:            dc.w     $18, $7F7, $1AF, $20
                    dc.l     SKID_SAM
HEART_FX:           dc.w     $16, $765, $1AF, $18
                    dc.l     HEART_SAM
ZZDAMA_FX:          dc.w     $13, $62D, $1AF, $40
                    dc.l     DAMA_SAM
WHIP_FX:            dc.w     $12, $73C, $165, $40
                    dc.l     WHIP_SAM
SMART_FX:           dc.w     $31, $1009, $1AF, $40
                    dc.l     SMART_SAM
BREAK_FX:           dc.w     $2A, $10B6, $165, $40
                    dc.l     BREAK_SAM
SPRING_FX:          dc.w     14, $4CC, $1AF, $40
                    dc.l     SPRING_SAM
RESTART_FX:         dc.w     $16, $727, $1AF, $40
                    dc.l     RESTART_SAM
POP_FX:             dc.w     1, $64, $1AF, $40
                    dc.l     POP_SAM
FSTEP_FX:           dc.w     5, $1DA, $1AF, $20
                    dc.l     STEP_SAM
FIRE_FX:            dc.w     13, $452, $1AF, $40
                    dc.l     FIRE_SAM
BARK_FX:            dc.w     $18, $7EE, $165, $40
                    dc.l     SNARL_SAM
WIMP_FX:            dc.w     12, $3F2, $1AF, $40
                    dc.l     WIMP_SAM
SNARL_FX:           dc.w     $18, $7EE, $1AF, $40
                    dc.l     SNARL_SAM
EDIE_FX:            dc.w     $2E, $10F1, $180, $30
                    dc.l     EDIE_SAM
ZLDAMA_FX:          dc.w     10, $34C, $1AF, $40
                    dc.l     ZLDAMA_SAM
SKID2_FX:           dc.w     12, $3FB, $1AF, $20
                    dc.l     SKID_SAM
SKID3_FX:           dc.w     6, $1FD, $1AF, $20
                    dc.l     SKID_SAM
EPOP_FX:            dc.w     1, $64, $180, $40
                    dc.l     POP_SAM
TWEET1_FX:          dc.w     10, $2BE, $1FF, $40
                    dc.l     END_PSAMS
TWEET2_FX:          dc.w     8, $2BE, $1BF, $40
                    dc.l     END_PSAMS
TWEET3_FX:          dc.w     7, $2BE, $18D, $40
                    dc.l     END_PSAMS
TWEET4_FX:          dc.w     7, $2BE, $165, $40
                    dc.l     END_PSAMS
COLLECT_FX:         dc.w     10, $3C1, $180, $30
                    dc.l     COLL_SAM
CHUPPA_FX:          dc.w     $24, $BB4, $1AF, $40
                    dc.l     CHUPPA_SAM
SLIDE_FX:           dc.w     10, $372, $1AF, $40
                    dc.l     SLIDE_SAM
COLPOW_FX:          dc.w     $25, $C2D, $1AF, $40
                    dc.l     CTOKEN_SAM
FLIP_FX:            dc.w     $24, $BF7, $1AF, $40
                    dc.l     FLIP_SAM
FLIPZZ_FX:          dc.w     $1E, $BF7, $165, $40
                    dc.l     FLIP_SAM
EAT_FX:             dc.w     8, $2AE, $1AF, $40
                    dc.l     EAT_SAM
BRICK_FX:           dc.w     11, $3C4, $1AF, $40
                    dc.l     BRICK_SAM
ICEPLAT_FX:         dc.w     $2B, $E0F, $1AF, $40
                    dc.l     END_PSAMS
SNOWF_FX:           dc.w     $1A, $88F, $1AF, $40
                    dc.l     SNOW_FXAD
LAVA_FX:            dc.w     $1B, $905, $1AF, $40
                    dc.l     END_PSAMS
HISS_FX:            dc.w     $1E, $A04, $1AF, $40
                    dc.l     lbL0471E8
PLATF_FX:           dc.w     $17, $7B5, $1AF, $40
                    dc.l     HISS_FXAD
SPARK_FX:           dc.w     $1A, $8BF, $1AF, $40
                    dc.l     END_PSAMS
BADHIT_FX:          dc.w     $19, $3F2, $37E, $40
                    dc.l     WIMP_SAM
LAND_FX:            dc.w     $1E, $3C4, $45E, $40
                    dc.l     BRICK_SAM
BOING_FX:           dc.w     $14, $4CC, $254, $40
                    dc.l     SPRING_SAM
MENT_SPDATA:        dc.l     0
END_MENT:           dc.l     0
DISK_BUFFER_ADDR:   dc.l     0
DRIVE:              dc.w     0
WHERE:              dc.l     0
DISK_SEC1:          dc.l     0
DISK_SEC2:          dc.l     0
MUSIC_PTR:          dc.l     0
MUSIC_AD:           dc.l     0
MUSIC_VOL:          dc.w     0
MUSIC_ON:           dc.b     0
DISK_IN:            dc.b     0
FORCE_CHAN:         dc.w     0
CLOCK:              dc.w     0
AUD0DUR:            dc.w     0
AUD1DUR:            dc.w     0
AUD2DUR:            dc.w     0
AUD3DUR:            dc.w     0
A_MBLK_SPRS:        dc.l     0
A_END_MB:           dc.l     0
A_BON_MAP:          dc.l     0
A_BON_REF:          dc.l     0
A_SNK_M1:           dc.l     0
A_SNK_R1:           dc.l     0
A_SNK_M2:           dc.l     0
A_SNK_R2:           dc.l     0
A_SNK_M3:           dc.l     0
A_SNK_R3:           dc.l     0
A_MBLK_SP2:         dc.l     0
A_END_MB2:          dc.l     0
A_END_ADD:          dc.l     0
BON_START:          dc.w     0
ICON_TAB:           dcb.l    9, 0
LAST_ICON:          dcb.l    2, 0
RBALLS_TAB:         dc.w     1, $9C, $C8, $FFFF, $FFFE, 0, $C8, $10E, 2, $FFFE, 0, $3C
                    dc.w     $10E, 3, $FFFE, 0, $F0, $10E, $FFFE, $FFFE
BALLS_TAB:          dcb.w    4, 0
lbW019FC6:          dcb.w    $10, 0
BALLS_ON:           dc.w     1
TILE_HIT:           dc.w     0
TILES_LEFT:         dc.w     0
TILE_CNT:           dc.w     0
TILEX:              dc.w     0
TILEY:              dc.w     0
DEL_TILES:          dcb.l    $18, 0
BOUNCE_TAB:         dc.w     $FFFD, $FFFE, $FFFF, 1, 2, 3
SLOW_TIME:          dc.w     0
FASTER:             dc.w     0
ZOON_X:             dc.w     0
ZOON_XDIS:          dc.w     0
ZOON_SPR:           dc.w     0
ZOON_FACE:          dc.w     0
ZOON_FRM:           dc.w     0
ZOON_SPK:           dc.w     0
ZOON_SPKSP:         dc.w     $14
BONUS_SP:           dc.w     $A4
BALL_X:             dc.w     0
BALL_Y:             dc.w     0
BALL_XDIS:          dc.w     0
BALL_YDIS:          dc.w     0
BALL_SPR:           dc.w     0
HIGH_RES:           dc.w     0
SCORE:              dc.l     0
HISCORE:            dc.l     $7D0
OLDSCORE:           dc.l     $FFFFFFFF
DECIMAL:            dc.l     0
                    dc.w     0
DEC_NUMS:           dc.l     100000
                    dc.l     10000
                    dc.l     1000
                    dc.l     100
                    dc.l     10
                    dc.l     1
DEC_NUMS2:          dc.l     10
                    dc.l     1
BLINK_CNT:          dc.w     0
ENERGY:             dc.w     0
OLDENERGY:          dc.w     0
REVIVE:             dc.b     1, 0
EXIT_DIR:           dc.w     6
OLDEX_DIR:          dc.w     0
PERCENT:            dc.w     0
OLDPERC:            dc.w     0
PERC_ADD:           dc.w     0
TIME:               dc.w     300
START_TIME:         dc.w     300
TIME_CNT:           dc.w     49
OLDTIME:            dc.w     0
LIVES:              dc.w     $19
OLDLIVES:           dc.w     0
RESETFLAG:          dc.b     0
TOPPAN_DONE:        dc.b     0
BOTPAN_DONE:        dc.b     0
ON_BONUS:           dc.b     0
OLD_FSTEP:          dc.b     $FF
BEG_GAME:           dc.b     0
JUST_ENDED:         dcb.b    2, 0
RESET_LN:           dcb.b    2, 0
LNS_TIL_COL:        dcb.b    2, 0
TEMPL:              dcb.b    4, 0
HISC_HERE:          dc.l     0
SCREEN_AD:          dc.l     0
TEXT_PTR:           dc.l     0
PAGE:               dc.w     0
MARKER_X:           dc.w     0
MARKER_Y:           dc.w     0
DIGIT1:             dc.b     0
DIGIT2:             dc.b     0
CURSOR_X:           dc.w     0
CURSOR_Y:           dc.w     0
MARKER_AN:          dc.w     0
WATER_SPR:          dc.w     0
CHARACTER:          dc.b     0
lbB01A0FB:          dc.b     0
TWEET_CNT:          dc.w     0
SLIDE_SND:          dc.w     0
SAVE_SPACE:         dc.b     0
INSERT_HSCR:        dc.b     1
LAST_CHAR:          dc.b     0
PLAYERS:            dc.b     0
AUDIO:              dc.b     1
DIFFICULTY:         dc.b     0
PLAY_CHG:           dc.b     0
AUDIO_CHG:          dc.b     0
DIFF_CHG:           dc.b     0
PLAY_CHOOSE:        dc.b     0
PLAY_CHAR:          dc.b     0
PLAY1_CHAR:         dc.b     0
PLAY2_CHAR:         dc.b     0
PLAY1_SAVED:        dc.b     0
PLAY2_SAVED:        dc.b     0
PSPR_CHANGE:        dc.b     0
START_SEL:          dc.b     0
ASSOC_ON:           dc.b     0
PLAYER_ON:          dc.b     0
WATER_AN:           dc.b     0

HISCR_TAB:          dcb.b    98, 0
HILAST_NAME:        dcb.b    14, 0

HISC_NAME:          dc.l     0
lbL01A188:          dc.l     0
lbB01A18C:          dc.b     0
lbB01A18D:          dc.b     0

ZONE:               dcb.b    2, 0
ZONE_TAB:           dc.l     0
SPRITE_BANK:        dc.l     0
FIXED_BANK:         dc.l     0
SPR_CNT:            dc.w     0
FIXED_SPRS:         dc.w     0
STAGE_SPRS:         dc.l     0
MENTB_AD:           dc.l     0
SPRITE_AD:          dc.l     0
MASK_AD:            dc.l     0
UNDRAW_PTR:         dc.l     0

SPR_DATA:           dcb.w    1018, 0
lbL01A9A8:          dcb.w    66, 0
lbL01AA2C:          dcb.w    52, 0
lbL01AA94:          dcb.w    128, 0
lbL01AB94:          dcb.w    22, 0
lbL01ABC0:          dcb.w    12, 0
lbL01ABD8:          dcb.w    74, 0
lbL01AC6C:          dcb.w    18, 0
lbL01AC90:          dcb.w    42, 0
lbW01ACE4:          dcb.w    189, 0
lbW01AE5E:          dcb.w    49, 0
lbL01AEC0:          dcb.w    8, 0
lbL01AED0:          dcb.w    38, 0
lbW01AF1C:          dc.w     0
lbW01AF1E:          dcb.w    335, 0
lbL01B1BC:          dcb.w    948, 0

SPR_TAB:            dcb.w    600, 0
SPRM_TAB:           dcb.w    600, 0
UNDRAW_TABA:        dcb.w    440, 0
UNDRAW_TABB:        dcb.w    440, 0
                    
                    ; note: there was a bug here in the original version
                    ; i don't know why it didn't trigger on the cd32
                    ; as it's been overwriting the data below
                    ; maybe it's because of the presence of fastmem.
                    dcb.w    3000, 0

; ---
NOZONES:            dc.l     NO_ZONE
                    dc.l     0

NO_ZONE:            dc.w     0, 0, 300, 300
                    dc.l     LS1_SPRS
                    dc.w     0

; ---
LS1_ZONES:          dc.l     LS1_ZONE1
                    dc.l     LS1_ZONE2
                    dc.l     0

LS1_ZONE1:          dc.w     0, 140, 90, 260
                    dc.l     LS1_SPRS
                    dc.w     17
LS1_ZONE2:          dc.w     0, 0, 90, 140
                    dc.l     LS1_BANK2
                    dc.w     21

; ---
IC1_ZONES:          dc.l     IC1_ZONE1
                    dc.l     IC1_ZONE2
                    dc.l     IC1_ZONE3
                    dc.l     0

IC1_ZONE1:          dc.w     0, 0, 80, 80
                    dc.l     IC1_SPRS
                    dc.w     20
IC1_ZONE2:          dc.w     80, 0, 210, 80
                    dc.l     IC1_BANK2
                    dc.w     20
IC1_ZONE3:          dc.w     210, 0, 300, 80
                    dc.l     IC1_BANK3
                    dc.w     19

; ---
IC2_ZONES:          dc.l     IC2_ZONE1
                    dc.l     IC2_ZONE2
                    dc.l     IC2_ZONE3
                    dc.l     IC2_ZONE4
                    dc.l     0

IC2_ZONE1:          dc.w     0, 0, 50, 60
                    dc.l     IC2_SPRS
                    dc.w     10
IC2_ZONE2:          dc.w     50, 0, 190, 60
                    dc.l     IC2_BANK2
                    dc.w     26
IC2_ZONE3:          dc.w     190, 0, 320, 60
                    dc.l     IC2_BANK3
                    dc.w     23
IC2_ZONE4:          dc.w     320, 0, 400, 60
                    dc.l     IC2_BANK4
                    dc.w     14

; ---
IC3_ZONES:          dc.l     IC3_ZONE1
                    dc.l     IC3_ZONE2
                    dc.l     0

IC3_ZONE1:          dc.w     0, 55, 90, 140
                    dc.l     IC3_SPRS
                    dc.w     21
IC3_ZONE2:          dc.w     0, 0, 90, 55
                    dc.l     IC3_BANK2
                    dc.w     15

; ---
SN1_ZONES:          dc.l     SN1_ZONE1
                    dc.l     SN1_ZONE2
                    dc.l     SN1_ZONE3
                    dc.l     0

SN1_ZONE1:          dc.w     0, 0, 90, 60
                    dc.l     SN1_SPRS
                    dc.w     17
SN1_ZONE2:          dc.w     90, 0, 230, 60
                    dc.l     SN1_BANK2
                    dc.w     19
SN1_ZONE3:          dc.w     230, 0, 400, 60
                    dc.l     SN1_BANK3
                    dc.w     22

; ---
SN2_ZONES:          dc.l     SN2_ZONE1
                    dc.l     0

SN2_ZONE1:          dc.w     0, 0, 600, 40
                    dc.l     SN2_SPRS
                    dc.w     0

; ---
SN3_ZONES:          dc.l     SN3_ZONE1
                    dc.l     SN3_ZONE2
                    dc.l     SN3_ZONE3
                    dc.l     0

SN3_ZONE1:          dc.w     0, 0, 70, 80
                    dc.l     SN3_SPRS
                    dc.w     18
SN3_ZONE2:          dc.w     70, 0, 190, 80
                    dc.l     SN3_BANK2
                    dc.w     14
SN3_ZONE3:          dc.w     190, 0, 300, 80
                    dc.l     SN3_BANK3
                    dc.w     17

; ---
TC1_ZONES:          dc.l     TC1_ZONE1
                    dc.l     TC1_ZONE2
                    dc.l     TC1_ZONE3
                    dc.l     0

TC1_ZONE1:          dc.w     0, 0, 70, 120
                    dc.l     TC1_SPRS
                    dc.w     13
TC1_ZONE2:          dc.w     70, 0, 200, 65
                    dc.l     TC1_BANK2
                    dc.w     17
TC1_ZONE3:          dc.w     70, 65, 200, 120
                    dc.l     TC1_BANK3
                    dc.w     9

; ---
TC2_ZONES:          dc.l     TC2_ZONE1
                    dc.l     TC2_ZONE2
                    dc.l     TC2_ZONE3
                    dc.l     0

TC2_ZONE1:          dc.w     0, 40, 125, 120
                    dc.l     TC2_SPRS
                    dc.w     16
TC2_ZONE2:          dc.w     125, 0, 200, 120
                    dc.l     TC2_BANK2
                    dc.w     15
TC2_ZONE3:          dc.w     0, 0, 125, 40
                    dc.l     TC2_BANK3
                    dc.w     15

; ---
TC3_ZONES:          dc.l     TC3_ZONE1
                    dc.l     TC3_ZONE2
                    dc.l     TC3_ZONE3
                    dc.l     0

TC3_ZONE1:          dc.w     0, 40, 120, 120
                    dc.l     TC3_SPRS
                    dc.w     15
TC3_ZONE2:          dc.w     0, 0, 200, 40
                    dc.l     TC3_BANK2
                    dc.w     21
TC3_ZONE3:          dc.w     120, 40, 200, 120
                    dc.l     TC3_BANK3
                    dc.w     8

; ---
TC4_ZONES:          dc.l     TC4_ZONE1
                    dc.l     0

TC4_ZONE1:          dc.w     0, 0, 400, 60
                    dc.l     TC3_SPRS
                    dc.w     0

; ---
BB1_ZONES:          dc.l     BB1_ZONE1
                    dc.l     BB1_ZONE2
                    dc.l     BB1_ZONE3
                    dc.l     BB1_ZONE4
                    dc.l     0

BB1_ZONE1:          dc.w     95, 30, 200, 120
                    dc.l     BB1_SPRS
                    dc.w     17
BB1_ZONE2:          dc.w     0, 70, 95, 120
                    dc.l     BB1_BANK2
                    dc.w     15
BB1_ZONE3:          dc.w     0, 0, 95, 70
                    dc.l     BB1_BANK3
                    dc.w     14
BB1_ZONE4:          dc.w     95, 0, 200, 30
                    dc.l     BB1_BANK4
                    dc.w     8

; ---
BB2_ZONES:          dc.l     BB2_ZONE1
                    dc.l     BB2_ZONE2
                    dc.l     BB2_ZONE3
                    dc.l     0

BB2_ZONE1:          dc.w     0, 25, 115, 120
                    dc.l     BB2_SPRS
                    dc.w     24
BB2_ZONE2:          dc.w     115, 25, 200, 120
                    dc.l     BB2_BANK2
                    dc.w     12
BB2_ZONE3:          dc.w     0, 0, 200, 25
                    dc.l     BB2_BANK3
                    dc.w     17

; ---
BB3_ZONES:          dc.l     BB3_ZONE1
                    dc.l     BB3_ZONE2
                    dc.l     BB3_ZONE3
                    dc.l     BB3_ZONE4
                    dc.l     0

BB3_ZONE1:          dc.w     0, 0, 80, 60
                    dc.l     BB3_SPRS
                    dc.w     10
BB3_ZONE2:          dc.w     0, 60, 105, 120
                    dc.l     BB3_BANK2
                    dc.w     12
BB3_ZONE3:          dc.w     105, 60, 200, 120
                    dc.l     BB3_BANK3
                    dc.w     11
BB3_ZONE4:          dc.w     80, 0, 200, 60
                    dc.l     BB3_BANK4
                    dc.w     15

; ---
BD1_ZONES:          dc.l     BD1_ZONE1
                    dc.l     BD1_ZONE2
                    dc.l     BD1_ZONE3
                    dc.l     BD1_ZONE4
                    dc.l     0

BD1_ZONE1:          dc.w     0, 0, 59, 59
                    dc.l     BD1_SPRS
                    dc.w     15
BD1_ZONE2:          dc.w     59, 0, 164, 59
                    dc.l     BD1_BANK2
                    dc.w     20
BD1_ZONE3:          dc.w     164, 0, 280, 59
                    dc.l     BD1_BANK3
                    dc.w     16
BD1_ZONE4:          dc.w     280, 0, 380, 59
                    dc.l     BD1_BANK4
                    dc.w     15

; ---
BD2_ZONES:          dc.l     BD2_ZONE1
                    dc.l     BD2_ZONE2
                    dc.l     BD2_ZONE3
                    dc.l     BD2_ZONE4
                    dc.l     BD2_ZONE5
                    dc.l     BD2_ZONE6
                    dc.l     0

BD2_ZONE1:          dc.w     0, 0, 70, 59
                    dc.l     BD2_SPRS
                    dc.w     9
BD2_ZONE2:          dc.w     70, 0, 130, 59
                    dc.l     BD2_BANK2
                    dc.w     10
BD2_ZONE3:          dc.w     130, 0, 224, 59
                    dc.l     BD2_BANK3
                    dc.w     18
BD2_ZONE4:          dc.w     224, 0, 284, 59
                    dc.l     BD2_BANK4
                    dc.w     13
BD2_ZONE5:          dc.w     284, 0, 360, 59
                    dc.l     BD2_BANK5
                    dc.w     12
BD2_ZONE6:          dc.w     360, 0, 400, 59
                    dc.l     BD2_BANK6
                    dc.w     4

; ---
BD3_ZONES:          dc.l     BD3_ZONE1
                    dc.l     BD3_ZONE2
                    dc.l     BD3_ZONE3
                    dc.l     BD3_ZONE4
                    dc.l     0

BD3_ZONE1:          dc.w     0, 0, 40, 75
                    dc.l     BD3_SPRS
                    dc.w     11
BD3_ZONE2:          dc.w     40, 0, 130, 75
                    dc.l     BD3_BANK2
                    dc.w     14
BD3_ZONE3:          dc.w     130, 0, 250, 75
                    dc.l     BD3_BANK3
                    dc.w     29
BD3_ZONE4:          dc.w     250, 0, 320, 75
                    dc.l     BD3_BANK4
                    dc.w     14

LEVEL_SPRS:         dc.w     0
BADDY_SPRS:         dc.w     0
EGG_BOMBS:          dc.l     0
EXP_BOMBS:          dc.l     0
ENEMY_BULLS:        dc.l     0
HOME_BULLS:         dc.l     0
ANIMS_TAB:          dc.l     EGGBERT_LD
                    dc.l     EGGBERT_JP
                    dc.l     EGGBERT_RL
                    dc.l     EGGBERT_RLR
                    dc.l     EGGCHICK_W
                    dc.l     GANNET_W
                    dc.l     GANNET_O
                    dc.l     LADYB_L
                    dc.l     LADYB_R
                    dc.l     FLYBIRD_F
                    dc.l     FLYBIRD_P
                    dc.l     SPITTER_SP
                    dc.l     NESTY_SP
                    dc.l     SWAN_SP
                    dc.l     GLITTER_SP
                    dc.l     VSPITS_SP
                    dc.l     FLYBULB_SP
                    dc.l     FLYBJET_SP
                    dc.l     FLYBJETR_SP
                    dc.l     VSPARK_SP
                    dc.l     TORCH_SP
                    dc.l     SWBULB_SP
                    dc.l     SWBJET_SP
                    dc.l     ANGLEP_LD
                    dc.l     ANGLEP_JP
                    dc.l     PYRA_SPIN
                    dc.l     ANKH_FLY
                    dc.l     SAW_LEFT
                    dc.l     SAW_RIGHT
                    dc.l     SPHINX_WL
                    dc.l     SPHINX_SH
                    dc.l     ROLLER_BALL
                    dc.l     HOMER_BUL
                    dc.l     SNKFLY_AN
                    dc.l     GLOVE_AN
                    dc.l     WINDOW_AN
                    dc.l     HOMESN_AN
                    dc.l     SNFIRE_AN
                    dc.l     MARSHM_AN
                    dc.l     CONE_AN
                    dc.l     DEMO_AN
                    dc.l     PENJ_LD
                    dc.l     BOOK_AN
                    dc.l     FROG_AN
                    dc.l     SCISS_AN
                    dc.l     PLANE_AN
EGGBERT_LD:         dc.b     1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
                    dc.b     1, $FF
EGGBERT_JP:         dc.b     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, $FF
EGGBERT_RL:         dc.b     4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, $FF
EGGBERT_RLR:        dc.b     7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, $FF
EGGCHICK_W:         dc.b     $40, $40, $40, $40, $40, $41, $41, $41, $41, $41, $42, $42
                    dc.b     $42, $42, $42, $43, $43, $43, $43, $43, $FF
GANNET_W:           dc.b     $14, $14, $14, $14, $14, $14, $14, $15, $15, $15, $15, $15
                    dc.b     $15, $15, $16, $16, $16, $16, $16, $16, $16, $17, $17, $17
                    dc.b     $17, $17, $17, $17, $FF, $27, $27, $27, $27, $27, $FF
GANNET_O:           dc.b     $18, $18, $18, $18, $18, $18, $18, $18, $18, $18, $18, $18
                    dc.b     $18, $18, $18, $18, $18, $18, $18, $18, $18, $18, $18, $18
                    dc.b     $18, $18, $18, $18, $18, $18, $18, $18, $FF
LADYB_L:            dc.b     $10, $10, $10, $10, $10, $10, $10, $10, $11, $11, $11, $11
                    dc.b     $11, $11, $11, $11, $12, $12, $12, $12, $12, $12, $12, $12
                    dc.b     $13, $13, $13, $13, $13, $13, $13, $13, $FF
LADYB_R:            dc.b     $33, $33, $33, $33, $33, $33, $33, $33, $34, $34, $34, $34
                    dc.b     $34, $34, $34, $34, $35, $35, $35, $35, $35, $35, $35, $35
                    dc.b     $36, $36, $36, $36, $36, $36, $36, $36, $FF
FLYBIRD_F:          dc.b     10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13
                    dc.b     $FF, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
                    dc.b     15, 15, 15, 15, 15, $FF
FLYBIRD_P:          dc.b     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 8, 8, 8, 8, 8
                    dc.b     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
                    dc.b     9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9
                    dc.b     $FF
SPITTER_SP:         dc.b     $20, $20, $20, $20, $20, $20, $21, $FF
NESTY_SP:           dc.b     $1C, $1C, $1C, $1C, $1C, $1C, $1C, $1C, $1C, $1C, $1D, $1D
                    dc.b     $1D, $1D, $1D, $1D, $1D, $1D, $FF
SWAN_SP:            dc.b     $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
                    dc.b     $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
                    dc.b     $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
                    dc.b     $28, $28, $1A, $1A, $1A, $1A, $1A, $28, $28, $28, $28, $28
                    dc.b     $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28, $28
                    dc.b     $28, $28, $28, $28, $1A, $1A, $1A, $1A, $1A, $FF
GLITTER_SP:         dc.b     $18, $18, $19, $19, $1A, $1A, $1B, $1B, $1C, $1C, $FF
VSPITS_SP:          dc.b     0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, $FF
FLYBULB_SP:         dc.b     $14, $14, $14, $14, $14, $14, $14, $14, $14, $14, $14, $14
                    dc.b     $14, $14, $14, $14, $14, $14, $14, $14, $14, $14, $14, $14
                    dc.b     $14, $14, $14, $14, $14, $14, $15, $15, $15, $15, $15, $14
                    dc.b     $14, $14, $14, $14, $14, $14, $14, $14, $14, $15, $15, $15
                    dc.b     $15, $15, $FF
FLYBJET_SP:         dc.b     $16, $16, $16, $17, $17, $17, $FF
FLYBJETR_SP:        dc.b     $2E, $2E, $2E, $2F, $2F, $2F, $FF
VSPARK_SP:          dc.b     $1E, $1E, $1E, $1F, $1F, $1F, $1F, $1F, $21, $21, $21, $21
                    dc.b     $21, $21, $21, $21, $21, $21, $20, $20, $20, $20, $21, $21
                    dc.b     $1E, $1E, $1E, $1E, $1F, $1F, $20, $20, $1F, $1F, $1E, $1E
                    dc.b     $1E, $1E, $1E, $20, $20, $20, $20, $20, $1E, $1E, $1E, $1E
                    dc.b     $1E, $1E, $1E, $21, $21, $21, $1F, $1F, $1F, $1F, $1F, $1F
                    dc.b     $1F, $1F, $1F, $1F, $20, $20, $20, $20, $20, $20, $FF
TORCH_SP:           dc.b     $22, $22, $22, $22, $22, $23, $23, $23, $23, $23, $24, $24
                    dc.b     $24, $24, $24, $25, $25, $25, $25, $25, $24, $24, $24, $24
                    dc.b     $24, $23, $23, $23, $23, $23, $FF
SWBULB_SP:          dc.b     $11, $11, $11, $11, $11, $11, $11, $11, $11, $11, $11, $11
                    dc.b     $12, $12, $12, $11, $11, $11, $11, $11, $11, $11, $11, $11
                    dc.b     $11, $11, $11, $11, $11, $11, $11, $11, $11, $11, $11, $11
                    dc.b     $11, $11, $11, $11, $11, $11, $11, $12, $12, $12, $11, $11
                    dc.b     $11, $11, $11, $11, $11, $11, $11, $11, $12, $12, $12
                    dc.b     $FF, $13, $13, $13, $13, $13, $13, $13, $FF
SWBJET_SP:          dc.b     9, 9, 9, 10, 10, 10, $FF
ANGLEP_LD:          dc.b     11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, $FF
ANGLEP_JP:          dc.b     13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14
                    dc.b     14, 15, $FF
PYRA_SPIN:          dc.b     0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, $FF
ANKH_FLY:           dc.b     $16, $16, $16, $17, $17, $17, $18, $18, $18, $19, $19, $19
                    dc.b     $1A, $1A, $1A, $19, $19, $19, $18, $18, $18, $17, $17, $17
                    dc.b     $FF
SAW_LEFT:           dc.b     $12, $12, $13, $13, $14, $14, $15, $15, $14, $14, $13, $13
                    dc.b     $FF
SAW_RIGHT:          dc.b     $32, $32, $33, $33, $34, $34, $35, $35, $34, $34, $33, $33
                    dc.b     $FF
SPHINX_WL:          dc.b     10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, $FF
SPHINX_SH:          dc.b     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 15, 15, 15, 15, 15, $10, $10, $10
                    dc.b     $10, $10, $11, $11, $11, $11, $11, $10, $10, $10, $10, $10
                    dc.b     15, 15, 15, 15, 15, 9, 9, 9, 9, 9, 9, $FF
ROLLER_BALL:        dc.b     $1B, $FF
HOMER_BUL:          dc.b     $3B, $3B, $3C, $3C, $3D, $3D, $3E, $3E, $FF
SNKFLY_AN:          dc.b     12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, $FF
GLOVE_AN:           dc.b     8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, $FF
WINDOW_AN:          dc.b     $64, $FF, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6
                    dc.b     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2
                    dc.b     1, 1, 1, 0, 0, 0, $FF
HOMESN_AN:          dc.b     $26, $26, $27, $27, $28, $28, $29, $29, $FF
SNFIRE_AN:          dc.b     $10, $10, $10, $10, $10, $10, $10, $10, $11, $11, $11, $11
                    dc.b     $11, $11, $11, $11, $12, $12, $12, $12, $12, $12, $12, $12
                    dc.b     $13, $13, $13, $13, $13, $13, $13, $13, $14, $14, $14, $14
                    dc.b     $14, $14, $14, $14, $15, $15, $15, $15, $15, $15, $15, $15
                    dc.b     $FF
MARSHM_AN:          dc.b     0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, $FF
CONE_AN:            dc.b     3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, $FF
DEMO_AN:            dc.b     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0
                    dc.b     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.b     0, 0, 0, 0, 1, 1, 1, 1, $FF
PENJ_LD:            dc.b     3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, $FF
BOOK_AN:            dc.b     6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, $FF
FROG_AN:            dc.b     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5
                    dc.b     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
                    dc.b     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6
                    dc.b     6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9
                    dc.b     9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11
                    dc.b     12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13
                    dc.b     14, 14, 14, 14, 14, 14, 14, 14, $FF
SCISS_AN:           dc.b     0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, $FF
PLANE_AN:           dc.b     15, 15, 15, 15, 15, $10, $10, $10, $10, $10, $11, $11, $11
                    dc.b     $11, $11, $12, $12, $12, $12, $12, $11, $11, $11, $11, $11
                    dc.b     $10, $10, $10, $10, $10, $FF
LOLLS_ON:           dc.w     0
LOLL_TAB:           dcb.l    6, 0
LAST_LOLL:          dc.l     BUTT1_TAB
LAST_BUTT:          dc.l     BUTT2_TAB
BUTSPR_AN:          dc.b     0, 1, 2, 1, $FF
BUTPLAT_AN:         dc.b     0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, $FF
BUTT1_TAB:          dc.w     0, 0, 0, $26, $35, 0, 0, 0, 0, 0, $1C, $35, 0, 0, 0, 0, 0, $13, $37
                    dc.w     0, 0, 0, 0, 0, $10, $2F, 0, 0, 0, 0, 0, $19, $2D, 0, 0, 0, 0, 0, $24
                    dc.w     $2B, 0, 0, 0, 0, 0, $2E, $29, 0, 0, 0, 0, 0, $2F, $20, 0, 0, 0, 0, 0
                    dc.w     $25, $21, 0, 0, 0, 0, 0, $25, $1B, 0, 0, 0, 0, 0, $19, $1B, 0, 0, 0
                    dc.w     0, 0, 12, 12, 0, 0, 0, 0, 0, $25, 7, 0, 0, 0, 0, 0, $3C, 12, 0, 0, 0
                    dc.w     0, 0, $48, 12, 0, 0, 0, 0, 0, $44, $18, 0, 0, 0, 0, 0, $10E, $37, 0
                    dc.w     0, 0, 0, 0, $109, $31, 0, 0, 0, 0, 0, $104, $2B, 0, 0, 0, 0, 0
                    dc.w     $10C, $28, 0, 0, 0, 0, 0, $11D, $15, 0, 0, 0, 0, 0, $127, $18, 0
                    dc.w     0, 0, 0, 0, $112, $23, 0, 0, 0, 0, 0, $107, $21, 0, 0, 0, 0, 0, $FD
                    dc.w     $20, 0, 0, 0, 0, 0, $E6, $13, 0, 0, 0, 0, 0, $F6, $13, 0, 0, 0, 0, 0
                    dc.w     $101, $13, 0, 0, 0, 0, 0, $FA, 14, 0, 0, 0, 0, 0, $100, 10, 0, 0, 0
                    dc.w     0, 0, $10A, 10, 0, 0, 0, 0, 0, $114, 10, 0, 0, 0, 0, 0, $123, $30
                    dc.w     0, 0, 0, 0, 0, $124, $2A, 0, 0
BUTT2_TAB:          dc.w     0, 0, 0, $1F, $2B, 0, 0, 0, 0, 0, $1F, $1E, 0, 0, 0, 0, 0, 8, $1B, 0
                    dc.w     0, 0, 0, 0, 14, $15, 0, 0, 0, 0, 0, 4, $11, 0, 0, 0, 0, 0, 15, 13, 0
                    dc.w     0, 0, 0, 0, $170, $2C, 0, 0, 0, 0, 0, $178, $28, 0, 0, 0, 0, 0
                    dc.w     $184, $28, 0, 0, 0, 0, 0, $186, $1E, 0, 0, 0, 0, 0, $17D, $1D, 0
                    dc.w     0, 0, 0, 0, $174, $20, 0, 0, 0, 0, 0, $182, $17, 0, 0, 0, 0, 0
                    dc.w     $189, $14, 0, 0, 0, 0, 0, $16A, $20, 0, 0, 0, 0, 0, $161, $24, 0
                    dc.w     0, 0, 0, 0, $162, $1C, 0, 0, 0, 0, 0, $168, $19, 0, 0, 0, 0, 0
                    dc.w     $16E, $14, 0, 0, 0, 0, 0, $160, $16, 0, 0, 0, 0, 0, $15C, $12, 0
                    dc.w     0, 0, 0, 0, $154, 15, 0, 0, 0, 0, 0, $173, 15, 0, 0, 0, 0, 0, $17A
                    dc.w     12, 0, 0, 0, 0, 0, $16B, 14, 0, 0, 0, 0, 0, $172, 10, 0, 0, 0, 0, 0
                    dc.w     $167, 8, 0, 0, 0, 0, 0, $15D, 9, 0, 0, 0, 0, 0, $159, 13, 0, 0, 0, 0
                    dc.w     0, 2, 12, 0, 0, 0, 0, 0, 6, 7, 0, 0
BUTT3_TAB:          dc.w     0, 0, 0, $43, $76, 0, 0, 0, 0, 0, $16, $78, 0, 0, 0, 0, 0, $10, $76
                    dc.w     0, 0, 0, 0, 0, 9, $71, 0, 0, 0, 0, 0, 3, $6D, 0, 0, 0, 0, 0, 11, $6A
                    dc.w     0, 0, 0, 0, 0, $14, $68, 0, 0, 0, 0, 0, $15, $5F, 0, 0, 0, 0, 0, 14
                    dc.w     $5B, 0, 0, 0, 0, 0, 6, $59, 0, 0, 0, 0, 0, 14, $55, 0, 0, 0, 0, 0
                    dc.w     $18, $57, 0, 0, 0, 0, 0, $34, $5C, 0, 0, 0, 0, 0, $3E, $5C, 0, 0, 0
                    dc.w     0, 0, $48, $5C, 0, 0, 0, 0, 0, $4B, $53, 0, 0, 0, 0, 0, $3F, $54, 0
                    dc.w     0, 0, 0, 0, $38, $50, 0, 0, 0, 0, 0, $33, $4B, 0, 0, 0, 0, 0, $2F
                    dc.w     $46, 0, 0, 0, 0, 0, $29, $42, 0, 0, 0, 0, 0, $34, $41, 0, 0, 0, 0, 0
                    dc.w     $29, $28, 0, 0, 0, 0, 0, $34, $28, 0, 0, 0, 0, 0, $38, $15, 0, 0, 0
                    dc.w     0, 0, $41, 15, 0, 0, 0, 0, 0, $2A, $37, 0, 0, 0, 0, 0, $38, 13, 0, 0
                    dc.w     0, 0, 0, $2F, $10, 0, 0, 0, 0, 0, $25, $13, 0, 0, 0, 0, 0, $1D, 15
                    dc.w     0, 0, 0, 0, 0, $34, $37, 0, 0, 0, 0, 0, 13, 10, 0, 0, 0, 0, 0, 3, 10
                    dc.w     0, 0
BLK_TRIES:          dc.w     0
BRICK_TILE:         dc.w     0
BRICKF_SPR:         dc.w     0
BRICKL_SPR:         dc.w     0
BRICKS_ON:          dc.w     0
BRICKS_TAB:         dcb.l    20, 0
END_BRICKS:         dc.l     SNAKEB1_TAB
SNAKEB_F:           dc.w     0
SNAKEB_AN:          dc.w     0
SNAKEBX:            dc.w     0
SNAKEBY:            dc.w     0
SNAKE_BLNK:         dc.w     0
SNAKE_BLNKS:        dc.w     10
SNAKEB1_AN:         dc.b     $16, $16, $16, $17, $17, $17, $17, $17, $17, $17, $16, $16
                    dc.b     $FF
SNAKEB2_AN:         dc.b     $1A, $1A, $1A, $1B, $1B, $1B, $1B, $1B, $1B, $1B, $1B, $1B
                    dc.b     $FF
SNAKEB1_TAB:        dc.w     $630, $2A0, 1, $820, $2A0, 0, $960, $240, 1, $9E0, $230, 0
                    dc.w     $AD0, $2B0, 1, $920, $50, 0, $F70, $A0, 0, $1090, $C0, 1
                    dc.w     $1110, $B0, 0, $1420, $230, 1
SNAKEB2_TAB:        dc.w     $3F0, $160, 1, $A70, $D0, 0, $B90, $D0, 1, $C90, $B0, 0, $F30
                    dc.w     $40, 1, $1150, $130, 0, $12E0, $90, 1, $1740, $90, 0, $1CA0
                    dc.w     $30, 0, $1A60, $10, 1, $1DC0, $10, 1
SNAKEB3_TAB:        dc.w     $3E0, $B0, 0, $490, $80, 1, $5C0, $20, 0, $640, $50, 1, $6C0
                    dc.w     $A0, 0, $830, $A0, 1, $900, $40, 0, $B20, $A0, 1, $B80, $70, 0
                    dc.w     $E50, $A0, 1, $F00, $80, 0, $1020, $80, 1, $AC0, $1E0, 1
                    dc.w     $9C0, $1E0, 0, $800, $1E0, 1, $6C0, $1E0, 0, $700, $2E0, 1
                    dc.w     $C00, $3A0, 0, $340, $420, 0, $3C0, $420, 0, $340, $30, 1
EYES_TAB:           dcb.l    6, 0
LAST_EYE:           dcb.l    9, 0
LAST_GONE:          dc.l     0
LAST_SNAKE:         dc.l     0
SNAKE_1:            dc.w     $5C0, $6C0, $230, $140, $430, $310, $E60, $460, 0, $280
                    dc.w     $CC0, $70, $12C0, $3C0, $1E0, $190, $CC0, $70
END_SN1:            dc.w     $220, $23A0, 0, $280, $80, $180
END_SN2:            dc.w     $280, $B40, 0, $190, $F0, $50, $710, $820, $320, $1E0, $580
                    dc.w     $330
END_SN3:            dc.w     0, 0, 0, 1, -6, 10, 1, -4, 5, 1, -4, 5, 0, -1, 2, 0
                    dc.w     -1, 2, 0, -1, 2, 0, -1, 2, 2, -6, -10, 2, -4
                    dc.w     -5, 2, -4, -5, 0, -1, -2, 0, -1, -2, 0
                    dc.w     -1, -2, 0, -1, -2
SNTL_TAB:           dc.w     3, 0, 0, 4, 0, -8, 4, 0, -8, 4, 0, -8, 3, 0, 0, 3, 0, 0, 3
                    dc.w     0, 0, 3, 0, 0, 5, 0, 8, 5, 0, 8, 5, 0, 8, 3, 0, 0, 3, 0, 0, 3, 0, 0, 3, 0
                    dc.w     0
SNAKE_TAB:          dcb.l    $3F, 0
                    dcb.l    $3F, 0
                    dcb.l    $12, 0
SNAKE_PTR:          dc.l     0
SNAKE_REF:          dc.l     0
SNAKE_MAP:          dc.l     0
SNAKEX:             dc.w     0
SNAKEY:             dc.w     0
SNBODY_SPR:         dc.w     0
OLDBODY_SPR:        dc.w     0
SNAKE_ON:           dc.b     0
SNAKE_STAT:         dc.b     0
SNAKE_CHK:          dc.b     0
SNAKE_GROW:         dc.b     0
BELL_RING:          dc.b     0
CLEAR_SNAKE:        dc.b     0
SPARK_AN:           dcb.b    2, 0
SPARK_TAB:          dc.b     4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1
SPARKX:             dc.w     0
SPARKY:             dc.w     0
LBEAM_TILE:         dc.w     $1EA
LBEAM_SPRS:         dc.w     7
TORCHES:            dc.w     0
TORCH_PTR:          dc.l     0
TORCH_TAB1:         dc.w     1, $10, 0, $4AD0, 0, 0, $8B0, $5F0, 0, 12, 0, $870, 0, 0, $AB0
                    dc.w     $A0, 0, 8, 0, $7F5, 0, 0, $2D0, $A0, 1, 12, 0, $292E, 0, 0, $840
                    dc.w     $340, 0, 12, 0, $43AF, 0, 0, $950, $560, 0, 4, 0, $243A, 0, 0
                    dc.w     $4F0, $2E0
TORCH_TAB2:         dc.w     1, 14, 0, $2627, 0, 0, $9C0, $300, 1, 12, 0, $3718, 0, 0, $5E0
                    dc.w     $460, 0, 12, 0, $331D, 0, 0, $5F0, $410, 1, 12, 0, $2F48, 0, 0
                    dc.w     $5E0, $3C0, 0, 12, 0, $2B4D, 0, 0, $5F0, $370, 1, 10, 0, $25E4
                    dc.w     0, 0, $5E0, $320, 0, 10, 0, $3EA5, 0, 0, $2C0, $500, 1, 10, 0
                    dc.w     $28E0, 0, 0, $380, $340
TORCH_TAB3:         dc.w     1, $14, 0, $A5C, 0, 0, $230, $D0, 1, $14, 0, $13BC, 0, 0, $230
                    dc.w     $190, 0, $14, 0, $EE9, 0, 0, $220, $130, 0, 6, 0, $2EFC, 0, 0
                    dc.w     $230, $3C0, 1, 6, 0, $2A59, 0, 0, $220, $360, 0, 5, 0, $164B, 0
                    dc.w     0, $700, $1C0, 1, 8, 0, $1BD0, 0, 0, $720, $230, 0, $10, 0
                    dc.w     $2203, 0, 0, $770, $2B0, 0, 8, 0, $1047, 0, 0, $AF0, $140
TRESET_TAB1:        dc.w     1, $10, 0, $4AD0, 0, 0, $8B0, $5F0, 0, 12, 0, $870, 0, 0, $AB0
                    dc.w     $A0, 0, 8, 0, $7F5, 0, 0, $2D0, $A0, 1, 12, 0, $292E, 0, 0, $840
                    dc.w     $340, 0, 12, 0, $43AF, 0, 0, $950, $560, 0, 4, 0, $243A, 0, 0
                    dc.w     $4F0, $2E0, 1, 14, 0, $2627, 0, 0, $9C0, $300, 1, 12, 0, $3718
                    dc.w     0, 0, $5E0, $460, 0, 12, 0, $331D, 0, 0, $5F0, $410, 1, 12, 0
                    dc.w     $2F48, 0, 0, $5E0, $3C0, 0, 12, 0, $2B4D, 0, 0, $5F0, $370, 1
                    dc.w     10, 0, $25E4, 0, 0, $5E0, $320, 0, 10, 0, $3EA5, 0, 0, $2C0
                    dc.w     $500, 1, 10, 0, $28E0, 0, 0, $380, $340, 1, $14, 0, $A5C, 0, 0
                    dc.w     $230, $D0, 1, $14, 0, $13BC, 0, 0, $230, $190, 0, $14, 0, $EE9
                    dc.w     0, 0, $220, $130, 0, 6, 0, $2EFC, 0, 0, $230, $3C0, 1, 6, 0
                    dc.w     $2A59, 0, 0, $220, $360, 0, 5, 0, $164B, 0, 0, $700, $1C0, 1, 8
                    dc.w     0, $1BD0, 0, 0, $720, $230, 0, $10, 0, $2203, 0, 0, $770, $2B0
                    dc.w     0, 8, 0, $1047, 0, 0, $AF0, $140
END_TRES:           dc.w     $180, $590, $FFF0, $2C0, $2C0, $630, $AF0, $130, $C30
                    dc.w     $130
PHAREYE_TAB2:       dc.w     $2D0, $4B0, $C80, $540, $1E0, $360, $AF0, $130, $C30, $E0
PHAREYE_TAB3:       dc.w     $900, $490, $C50, $420
LAVA_TAB:           dc.w     0, 0
LAVA_EXPSPR:        dc.w     0
TOOT_LAVA:          dc.b     $23, $23, $23, $23, $22, $22, $22, $22, $21, $21, $21, $21
                    dc.b     $20, $20, $20, $20
SNAKE_LAVA:         dc.b     $1B, $1B, $1B, $1B, $1A, $1A, $1A, $1A, $19, $19, $19, $19
                    dc.b     $18, $18, $18, $18
PTILE_PTR:          dc.w     0
COLUMNS:            dc.w     0
COLUMN_REF:         dc.w     0
COLUMN_PTR:         dc.w     0, 0
COLUMN1_TAB:        dc.w     $100, 5, 0, $597A, 0, 0, $6A0, $6F0, $100, 7, 0, $5984, 0, 0
                    dc.w     $740, $6E0
LIFT1:              dc.w     $100, $18, 0, $598E, 0, 0, 0, 0, $100, 4, 0, $300C, 0, 0, $640
                    dc.w     $3B0
COLUMN2_TAB:        dc.w     $100, 10, 0, $4C32, 0, 0, $6A0, $5C0, $100, 9, 0, $4C3C, 0, 0
                    dc.w     $740, $5D0, $100, 8, 0, $4C46, 0, 0, $7E0, $5D0, $100, 7, 0
                    dc.w     $4C50, 0, 0, $880, $5E0, $100, 6, 0, $4C5B, 0, 0, $930, $5E0
                    dc.w     $100, 10, 0, $1717, 0, 0, $6F0, $180, $100, 9, 0, $1721, 0, 0
                    dc.w     $790, $180, $100, 8, 0, $172B, 0, 0, $830, $190, $100, 7, 0
                    dc.w     $1735, 0, 0, $8D0, $1A0, $100, 6, 0, $173F, 0, 0, $970, $1A0
COLUMN3_TAB:        dc.w     $100, 10, 0, $3FE5, 0, 0, $9D0, $4C0, $100, 8, 0, $3FF1, 0, 0
                    dc.w     $A90, $4D0, $100, 6, 0, $3FFD, 0, 0, $B50, $4E0
LIFT2:              dc.w     $100, 13, 0, $4031, 0, 0, 0, 0
LIFT3:              dc.w     $100, 9, 0, $2F47, 0, 0, 0, 0
RESET_COLS:         dc.w     $100, 5, 0, $597A, 0, 0, $6A0, $6F0, $100, 7, 0, $5984, 0, 0
                    dc.w     $740, $6E0, $100, $18, 0, $598E, 0, 0, 0, 0, $100, 4, 0, $300C
                    dc.w     0, 0, $640, $3B0, $100, 10, 0, $4C32, 0, 0, $6A0, $5C0, $100
                    dc.w     9, 0, $4C3C, 0, 0, $740, $5D0, $100, 8, 0, $4C46, 0, 0, $7E0
                    dc.w     $5D0, $100, 7, 0, $4C50, 0, 0, $880, $5E0, $100, 6, 0, $4C5B
                    dc.w     0, 0, $930, $5E0, $100, 10, 0, $1717, 0, 0, $6F0, $180, $100
                    dc.w     9, 0, $1721, 0, 0, $790, $180, $100, 8, 0, $172B, 0, 0, $830
                    dc.w     $190, $100, 7, 0, $1735, 0, 0, $8D0, $1A0, $100, 6, 0, $173F
                    dc.w     0, 0, $970, $1A0, $100, 10, 0, $3FE5, 0, 0, $9D0, $4C0, $100
                    dc.w     8, 0, $3FF1, 0, 0, $A90, $4D0, $100, 6, 0, $3FFD, 0, 0, $B50
                    dc.w     $4E0, $100, 13, 0, $4031, 0, 0, 0, 0, $100, 9, 0, $2F47, 0, 0, 0
                    dc.w     0
END_COLS:           dc.w     $FFD7, $FFFE, $18E, 0, $18E, 0
LINK_SCRO:          dc.w     $1D7, $FFFE, $18E, 0, $18E, 0
RESET_PTRS:         dc.w     $E0, 7, $E2, $8000, $E8, 7, $EA, $8028, $F0, 7, $F2, $8050
                    dc.w     $F8, 7, $FA, $8078
MASK_PAL:           dc.w     0, 0
PANEL_PAL:          dc.w     0, $100, $555, $777, $101, $202, $303, $404, $505, $606
                    dc.w     $707, 0
HSP_PAL:            dc.w     0, 0, $333, $777, 0, $300, $500, $700, 0, $300, $500, $700, 0
                    dc.w     0, $333, $777
BLACK_PAL:          dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
WHITE_PAL:          dc.w     $FFF, $FFF, $FFF, $FFF, $FFF, $FFF, $FFF, $FFF, $FFF, $FFF
                    dc.w     $FFF, $FFF, $FFF, $FFF, $FFF, $FFF
PAPER_PAL:          dc.w     0, $13, 2, $555, $635, $514, $625, $344, $666, $20, $30, $10
                    dc.w     0, $500, $300, $440, 0, $266, $545, $656, $434, $323, $101
                    dc.w     $706
BIRD_PAL:           dc.w     0, $520, $310, $444, $555, $630, $332, $62, $777, $120, $41
                    dc.w     $111, 0, $500, $400, $650, $66, 0, $533, $733, $211, $422
                    dc.w     $777, $44
BULB_PAL:           dc.w     0, $23, $44, $721, $625, $403, $774, $256, $777, $120, $41
                    dc.w     $111, 0, $500, $300, $650, 0, $606, $777, $667, $404, $202
                    dc.w     $222, $707
TOOT_PAL:           dc.w     0, $113, $134, $530, $420, $56, $210, $162, $777, $120, $41
                    dc.w     $100, 0, $511, $400, $640, 0, $30, $210, $320, $10, $20
                    dc.w     $431, $40
SNAKE_PAL:          dc.w     0, $320, $430, $200, $52, $555, $333, $722, $777, $120, $41
                    dc.w     $111, 0, $500, $300, $650, 0, $753, $633, $242, $464, $131
                    dc.w     $522, $772
ICES_PAL:           dc.w     0, $30, $451, $300, $621, $456, $234, $641, $777, $120, $41
                    dc.w     $100, 0, $511, $400, $751, 0, $505, $666, $20, $151, $606
                    dc.w     $202, $141
LAST_PAL:           dc.w     0, $667, $556, $445, $410, $530, $734, $223, $777, $130
                    dc.w     $52, $112, 0, $511, $300, $751, 0, $266, $555, $777, $444
                    dc.w     $333, $666, $706
BONUS_PAL:          dc.w     0, $63, $722, $34, $625, $403, $444, 0, $777, $120, $41
                    dc.w     $222, $100, $500, $301, $660
STAGE_PAL:          dc.w     0, $666, $555, $611, $736, 1, 0, 0, $777, $120, $41, $111, 0
                    dc.w     $500, $300, $540
HISC_PAL:           dc.w     0, $300, $445, $411, $323, $334, $101, $212, $556, $555
                    dc.w     $777, $767, $440, $662, 0, $200
PROT_PAL:           dc.w     0, $666, $555, $611, $300, $444, $41, $212, $777, $120
                    dc.w     $777, $111, 0, $500, 0, $650
CHOOSE_PAL:         dc.w     0, $130, $445, $101, $334, $722, $666, $666, $556, $300
                    dc.w     $777, $600, $640, $20, 0, $770
END_PAL:            dc.w     0, $255, $223, $366, $200, $112, $101, $611, $334, $440
                    dc.w     $777, $660, $400, $30, 0, $20
SHOW_PAL:           dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $776, $774, $640, $760, 0, $770
ROLL_PAL:           dc.w     4, $630, $520, $304, $103, $347, $750, $666, $510, $642
                    dc.w     $400, $43, $557, $546, $334, 0
MAG_PAL:            dc.w     0, $630, $520, $304, $103, $347, $750, $666, $510, $642
                    dc.w     $400, $43, $557, $446, $334, $20
GREM_PAL:           dc.w     0, $464, $760, $777, $236, $500, $740, $243, $300, $20
                    dc.w     $302, $710, $113, $457, $131, 1
WARP_PAL:           dc.w     0, $333, $555, $777, $300, 0, $777, $775, $753, $531, $410
                    dc.w     $200, 1, $330, $111, $222
CHUP_PAL:           dc.w     3, $335, $557, $10, $340, $220, $101, $611, $777, $440
                    dc.w     $200, $660, $400, $30, $20, 0
ZEYES_PAL:          dc.w     3, $600, $445, $300, $600, $334, $130, $20, $556, $722
                    dc.w     $776, $774, $640, $760, 0, $770
ZEYES_MPAL:         dc.w     $FFF, $FFF, $FFFF, $FFF, $FFFF, $FFFF, $FFF, $FFF, $FFF
                    dc.w     $FFFF, $FFFF, $FFF, $FFFF, $FFFF, $FFFF, $FFFF
ASSOC_MPAL:         dc.w     0, 0, $FFF, 0, 0, $FFF, 0, 0, $FFF, 0, $FFF, 0, 0, 0, 0, 0
ASSOFF_MPAL:        dc.w     $FFFF, $FFFF, $FFF, $FFFF, $FFFF, $FFF, $FFFF, $FFFF
                    dc.w     $FFF, $FFFF, $FFF, $FFFF, $FFFF, $FFFF, $FFFF, $FFFF
CHUP_MPAL:          dc.w     $FFF, $FFF, $FFFF, $FFF, $FFF, $FFFF, $FFF, $FFF, $FFFF
                    dc.w     $FFF, $FFFF, $FFF, $FFF, $FFF, $FFF, $FFF
LEV_PAL:            dc.l     0
DOUBLE_COP:         dc.w     0
FADE_CNT:           dc.w     0
FADECOP_CNT:        dc.w     0
FADECOL_ON:         dc.w     0
COLBAND:            dc.l     PC1_1
COLBAND2:           dc.l     COLOUR
USED_COL1:          dc.l     0
USED_COL2:          dc.l     0
DOCOL_RTN:          dc.l     0
COLBAND_PTR:        dc.w     0
FADBAND_PTR:        dc.w     0
RASTER_BEG:         dc.w     0
SK_PAL:             dc.l     0
COL_MOVE:           dc.w     0
SK_PAL2:            dc.l     0
COL_MOVE2:          dc.w     0
BAND_HGT:           dcb.b    2, 0
BAND_REP:           dcb.b    2, 0
BLACK_BAND:         dcb.l    $1E, 0

BONUS_BAND:         dc.w     $7CE, $7BD, $7AD, $69C, $68C, $68C, $67C, $56B, $55B, $55B
                    dc.w     $64A, $64A, $64A, $749, $749, $749, $849, $849, $849, $A4A
                    dc.w     0, 0, 0, 0

USED_BAND1:         dcb.w    148,0

USED_BAND2:         dcb.w    148,0

; background cols
PC7_1:              dc.w     $CC0, $BB0, $AA0, $990, $880, $770, $660, $550, $440, 0, 0
                    dc.w     0, 0, 0, $383, $282, $181, $80, $70, $60, $50, $40, $30, $20
                    dc.w     $10, 0
PC7_2:              dc.w     $EE0, $DD0, $CC0, $BB0, $AA0, $990, $880, $770, $660, 0, 0
                    dc.w     0, 0, 0, $161, $60, $50, $40, $30, $20, $10, 0, 0, 0, 0, 0
PC1_2:              dc.w     $EDF, $DDF, $DCF, $CCE, $CBE, $CBE, $CBE, $BAE, $BAD, $C4F
                    dc.w     $B3F, $B2E, $A2C, $A1C, $91B, $80A, $808, $708, $707, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, $D33, $808, $880, 0, $60, $DDF, $CCF, $ABF
                    dc.w     $8AF, $7AE, $5AE, $4AE, $2BD, $1CD, $CC
PC1_1:              dc.w     $9A, $BA, $1B9, $1B8, $2C7, $2C6, $3C4, $3D4, $5D4, $7E4
                    dc.w     $8E5, $AE6, $CF6, $EF7, $FF7, $A0, $A0, $90, $80, $70, $60
                    dc.w     $50, $50, $40, $30, $20, $10, 0, $F55, $C0C, $CC2, 0, $B0
                    dc.w     $DDF, $BBF, $9AE, $89E, $68D, $47C, $36C, $26B, $6A, $59
PC2_1:              dc.w     $C0C, $C0C, $C0C, $C0C, 14, $11E, $22E, $33E, $44E, $55E
                    dc.w     $66E, $77E, $76E, $75E, $74E, $73E, $72E, $C0C, $C0C, $C0C
                    dc.w     $C0C, $C0C, $C0C, $C0C, $C0C, $72E, $73E, $74E, $75E, $76E
                    dc.w     $77E, $66E, $55E, $44E, $33E, $22E, $11E, 14, $C0C, $C0C
                    dc.w     $C0C, $C0C
PC2_2:              dc.w     $E0E, $E0E, $E0E, $E0E, $11F, $22F, $33F, $44F, $55F, $66F
                    dc.w     $77F, $88F, $87F, $86F, $85F, $84F, $83F, $E0E, $E0E, $E0E
                    dc.w     $E0E, $E0E, $E0E, $E0E, $E0E, $83F, $84F, $85F, $86F, $87F
                    dc.w     $88F, $77F, $66F, $55F, $44F, $33F, $22F, $11F, $E0E, $E0E
                    dc.w     $E0E, $E0E
PC3_1:              dc.w     $4F4, $3F3, $2F2, $3F3, $4F4, $F68, $E57, $D46, $E57, $F68
                    dc.w     $4F4, $3F3, $2F2, $3F3, $4F4, $F68, $E57, $D46, $E57, $F68
                    dc.w     $4F4, $3F3, $2F2, $3F3, $4F4, $F68, $E57, $D46, $E57, $F68
                    dc.w     $4F4, $3F3, $2F2, $3F3, $4F4, $F68, $E57, $D46, $E57, $F68
                    dc.w     $4F4, $3F3, $2F2, $3F3, $4F4, $F68, $E57, $D46, $E57, $F68
PC3_2:              dc.w     $5F5, $4F4, $3F3, $4F4, $5F5, $F79, $F68, $E57, $F68, $F79
                    dc.w     $5F5, $4F4, $3F3, $4F4, $5F5, $F79, $F68, $E57, $F68, $F79
                    dc.w     $5F5, $4F4, $3F3, $4F4, $5F5, $F79, $F68, $E57, $F68, $F79
                    dc.w     $5F5, $4F4, $3F3, $4F4, $5F5, $F79, $F68, $E57, $F68, $F79
                    dc.w     $5F5, $4F4, $3F3, $4F4, $5F5, $F79, $F68, $E57, $F68, $F79
                    dc.w     $1F1, $2F2, $3F3, $2F2, $1F1, $C35, $D46, $E57, $D46, $C35
                    dc.w     $1F1, $2F2, $3F3, $2F2, $1F1, $C35, $D46, $E57, $D46, $C35
                    dc.w     $1F1, $2F2, $3F3, $2F2, $1F1, $C35, $D46, $E57, $D46, $C35
                    dc.w     $1F1, $2F2, $3F3, $2F2, $1F1, $C35, $D46, $E57, $D46, $C35
                    dc.w     $1F1, $2F2, $3F3, $2F2, $1F1, $C35, $D46, $E57, $D46, $C35
PC4_1:              dc.w     5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, $10F, $20F, $30F
                    dc.w     $40F, $50F, $60F, $70F, $80F, $90F, $A0F, $B0F, $C0F, $66
                    dc.w     $55, $44, $33, $22, $11, 0, $6C6, $5C5, $4C4, $3C3, $2C2
                    dc.w     $1C1, $C0, $B0, $A0, $90, $80, $70, $60, $50, $40, $30
PC4_2:              dc.w     $777, $777, $777, $777, $777, $777, $777, $777, $777, $777
                    dc.w     $777, $777, $777, $777, $777, $777, $777, $777, $777, $777
                    dc.w     $777, $999, $888, $777, $666, $555, $444, $333, $222, $111
                    dc.w     0, $C6C, $C5C, $C4C, $C3C, $C2C, $C1C, $C0C, $B0B, $A0A
                    dc.w     $909, $808, $707, $606, $505, $404, $303
PC5_1:              dc.w     $CFF, $BFF, $AFF, $9FF, $8FF, $7FF, $6FF, $5FF, $4FF, $3FF
                    dc.w     $2FF, $1FF, $FF, $EF, $DF, $CF, $BF, $AF, $9F, $8F, $7F, $6F
                    dc.w     $5F, $4F, $3F, $2F, $1F, 15, $10F, $20F, $30F, $40F, $50F
                    dc.w     $60F, $70F, $80F, $90F, $A0F, $B0F, $C0F, $D0F, $E0F, $F0F
                    dc.w     $F1F, $F2F, $F3F, $F4F, $F5F, $F6F, $F7F, $F8F, $F9F, $FAF
                    dc.w     $FBF, $FCF, $FDF, $FEE, $FED, $FFC, $EFB, $DFC, $CFD, $DFE
                    dc.w     $DFF, $DFF, $DFE, $CFD, $DFC, $EFB, $FFC, $FED, $FEE, $FDF
                    dc.w     $FCF, $FBF, $FAF, $F9F, $F8F, $F7F, $F6F

PC5_2:              dc.w     $BEE, $AEE, $9EE, $8EE, $7EE, $6EE, $5EE, $4EE, $3EE, $2EE
                    dc.w     $1EE, $EE, $EE, $DE, $CE, $BE, $AE, $9E, $8E, $7E, $6E, $5E
                    dc.w     $4E, $3E, $2E, $1E, 14, 14, 14, $10E, $20E, $30E, $40E, $50E
                    dc.w     $60E, $70E, $80E, $90E, $A0E, $B0E, $C0E, $D0E, $E0E, $E0E
                    dc.w     $E1E, $E2E, $E3E, $E4E, $E5E, $E6E, $E7E, $E8E, $E9E, $EAE
                    dc.w     $EBE, $ECE, $EDD, $EDC, $EEB, $DEA, $CEB, $BEC, $CED, $CEE
                    dc.w     $CEE, $CED, $BEC, $CEB, $DEA, $EEB, $EDC, $EDD, $ECE, $EBE
                    dc.w     $EAE, $E9E, $E8E, $E7E, $E6E, $E5E

PC6_1:              dc.w     $2E, $3E, $4E, $5E, $6E, $7E, $8E, $9E, $AE, $BE, $CE, $DE
                    dc.w     $EE, $1EE, $2EE, $3EE, $4EE, $5EE, $6EE, $7EE, $8EE, $9EE
                    dc.w     $AEE, 0, 0, $282, $281, $181, $180, $80, $70, $60, $50, $40
                    dc.w     $30, $20, $20, $20, $20, $20, $20, $20

PC6_2:              dc.w     $977, $A77, $A78, $A88, $B88, $B89, $B99, $C99, $C9A, $CAA
                    dc.w     $DAA, $DAB, $DBB, $EBB, $EBC, $ECC, $A80, $A92, $AA4, $AB6
                    dc.w     $AC8, $ADA, $AEC, 0, 0, $4A4, $4A3, $3A3, $3A2, $2A2, $292
                    dc.w     $282, $272, $262, $252, $242, $242, $242, $242, $242, $242
                    dc.w     $242
COLOUR:             dc.w     0
BAND_POS:           dc.w     0
SWITCH_CNT:         dc.w     0
TEMPW:              dcb.b    2, 0
TEMPW3:             dc.w     0
VBEAM_POS:          dc.w     0
HOLD_SPRS:          dcb.b    2, 0
SEED:               dc.l     $00BBE307, $89ABCDEF, $BB40E64D
RESET_POS:          dc.b     0
OVERFLAG:           dc.b     0
MAX_UNDS:           dc.w     0
SPR_YADD:           dc.w     0
SPRITES_CNTA:       dc.w     0
SPRITES_CNTB:       dc.w     0
XCOORD:             dc.w     0
YCOORD:             dc.w     0
SPRITE:             dc.b     0
lbB01F0F5:          dc.b     0
SPRLNS_OFF:         dc.b     0
lbB01F0F7:          dc.b     0
SPRCLIP_T:          dc.w     0
MASK_TYPE:          dc.w     0
LIGHT_SPR:          dc.b     0
BLACK_SPR:          dc.b     0
WHITE_SPR:          dc.b     0
P_DRAW:             dc.b     0
XOFF:               dc.w     0
SPR_MOD:            dc.w     0
DBL_BLIT:           dcb.b    2, 0
TOKENS:             dc.w     0
GAME_FADE:          dc.b     0
HEART_CNT:          dc.b     0
HEART_RES:          dc.b     0
HEART_AN:           dc.b     $3A, $3A, $3A, $3B, $3B, $3B, $3C, $3C, $3C, $3D, $3D, $3D
                    dc.b     $3C, $3C, $3C, $3B, $3B, $3B, $FF
POWER_TAB:          dc.w     $6F, $71, $72, $70, $73, $74, $75
HEART_TAB:          dcb.w    15, 0
SMART_TAB:          dcb.l    12, 0
SMARTS:             dc.w     0
ARCHEX_TAB:         dcb.l    $1A, 0
DEL_ARCH:           dcb.b    2, 0
ARCHEX_SPR:         dc.w     0
HOLDX:              dc.w     0
HOLDY:              dc.w     0
TOKEN_SPR:          dc.w     $44
TOKEN_TAB:          dcb.l    $19, 0
TOKENS_ON:          dc.w     0
FEATH_ANIM:         dc.w     $47, $47, $47, $47, $47, $48, $48, $48, $48, $48, $49, $49
                    dc.w     $49, $49, $49, $48, $48, $48, $48, $48, -1
SPEXP_TAB:          dcb.l    8, 0
BACKSP_TAB:         dcb.l    2, 0
lbL01F2A6:          dcb.l    $16, 0
BACKSP_LNS:         dcb.b    2, 0
ENDICON_X:          dc.w     0
ENDICON_Y:          dc.w     0
ENDIC_YDIS:         dc.w     0
ENDIC_FLR:          dc.w     0
END_OF_STG:         dc.w     0
ENDICON_ON:         dc.b     0
NO_BADDY:           dc.b     0
TILEFX_ANIM:        dc.l     EGG_BRIDGE
                    dc.l     TOKEN_FLOAT
                    dc.l     SPRING_UP
                    dc.l     ROOF_EXP
                    dc.l     WALLP_EXP
                    dc.l     SPLAT_AN
                    dc.l     SPRING_L
                    dc.l     SPRING_R
                    dc.l     SPLAT2_AN
                    dc.l     WALLP2_EXP
                    dc.l     ROOF2_EXP
                    dc.l     SPLAT3_AN
                    dc.l     WALLP3_EXP
                    dc.l     ROOF3_EXP
                    dc.l     SPLAT4_AN
                    dc.l     PLAT2_EXP
                    dc.l     SPRING_UP2
                    dc.l     SPLAT5_AN
EGG_BRIDGE:         dc.w     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
                    dc.w     -1
TOKEN_FLOAT:        dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, -1
SPRING_UP2:         dc.w     $16, $16, $16, $16
                    dc.w     -1
SPRING_UP:          dc.w     5, 5, 5, 5, -1
ROOF_EXP:           dc.w     6, -4
WALLP_EXP:          dc.w     6, -3
SPLAT_AN:           dc.w     $1B, $1B, $1B, $1B, $1B, $1C, $1C, $1C, $1C, $1C, $1D, $1D
                    dc.w     $1D, $1D, $1D, $1E, $1E, $1E, $1E, $1E, $1F, $1F, $1F, $1F
                    dc.w     $1F, $20, $20, $20, $20, $20, -1
SPRING_L:           dc.w     6, 6, 6, 6, -1
SPRING_R:           dc.w     7, 7, 7, 7, -1
SPLAT2_AN:          dc.w     $1E, $1E, $1E, $1E, $1E, $1F, $1F, $1F, $1F, $1F, $20, $20
                    dc.w     $20, $20, $20, $21, $21, $21, $21, $21, $22, $22, $22, $22
                    dc.w     $22, $23, $23, $23, $23, $23
                    dc.w     -1
WALLP2_EXP:         dc.w     $12, -3
ROOF2_EXP:          dc.w     $12, -3
SPLAT3_AN:          dc.w     $12, $12, $12, $12, $12, $13, $13, $13, $13, $13, $14, $14
                    dc.w     $14, $14, $14, $15, $15, $15, $15, $15, $16, $16, $16, $16
                    dc.w     $16, $17, $17, $17, $17, $17, -1
WALLP3_EXP:         dc.w     7, -3
ROOF3_EXP:          dc.w     7, -3
SPLAT4_AN:          dc.w     $13, $13, $13, $13, $13, $14, $14, $14, $14, $14, $15, $15
                    dc.w     $15, $15, $15, $16, $16, $16, $16, $16, $17, $17, $17, $17
                    dc.w     $17, $18, $18, $18, $18, $18, -1
PLAT2_EXP:          dc.w     $64, $64, $64, $64, $64, $64, $64, $64, $64, $64, $64, $64
                    dc.w     $64, $64, $64, $64, $64, $64, -3
SPLAT5_AN:          dc.w     $10, $10, $10, $10, $10, $11, $11, $11, $11, $11, $12, $12
                    dc.w     $12, $12, $12, $13, $13, $13, $13, $13, $14, $14, $14, $14
                    dc.w     $14, $15, $15, $15, $15, $15, -1
FILL_TILE1:         dc.w     0
FILL_TILE2:         dc.w     0
AFTER_EXP:          dcb.b    2, 0
BACKFX_SPRS:        dc.w     $83
FILLTILE_SPR:       dc.w     6
FILLTILE_SP2:       dc.w     6
DEL_TILEFX:         dc.w     0
TILEFX_TAB:         dcb.l    $38, 0
END_TILEFX:         dc.b     4
ROOFEX_ANIM:        dc.b     3
PLATEX_ANIM:        dcb.b    2, 0
SPLAT_ANIM:         dc.w     0
MINI_TAB:           dcb.l    $10, 0
END_MINIS:          dcb.l    4, 0
PERM_TAB:           dcb.l    $1C, 0
END_PERMS:          dcb.b    2, 0
SMASH2_SPR:         dc.w     0
SMASH1L:            dc.w     $CA
SMASH1R:            dc.w     $CF
SMASH2L:            dc.w     $4A
SMASH2R:            dc.w     $1EF
SMASH_XOFF:         dc.w     0
SMASH_YOFF:         dc.w     0
BULB_SMASH:         dc.w     $55, $56, $57, $59, $5A, $5B, 0, 0, 0
URN_SMASH:          dc.w     0, 0, $1B0, $1BF
ICE_SMASH1:         dc.w     $66, $67, $68, $69
ICE_SMASH2:         dc.w     $6A, $6B, $6C, $6D
LAST_SMASH1:        dc.w     $12C, $12D, $12E, $12F
LAST_SMASH2:        dc.w     $1A2, $1A3, $1A4, $1A5
HINT_ONLY:          dc.w     0
SECRET_TILE:        dc.w     0
SECTL_AN:           dc.w     0
SECHINT_SPR:        dc.w     0
SECRET_SPR:         dc.w     0
SECTL_X:            dc.w     0
SECTL_Y:            dc.w     0
SECTL_TAB:          dc.b     0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, $FF, 0
ZOOL_DIES:          dc.b     0
MAINGUY_ON:         dc.b     0
LEVEL_NUM:          dc.b     0
STAGE_NUM:          dc.b     0
LEVNAME_X:          dc.w     0
LEVNAME_Y:          dc.w     0
LEVNAME_S:          dc.w     0
FOOT_TAP:           dc.w     0
SHOT_TAB:           dcb.l    10, 0
lbL01F788:          dcb.l    6, 0
Y_CENTRE:           dc.b     0, $80
PULLBACK:           dc.w     0
ITS_ZOOL:           dcb.b    2, 0
ZOOL_HGT:           dc.w     0
DESTROYER:          dc.w     0
INVULNERABLE:       dc.w     0
SHADE_BUFF:         dc.w     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
                    dc.w     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
END_SHADE:          dc.l     0
SHADE_FACE:         dc.w     0
SHADE_TIME:         dc.w     0
SHADE_WAIT:         dc.b     0
SHADE_ON:           dc.b     0
COL_MODE:           dcb.b    2, 0
OLDKEY:             dcb.b    2, 0
FIRE_REPT:          dc.w     0
FIRE_CNT:           dcb.b    2, 0
FORCE_LR:           dcb.b    2, 0
SPIN_ANIM:          dc.w     43, 43, 44, 44, 51, 51, 52, 52, -1
BIGB_POW:           dc.b     0
BIG_BOMB:           dc.b     0
SHOT_SPR:           dc.w     0
DRAGGED:            dc.b     0
AFTER_DRAG:         dc.b     0
ZOOL_MOVE:          dc.w     0
ZOOL_FACE:          dc.b     0
INAIR:              dc.b     0
ZOOL_SPR:           dc.w     0
ZOOL_ANIM:          dc.w     0
AGRIP_WALL:         dc.b     0
GRIP_FIRE:          dc.b     0
STOP_OFF:           dc.b     0
JOY_REPT:           dc.b     0
OLDJOY:             dc.b     0, 0
GRIPSWR_AN:         dc.w     62, 62, 62, 62, 62, 63, 63, 63, 63, 63, 64, 64
                    dc.w     64, 64, 64, -1
GRIPSWL_AN:         dc.w     64, 64, 64, 64, 64, 63, 63, 63, 63, 63, 62, 62
                    dc.w     62, 62, 62, -1
X_FRICTION:         dc.w     4
Y_FRICTION:         dc.w     1
GRAVITY:            dc.w     9
PAUSE_MOVE:         dc.b     0, 0
GRAD_CHK:           dc.b     0
GRADIENT:           dc.b     0
SLIPPY_SLP:         dc.b     0
SLIPPY_CNT:         dc.b     0
UPSLOPE:            dc.b     0
FEET_CHECK:         dc.b     0
SLIP_WALL:          dc.w     0
GRAV_WAIT:          dc.w     0
X_FORCE:            dc.w     0
X_SPEED:            dc.w     0
ZOOL_XDIS:          dc.w     0
XSPD_REM:           dc.w     0
Y_FORCE:            dc.w     0
Y_SPEED:            dc.w     0
ZOOL_YDIS:          dc.w     0
YSPD_REM:           dc.w     0
ZOOL_SCRX:          dc.w     160
ZOOL_SCRY:          dc.w     128
ZOOL_X:             dc.w     0
ZOOL_Y:             dc.w     0
SHIELD_PTR:         dc.w     0
SHIELD_SPR:         dc.w     0
SHIELD_ON:          dc.w     0
BLINK_WAIT:         dc.w     0
ZOOL_HIT:           dc.b     0
BULL_POWER:         dc.b     1
WHIP_ON:            dc.w     0
BIGB_SPR:           dc.w     0
SMART_BOMB:         dc.w     0
WHIP_X:             dc.w     0
WHIP_Y:             dc.w     0
CANNOT_KILL:        dc.w     0
GROUNDED:           dc.b     0
WALLED:             dc.b     0
JOYPOS:             dc.w     0
KEYREC:             dc.w     0
KEYVALUE:           dc.w     0
RUN_ANIM:           dc.b     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2
                    dc.b     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4
                    dc.b     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6
                    dc.b     6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
BEACONS_TAB:        dcb.l    12, 0
BEACONS:            dcb.b    2, 0
BEG_BEACS:          dc.l     0
ZOONB_X:            dc.w     0
ZOONB_Y:            dc.w     0
ZOONB_YDIS:         dc.w     0
ZOONB_GOT:          dc.w     0
ZOONB_DRAWN:        dc.w     0
ZOONB_WAIT:         dc.w     0
BADETS_TAB:         dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
BIRD_BANIM:         dc.l     MENT_SHAKE
                    dc.l     MENT_APPR
                    dc.l     MENT_POSE
                    dc.l     MENT_CRCH
                    dc.l     MENT_JUMP
MENT_SHAKE:         dc.w     0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0
                    dc.w     0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1
                    dc.w     0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0
                    dc.w     0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, -1
MENT_APPR:          dc.w     3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10
                    dc.w     10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15
                    dc.w     $10, $10, $10, $11, $11, $11, $12, $12, $12, $13, $13, $13
                    dc.w     $13, $13, $13, $13, $13, $13, $13, $13, $13, $13, $13, $13
                    dc.w     $13, $13, $13, $13, $13, $13, $13, $13, $13, $13, $13, $13
                    dc.w     $13, $13, $13, $13, $13, $13, $13, $13, -1
MENT_POSE:          dc.w     $14, $14, $14, $15, $15, $15, $16, $16, $16, $17, $17, $17
                    dc.w     $16, $16, $16, $15, $15, $15, $18, $18, $18, $19, $19, $19
                    dc.w     $1A, $1A, $1A, $1B, $1B, $1B, $1A, $1A, $1A, $19, $19, $19
                    dc.w     $1C, $1C, $1C, $1D, $1D, $1D, $1E, $1E, $1E, $1F, $1F, $1F
                    dc.w     $1E, $1E, $1E, $1D, $1D, $1D, $1C, $1C, $1C, $18, $18, $18
                    dc.w     $19, $19, $19, $1A, $1A, $1A, $1B, $1B, $1B, $1A, $1A, $1A
                    dc.w     $19, $19, $19, -1
MENT_CRCH:          dc.w     $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20, $20
                    dc.w     $20, $20, $20, $20, $20, $20, $20, $20, -1
MENT_JUMP:          dc.w     $21, $21, $21, $21, $21, $21, $21, $21, $21, $21, $21, $21
                    dc.w     $21, $21, $21, $21, $21, $21, $22, $22, $22, $23, $23, $23
                    dc.w     $24, $24, $24, $25, $25, $25, $25, $25, $25, $25, $25, $25
                    dc.w     $25, $25, $25, $25, $25, $25, $25, $25, $25, $25, $24, $24
                    dc.w     $23, $23, $22, $22, $26, $26, $26, $26, $26, $26, $27, $28
                    dc.w     $29, $2A, $2B, $2C, $2D, -1
BIRDM_FRMS:         dc.l     MBBD0
                    dc.l     MBBD1
                    dc.l     MBBD2
                    dc.l     MBBD3
                    dc.l     MBBD4
                    dc.l     MBBD5
                    dc.l     MBBD6
                    dc.l     MBBD7
                    dc.l     MBBD8
                    dc.l     MBBD9
                    dc.l     MBBD10
                    dc.l     MBBD11
                    dc.l     MBBD12
                    dc.l     MBBD13
                    dc.l     MBBD14
                    dc.l     MBBD15
                    dc.l     MBBD16
                    dc.l     MBBD17
                    dc.l     MBBD18
                    dc.l     MBBD19
                    dc.l     MBBD20
                    dc.l     MBBD21
                    dc.l     MBBD22
                    dc.l     MBBD23
                    dc.l     MBBD24
                    dc.l     MBBD25
                    dc.l     MBBD26
                    dc.l     MBBD27
                    dc.l     MBBD28
                    dc.l     MBBD29
                    dc.l     MBBD30
                    dc.l     MBBD31
                    dc.l     MBBD32
                    dc.l     MBBD33
                    dc.l     MBBD34
                    dc.l     MBBD35
                    dc.l     MBBD36
                    dc.l     MBBD37
                    dc.l     MBBD38
                    dc.l     MBBD39
                    dc.l     MBBD40
                    dc.l     MBBD41
                    dc.l     MBBD42
                    dc.l     MBBD43
                    dc.l     MBBD44
                    dc.l     MBBD45
                    dc.l     MBBD46
BULB_BANIM:         dc.l     MB1_APPEAR
                    dc.l     MB1_CROUCH
                    dc.l     MB1_JUMP
MB1_APPEAR:         dc.w     0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8
                    dc.w     8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14
                    dc.w     14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
                    dc.w     15, 15, 15, 15, 15, 15, 15, 15, -1
MB1_CROUCH:         dc.w     16, 16, 17, 17, 17, 18, 18, 18, 18, 18, 18, 17
                    dc.w     17, 17, 16, 16, -1
MB1_JUMP:           dc.w     19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19
                    dc.w     19, 19, 19, 19, 19, 19, 16, 16, 20, 20, 16, 16
                    dc.w     20, 20, 16, 16, 20, 20, 16, 16, 20, 20, 16, 16
                    dc.w     20, 20, 16, 16, 20, 20, 16, 16, 20, 20, 16, 16
                    dc.w     20, 20, 16, 16, 21, 21, 21, 21, 21, 21, 21, 21
                    dc.w     21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, -1
BULBM_FRMS:         dc.l     MBBB0
                    dc.l     MBBB1
                    dc.l     MBBB2
                    dc.l     MBBB3
                    dc.l     MBBB4
                    dc.l     MBBB5
                    dc.l     MBBB6
                    dc.l     MBBB7
                    dc.l     MBBB8
                    dc.l     MBBB9
                    dc.l     MBBB10
                    dc.l     MBBB11
                    dc.l     MBBB12
                    dc.l     MBBB13
                    dc.l     MBBB14
                    dc.l     MBBB15
                    dc.l     MBBB16
                    dc.l     MBBB17
                    dc.l     MBBB18
                    dc.l     MBBB19
                    dc.l     MBBB20
                    dc.l     MBBB21
                    dc.l     MBBB22
                    dc.l     MBBB23
                    dc.l     MBBB24
TOOT_BANIM:         dc.l     MB2_SWAY
                    dc.l     MB2_CRACK
MB2_SWAY:           dc.w     1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2
                    dc.w     2, -1
MB2_CRACK:          dc.w     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
                    dc.w     6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
                    dc.w     7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
                    dc.w     8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9
                    dc.w     9, 9, 9, 9, -1
TOOTM_FRMS:         dc.l     MBTC0
                    dc.l     MBTC1
                    dc.l     MBTC2
                    dc.l     MBTC3
                    dc.l     MBTC4
                    dc.l     MBTC5
                    dc.l     MBTC6
                    dc.l     MBTC7
                    dc.l     MBTC8
                    dc.l     MBTC9
MBTC0:              dc.w     3, -22, -46, 4, 5, -45, 1, -32, -120, 0, -32
                    dc.w     -104, 2, $17, -75, -1
MBTC1:              dc.w     3, -20, -45, 4, 7, -44, 1, -32, -120, 0, -32
                    dc.w     -104, 2, $17, -75, -1
MBTC2:              dc.w     3, -18, -46, 4, 9, -45, 1, -31, -120, 0, -32
                    dc.w     -104, 2, $17, -75, -1
MBTC3:              dc.w     6, -32, -64, -1
MBTC4:              dc.w     7, -32, -64, -1
MBTC5:              dc.w     8, -32, -64, -1
MBTC6:              dc.w     9, -32, -64, -1
MBTC7:              dc.w     10, -32, -64, -1
MBTC8:              dc.w     11, -32, -64, -1
MBTC9:              dc.w     12, -32, -64, -1
MBBD0:              dc.w     $14, -32, -64, -1
MBBD1:              dc.w     $15, -32, -64, -1
MBBD2:              dc.w     0, -30, -60, -1
MBBD3:              dc.w     5, -32, -32, 3, -5, -32, 11, -32, -47, 8, 1
                    dc.w     -47, 0, -30, -62, -1
MBBD4:              dc.w     5, -32, -32, 3, -5, -32, 11, -33, -47, 8, 2
                    dc.w     -47, 0, -30, -64, -1
MBBD5:              dc.w     5, $FFE0, $FFE0, 3, $FFFB, $FFE0, 11, $FFDE, $FFD1, 8
                    dc.w     3, $FFD1, 0, $FFE0, $FFBE, -1
MBBD6:              dc.w     5, $FFDF, $FFE0, 3, $FFFC, $FFE0, 11, $FFDD, $FFD0, 8, 4
                    dc.w     $FFD0, 0, $FFE0, $FFBD, -1
MBBD7:              dc.w     5, $FFDE, $FFE0, 3, $FFFD, $FFE0, 11, $FFDC, $FFD0, 8, 5
                    dc.w     $FFD0, 1, $FFE0, $FFBC, -1
MBBD8:              dc.w     5, $FFDD, $FFE0, 3, $FFFE, $FFE0, 11, $FFDB, $FFD0, 8, 6
                    dc.w     $FFD0, 1, $FFE0, $FFBB, -1
MBBD9:              dc.w     5, $FFDC, $FFE0, 3, $FFFF, $FFE0, 11, $FFDA, $FFCF, 8, 7
                    dc.w     $FFCF, 1, $FFE0, $FFBA, -1
MBBD10:             dc.w     5, $FFDB, $FFE0, 3, 0, $FFE0, 11, $FFD9, $FFCF, 8, 8, -49
                    dc.w     1, $FFE0, $FFB9, $10, $FFE6, $FFCA, -1
MBBD11:             dc.w     5, $FFDA, $FFE0, 3, 1, $FFE0, 11, $FFD8, $FFCF, 8, 9, -49
                    dc.w     1, $FFE0, $FFB8, $10, $FFE6, $FFC9, -1
MBBD12:             dc.w     5, $FFD9, $FFE0, 3, 2, $FFE0, 11, $FFD6, $FFCE, 8, 11, -50
                    dc.w     2, $FFE0, $FFB7, $10, $FFE6, $FFC8, -1
MBBD13:             dc.w     5, $FFD8, $FFE0, 3, 3, $FFE0, 11, $FFD4, $FFCE, 8, 13, -50
                    dc.w     2, $FFE0, $FFB6, $10, $FFE6, $FFC7, -1
MBBD14:             dc.w     5, $FFD7, $FFE0, 3, 4, $FFE0, 11, $FFD2, $FFCE, 8, 15, -50
                    dc.w     2, $FFE0, $FFB5, $10, $FFE6, $FFC6, -1
MBBD15:             dc.w     5, $FFD6, $FFE0, 3, 5, $FFE0, 11, $FFD0, $FFCD, 8, 17
                    dc.w     $FFCD, 2, $FFE0, $FFB4, $10, $FFE6, $FFC5, -1
MBBD16:             dc.w     5, $FFD5, $FFE0, 3, 6, $FFE0, 11, $FFCE, $FFCD, 8, 19
                    dc.w     $FFCD, 2, $FFE0, $FFB3, $10, $FFE6, $FFC4, -1
MBBD17:             dc.w     5, $FFD4, $FFE0, 3, 7, $FFE0, 11, $FFCB, $FFCC, 8, 22
                    dc.w     $FFCC, 2, $FFE0, $FFB2, $10, $FFE6, $FFC3, -1
MBBD18:             dc.w     5, $FFD3, $FFE0, 3, 8, $FFE0, 12, $FFBA, $FFB0, 9, 23
                    dc.w     $FFB0, 2, $FFE0, $FFB1, 15, $FFE7, $FFC2, -1
MBBD19:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 12, $FFBA, $FFB0, 9, 23
                    dc.w     $FFB0, 2, $FFE0, $FFB0, 15, $FFE7, $FFC1, -1
MBBD20:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFC8, 8, 23
                    dc.w     $FFC9, 2, $FFE0, $FFB0, 13, $FFEA, $FFBB, -1
MBBD21:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFC9, 8, 23
                    dc.w     $FFCA, 2, $FFE0, $FFB1, 13, $FFEA, $FFBC, -1
MBBD22:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFCA, 8, 23
                    dc.w     $FFCB, 2, $FFE0, $FFB2, 13, $FFEA, $FFBD, -1
MBBD23:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFCB, 8, 23
                    dc.w     $FFCC, 2, $FFE0, $FFB3, 13, $FFEA, $FFBE, -1
MBBD24:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFC8, 8, 23
                    dc.w     $FFC9, 2, $FFE0, $FFB0, 15, $FFEA, $FFBF, -1
MBBD25:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFC9, 8, 23
                    dc.w     $FFCA, 2, $FFE0, $FFB1, 15, $FFEA, $FFC0, -1
MBBD26:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFCA, 8, 23
                    dc.w     $FFCB, 2, $FFE0, $FFB2, 15, $FFEA, $FFC1, -1
MBBD27:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFCB, 8, 23
                    dc.w     $FFCC, 2, $FFE0, $FFB3, 15, $FFEA, $FFC2, -1
MBBD28:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFC8, 8, 23
                    dc.w     $FFC9, 2, $FFE0, $FFB0, 14, $FFE7, $FFBB, -1
MBBD29:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFC9, 8, 23
                    dc.w     $FFCA, 2, $FFE0, $FFB1, 14, $FFE7, $FFBC, -1
MBBD30:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFCA, 8, 23
                    dc.w     $FFCB, 2, $FFE0, $FFB2, 14, $FFE7, $FFBD, -1
MBBD31:             dc.w     5, $FFD2, $FFE0, 3, 9, $FFE0, 10, $FFC9, $FFCB, 8, 23
                    dc.w     $FFCC, 2, $FFE0, $FFB3, 14, $FFE7, $FFBE, -1
MBBD32:             dc.w     5, $FFD7, $FFE0, 3, 4, $FFE0, 11, $FFD8, $FFD5, 8, 9, -43
                    dc.w     2, $FFE0, $FFB8, 15, $FFE7, $FFC6, -1
MBBD33:             dc.w     6, $FFDC, $FFDE, 4, 6, $FFDE, 10, $FFCA, $FFC5, 7, 22
                    dc.w     $FFC5, 2, $FFE0, $FFA6, 15, $FFE7, $FFB3, -1
MBBD34:             dc.w     6, $FFDC, $FFDE, 4, 6, $FFDE, 12, $FFB9, $FFA5, 9, 24
                    dc.w     $FFA5, 2, $FFE0, $FFA6, $10, $FFE6, $FFB8, -1
MBBD35:             dc.w     6, $FFDC, $FFDE, 4, 6, $FFDE, 12, $FFB9, $FFA5, 9, 24
                    dc.w     $FFA5, $16, $FFE0, $FFA6, $11, $FFE0, $FF9E, -1
MBBD36:             dc.w     6, $FFDC, $FFDE, 4, 6, $FFDE, 12, $FFB9, $FFA5, 9, 24
                    dc.w     $FFA5, $16, $FFE0, $FFA6, $12, $FFE0, $FF8E, -1
MBBD37:             dc.w     6, $FFDC, $FFDE, 4, 6, $FFDE, 12, $FFB9, $FFA5, 9, 24
                    dc.w     $FFA5, $16, $FFE0, $FFA6, $13, $FFE0, $FF7E, -1
MBBD38:             dc.w     6, $FFDC, $FFDE, 4, 6, $FFDE, 12, $FFBC, $FFA9, 9, 20
                    dc.w     $FFA9, 2, $FFE0, $FFA6, 15, $FFE7, $FFB3, -1
MBBD39:             dc.w     5, $FFD8, $FFE0, 3, 4, $FFE0, 11, $FFC8, $FFC1, 8, 24
                    dc.w     $FFC1, 2, $FFE0, $FFA8, 15, $FFE7, $FFB8, -1
MBBD40:             dc.w     5, $FFD8, $FFE0, 3, 4, $FFE0, 11, $FFC8, $FFC3, 8, 24
                    dc.w     $FFC3, 2, $FFE0, $FFAA, 15, $FFE7, $FFBA, -1
MBBD41:             dc.w     5, $FFD8, $FFE0, 3, 4, $FFE0, 11, $FFC8, $FFC5, 8, 24
                    dc.w     $FFC5, 2, $FFE0, $FFAC, 15, $FFE7, $FFBC, -1
MBBD42:             dc.w     5, $FFD7, $FFE0, 3, 5, $FFE0, 11, $FFC8, $FFC7, 8, 24
                    dc.w     $FFC7, 2, $FFE0, $FFAE, 15, $FFE7, $FFBE, -1
MBBD43:             dc.w     5, $FFD6, $FFE0, 3, 6, $FFE0, 11, $FFC8, $FFC9, 8, 24
                    dc.w     $FFC9, 2, $FFE0, $FFB0, 15, $FFE7, $FFC0, -1
MBBD44:             dc.w     5, $FFD5, $FFE0, 3, 7, $FFE0, 11, $FFC8, $FFCB, 8, 24
                    dc.w     $FFCB, 2, $FFE0, $FFB2, 15, $FFE7, $FFC2, -1
MBBD45:             dc.w     5, $FFD4, $FFE0, 3, 8, $FFE0, 11, $FFC8, $FFCA, 8, 24
                    dc.w     $FFCA, 2, $FFE0, $FFB1, 15, $FFE7, $FFC1, -1
MBBD46:             dc.w     5, $FFD7, $FFE0, 3, 4, $FFE0, 11, $FFD8, $FFD5, 8, 9, -43
                    dc.w     $17, $FFE0, $FFB8, -1
MBBB0:              dc.w     11, $FFE2, $FFD9, 8, $FFFF, $FFD9, 13, $FFF0, $FFC8, 0
                    dc.w     $FFE0, $FFC0, -1
MBBB1:              dc.w     11, $FFE1, $FFD8, 8, 0, $FFD8, 13, $FFF0, $FFC8, 0, -32
                    dc.w     $FFC0, -1
MBBB2:              dc.w     11, $FFE0, $FFD7, 8, 1, $FFD7, 13, $FFF0, $FFC4, 0, -32
                    dc.w     $FFC0, -1
MBBB3:              dc.w     11, $FFDF, $FFD6, 8, 2, $FFD6, 13, $FFF0, $FFC1, 0, -32
                    dc.w     $FFC0, -1
MBBB4:              dc.w     11, $FFDE, $FFD5, 8, 3, $FFD5, 13, $FFF0, $FFBE, 1, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB5:              dc.w     11, $FFDD, $FFD4, 8, 4, $FFD4, 13, $FFF0, $FFBC, 1, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB6:              dc.w     11, $FFDC, $FFD3, 8, 5, $FFD3, 13, $FFF0, $FFBA, 1, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB7:              dc.w     11, $FFDB, $FFD2, 8, 6, $FFD2, 13, $FFF0, $FFB8, 1, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB8:              dc.w     11, $FFDA, $FFD1, 8, 7, $FFD1, 13, $FFF0, $FFB6, 2, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB9:              dc.w     11, $FFD9, $FFD0, 8, 8, $FFD0, 13, $FFF0, $FFB5, 2, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB10:             dc.w     11, $FFD8, $FFCF, 8, 9, $FFCF, 13, $FFF0, $FFB4, 2, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB11:             dc.w     11, $FFD7, $FFCE, 8, 10, $FFCE, 13, $FFF0, $FFB3, 2, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB12:             dc.w     11, $FFD6, $FFCD, 8, 11, $FFCD, 13, $FFF0, $FFB2, 3, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB13:             dc.w     11, $FFD5, $FFCC, 8, 12, $FFCC, 13, $FFF0, $FFB1, 3, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB14:             dc.w     11, $FFD4, $FFCB, 8, 13, $FFCB, 13, $FFF0, $FFB0, 3, -32
                    dc.w     $FFC0, $12, $FFE7, $FFD5, -1
MBBB15:             dc.w     10, $FFC1, $FFB8, 7, $10, $FFB8, 13, $FFF0, $FFAF, 3, -32
                    dc.w     $FFC0, $10, $FFE7, $FFD7, -1
MBBB16:             dc.w     11, $FFCF, $FFD0, 8, $12, $FFD0, 13, $FFF0, $FFAF, 3, -32
                    dc.w     $FFC0, $10, $FFE7, $FFD9, -1
MBBB17:             dc.w     11, $FFCD, $FFD4, 8, $14, $FFD4, 13, $FFF0, $FFB3, 4, -32
                    dc.w     $FFC0, 15, $FFE8, $FFD7, -1
MBBB18:             dc.w     11, $FFCC, $FFD6, 8, $16, $FFD6, 13, $FFF0, $FFB5, 5, -32
                    dc.w     $FFC0, 14, $FFE7, $FFD9, -1
MBBB19:             dc.w     12, $FFCF, $FFD1, 9, $11, $FFD1, 13, $FFF0, $FFAF, 3, -32
                    dc.w     $FFC0, $10, $FFE8, $FFD8, -1
MBBB20:             dc.w     11, $FFCF, $FFD0, 8, $12, $FFD0, 13, $FFF0, $FFAF, 6, -32
                    dc.w     $FFC0, $11, $FFE7, $FFD9, -1
MBBB21:             dc.w     10, $FFC0, $FFBA, 7, $11, $FFBA, 13, $FFF0, $FFAB, 3, -32
                    dc.w     $FFC0, $10, $FFE7, $FFD6, -1
MBBB22:             dc.w     0, $FFE0, $FFC0, -1
MBBB23:             dc.w     11, $FFCF, $FFD0, 8, $12, $FFD0, 13, $FFF0, $FFAF, $13
                    dc.w     $FFE0, $FFC0, $12, $FFE7, $FFD9, -1
MBBB24:             dc.w     11, $FFCF, $FFD0, 8, $12, $FFD0, 13, $FFF0, $FFAF, $13
                    dc.w     $FFE0, $FFC0, 14, $FFE7, $FFD9, -1
SNKM_FRMS:          dc.l     MBSN0
                    dc.l     MBSN1
                    dc.l     MBSN2
                    dc.l     MBSN3
                    dc.l     MBSN4
                    dc.l     MBSN5
                    dc.l     MBSN6
                    dc.l     MBSN7
                    dc.l     MBSN8
                    dc.l     MBSN9
                    dc.l     MBSN10
                    dc.l     MBSN11
                    dc.l     MBSN12
                    dc.l     MBSN13
                    dc.l     MBSN14
                    dc.l     MBSN15
SNK_BANIM:          dc.l     MB3_APP
MB3_APP:            dc.w     1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5
                    dc.w     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7
                    dc.w     8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, -1
MBSN0:              dc.w     0, -32, -64, -1
MBSN1:              dc.w     0, -32, -64, -1
MBSN2:              dc.w     1, -32, -64, -1
MBSN3:              dc.w     2, -32, -64, -1
MBSN4:              dc.w     3, -32, -64, -1
MBSN5:              dc.w     4, -32, -64, -1
MBSN6:              dc.w     5, -38, -70, 5, -16, -70, 5, 6, -70, 5, -38
                    dc.w     -48, 5, 6, -48, 5, -38, -26, 5, -16, -26, 5, 6
                    dc.w     -26, 13, -16, -48, -1
MBSN7:              dc.w     6, -38, -70, 6, -16, -70, 6, 6, -70, 6, -38
                    dc.w     -48, 6, 6, -48, 6, -38, -26, 6, -16, -26, 6, 6
                    dc.w     -26, 14, -16, -48, -1
MBSN8:              dc.w     7, -38, -70, 7, -16, -70, 7, 6, -70, 7, -38
                    dc.w     -48, 7, 6, -48, 7, -38, -26, 7, -16, -26, 7, 6
                    dc.w     -26, 15, -16, -48, -1
MBSN9:              dc.w     8, -39, -70, 8, -16, -70, 8, 7, -70, 8, -39
                    dc.w     -48, 8, 7, -48, 8, -39, -26, 8, -16, -26, 8, 7
                    dc.w     -26, $10, -16, -48, -1
MBSN10:             dc.w     9, -41, -70, 9, -16, -70, 9, 10, -70, 9, -41
                    dc.w     -48, 9, 10, -48, 9, -41, -26, 9, -16
                    dc.w     -26, 9, 10, -26, 17, -16, -48, -1
MBSN11:             dc.w     9, -16, -16, -1
MBSN12:             dc.w     10, -16, -16, -1
MBSN13:             dc.w     11, -16, -16, -1
MBSN14:             dc.w     12, -16, -16, -1
MBSN15:             dc.w     17, -16, -16, -1
MENTAL_SPRS:        dc.w     0
BADDY_ON:           dc.b     0
SCROLL_OFF:         dc.b     0
END_FIGHT:          dcb.b    2, 0
END_X:              dc.w     0
END_YT:             dc.w     0
END_YB:             dc.w     0
BADDY_RTN:          dc.l     0
ICECUBES:           dc.w     0
ICEBAD_TAB:         dcb.w    $3F, 0
                    dcb.w    $3F, 0
                    dcb.w    9, 0
CSEG_TAB:           dcb.w    7, 0
SNK_SEGCIRC:        dc.w     $1208, $120, $121D, $119, $1227, $106, $1224, $F0, $1213
                    dc.w     $E2, $11FD, $E2, $11EC, $F0, $11E8, $106, $11F3, $119
BADDY_PATH:         dc.w     0
CIRC_CNT:           dc.w     $32
SEGMENTS:           dc.w     0
SNBAD_XYS:          dc.w     $1208, $106
                    dc.w     $1208, $120, $1221, $120, $1221, $10A, $1221, $F4, $1208
                    dc.w     $F4, $11EF, $F4, $11EF, $10A, $11EF, $120
SNMB_PATH:          dcb.w    $3F, 0
                    dcb.w    $3F, 0
                    dcb.w    $22, 0
BADDY_Y2:           dc.l     0
PILL_HIT:           dc.w     0
PILL_CRUSH:         dc.w     0
PILL_X:             dc.w     0
PILLS:              dc.w     4, 24, 4, 104, 4, 216, 4, 296
PILL_RES:           dc.l     0
MBGENER_ON:         dc.b     0
MBBOLTS_ON:         dc.b     0
MBGENER_X:          dc.w     0
MBBOLT1_X:          dc.w     0
MBBOLT2_X:          dc.w     0
MBBOLT3_X:          dc.w     0
MBBOLT_Y:           dc.w     0
MBBOLT_FRM:         dc.w     0
MBBOLT_AN:          dc.w     0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 2, 2, 2, 1, 1, 1, -1
BADET_LNCH:         dc.w     -1, -64, 2, -32, 1, -48, -2, -16, -1
                    dc.w     -96, 1, -32, 3, -32, -3, -48
BADDY_WID:          dc.w     64
BADDY_HGT:          dc.w     88
BADEXP_OFFS:        dc.w     16, 16, 24, 5, 4, 24, 32, 20, 11, 27, 14, 5, 12, 22
                    dc.w     31, 9, 5, 17, 27, 2, 19, 11, 13, 24, 15, 7, 6, 6, 3, 21
                    dc.w     30, 5, 29, 4, 13, 18, 20, 23, 22, 10, -1
BADDY_LX:           dc.l     0
BADDY_LY:           dc.l     0
BADDY_X:            dc.w     0
BADDY_XDIS:         dc.w     0
BADDY_Y:            dc.w     0
BADDY_YDIS:         dc.w     0
BADDY_SPR:          dc.w     0
BADDY_CNT:          dc.w     0
BADDY_FRM:          dc.w     0
BADDY_AN:           dc.w     0
FIST1_X:            dc.w     0
FIST2_X:            dc.w     0
FIST1_Y:            dc.w     0
FIST2_Y:            dc.w     0
FIST1XDIS:          dc.w     0
FIST2XDIS:          dc.w     0
FIST1YDIS:          dc.w     0
FIST2YDIS:          dc.w     0
FIST1_ACT:          dc.w     0
FIST2_ACT:          dc.w     0
BADDY_FRMS:         dc.l     0
MESSAGE_X:          dc.w     0
MESSAGE_Y:          dc.w     0
MESSAGE_SPR:        dc.w     0
BADDY_EN:           dc.w     0
SHRINK_X:           dc.w     0
SHRINK_Y:           dc.w     0
BADDY_DEAD:         dc.b     0
FIST_NUM:           dc.b     0
BADDY_ACT:          dc.b     0
BADDY_HIT:          dc.b     0
BADET_AN:           dc.b     0
MESSAGE_ON:         dc.b     0
LASTLOOK:           dcb.b    2, 0
BANIMS_TAB:         dc.l     0
FIST_X:             dc.w     0
FIST_Y:             dc.w     0
FISTXDIS:           dc.w     0
FISTYDIS:           dc.w     0
FIST_ACT:           dc.w     0
CUST_ATTRBS:        dc.l     0
CUST_REFS:          dc.l     0
CUSTOM_ON:          dcb.b    2, 0
CUST_TILE:          dc.w     0
RIGHT_MARG:         dc.w     0
BOT_MARG:           dc.w     0
STARTX:             dc.w     0
STARTY:             dc.w     0
REF_MAP:            dc.l     0
TILES:              dc.l     0
CURRENT_MAP:        dc.l     0
PHYSADR:            dc.l     BUFF1_START
LOGADR:             dc.l     BUFF2_START
PRLX_XDIS:          dc.l     0
PRLX_YDIS:          dc.l     0
XBEGSCROLL:         dc.w     0
YBEGSCROLL:         dc.w     0
PARA_XSCRO:         dc.w     0
PARA_YSCRO:         dc.w     0
PARA_XREM:          dc.w     0
HORZ_OFF:           dc.w     0
MAP_SRC:            dcb.w    2, 0
MAP_DEST:           dcb.w    2, 0
MAP_WIDTH:          dc.w     0
MAP_HEIGHT:         dc.w     0
XSCRO:              dc.w     0
XPOS:               dc.w     0
XSCROLL:            dc.w     0
YSCROLL:            dc.w     0
OLD_XSCR:           dc.w     0
OLD_YSCR:           dc.w     0
ANDYFRAME:          dcb.b    2, 0
SCROLL:             dc.l     BUFF1_START
BUFF_PTR:           dc.l     BUFF1_START
MAP_PTR:            dc.l     0
MAP_LINE:           dc.w     0
YSCRO:              dc.w     0
XREM:               dcb.b    2, 0
YREM:               dcb.b    2, 0
SCROLLA:            dc.l     BUFF1_START
BUFF_PTR_A:         dc.l     BUFF1_START
MAP_PTR_A:          dc.l     0
MAP_LINE_A:         dc.w     0
YSCRO_A:            dc.w     0
XREM_A:             dc.w     0
YREM_A:             dc.w     0
SCROLLB:            dc.l     BUFF2_START
BUFF_PTR_B:         dc.l     BUFF2_START
MAP_PTR_B:          dc.l     0
MAP_LINE_B:         dc.w     0
YSCRO_B:            dc.w     0
XREM_B:             dc.w     0
YREM_B:             dc.w     0
SCROLLC:            dc.l     BUFF3_START
BUFF_PTR_C:         dc.l     BUFF3_START
MAP_PTR_C:          dc.l     0
MAP_LINE_C:         dc.w     0
YSCRO_C:            dc.w     0
XREM_C:             dc.w     0
YREM_C:             dc.w     0
CLEAR:              dc.l     0
TEST_MODE:          dc.b     1, 0
HARDSPR:            dcb.b    4, 0
RESTART_TAB:        dcb.l    3, 0

; these datas are filled up to END_SPRDATA when loading the datas/enemydat file
; 51784 bytes
BD1_SPRS:           dcb.b    814, 0
BD1_BANK2:          dcb.b    1034, 0
BD1_BANK3:          dcb.b    774, 0
BD1_BANK4:          dcb.b    656, 0
CRAPPER:            dcb.b    154, 0
BD1_FIXSPRS:        dcb.b    40, 0
EGG_BOMB_S:         dcb.b    280, 0
BD2_SPRS:           dcb.b    480, 0
BD2_BANK2:          dcb.b    482, 0
BD2_BANK3:          dcb.b    864, 0
BD2_BANK4:          dcb.b    634, 0
BD2_BANK5:          dcb.b    600, 0
BD2_BANK6:          dcb.b    182, 0
BD2_FIXSPRS:        dcb.b    150, 0
CHECKIT:            dcb.b    592, 0
CHECK2:             dcb.b    14, 0
BD3_SPRS:           dcb.b    562, 0
BD3_BANK2:          dcb.b    744, 0
BD3_BANK3:          dcb.b    1384, 0
BD3_BANK4:          dcb.b    744, 0
BD3_FIXSPRS:        dcb.b    88, 0
EGG_BOMB_S3:        dcb.b    1096, 0
BB1_SPRS:           dcb.b    920, 0
BB1_BANK2:          dcb.b    824, 0
BB1_BANK3:          dcb.b    736, 0
BB1_BANK4:          dcb.b    384, 0
BB1_FIXSPRS:        dcb.b    48, 0
EGG_BOMB_S_BB:      dcb.b    240, 0
EXP_BOMB_S_BB:      dcb.b    684, 0
BB2_SPRS:           dcb.b    1244, 0
BB2_BANK2:          dcb.b    640, 0
BB2_BANK3:          dcb.b    896, 0
BB2_FIXSPRS:        dcb.b    48, 0
EGG_BOMB_S2_BB:     dcb.b    144, 0
EXP_BOMB_S2_BB:     dcb.b    184, 0
BB3_SPRS:           dcb.b    552, 0
BB3_BANK2:          dcb.b    624, 0
BB3_BANK3:          dcb.b    576, 0
BB3_BANK4:          dcb.b    820, 0
BB3_FIXSPRS:        dcb.b    48, 0
EGG_BOMB_S3_BB:     dcb.b    240, 0
EXP_BOMB_S3_BB:     dcb.b    252, 0
TC1_SPRS:           dcb.b    708, 0
TC1_BANK2:          dcb.b    960, 0
TC1_BANK3:          dcb.b    500, 0
TC1_FIXSPRS:        dcb.b    544, 0
EGG_BOMB_S_TC:      dcb.b    136, 0
TC2_SPRS:           dcb.b    924, 0
TC2_BANK2:          dcb.b    828, 0
TC2_BANK3:          dcb.b    844, 0
TC2_FIXSPRS:        dcb.b    136, 0
EGG_BOMB_S2_TC:     dcb.b    136, 0
TC3_SPRS:           dcb.b    868, 0
TC3_BANK2:          dcb.b    1220, 0
TC3_BANK3:          dcb.b    440, 0
TC3_FIXSPRS:        dcb.b    136, 0
EGG_BOMB_S3_TC:     dcb.b    136, 0
SN1_SPRS:           dcb.b    952, 0
SN1_BANK2:          dcb.b    1088, 0
SN1_BANK3:          dcb.b    1316, 0
SN1_FIXSPRS:        dcb.b    96, 0
HOMING_BLS_SN:      dcb.b    136, 0
SN2_SPRS:           dcb.b    620, 0
EGG_BOMB_S2_SN:     dcb.b    96, 0
SN3_SPRS:           dcb.b    1008, 0
SN3_BANK2:          dcb.b    784, 0
SN3_BANK3:          dcb.b    952, 0
SN3_FIXSPRS:        dcb.b    136, 0
EGG_BOMB_S3_SN:     dcb.b    96, 0
IC1_SPRS:           dcb.b    1136, 0
IC1_BANK2:          dcb.b    1120, 0
IC1_BANK3:          dcb.b    1084, 0
IC2_SPRS:           dcb.b    564, 0
IC2_BANK2:          dcb.b    1456, 0
IC2_BANK3:          dcb.b    1288, 0
IC2_BANK4:          dcb.b    796, 0
IC3_SPRS:           dcb.b    1188, 0
IC3_BANK2:          dcb.b    848, 0
LS1_SPRS:           dcb.b    1000, 0
LS1_BANK2:          dcb.b    1272, 0
PP1_SPRS:           dcb.b    584, 0
PP2_SPRS:           dcb.b    652, 0
PP3_SPRS:           dcb.b    528, 0
END_SPRDATA:

                    SECTION  zool2rs02DAEC, DATA_C

XY_DATA:            dc.w     0, $FF80, 2, $FF80, 4, $FF80, 7, $FF80, 9, $FF80, 11, $FF80
                    dc.w     13, $FF81, $10, $FF81, $12, $FF81, $14, $FF82, $16, $FF82
                    dc.w     $18, $FF82, $1B, $FF83, $1D, $FF83, $1F, $FF84, $21, $FF84
                    dc.w     $23, $FF85, $25, $FF86, $28, $FF86, $2A, $FF87, $2C, $FF88
                    dc.w     $2E, $FF89, $30, $FF89, $32, $FF8A, $34, $FF8B, $36, $FF8C
                    dc.w     $38, $FF8D, $3A, $FF8E, $3C, $FF8F, $3E, $FF90, $40, $FF91
                    dc.w     $42, $FF92, $44, $FF93, $46, $FF95, $48, $FF96, $49, $FF97
                    dc.w     $4B, $FF98, $4D, $FF9A, $4F, $FF9B, $51, $FF9D, $52, $FF9E
                    dc.w     $54, $FF9F, $56, $FFA1, $57, $FFA2, $59, $FFA4, $5B, $FFA5
                    dc.w     $5C, $FFA7, $5E, $FFA9, $5F, $FFAA, $61, $FFAC, $62, $FFAE
                    dc.w     $63, $FFAF, $65, $FFB1, $66, $FFB3, $68, $FFB5, $69, $FFB7
                    dc.w     $6A, $FFB8, $6B, $FFBA, $6D, $FFBC, $6E, $FFBE, $6F, $FFC0
                    dc.w     $70, $FFC2, $71, $FFC4, $72, $FFC6, $73, $FFC8, $74, $FFCA
                    dc.w     $75, $FFCC, $76, $FFCE, $77, $FFD0, $77, $FFD2, $78, $FFD4
                    dc.w     $79, $FFD6, $7A, $FFD8, $7A, $FFDB, $7B, $FFDD, $7C, $FFDF
                    dc.w     $7C, $FFE1, $7D, $FFE3, $7D, $FFE5, $7E, $FFE8, $7E, $FFEA
                    dc.w     $7E, $FFEC, $7F, $FFEE, $7F, $FFF0, $7F, $FFF3, $80, $FFF5
                    dc.w     $80, $FFF7, $80, $FFF9, $80, $FFFC, $80, $FFFE, $80, 0, $80
                    dc.w     2, $80, 4, $80, 7, $80, 9, $80, 11, $7F, 13, $7F, $10, $7F, $12
                    dc.w     $7E, $14, $7E, $16, $7E, $18, $7D, $1B, $7D, $1D, $7C, $1F
                    dc.w     $7C, $21, $7B, $23, $7A, $25, $7A, $28, $79, $2A, $78, $2C
                    dc.w     $77, $2E, $77, $30, $76, $32, $75, $34, $74, $36, $73, $38
                    dc.w     $72, $3A, $71, $3C, $70, $3E, $6F, $40, $6E, $42, $6D, $44
                    dc.w     $6B, $46, $6A, $48, $69, $49, $68, $4B, $66, $4D, $65, $4F
                    dc.w     $63, $51, $62, $52, $61, $54, $5F, $56, $5E, $57, $5C, $59
                    dc.w     $5B, $5B, $59, $5C, $57, $5E, $56, $5F, $54, $61, $52, $62
                    dc.w     $51, $63, $4F, $65, $4D, $66, $4B, $68, $49, $69, $48, $6A
                    dc.w     $46, $6B, $44, $6D, $42, $6E, $40, $6F, $3E, $70, $3C, $71
                    dc.w     $3A, $72, $38, $73, $36, $74, $34, $75, $32, $76, $30, $77
                    dc.w     $2E, $77, $2C, $78, $2A, $79, $28, $7A, $25, $7A, $23, $7B
                    dc.w     $21, $7C, $1F, $7C, $1D, $7D, $1B, $7D, $18, $7E, $16, $7E
                    dc.w     $14, $7E, $12, $7F, $10, $7F, 13, $7F, 11, $80, 9, $80, 7, $80
                    dc.w     4, $80, 2, $80, 0, $80, $FFFE, $80, $FFFC, $80, $FFF9, $80
                    dc.w     $FFF7, $80, $FFF5, $80, $FFF3, $7F, $FFF0, $7F, $FFEE, $7F
                    dc.w     $FFEC, $7E, $FFEA, $7E, $FFE8, $7E, $FFE5, $7D, $FFE3, $7D
                    dc.w     $FFE1, $7C, $FFDF, $7C, $FFDD, $7B, $FFDB, $7A, $FFD8, $7A
                    dc.w     $FFD6, $79, $FFD4, $78, $FFD2, $77, $FFD0, $77, $FFCE, $76
                    dc.w     $FFCC, $75, $FFCA, $74, $FFC8, $73, $FFC6, $72, $FFC4, $71
                    dc.w     $FFC2, $70, $FFC0, $6F, $FFBE, $6E, $FFBC, $6D, $FFBA, $6B
                    dc.w     $FFB8, $6A, $FFB7, $69, $FFB5, $68, $FFB3, $66, $FFB1, $65
                    dc.w     $FFAF, $63, $FFAE, $62, $FFAC, $61, $FFAA, $5F, $FFA9, $5E
                    dc.w     $FFA7, $5C, $FFA5, $5B, $FFA4, $59, $FFA2, $57, $FFA1, $56
                    dc.w     $FF9F, $54, $FF9E, $52, $FF9D, $51, $FF9B, $4F, $FF9A, $4D
                    dc.w     $FF98, $4B, $FF97, $49, $FF96, $48, $FF95, $46, $FF93, $44
                    dc.w     $FF92, $42, $FF91, $40, $FF90, $3E, $FF8F, $3C, $FF8E, $3A
                    dc.w     $FF8D, $38, $FF8C, $36, $FF8B, $34, $FF8A, $32, $FF89, $30
                    dc.w     $FF89, $2E, $FF88, $2C, $FF87, $2A, $FF86, $28, $FF86, $25
                    dc.w     $FF85, $23, $FF84, $21, $FF84, $1F, $FF83, $1D, $FF83, $1B
                    dc.w     $FF82, $18, $FF82, $16, $FF82, $14, $FF81, $12, $FF81, $10
                    dc.w     $FF81, 13, $FF80, 11, $FF80, 9, $FF80, 7, $FF80, 4, $FF80, 2
                    dc.w     $FF80, 0, $FF80, $FFFE, $FF80, $FFFC, $FF80, $FFF9, $FF80
                    dc.w     $FFF7, $FF80, $FFF5, $FF81, $FFF3, $FF81, $FFF0, $FF81
                    dc.w     $FFEE, $FF82, $FFEC, $FF82, $FFEA, $FF82, $FFE8, $FF83
                    dc.w     $FFE5, $FF83, $FFE3, $FF84, $FFE1, $FF84, $FFDF, $FF85
                    dc.w     $FFDD, $FF86, $FFDB, $FF86, $FFD8, $FF87, $FFD6, $FF88
                    dc.w     $FFD4, $FF89, $FFD2, $FF89, $FFD0, $FF8A, $FFCE, $FF8B
                    dc.w     $FFCC, $FF8C, $FFCA, $FF8D, $FFC8, $FF8E, $FFC6, $FF8F
                    dc.w     $FFC4, $FF90, $FFC2, $FF91, $FFC0, $FF92, $FFBE, $FF93
                    dc.w     $FFBC, $FF95, $FFBA, $FF96, $FFB8, $FF97, $FFB7, $FF98
                    dc.w     $FFB5, $FF9A, $FFB3, $FF9B, $FFB1, $FF9D, $FFAF, $FF9E
                    dc.w     $FFAE, $FF9F, $FFAC, $FFA1, $FFAA, $FFA2, $FFA9, $FFA4
                    dc.w     $FFA7, $FFA5, $FFA5, $FFA7, $FFA4, $FFA9, $FFA2, $FFAA
                    dc.w     $FFA1, $FFAC, $FF9F, $FFAE, $FF9E, $FFAF, $FF9D, $FFB1
                    dc.w     $FF9B, $FFB3, $FF9A, $FFB5, $FF98, $FFB7, $FF97, $FFB8
                    dc.w     $FF96, $FFBA, $FF95, $FFBC, $FF93, $FFBE, $FF92, $FFC0
                    dc.w     $FF91, $FFC2, $FF90, $FFC4, $FF8F, $FFC6, $FF8E, $FFC8
                    dc.w     $FF8D, $FFCA, $FF8C, $FFCC, $FF8B, $FFCE, $FF8A, $FFD0
                    dc.w     $FF89, $FFD2, $FF89, $FFD4, $FF88, $FFD6, $FF87, $FFD8
                    dc.w     $FF86, $FFDB, $FF86, $FFDD, $FF85, $FFDF, $FF84, $FFE1
                    dc.w     $FF84, $FFE3, $FF83, $FFE5, $FF83, $FFE8, $FF82, $FFEA
                    dc.w     $FF82, $FFEC, $FF82, $FFEE, $FF81, $FFF0, $FF81, $FFF3
                    dc.w     $FF81, $FFF5, $FF80, $FFF7, $FF80, $FFF9, $FF80, $FFFC
                    dc.w     $FF80, $FFFE, $FF80

PANEL_DATA:         dc.w     $38E0, 0, 0, 0, $4510, $38E0, 0, 0, $BAE8, $7DF0, 0, 0, $FFB8
                    dc.w     $6DF0, 0, 0, $FFF0, $6180, 0, 0, $A7C0, $7980, 0, 0, $5BC0
                    dc.w     $3D80, 0, 0, $6FF0, $1D80, 0, 0, $FFB8, $6DF0, 0, 0, $BAE8
                    dc.w     $7DF0, 0, 0, $4510, $38E0, 0, 0, $38E0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $FFF0, 0, 0, 0, $9210, $6DE0, 0, 0
                    dc.w     $FFF0, $6DE0, 0, 0, $FFF0, $6CC0, 0, 0, $EFE0, $7CC0, 0, 0
                    dc.w     $FFE0, $7CC0, 0, 0, $FFE0, $7CC0, 0, 0, $EFE0, $7CC0, 0, 0
                    dc.w     $FFF0, $6CC0, 0, 0, $FFF0, $6DE0, 0, 0, $9210, $6DE0, 0, 0
                    dc.w     $FFF0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $F3F0
                    dc.w     0, 0, 0, $9210, $61E0, 0, 0, $F3F0, $61E0, 0, 0, $F3F0, $60C0
                    dc.w     0, 0, $F1E0, $60C0, 0, 0, $F1E0, $60C0, 0, 0, $F1E0, $60C0, 0
                    dc.w     0, $F1E0, $60C0, 0, 0, $FFF0, $60C0, 0, 0, $FFF0, $7DE0, 0, 0
                    dc.w     $8210, $7DE0, 0, 0, $FFF0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0

lbW02E20C:          dc.w     $3C00, 0, 0, 0, $4200, $3C00, 0, 0, $BD00, $7E00, 0, 0, $E700
                    dc.w     $7E00, 0, 0, $FF00, $6600, 0, 0, $FF00, $6600, 0, 0, $FF00
                    dc.w     $6600, 0, 0, $FF00, $6600, 0, 0, $E700, $7E00, 0, 0, $BD00
                    dc.w     $7E00, 0, 0, $4200, $3C00, 0, 0, $3C00, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $3C00, 0, 0, 0, $4400, $3800, 0, 0
                    dc.w     $BC00, $7800, 0, 0, $FC00, $7800, 0, 0, $FC00, $1800, 0, 0
                    dc.w     $3C00, $1800, 0, 0, $3C00, $1800, 0, 0, $3C00, $1800, 0, 0
                    dc.w     $FF00, $1800, 0, 0, $FF00, $7E00, 0, 0, $8100, $7E00, 0, 0
                    dc.w     $FF00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $3C00
                    dc.w     0, 0, 0, $4200, $3C00, 0, 0, $BD00, $7E00, 0, 0, $FF00, $6600
                    dc.w     0, 0, $FF00, $600, 0, 0, $1500, $E00, 0, 0, $2E00, $1C00, 0, 0
                    dc.w     $5C00, $3800, 0, 0, $BF00, $7000, 0, 0, $BF00, $7E00, 0, 0
                    dc.w     $8100, $7E00, 0, 0, $FF00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, $3C00, 0, 0, 0, $4200, $3C00, 0, 0, $BD00, $7E00
                    dc.w     0, 0, $FF00, $6600, 0, 0, $ED00, $600, 0, 0, $1E00, $C00, 0, 0
                    dc.w     $1E00, $C00, 0, 0, $ED00, $600, 0, 0, $FF00, $6600, 0, 0
                    dc.w     $BD00, $7E00, 0, 0, $4200, $3C00, 0, 0, $3C00, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $F000, 0, 0, 0, $9000, $6000
                    dc.w     0, 0, $F000, $6000, 0, 0, $F000, $6000, 0, 0, $F000, $6000, 0
                    dc.w     0, $F000, $6000, 0, 0, $FE00, $6000, 0, 0, $FF00, $6C00, 0, 0
                    dc.w     $FF00, $7E00, 0, 0, $8D00, $7E00, 0, 0, $F300, $C00, 0, 0
                    dc.w     $1E00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $FF00
                    dc.w     0, 0, 0, $FF00, $7E00, 0, 0, $E100, $7E00, 0, 0, $FF00, $6000
                    dc.w     0, 0, $FC00, $6000, 0, 0, $FA00, $7C00, 0, 0, $8D00, $7E00, 0
                    dc.w     0, $FF00, $600, 0, 0, $FF00, $600, 0, 0, $FD00, $7E00, 0, 0
                    dc.w     $8200, $7C00, 0, 0, $FC00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, $3E00, 0, 0, 0, $5E00, $3C00, 0, 0, $BE00, $7C00
                    dc.w     0, 0, $FE00, $6000, 0, 0, $FC00, $6000, 0, 0, $FA00, $7C00, 0
                    dc.w     0, $FD00, $7E00, 0, 0, $FF00, $6600, 0, 0, $FF00, $6600, 0, 0
                    dc.w     $BD00, $7E00, 0, 0, $4200, $3C00, 0, 0, $3C00, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $FF00, 0, 0, 0, $FF00, $7E00
                    dc.w     0, 0, $FF00, $7E00, 0, 0, $FF00, $600, 0, 0, $F00, $600, 0, 0
                    dc.w     $1500, $E00, 0, 0, $2E00, $1C00, 0, 0, $3C00, $1800, 0, 0
                    dc.w     $3C00, $1800, 0, 0, $3C00, $1800, 0, 0, $3C00, $1800, 0, 0
                    dc.w     $3C00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $3C00
                    dc.w     0, 0, 0, $5A00, $3C00, 0, 0, $BD00, $7E00, 0, 0, $FF00, $6600
                    dc.w     0, 0, $FD00, $6600, 0, 0, $7E00, $3C00, 0, 0, $7E00, $3C00, 0
                    dc.w     0, $BD00, $6600, 0, 0, $FF00, $6600, 0, 0, $BD00, $7E00, 0, 0
                    dc.w     $5A00, $3C00, 0, 0, $3C00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, $3C00, 0, 0, 0, $4200, $3C00, 0, 0, $BD00, $7E00
                    dc.w     0, 0, $FF00, $6600, 0, 0, $FF00, $6600, 0, 0, $BF00, $7E00, 0
                    dc.w     0, $5F00, $3E00, 0, 0, $3F00, $600, 0, 0, $7F00, $600, 0, 0
                    dc.w     $7D00, $3E00, 0, 0, $7A00, $3C00, 0, 0, $7C00, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

lbL02E70C:          dc.l     0, 0, $3000000, 0, $7800300, 0, $FC00780, 0, $1FE00FC0, 0
                    dc.l     $3FF01FE0, 0, $7FF83FF0, 0, $FFFC0780, 0, $FC00780, 0
                    dc.l     $FC00780, 0, $FC00780, 0, $FC00000, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.l     0, 0, 0, 0, $3FF00000, 0, $3FE01FC0, 0, $3FC01F80, 0
                    dc.l     $3F801F00, 0, $3FC01F80, 0, $3FE01FC0, 0, $3FF01BE0, 0
                    dc.l     $3BE011C0, 0, $31C00080, 0, $20800000, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.l     0, $1000000, 0, $3000000, 0, $7000200, 0, $F000600, 0
                    dc.l     $1FF00E00, 0, $3FF01FE0, 0, $7FF03FE0, 0, $7FF03FE0, 0
                    dc.l     $3FF01FE0, 0, $1FF00E00, 0, $F000600, 0, $7000200, 0
                    dc.l     $3000000, 0, $1000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, $20800000, 0
                    dc.l     $31C00080, 0, $3BE011C0, 0, $3FF01BE0, 0, $3FE01FC0, 0
                    dc.l     $3FC01F80, 0, $3F801F00, 0, $3FC01F80, 0, $3FE01FC0, 0
                    dc.l     $3FF00000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $FC00000, 0
                    dc.l     $FC00780, 0, $FC00780, 0, $FC00780, 0, $FFFC0780, 0
                    dc.l     $7FF83FF0, 0, $3FF01FE0, 0, $1FE00FC0, 0, $FC00780, 0
                    dc.l     $7800300, 0, $3000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.l     $4100000, 0, $E300400, 0, $1F700E20, 0, $3FF01F60, 0
                    dc.l     $1FF00FE0, 0, $FF007E0, 0, $7F003E0, 0, $FF007E0, 0
                    dc.l     $1FF00FE0, 0, $3FF00000, 0, 0, 0, 0, 0, 0, 0, 0, 0, $2000000
                    dc.l     0, $3000000, 0, $3800100, 0, $3C00180, 0, $3FE001C0, 0
                    dc.l     $3FF01FE0, 0, $3FF81FF0, 0, $3FF81FF0, 0, $3FF01FE0, 0
                    dc.l     $3FE001C0, 0, $3C00180, 0, $3800100, 0, $3000000, 0
                    dc.l     $2000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, $3FF00000, 0, $1FF00FE0
                    dc.l     0, $FF007E0, 0, $7F003E0, 0, $FF007E0, 0, $1FF00FE0, 0
                    dc.l     $3FF01F60, 0, $1F700E20, 0, $E300400, 0, $4100000, 0, 0, 0
                    dc.l     0, 0, 0, 0, 0, 0

lbL02EB0C:          dc.l     $7FFE7FFE, 0, $FFFFFFFF, 0, $FFFFFFFF, 0, $7FFEFFFF, 0
                    dc.l     $80017FFE, 0, $7FFE0000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.l     0, 0, 0, 0, 0, 0, 0, $3FF00000, 0, $20101FE0, 0, $3FF01FE0, 0
                    dc.l     $3FF00300, 0, $7800300, 0, $7800300, 0, $7800300, 0
                    dc.l     $7800300, 0, $7800300, 0, $7800300, 0, $4800300, 0
                    dc.l     $7800000, 0, 0, 0, 0, 0, 0, 0, 0, 0

BONUS_B:            dc.w     $7CE, $7BD, $7AD, $69C, $68C, $68C, $67C, $56B, $55B, $55B
                    dc.w     $64A, $64A, $64A, $749, $749, $749, $849, $849, $849, $A4A

PSAMS:              incbin   "samples/psams"
SPIN_SAM:           incbin   "samples/spin.sam"
SKID_SAM:           incbin   "samples/skid.sam"
HEART_SAM:          incbin   "samples/heart.sam"
DAMA_SAM:           incbin   "samples/dama.sam"
ZLDAMA_SAM:         incbin   "samples/zldama.sam"
WHIP_SAM:           incbin   "samples/whip.sam"
SMART_SAM:          incbin   "samples/smart.sam"
BREAK_SAM:          incbin   "samples/break.sam"
SPRING_SAM:         incbin   "samples/spring.sam"
RESTART_SAM:        incbin   "samples/restart.sam"
POP_SAM:            incbin   "samples/pop.sam"
STEP_SAM:           incbin   "samples/step.sam"
FIRE_SAM:           incbin   "samples/fire.sam"
WIMP_SAM:           incbin   "samples/wimp.sam"
SNARL_SAM:          incbin   "samples/snarl.sam"
EDIE_SAM:           incbin   "samples/edie.sam"
CTOKEN_SAM:         incbin   "samples/ctoken.sam"
CHUPPA_SAM:         incbin   "samples/chuppa.sam"
SLIDE_SAM:          incbin   "samples/slide.sam"
COLL_SAM:           incbin   "samples/coll.sam"
FLIP_SAM:           incbin   "samples/flip.sam"
EAT_SAM:            incbin   "samples/eat.sam"
BRICK_SAM:          incbin   "samples/brick.sam"

END_PSAMS:          dcb.w    2409, 0
HISS_FXAD:          dcb.l    500, 0
lbL0471E8:          dcb.l    95, 0
SNOW_FXAD:          dcb.w    2465, 0

OVER_NAME:          dc.b     'datas/over', 0
INTSP_NAME:         dc.b     'datas/intro', 0
INTMUS_NAME:        dc.b     'datas/zool2.mod', 0
ENDSPR_NAME:        dc.b     'datas/endseq', 0
HITEXT_NAME:        dc.b     'datas/htext', 0
ENDSCR_NAME:        dc.b     'datas/end', 0
STGMUS_NAME:        dc.b     'datas/ready.mod', 0
STGSPR_NAME1:       dc.b     'datas/stage', 0
BON2_NAME1:         dc.b     'datas/bon2', 0

BIRMUS_NAME:        dc.b     'datas/bir.mod', 0
BULMUS_NAME:        dc.b     'datas/bul.mod', 0
TOOMUS_NAME:        dc.b     'datas/too.mod', 0
SODMUS_NAME:        dc.b     'datas/sod.mod', 0

BIRMAP_NAME:        dc.b     'datas/birmaps', 0
BULMAP_NAME:        dc.b     'datas/bulmaps', 0
TOOMAP_NAME:        dc.b     'datas/toomaps', 0
SNAMAP_NAME:        dc.b     'datas/snamaps', 0
SODMAP_NAME:        dc.b     'datas/sodmaps', 0
LASMAP_NAME:        dc.b     'datas/lasmaps', 0
PAPMAP_NAME:        dc.b     'datas/papmaps', 0

PAPSPR_NAME:        dc.b     'datas/papspr', 0
BIRSPR_NAME:        dc.b     'datas/birspr', 0
BULSPR_NAME:        dc.b     'datas/bulspr', 0
TOOSPR_NAME:        dc.b     'datas/toospr', 0
SNASPR_NAME:        dc.b     'datas/snaspr', 0
SODSPR_NAME:        dc.b     'datas/sodspr', 0
LASSPR_NAME:        dc.b     'datas/lasspr', 0

PAPTLS_NAME:        dc.b     'datas/pap', 0
BIRTLS_NAME:        dc.b     'datas/bir', 0
BULTLS_NAME:        dc.b     'datas/bul', 0
TOOTLS_NAME:        dc.b     'datas/too', 0
SNATLS_NAME:        dc.b     'datas/sna', 0
SODTLS_NAME:        dc.b     'datas/sod', 0
LASTLS_NAME:        dc.b     'datas/las', 0

BIRMB_NAME:         dc.b     'datas/birblk', 0
BULMB_NAME:         dc.b     'datas/bulblk', 0
TOOMB_NAME:         dc.b     'datas/tooblk', 0
SNAMB_NAME:         dc.b     'datas/snablk', 0
SODMB_NAME:         dc.b     'datas/sodblk', 0
LASMB_NAME:         dc.b     'datas/lasblk', 0

PSAMS_NAME1:        dc.b     'datas/perm2', 0
ICEFX_NAME:         dc.b     'datas/ziceplat', 0
SNOFX_NAME:         dc.b     'datas/zsnowfla', 0
HISFX_NAME:         dc.b     'datas/zhissing', 0
LAVFX_NAME:         dc.b     'datas/zlava', 0
PLTFX_NAME:         dc.b     'datas/zplatfor', 0
SPKFX_NAME:         dc.b     'datas/zspark', 0
TWEFX_NAME:         dc.b     'datas/ztweet', 0
READY_NAME1:        dc.b     'datas/ready1', 0
READY2_NAME1:       dc.b     'datas/ready2', 0
PERM2_NAME1:        dc.b     'datas/perm2', 0
PERM_NAME1:         dc.b     'datas/zperm', 0
ENEMY_NAME1:        dc.b     'datas/enemydat', 0
CHOOSE_NAME:        dc.b     'datas/zchs', 0
GREM_NAME:          dc.b     'datas/grem', 0
WARP_NAME:          dc.b     'datas/warp', 0
CHPS_NAME:          dc.b     'datas/chps', 0
ZOPT_NAME:          dc.b     'datas/zopt', 0
ZHISC_NAME:         dc.b     'datas/zhisc', 0

BONUS_TILES_NAME:   dc.b     'datas/bonus_tiles', 0
BONUS_SPRITES_NAME: dc.b     'datas/bonus_sprites', 0
ZOOL_STAGE_NAME:    dc.b     'datas/zool_stage_screen', 0
ZOOZ_STAGE_NAME:    dc.b     'datas/zooz_stage_screen', 0

HISCORE_NAME:       dc.b     'datas/hiscores', 0
                    even

; note: sprites are only used for the top infos bar
;       so the rest is probably just 2 16 colors dual playfield bitplanes
;       with bobs added to the foreground.

NULLCOP:            dc.w     $106, $20
                    dc.w     $180, 0, $182, 0, $184, 0, $186, 0, $188, 0, $18A, 0, $18C, 0, $18E, 0
                    dc.w     $190, 0, $192, 0, $194, 0, $196, 0
                    dc.w     $1A0, 0, $1A2, 0, $1A4, 0, $1A6, 0, $1A8, 0, $1AA, 0, $1AC, 0, $1AE, 0
                    dc.w     $1B0, 0, $1B2, 0, $1B4, 0, $1B6, 0, $1B8, 0, $1BA, 0, $1BC, 0, $1BE, 0
                    dc.w     $106, $3460
                    dc.w     $180, 0, $182, 0, $184, 0, $186, 0, $188, 0, $18A, 0, $18C, 0, $18E, 0
                    dc.w     $190, 0, $192, 0, $194, 0, $196, 0, $198, 0, $19A, 0, $19C, 0, $19E, 0
                    dc.w     $106, $1420
                    dc.w     $144, 0, $146, 0, $14C, 0, $14E, 0, $154, 0, $156, 0, $15C, 0
                    dc.w     $15E, 0, $164, 0, $166, 0, $16C, 0, $16E, 0, $174, 0, $176, 0
                    dc.w     $17C, 0, $17E, 0
                    dc.w     $180, 0, $182, 0, $184, 0, $186, 0, $188, 0, $18A, 0, $18C, 0, $18E, 0
                    dc.w     $190, 0, $192, 0, $194, 0, $196, 0, $198, 0, $19A, 0, $19C, 0, $19E, 0
                    dc.w     $FFFF, $FFFE

; main copperlist used during the game
COPPER_GAME:        dc.w     $100, $4200, $104
lbW048B54:          dc.w     $24
                    dc.w     $8E, $2C81, $90, $2CC1
                    dc.w     $92, $30, $94, $D8
                    dc.w     $1FC, 0
                    dc.w     $106, $20
                    dc.w     $180
PANELCOLS:          dc.w     0, $182, 0, $184, 0, $186, 0, $188, 0, $18A, 0, $18C, 0, $18E
                    dc.w     0, $190, 0, $192, 0, $194, 0, $196, 0
NRG_BLINK:          dc.w     $1A0
lbW048BA0:          dc.w     0, $1A2, 0, $1A4, 0, $1A6, 0, $1A8, 0, $1AA
lbB048BB4:          dc.w     0
                    dc.w     $1AC
lbB048BB8:          dc.w     0
                    dc.w     $1AE
lbB048BBC:          dc.w     0
                    dc.w     $1B0, 0, $1B2
lbB048BC4:          dc.w     0
                    dc.w     $1B4
lbB048BC8:          dc.w     0
                    dc.w     $1B6
lbB048BCC:          dc.w     0
                    dc.w     $1B8, 0, $1BA, 0, $1BC, 0, $1BE, 0, $106, $3460

                    ; background planes colors
                    dc.w     $180
PARACOLS:           dc.w     0, $182, 0, $184, 0, $186, 0, $188, 0, $18A, 0, $18C, 0, $18E
                    dc.w     0, $106
FPARACOLS:          dc.w     $1420, $102
                    dc.w     0
BPLMOD1:            dc.w     $108, $94, $10A, $94, $96, $8100
PANEL_BEG:          dc.w     $2AD7, $FFFE
PANELBPLS:          dc.w     $E0, 0, $E2, 0
                    dc.w     $E4, 0, $E6, 0
                    dc.w     $E8, 0, $EA, 0
                    dc.w     $EC, 0, $EE, 0
TOP_PANEL:          dc.w     $2D3B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $2E3B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $2F3B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
lbW048DF6:          dc.w     $303B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $313B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $323B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $333B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $343B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $353B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $363B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $373B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $383B, $FFFE
                    dc.w     $178, $50, $17C, 0, $17E, 0, $140, $58, $144, 0, $146, 0, $178, $60, $17C, 0, $17E, 0
                    dc.w     $140, $70, $144, 0, $146, 0, $150, $7E, $154, 0, $156, 0, $158, $88, $15C, 0, $15E, 0
                    dc.w     $168, $92, $16C, 0, $16E, 0, $160, $9C, $164, 0, $166, 0, $170, $A8, $174, 0, $176, 0
                    dc.w     $140, $BC, $144, 0, $146, 0, $148, $C4, $14C, 0, $14E, 0, $140, $D4, $144, 0, $146, 0
                    dc.w     $3911, $FFFE
                    dc.w     $17C, 0, $17E, 0, $144, 0, $146, 0, $154, 0, $156, 0, $15C, 0, $15E, 0, $164, 0, $166, 0
                    dc.w     $16C, 0, $16E, 0, $174, 0, $176, 0, $14C, 0, $14E, 0
PANEL_END:          dc.w     $39D7, $FFFE, $96, $100
                    dc.w     $3A5B, $FFFE

                    ; foreground planes colors
                    dc.w     $180
LEVCOLS:            dc.w     0, $182, 0, $184, 0, $186, 0, $188, 0, $18A, 0, $18C, 0, $18E
                    dc.w     0, $190, 0, $192, 0, $194, 0, $196, 0, $198, 0, $19A, 0, $19C
                    dc.w     0, $19E, 0
                    dc.w     $106
USED_CBANK:         dc.w     $3420
                    dc.w     $10A
BPLMOD2:            dc.w     0
BPL4_H:             dc.w     $F0, 8
BPL4_L:             dc.w     $F2, 0
BPL5_H:             dc.w     $F4, 8
BPL5_L:             dc.w     $F6, $28
BPL6_H:             dc.w     $F8, 8
BPL6_L:             dc.w     $FA, $50
BPL7_H:             dc.w     $FC, 8
BPL7_L:             dc.w     $FE, $78
DISP_BEG:           dc.w     $3A29, $FFFE
                    dc.w     $100
DUAL_ON:            dc.w     $7401, $96, $8100
BPL0_H:             dc.w     $E0, 7
BPL0_L:             dc.w     $E2, $8000
BPL1_H:             dc.w     $E4, 7
BPL1_L:             dc.w     $E6, $8028
BPL2_H:             dc.w     $E8, 7
BPL2_L:             dc.w     $EA, $8050
BPL3_H:             dc.w     $EC, 7
BPL3_L:             dc.w     $EE, $8078
                    dc.w     $102
HORZ_SCRO:          dc.w     0

                    ; background copper gradients
WRITE_COP:          dcb.w    300, 0
COL_SCROLL:         dc.w     $FFFF, $FFFE

; used for still picture with copper gradients
COPPER3:            dc.w     $100, $4200
                    dc.w     $104, $24
                    dc.w     $8E, $2C81, $90, $2CC1
                    dc.w     $92, $30, $94, $D8
                    dc.w     $1FC, 0
                    dc.w     $102, 0
                    dc.w     $108, $94, $10A, $94
                    dc.w     $106, $1420
                    dc.w     $144, 0, $146, 0, $14C, 0, $14E, 0, $154, 0, $156, 0
                    dc.w     $15C, 0, $15E, 0, $164, 0, $166, 0, $16C, 0, $16E, 0, $174, 0
                    dc.w     $176, 0, $17C, 0, $17E, 0
                    dc.w     $180
SCRCOLS:            dc.w     0, $182, 0, $184, 0, $186, 0, $188, 0, $18A, 0, $18C, 0, $18E
                    dc.w     0, $190, 0, $192, 0, $194, 0, $196, 0, $198, 0, $19A, 0, $19C
                    dc.w     0, $19E, 0, $102, 0, $96, $8100
                    dc.w     $2AD7, $FFFE
SCRO_PTRS3:         dc.w     $E0, 7, $E2, $8000
                    dc.w     $E4, 7, $E6, $8028
                    dc.w     $E8, 7, $EA, $8050
                    dc.w     $EC, 7, $EE, $8078
WRITE_COP3:         dcb.w    90, 0
COL_SCROLL3:        dc.w     $FFFF, $FFFE
                    dcb.w    100, 0

COPPER2:            dc.w     $100, $4200
                    dc.w     $104, $24
                    dc.w     $108, $94, $10A, $94
                    dc.w     $106, $1420
                    dc.w     $8E, $2C81, $90, $2CC1
                    dc.w     $92, $30, $94, $D8
                    dc.w     $144, 0, $146, 0
                    dc.w     $14C, 0, $14E, 0, $154, 0, $156, 0, $15C, 0, $15E, 0, $164, 0
                    dc.w     $166, 0, $16C, 0, $16E, 0, $174, 0, $176, 0, $17C, 0, $17E, 0
                    dc.w     $96, $8100
                    dc.w     $2AD7, $FFFE
SETSCR_BPLS:        dc.w     $E0, 7, $E2, $8000
                    dc.w     $E4, 7, $E6, $8028
                    dc.w     $E8, 7, $EA, $8050
                    dc.w     $EC, 7, $EE, $8078
                    dc.w     $102, 0, $84, 0
COL_SCROLL2:        dcb.w    240, 0
                    dc.w     $FFFF, $FFFE

PANEL:              dc.w     $6F7F, $D698, $1F57, $AAF5, $AF7F, $D698, $1F57, $AAF5
                    dc.w     $AF7F, $D698, $1F57, $AAF5, $AF7F, $D698, $1F57, $AAF5
                    dc.w     $AF7F, $D698, $1F57, $AAF6, 0, 0, 0, 0, $2040, $3160, $E070
                    dc.w     $3606, $6040, $3160, $E070, $3606, $6040, $3160, $E070
                    dc.w     $3606, $6040, $3160, $E070, $3606, $6040, $3160, $E070
                    dc.w     $3604, 0, 0, 0, 0, $E07F, $F000, $70, $3E07, $E07F, $F000
                    dc.w     $70, $3E07, $E07F, $F000, $70, $3E07, $E07F, $F000, $70
                    dc.w     $3E07, $E07F, $F000, $70, $3E07, 0, 0, 0, 0, $1F80, $FFF
                    dc.w     $FF8F, $C1F8, $1F80, $FFF, $FF8F, $C1F8, $1F80, $FFF
                    dc.w     $FF8F, $C1F8, $1F80, $FFF, $FF8F, $C1F8, $1F80, $FFF
                    dc.w     $FF8F, $C1F8, 0, 0, 0, 0, $6F3E, $73CD, $F617, $9CF7, $AF3E
                    dc.w     $13FF, $F617, $9CF7, $AF3E, $13CD, $F617, $9CF7, $AF3E
                    dc.w     $13CD, $F63F, $FCF7, $AF3E, $13DF, $F617, $9CF4, 0, 0, 0, 0
                    dc.w     $205D, $F030, $70, $2204, $6041, $F0F7, $70, $2204, $6041
                    dc.w     $F030, $70, $2204, $6041, $F030, $7F, $F204, $6041, $F03E
                    dc.w     $70, $2204, 0, 0, 0, 0, $E063, $8000, $70, $3E07, $E07F
                    dc.w     $F000, $70, $3E07, $E07F, $F000, $70, $3E07, $E07F, $F000
                    dc.w     $40, $E07, $E07F, $F000, $70, $3E07, 0, 0, 0, 0, $1F9C
                    dc.w     $7FFF, $FF8F, $C1F8, $1F80, $FFF, $FF8F, $C1F8, $1F80
                    dc.w     $FFF, $FF8F, $C1F8, $1F80, $FFF, $FFBF, $F1F8, $1F80
                    dc.w     $FFF, $FF8F, $C1F8, 0, 0, 0, 0, $D062, $8C18, $1D68, $220C
                    dc.w     $104C, $6CFF, $1D68, $220C, $104C, $6C18, $1D68, $220C
                    dc.w     $104C, $6C18, $1D60, $120C, $104C, $6C12, $1D68, $220F, 0
                    dc.w     0, 0, 0, $30BF, $FC00, $1D98, $4108, $3083, $9CFF, $1D98
                    dc.w     $4108, $3083, $9C00, $1D98, $4108, $3083, $9C00, $1DBF
                    dc.w     $F108, $3083, $9C1E, $1D98, $4108, 0, 0, 0, 0, $F0C1, $400
                    dc.w     $1DF8, $7F0F, $F0FF, $FC00, $1DF8, $7F0F, $F0FF, $FC00
                    dc.w     $1DF8, $7F0F, $F0FF, $FC00, $1DC0, $F0F, $F0FF, $FC00
                    dc.w     $1DF8, $7F0C, 0, 0, 0, 0, $F22, $8BFF, $E207, $80F0, $F00
                    dc.w     $399, $E207, $80F0, $F00, $3FF, $E207, $80F0, $F00, $3FF
                    dc.w     $E220, $10F0, $F00, $3F3, $E207, $80F0, 0, 0, 0, 0, $E95D
                    dc.w     $74C8, $777C, $A294, $294F, $D8FF, $777C, $A294, $294F
                    dc.w     $D8C8, $777C, $A294, $294F, $D8C8, $777F, $F294, $294F
                    dc.w     $D8DE, $777C, $A297, 0, 0, 0, 0, $19FF, $FCC0, $7F8C, $C198
                    dc.w     $1987, $F8FF, $7F8C, $C198, $1987, $F8C0, $7F8C, $C198
                    dc.w     $1987, $F8C0, $7FBF, $F198, $1987, $F8DE, $7F8C, $C198, 0
                    dc.w     0, 0, 0, $F980, $C0, $7FFC, $FF9F, $F9FF, $F800, $7FFC
                    dc.w     $FF9F, $F9FF, $F8C0, $7FFC, $FF9F, $F9FF, $F8C0, $7FC0
                    dc.w     $F9F, $F9FF, $F8C0, $7FFC, $FF9C, 0, 0, 0, 0, $641, $73F
                    dc.w     $8003, $60, $600, $799, $8003, $60, $600, $73F, $8003, $60
                    dc.w     $600, $73F, $8020, $1060, $600, $733, $8003, $60, 0, 0, 0, 0
                    dc.w     $E0FF, $DDE1, $C3B4, $B674, $20F4, $F1FF, $C3B4, $B674
                    dc.w     $20F4, $F1E1, $C3B4, $B674, $20F4, $F1E1, $C3BF, $F674
                    dc.w     $20F4, $F1FF, $C3B4, $B677, 0, 0, 0, 0, $1F7F, $FDE1, $FFCC
                    dc.w     $C1F8, $1F0C, $F1FF, $FFCC, $C1F8, $1F0C, $F1E1, $FFCC
                    dc.w     $C1F8, $1F0C, $F1E1, $FFFF, $F1F8, $1F0C, $F1FF, $FFCC
                    dc.w     $C1F8, 0, 0, 0, 0, $FF80, $1E1, $FFFC, $FFFF, $FFFC, $F101
                    dc.w     $FFFC, $FFFF, $FFFC, $F1E1, $FFFC, $FFFF, $FFFC, $F1E1
                    dc.w     $FFC0, $FFF, $FFFC, $F1E1, $FFFC, $FFFC, 0, 0, 0, 0, $49
                    dc.w     $61E, 3, 0, 3, $EF2, 3, 0, 3, $E1E, 3, 0, 3, $E1E, $3C, $F000, 3
                    dc.w     $E12, 3, 0, 0, 0, 0, 0, $CFFF, $F9F7, $1AD7, $BE3A, $FF4
                    dc.w     $41FF, $1AD7, $BE3A, $FF4, $41F7, $1AD7, $BE3A, $FF4
                    dc.w     $41F7, $1AD7, $BE3A, $FF4, $41FF, $1AD7, $BE39, 0, 0, 0, 0
                    dc.w     $7F, $F9F7, $E6EF, $C1FC, 12, $41FF, $E6EF, $C1FC, 12
                    dc.w     $41F7, $E6EF, $C1FC, 12, $41F7, $E6EF, $C1FC, 12, $41FF
                    dc.w     $E6EF, $C1FC, 0, 0, 0, 0, $FF80, $1F7, $FEFF, $FFFF, $FFFC
                    dc.w     $41E1, $FEFF, $FFFF, $FFFC, $41F7, $FEFF, $FFFF, $FFFC
                    dc.w     $41F7, $FEF8, $7FFF, $FFFC, $41E1, $FEFF, $FFFE, 0, 0, 0, 0
                    dc.w     $4F, $3E08, $100, 0, 3, $BE12, $100, 0, 3, $BE08, $100, 0, 3
                    dc.w     $BE08, $104, $8000, 3, $BE12, $100, 0, 0, 0, 0, 0, $40D3
                    dc.w     $E1EC, $FA6F, $383D, $80F7, $E1FC, $FA6F, $383D, $80F7
                    dc.w     $E1EC, $FA6F, $383D, $80F7, $E1EC, $FA6F, $B83D, $80F7
                    dc.w     $E1FE, $FA6F, $383E, 0, 0, 0, 0, $807F, $E1EF, $67F, $C7FE
                    dc.w     15, $E1FF, $67F, $C7FE, 15, $E1EF, $67F, $C7FE, 15, $E1EF
                    dc.w     $67F, $C7FE, 15, $E1FF, $67F, $C7FC, 0, 0, 0, 0, $FF80, $1EF
                    dc.w     $FE7F, $FFFF, $FFFF, $E1C3, $FE7F, $FFFF, $FFFF, $E1EF
                    dc.w     $FE7F, $FFFF, $FFFF, $E1EF, $FE78, $7FFF, $FFFF, $E1E1
                    dc.w     $FE7F, $FFFF, 0, 0, 0, 0, $43, $3E10, $180, 0, 0, $1E24, $180
                    dc.w     0, 0, $1E10, $180, 0, 0, $1E10, $184, $8000, 0, $1E12, $180
                    dc.w     0, 0, 0, 0, 0, $402D, $F0DB, $9A3C, $7BE2, $8038, $30FF
                    dc.w     $9A3C, $7BE2, $8038, $30DB, $9A3C, $7BE2, $8038, $30DB
                    dc.w     $9A3F, $FBE2, $8038, $30DF, $9A3C, $7BE1, 0, 0, 0, 0, $803F
                    dc.w     $F0DC, $63F, $87E3, 7, $F0FC, $63F, $87E3, 7, $F0DC, $63F
                    dc.w     $87E3, 7, $F0DC, $63F, $87E3, 7, $F0DE, $63F, $87E2, 0, 0, 0
                    dc.w     0, $FFC0, $10DF, $FE3F, $FFE3, $FFFF, $F0C3, $FE3F, $FFE3
                    dc.w     $FFFF, $F0DF, $FE3F, $FFE3, $FFFF, $F0DF, $FE38, $7FE3
                    dc.w     $FFFF, $F0C1, $FE3F, $FFE3, 0, 0, 0, 0, $21, $2F20, $1C0
                    dc.w     $1C, 0, $F24, $1C0, $1C, 0, $F20, $1C0, $1C, 0, $F20, $1C4
                    dc.w     $801C, 0, $F32, $1C0, $1C, 0, 0, 0, 0, $8077, $FF16, $3A1B
                    dc.w     $1EC1, $407F, $E77E, $3A1B, $1EC1, $407F, $E716, $3A1B
                    dc.w     $1EC1, $407F, $E716, $3A1F, $9EC1, $407F, $E71E, $3A1B
                    dc.w     $1EC2, 0, 0, 0, 0, $C03F, $FF18, $61C, $6C1, $8000, $1F78
                    dc.w     $61C, $6C1, $8000, $1F18, $61C, $6C1, $8000, $1F18, $61F
                    dc.w     $86C1, $8000, $1F1E, $61C, $6C3, 0, 0, 0, 0, $FFC0, $71F
                    dc.w     $FE1F, $FEC1, $FFFF, $FF07, $FE1F, $FEC1, $FFFF, $FF1F
                    dc.w     $FE1F, $FEC1, $FFFF, $FF1F, $FE18, $7EC1, $FFFF, $FF01
                    dc.w     $FE1F, $FEC3, 0, 0, 0, 0, $31, $38E0, $1E0, $13E, 0, $C8
                    dc.w     $1E0, $13E, 0, $E0, $1E0, $13E, 0, $E0, $1E4, $813E, 0, $F2
                    dc.w     $1E0, $13C, 0, 0, 0, 0, $BA7F, $DD94, $6436, $1301, $7A07
                    dc.w     $9DFF, $6436, $1301, $7A07, $9D94, $6436, $1301, $7A07
                    dc.w     $9D94, $6437, $9301, $7A07, $9D9F, $F436, $1302, 0, 0, 0, 0
                    dc.w     $C5FF, $FF98, $1C38, $F01, $85F8, $3FF, $1C38, $F01
                    dc.w     $85F8, $398, $1C38, $F01, $85F8, $398, $1C3F, $8F01
                    dc.w     $85F8, $39F, $FC38, $F03, 0, 0, 0, 0, $FF80, $39F, $FC3F
                    dc.w     $FF01, $FFFF, $FF80, $FC3F, $FF01, $FFFF, $FF9F, $FC3F
                    dc.w     $FF01, $FFFF, $FF9F, $FC38, $7F01, $FFFF, $FF80, $C3F
                    dc.w     $FF03, 0, 0, 0, 0, $49, $460, $3C0, $FE, 0, $4F, $3C0, $FE, 0
                    dc.w     $60, $3C0, $FE, 0, $60, $3C4, $80FE, 0, $73, $F3C0, $FC, 0, 0
                    dc.w     0, 0, $CFDD, $75F4, $D81B, $1983, $FFB, $C1FF, $D81B
                    dc.w     $1983, $FFB, $C1F4, $D81B, $1983, $FFB, $C1F4, $D81F
                    dc.w     $9983, $FFB, $C1FF, $F81B, $1980, 0, 0, 0, 0, $FFFF, $FCF8
                    dc.w     $381C, $783, $FFFC, $FF, $381C, $783, $FFFC, $F8, $381C
                    dc.w     $783, $FFFC, $F8, $381F, $8783, $FFFC, $FF, $F81C, $783, 0
                    dc.w     0, 0, 0, $FF80, $3FF, $F81F, $FF83, $FFFF, $FF00, $F81F
                    dc.w     $FF83, $FFFF, $FFFF, $F81F, $FF83, $FFFF, $FFFF, $F818
                    dc.w     $7F83, $FFFF, $FFE0, $81F, $FF83, 0, 0, 0, 0, $41, $400
                    dc.w     $7E0, $7C, 0, $99, $7E0, $7C, 0, 0, $7E0, $7C, 0, 0, $7E4
                    dc.w     $807C, 0, $10, $17E0, $7C, 0, 0, 0, 0, $F823, $8988, $300B
                    dc.w     $F30E, $380B, $E1FF, $300B, $F30E, $380B, $E188, $300B
                    dc.w     $F30E, $380B, $E188, $300C, $F30E, $380B, $E190, $100B
                    dc.w     $F30D, 0, 0, 0, 0, $F83E, $F871, $F00C, $F0F, $F80C, $FF
                    dc.w     $F00C, $F0F, $F80C, $71, $F00C, $F0F, $F80C, $71, $F00F
                    dc.w     $8F0F, $F80C, $7F, $F00C, $F0E, 0, 0, 0, 0, $F801, $7FF
                    dc.w     $F00F, $FF0F, $F80F, $FF00, $F00F, $FF0F, $F80F, $FFFF
                    dc.w     $F00F, $FF0F, $F80F, $FFFF, $F008, $7F0F, $F80F, $FFE0
                    dc.w     15, $FF0F, 0, 0, 0, 0, $7E2, $8800, $FF0, $F0, $7F0, $99
                    dc.w     $FF0, $F0, $7F0, 0, $FF0, $F0, $7F0, 0, $FF4, $80F0, $7F0
                    dc.w     $10, $1FF0, $F0, 0, 0, 0, 0, $A01E, $78F8, $E007, $E61B
                    dc.w     $600A, $78FF, $E007, $E61B, $600A, $78F8, $E007, $E61B
                    dc.w     $600A, $78F8, $E007, $E61B, $600A, $78FF, $F007, $E618, 0
                    dc.w     0, 0, 0, $A01C, $7003, $E004, $1E1C, $E00C, $FF, $E004
                    dc.w     $1E1C, $E00C, 3, $E004, $1E1C, $E00C, 3, $E007, $9E1C
                    dc.w     $E00C, $1F, $F004, $1E1C, 0, 0, 0, 0, $A003, $8FFF, $E007
                    dc.w     $FE1F, $E00F, $FF00, $E007, $FE1F, $E00F, $FFFF, $E007
                    dc.w     $FE1F, $E00F, $FFFF, $E000, $7E1F, $E00F, $FFE0, 7, $FE1F
                    dc.w     0, 0, 0, 0, $5FFC, $7000, $1FF8, $1E0, $1FF0, $FF, $1FF8
                    dc.w     $1E0, $1FF0, 0, $1FF8, $1E0, $1FF0, 0, $1FFF, $81E0, $1FF0
                    dc.w     $1F, $FFF8, $1E0, 0, 0, 0, 0, $600B, $FFFC, $F004, $3C37
                    dc.w     $9FF5, $800F, $FFB, $C3C8, $9FF5, $800F, $FFB, $C3C8
                    dc.w     $9FF5, $800F, $FFB, $C3C8, $9FF5, $800F, $FFB, $C3CB, 0, 0
                    dc.w     0, 0, $DFF0, 0, $FF8, $3C0, $7FF8, 0, $FFFC, $3FF0, $7FF8, 0
                    dc.w     $FFFC, $3FF0, $7FF8, 0, $FFFC, $3FF0, $7FF8, 0, $FFFC
                    dc.w     $3FF0, 0, 0, 0, 0, $FFFE, $7FF3, $FFFF, $FFFF, $FFFE, $7FF3
                    dc.w     $FFFF, $FFFF, $FFFE, $7FF3, $FFFF, $FFFF, $FFFE, $7FF3
                    dc.w     $FFFF, $FFFF, $FFFE, $7FF3, $FFFF, $FFFC, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, $381B
                    dc.w     $FC3C, $D94D, $E1B7, $C7E5, $F3CF, $26B2, $1E48, $C7E5
                    dc.w     $F3CF, $26B2, $1E48, $C7E5, $F3CF, $26B2, $1E48, $C7E5
                    dc.w     $F3CF, $26B2, $1E4B, 0, 0, 0, 0, $C7E0, 0, $6B0, $1E40
                    dc.w     $3FF8, 0, $DFFD, $FFF0, $3FF8, 0, $DFFD, $FFF0, $3FF8, 0
                    dc.w     $DFFD, $FFF0, $3FF8, 0, $DFFD, $FFF0, 0, 0, 0, 0, $FFFE
                    dc.w     $FF3, $FFFF, $FFFF, $FFFE, $FF3, $FFFF, $FFFF, $FFFE
                    dc.w     $FF3, $FFFF, $FFFF, $FFFE, $FF3, $FFFF, $FFFF, $FFFE
                    dc.w     $FF3, $FFFF, $FFFC, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                    dc.w     0, 0, 0, 0, 0, 0, 0, 0
PLAY_MOD:           dc.w     0
PLAYF_BPL1:         dc.l     0
PLAYF_BPL2:         dc.l     0
PLAYF_BPL3:         dc.l     0
PLAYF_BPL4:         dc.l     0
PARA_WIDTH:         dc.w     0

PLAY0_MAP:          incbin   "maps/play0.map"
PLAY0_TLS:          incbin   "maps/play0.tls"
PLAY1_MAP:          incbin   "maps/play1.map"
PLAY1_TLS:          incbin   "maps/play1.tls"
PLAY2_MAP:          incbin   "maps/play2.map"
PLAY2_TLS:          incbin   "maps/play2.tls"
PLAY3_MAP:          incbin   "maps/play3.map"
PLAY3_TLS:          incbin   "maps/play3.tls"
PLAY4_MAP:          incbin   "maps/play4.map"
PLAY4_TLS:          incbin   "maps/play4.tls"
PLAY5_MAP:          incbin   "maps/play5.map"
PLAY5_TLS:          incbin   "maps/play5.tls"
PLAY7_MAP:          incbin   "maps/play7.map"
PLAY7_TLS:          incbin   "maps/play7.tls"

                    SECTION  zool2rs060018, BSS_C

SPSAVE:             ds.l     20
SNAKE_R:            ds.l     2400
PACKED_SCRS:        ds.l     3600
STORE_SPACE1:       ds.l     50
STORE_SPACE2:       ds.l     50
SNAKE_M:            ds.l     12002
LEV_MAPS:           ds.l     136
lbL071D60:          ds.l     8
                    ds.w     1
lbL071D82:          ds.l     119
                    ds.w     1
lbL071F60:          ds.l     16
lbL071FA0:          ds.l     4
lbL071FB0:          ds.l     12
lbL071FE0:          ds.l     6
lbL071FF8:          ds.l     2
lbL072000:          ds.l     10
                    ds.w     1
lbL07202A:          ds.l     8
                    ds.w     1
lbL07204C:          ds.l     145
lbL072290:          ds.l     29
                    ds.w     1
lbL072306:          ds.l     5249
lbL07750A:          ds.l     2800
lbL07A0CA:          ds.l     3435
                    ds.w     1
lbL07D678:          ds.l     164
                    ds.w     1
lbL07D90A:          ds.l     157
                    ds.w     1
lbL07DB80:          ds.l     12
                    ds.w     1
lbL07DBB2:          ds.l     8
                    ds.w     1
lbL07DBD4:          ds.l     174
                    ds.w     1
lbL07DE8E:          ds.l     1649
lbL07F852:          ds.l     2800
lbL082412:          ds.l     1198
lbL0836CA:          ds.l     157
                    ds.w     1
lbL083940:          ds.l     12
                    ds.w     1
lbL083972:          ds.l     8
                    ds.w     1
lbL083994:          ds.l     174
                    ds.w     1
lbL083C4E:          ds.l     4051
lbL087B9A:          ds.l     7598
lbL08F252:          ds.l     157
                    ds.w     1
lbL08F4C8:          ds.l     12
                    ds.w     1
lbL08F4FA:          ds.l     8
                    ds.w     1
lbL08F51C:          ds.l     174
                    ds.w     1
lbL08F7D6:          ds.l     5647
lbL095012:          ds.l     157
                    ds.w     1
lbL095288:          ds.l     12
                    ds.w     1
lbL0952BA:          ds.l     8
                    ds.w     1
lbL0952DC:          ds.l     174
                    ds.w     1
lbL095596:          ds.l     6106
                    ds.w     1
lbL09B500:          ds.l     3917

lbL09F234:          ds.l     1625
                    ds.w     1
lbL0A0B9A:          ds.l     170
lbL0A0E42:          ds.l     8
                    ds.w     1
lbL0A0E64:          ds.l     174
                    ds.w     1
lbL0A111E:          ds.l     6021
                    ds.w     1
END_MB:             ds.l     7315

BOT_CHIP:           ds.l     32
INTRO_SPSTART:      ds.l     18208
SPSTART:            ds.l     38701
MASKHERE:           ds.l     38700
MUSICHERE:          ds.l     22829
SPEND:              ds.l     13484
lbL12EF48:          ds.l     1326
lbL130400:          ds.l     269
                    ds.w     1
lbL130836:          ds.l     1004
                    ds.w     1
lbL1317E8:          ds.l     7275
lbL138994:          ds.l     2591
lbL13B210:          ds.l     12450
START_SCRN:         ds.l     300
BUFF1_START:        ds.w     1
lbL14794A:          ds.l     767
                    ds.w     1
lbL148548:          ds.l     384
lbL148B48:          ds.l     11908
BUFF2_START:        ds.w     1
lbL15455A:          ds.l     1151
                    ds.w     1
lbL155758:          ds.l     11908
BUFF3_START:        ds.w     1
lbL16116A:          ds.l     1151
                    ds.w     1
lbL162368:          ds.l     12204
END_OF_DATA:

                    SECTION  zool2rs16E218, CODE

OPEN_LIBRARIES:     lea      DOSNAME, a1
                    moveq    #0, d0
                    move.l   4.w, a6
                    jsr      _LVOOpenLibrary(a6)
                    move.l   d0, DOSBASE
                    beq.s    FATAL_ERROR
                    moveq    #0, d0
                    lea      GNAME, a1
                    move.l   4, a6
                    jsr      _LVOOpenLibrary(a6)
                    move.l   d0, GFXBASE
                    beq.s    FATAL_ERROR
                    lea      GAMENAME, a1
                    moveq    #0, d0
                    move.l   4.w, a6
                    jsr      _LVOOpenLibrary(a6)
                    move.l   d0, GAMEBASE
                    beq.s    FATAL_ERROR
                    rts

FATAL_ERROR:        move.w   #$F00, $DFF180
                    bra.s    FATAL_ERROR

CDSTARTUP:          move.l   4.w, a6
                    jsr      _LVODisable(a6)
                    
                    move.l   GAMEBASE, a6
       
                    lea      VBI, a0
                    jsr      _LVOAddVBlankInt(a6)

                    lea      KBI, a0
                    jsr      _LVOAddKBInt(a6)

                    move.l   4.w, a6
                    jsr      _LVOEnable(a6)

                    lea      TAGLIST_OFF, a1
                    move.l   GAMEBASE, a6
                    jsr      _LVOSystemControl(a6)
                    rts

DOSBASE:            dc.l     0
GFXBASE:            dc.l     0
GAMEBASE:           dc.l     0
TAGLIST_OFF:        dc.l     $80C00001, 1
                    dc.l     $80C00003, 1
                    dc.l     0
DOSNAME:            dc.b     'dos.library', 0
                    even
GNAME:              dc.b     'graphics.library', 0
                    even
GAMENAME:           dc.b     'lowlevel.library', 0
                    even

                    end
