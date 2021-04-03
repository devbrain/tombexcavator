unit GSprite;

{$V-}

{ Sprite engine for DN1 editor }

interface

type
{ Pointers to objects - forward declarations }
    PSmallSprite=^TSmallSprite;
    PMedSprite=^TMedSprite;
    PLargeSprite=^TLargeSprite;

{ Each sprite holds a image (bit data for each of 4 planes) and a mask, }
{ which is a single array which is applied to all 4 planes. }

{ For each of the sprite object methods: }
{ Clear: Sets both the image and mask to zero }
{ Load: Reads in the sprite and mask data from a binary file variable }
{ Merge: ANDs mask and XORs image from another sprite of same size }
{ Reduce: Shrinks the mask and image of the source sprite (which is twice as }
{   big) and writes it into the object's own image/mask arrays. }
{ Enlarge: Enlarges the mask and image of the source sprite (which is half as }
{   big) and writes in into the object's own image/mask arrays. }
{ Cut: Takes the specified quarter of the source sprite image/mask and writes }
{   it into the object's own array. }
{ Paste: Writes the source sprite (half the size) onto the specified corner }
{   of the object's own image. }
{ HorizInvert: Invert image horizontally. }
{ Draw: Draws the sprite onto the screen at the X/Y multiple of it's own size }
{   No clipping is performed, so please avoid screen wraps if possible. }

{ --- 8x8 sprite object --- }
    TSmallSprite=object
        Image:array[0..3,0..7] of Byte;
        Mask:array[0..7] of Byte;
        procedure Clear;
        procedure Load(var F:File);
        procedure Merge(Source:PSmallSprite);
        procedure Reduce(Source:PMedSprite);
        procedure Cut(Source:PMedSprite;X,Y:Word);
        procedure Draw(X,Y:Word);
    end;

{ --- 16x16 sprite object --- }
    TMedSprite=object
        Image:array[0..3,0..15] of Word;
        Mask:array[0..15] of Word;
        procedure Clear;
        procedure Load(var F:File);
        procedure Merge(Source:PMedSprite);
        procedure Reduce(Source:PLargeSprite);
        procedure Enlarge(Source:PSmallSprite);
        procedure Paste(Source:PSmallSprite;X,Y:Byte);
        procedure Draw(X,Y:Word);
    end;

{ --- 32x32 sprite object --- }
    TLargeSprite=object
        Image:array[0..3,0..31] of Longint;
        Mask:array[0..31] of Longint;
        procedure Clear;
        procedure Merge(Source:PLargeSprite);
        procedure Enlarge(Source:PMedSprite);
        procedure Draw(X,Y:Word);
    end;

procedure SetGMouseCursor(NewMouseCursor:PMedSprite);
procedure ShowGMouse;
procedure HideGMouse;
procedure UpdateGMouse(X,Y:Integer);

implementation

uses GCore;

{ --- Internal declarations ----------------------------------------------- }

const
    ReadWindow=0;
    WriteWindow=SizeOf(TGraphWin);

    NoBlit=$FFFF;

type
    TBlit=record
        VidBank,VidOffset,RepeatCount,ScanSkip:Word;
    end;

var
    SaveUpdateProc:procedure;
    WriteBankIncrement:Word;

    MouseCursor:TMedSprite;
    MouseSave:array[0..9,0..15] of Word;
    MouseCoord:TGraphCoord;
    MouseVisible:Boolean;

{ Note: Only assumptions that can be made about video card is that }
{ granularity, window size and scan line length are multiples of 4 bytes }

{ --- Internal miscellaneous routines ------------------------------------- }

{ Scale 16-bit word to 8-bit byte }

function Word2Byte(N:Word):Byte; assembler;
asm
    MOV CX,8
    MOV DX,N
    XCHG DH,DL
    @ShredLoop:
        SHR DX,2
        RCR AL,1
        LOOP @ShredLoop
end;

{ Scale 32-bit longint to 16-bit word }

function Longint2Word(N:Longint):Word; assembler;
asm
    MOV DX,Word(N[0])
    MOV BX,Word(N[2])
    XCHG DH,BL
    XCHG DL,BH
    MOV CX,16
    @ShredLoop:
        SHR BX,1; RCR DX,1
        SHR BX,1; RCR DX,1
        RCR AX,1
        LOOP @ShredLoop
    XCHG AL,AH
end;

{ Scale 16-bit word to 32-bit longint }

function Word2Longint(N:Word):Longint; assembler;
asm
    MOV BX,N
    XCHG BL,BH
    MOV CX,16
    @SpanLoop:
        SHL AX,1; RCL DX,1
        SHL AX,1; RCL DX,1
        SHL BX,1
        JNC @L1
        OR AL,$03
    @L1:LOOP @SpanLoop
    XCHG AL,DH
    XCHG AH,DL
end;

procedure UpdateProc; far;
begin
    if GraphModeCurrent<>0 then
    begin
        WriteBankIncrement:=GraphMode.Win[1].Size div GraphMode.WinGranularity;
    end;
    SaveUpdateProc;
end;

{ PosX: Scanline offset in bytes (must NOT cross a scanline boundary) }
{ PosY: Scanline number }
{ SizeX: Sprite width in bytes (must be 1, 2 or 4) }
{ SizeY: Sprite height in scanlines }

function BuildBlitTable(PosX,PosY,SizeY:Word;var BlitTable:array of TBlit):Word; assembler;
var
    BlitTableLength:Word;
asm
    MOV AX,PosY
    MUL GraphMode.ScanLineSize
    ADD AX,PosX
    ADC DX,0
    CMP Word(GraphMode.WinGranularity).[0],0; JE @@1
    DIV Word(GraphMode.WinGranularity).[0]; JMP @@2
@@1:XCHG DX,AX
@@2:

    @BlitCalcInit:
        LES DI,BlitTable
        MOV BlitTableLength,0
        MOV BX,GraphMode.ScanLineSize
        MOV SI,Word(TGraphWin(GraphMode.Win[WriteWindow]).Size).[0]
        DEC SI
    @BlitCalcWrite:
        MOV CX,1
        MOV TBlit(ES:[DI]).VidBank,AX
        MOV TBlit(ES:[DI]).VidOffset,DX
        MOV TBlit(ES:[DI]).ScanSkip,BX
    @BlitCalcLoop:
        DEC SizeY
        JZ @BlitCalcDone
        ADD DX,BX; JC @FlipBank
        CMP DX,SI; JA @FlipBank
        INC CX
        JMP @BlitCalcLoop
    @FlipBank:
        MOV TBlit(ES:[DI]).RepeatCount,CX
        SUB DX,SI
        DEC DX
        ADD AX,WriteBankIncrement
        ADD DI,TYPE TBlit
        INC BlitTableLength
        JMP @BlitCalcWrite
    @BlitCalcDone:
        MOV TBlit(ES:[DI]).RepeatCount,CX
        INC BlitTableLength
        MOV AX,BlitTableLength
end;

{ --- Small sprite routines ----------------------------------------------- }

procedure TSmallSprite.Clear;
var
    Y,C:Byte;
begin
    for Y:=0 to 7 do
    begin
        for C:=0 to 3 do Image[C,Y]:=0;
        Mask[Y]:=0;
    end;
end;

procedure TSmallSprite.Load(var F:File);
var
    HodgePodge:array[0..7,0..4] of Byte;
    Y,C:Byte;
begin
    BlockRead(F,HodgePodge,SizeOf(HodgePodge));
    for Y:=0 to 7 do
    begin
        for C:=0 to 3 do Image[C,Y]:=HodgePodge[Y,C+1];
        Mask[Y]:=not HodgePodge[Y,0];
    end;
end;

procedure TSmallSprite.Merge(Source:PSmallSprite);
var
    Y,C:Byte;
begin
    for C:=0 to 3 do
        for Y:=0 to 7 do
            Image[C,Y]:=(Image[C,Y] and Source^.Mask[Y]) xor Source^.Image[C,Y];
end;

procedure TSmallSprite.Reduce(Source:PMedSprite);
var
    Y,C:Word;
begin
    for Y:=0 to 7 do
    begin
        for C:=0 to 3 do Image[C,Y]:=Word2Byte(Source^.Image[C,Y*2]);
        Mask[Y]:=Word2Byte(Source^.Mask[Y*2]);
    end;
end;

procedure TSmallSprite.Cut(Source:PMedSprite;X,Y:Word); assembler;
asm
    PUSH DS
    LDS SI,Source
    LES DI,Self
    MOV BX,Y
    SHL BX,4
    ADD SI,BX
    ADD SI,X
    MOV CX,8
    @CutLoop:
        MOV AL,[SI]; MOV ES:[DI],AL
        MOV AL,[SI+32]; MOV ES:[DI+8],AL
        MOV AL,[SI+64]; MOV ES:[DI+16],AL
        MOV AL,[SI+96]; MOV ES:[DI+24],AL
        MOV AL,[SI+128]; MOV ES:[DI+32],AL
        ADD SI,2
        INC DI
        LOOP @CutLoop
    POP DS
end;

procedure TSmallSprite.Draw(X,Y:Word); assembler;
var
    CurrentWritePlane:Word;
    BlitTable:array[0..3] of TBlit;
    BlitTableLength,BlitTableCounter:Word;
asm
    SHL Y,3
    LEA BX,BlitTable
    PUSH X
    PUSH Y
    PUSH 8
    PUSH SS
    PUSH BX
    PUSH 3
    CALL BuildBlitTable
    MOV BlitTableLength,AX

    @PlaneLoopInit:
        MOV CurrentWritePlane,1
        LEA BX,BlitTable
        PUSH TBlit(SS:[BX]).VidBank
        CALL SetWriteBank
        MOV ES,Word(TGraphWin(GraphMode.Win[WriteWindow])).Segment
        MOV SI,Word(Self).[0]
        CLD

    @PlaneLoopRun:
        PUSH CurrentWritePlane
        CALL SetWritePlane
        MOV AX,BlitTableLength
        MOV BlitTableCounter,AX
        LEA BX,BlitTable

        @CopyLoopInit:
            PUSH DS
            MOV DS,Word(Self).[2]
            MOV DI,TBlit(SS:[BX]).VidOffset
            MOV CX,TBlit(SS:[BX]).RepeatCount
            MOV DX,TBlit(SS:[BX]).ScanSkip
            DEC DX

        @CopyLoopRun:
            MOVSB
            ADD DI,DX
            LOOP @CopyLoopRun

        @CopyLoopDone:
            POP DS
            DEC BlitTableCounter
            JZ @PlaneLoopDone

        @FlipBank:
            ADD BX,TYPE TBlit
            PUSH BX
            PUSH TBlit(SS:[BX]).VidBank
            CALL SetWriteBank
            POP BX
            JMP @CopyLoopInit

    @PlaneLoopDone:
        SHL CurrentWritePlane,1
        CMP CurrentWritePlane,$10
        JE @Done
        CMP BlitTableLength,1
        JBE @PlaneLoopRun
        PUSH TBlit[0].VidBank
        CALL SetWriteBank
        JMP @PlaneLoopRun
@Done:
end;

{ --- Medium sprite routines ---------------------------------------------- }

procedure TMedSprite.Clear;
var
    Y,C:Byte;
begin
    for Y:=0 to 15 do
    begin
        for C:=0 to 3 do Image[C,Y]:=0;
        Mask[Y]:=0;
    end;
end;

procedure TMedSprite.Load(var F:File);
var
    HodgePodge:array[0..15,0..1,0..4] of Byte;
    Y,C:Byte;
begin
    BlockRead(F,HodgePodge,SizeOf(HodgePodge));
    for Y:=0 to 15 do
    begin
        for C:=0 to 3 do Image[C,Y]:=HodgePodge[Y,1,C+1] shl 8+HodgePodge[Y,0,C+1];
        Mask[Y]:=not (HodgePodge[Y,1,0] shl 8+HodgePodge[Y,0,0]);
    end;
end;

procedure TMedSprite.Merge(Source:PMedSprite); assembler;
asm
    PUSH DS
    LDS SI,Source
    LES DI,Self
    MOV CX,16
    @MergeLoop:
        MOV DX,[SI+128]
        MOV AX,ES:[DI];    AND AX,DX; XOR AX,[SI];    MOV ES:[DI],AX
        MOV AX,ES:[DI+32]; AND AX,DX; XOR AX,[SI+32]; MOV ES:[DI+32],AX
        MOV AX,ES:[DI+64]; AND AX,DX; XOR AX,[SI+64]; MOV ES:[DI+64],AX
        MOV AX,ES:[DI+96]; AND AX,DX; XOR AX,[SI+96]; MOV ES:[DI+96],AX
        ADD SI,2
        ADD DI,2
        LOOP @MergeLoop
    POP DS
end;

procedure TMedSprite.Reduce(Source:PLargeSprite);
var
    Y,C:Byte;
begin
    for Y:=0 to 15 do
    begin
        for C:=0 to 3 do Image[C,Y]:=Longint2Word(Source^.Image[C,Y*2]);
        Mask[Y]:=Longint2Word(Source^.Mask[Y*2]);
    end;
end;

procedure TMedSprite.Enlarge(Source:PSmallSprite); assembler;
asm
    PUSH DS
    CLD
    LDS SI,Source
    LES DI,Self
    MOV CX,40
    @EnlargeLoop:
        MOV BL,[SI]
        MOV BH,BL
        SHL BL,1; RCL AX,1; SHL BH,1; RCL AX,1
        SHL BL,1; RCL AX,1; SHL BH,1; RCL AX,1
        SHL BL,1; RCL AX,1; SHL BH,1; RCL AX,1
        SHL BL,1; RCL AX,1; SHL BH,1; RCL AX,1
        SHL BL,1; RCL AX,1; SHL BH,1; RCL AX,1
        SHL BL,1; RCL AX,1; SHL BH,1; RCL AX,1
        SHL BL,1; RCL AX,1; SHL BH,1; RCL AX,1
        SHL BL,1; RCL AX,1; SHL BH,1; RCL AX,1
        INC SI
        XCHG AL,AH
        STOSW; STOSW
        LOOP @EnlargeLoop
    POP DS
end;

procedure TMedSprite.Paste(Source:PSmallSprite;X,Y:Byte);
var
    L,C:Byte;
begin
    for L:=0 to 7 do
    begin
        for C:=0 to 3 do Byte(Ptr(Seg(Image[C]),Ofs(Image[C])+Y*16+L*2+X)^):=Source^.Image[C,L];
        Byte(Ptr(Seg(Mask),Ofs(Mask)+Y*16+L*2+X)^):=Source^.Mask[L];
    end;
end;

procedure TMedSprite.Draw(X,Y:Word); assembler;
var
    CurrentWritePlane:Word;
    BlitTable:array[0..3] of TBlit;
    BlitTableLength,BlitTableCounter:Word;
asm
    SHL X,1
    SHL Y,4
    LEA BX,BlitTable
    PUSH X
    PUSH Y
    PUSH 16
    PUSH SS
    PUSH BX
    PUSH 3
    CALL BuildBlitTable
    MOV BlitTableLength,AX

    @PlaneLoopInit:
        MOV CurrentWritePlane,1
        LEA BX,BlitTable
        PUSH TBlit(SS:[BX]).VidBank
        CALL SetWriteBank
        MOV ES,Word(TGraphWin(GraphMode.Win[WriteWindow])).Segment
        MOV SI,Word(Self).[0]
        CLD

    @PlaneLoopRun:
        PUSH CurrentWritePlane
        CALL SetWritePlane
        MOV AX,BlitTableLength
        MOV BlitTableCounter,AX
        LEA BX,BlitTable

        @CopyLoopInit:
            PUSH DS
            MOV DS,Word(Self).[2]
            MOV DI,TBlit(SS:[BX]).VidOffset
            MOV CX,TBlit(SS:[BX]).RepeatCount
            MOV DX,TBlit(SS:[BX]).ScanSkip
            SUB DX,2

        @CopyLoopRun:
            MOVSW
            ADD DI,DX
            LOOP @CopyLoopRun

        @CopyLoopDone:
            POP DS
            DEC BlitTableCounter
            JZ @PlaneLoopDone

        @FlipBank:
            ADD BX,TYPE TBlit
            PUSH BX
            PUSH TBlit(SS:[BX]).VidBank
            CALL SetWriteBank
            POP BX
            JMP @CopyLoopInit

    @PlaneLoopDone:
        SHL CurrentWritePlane,1
        CMP CurrentWritePlane,$10
        JE @Done
        CMP BlitTableLength,1
        JBE @PlaneLoopRun
        PUSH TBlit[0].VidBank
        CALL SetWriteBank
        JMP @PlaneLoopRun
@Done:
end;

{ --- Large sprite routines ----------------------------------------------- }

procedure TLargeSprite.Clear;
var
    Y,C:Byte;
begin
    for Y:=0 to 31 do
    begin
        for C:=0 to 3 do Image[C,Y]:=0;
        Mask[Y]:=0;
    end;
end;

procedure TLargeSprite.Merge(Source:PLargeSprite);
var
    Y,C:Byte;
begin
    for C:=0 to 3 do
        for Y:=0 to 31 do
            Image[C,Y]:=(Image[C,Y] and Source^.Mask[Y]) xor Source^.Image[C,Y];
end;

procedure TLargeSprite.Enlarge(Source:PMedSprite); assembler;
var
    L:Word;
asm
    PUSH DS
    CLD
    LDS SI,Source
    LES DI,Self
    MOV L,80
    @EnlargeLoop:
        MOV BX,[SI]
        XCHG BL,BH
        MOV CX,BX
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        SHL BX,1; RCL AX,1; RCL DX,1; SHL CX,1; RCL AX,1; RCL DX,1
        ADD SI,2
        XCHG AL,DH
        XCHG AH,DL
        MOV ES:[DI],AX
        MOV ES:[DI+2],DX
        MOV ES:[DI+4],AX
        MOV ES:[DI+6],DX
        ADD DI,8
        DEC L
        JNZ @EnlargeLoop
    POP DS
end;

procedure TLargeSprite.Draw(X,Y:Word); assembler;
var
    CurrentWritePlane:Word;
    BlitTable:array[0..3] of TBlit;
    BlitTableLength,BlitTableCounter:Word;
asm
    SHL X,2
    SHL Y,5
    LEA BX,BlitTable
    PUSH X
    PUSH Y
    PUSH 32
    PUSH SS
    PUSH BX
    PUSH 3
    CALL BuildBlitTable
    MOV BlitTableLength,AX

    @PlaneLoopInit:
        MOV CurrentWritePlane,1
        LEA BX,BlitTable
        PUSH TBlit(SS:[BX]).VidBank
        CALL SetWriteBank
        MOV ES,Word(TGraphWin(GraphMode.Win[WriteWindow])).Segment
        MOV SI,Word(Self).[0]
        CLD

    @PlaneLoopRun:
        PUSH CurrentWritePlane
        CALL SetWritePlane
        MOV AX,BlitTableLength
        MOV BlitTableCounter,AX
        LEA BX,BlitTable

        @CopyLoopInit:
            PUSH DS
            MOV DS,Word(Self).[2]
            MOV DI,TBlit(SS:[BX]).VidOffset
            MOV CX,TBlit(SS:[BX]).RepeatCount
            MOV DX,TBlit(SS:[BX]).ScanSkip
            SUB DX,4

        @CopyLoopRun:
            MOVSW
            MOVSW
            ADD DI,DX
            LOOP @CopyLoopRun

        @CopyLoopDone:
            POP DS
            DEC BlitTableCounter
            JZ @PlaneLoopDone

        @FlipBank:
            ADD BX,TYPE TBlit
            PUSH BX
            PUSH TBlit(SS:[BX]).VidBank
            CALL SetWriteBank
            POP BX
            JMP @CopyLoopInit

    @PlaneLoopDone:
        SHL CurrentWritePlane,1
        CMP CurrentWritePlane,$10
        JE @Done
        CMP BlitTableLength,1
        JBE @PlaneLoopRun
        PUSH TBlit[0].VidBank
        CALL SetWriteBank
        JMP @PlaneLoopRun
@Done:
end;

{ --- Mouse pointer drawing routines -------------------------------------- }

procedure DrawGMouse(X,Y:Integer); assembler;
var
    CurrentReadPlane,CurrentWritePlane:Word;
    BlitTable:array[0..3] of TBlit;
    BlitTableLength,BlitTableCounter:Word;
    MouseClip,ScanSkip,RepeatCount:Word;
    MouseLeftShift,MouseRightShift:Byte;
    ImageOffset,MaskOffset,SaveOffset:Word;
asm
    MOV AX,X
    AND AX,$000F
    MOV MouseRightShift,AL
    MOV AH,16
    SUB AH,AL
    MOV MouseLeftShift,AH

    SHR X,3
    AND X,$FFFE
    MOV AX,GraphMode.ScanLineSize
    SUB AX,X
    MOV MouseClip,AX

    MOV ImageOffset,OFFSET TMedSprite(MouseCursor).Image
    MOV SaveOffset,OFFSET MouseSave

    LEA BX,BlitTable
    PUSH X
    PUSH Y
    PUSH 16
    PUSH SS
    PUSH BX
    PUSH 3
    CALL BuildBlitTable
    MOV BlitTableLength,AX

    @PlaneLoopInit:
        MOV CurrentReadPlane,0
        MOV CurrentWritePlane,1
        LEA BX,BlitTable
        PUSH TBlit(SS:[BX]).VidBank
        PUSH TBlit(SS:[BX]).VidBank
        CALL SetReadBank
        CALL SetWriteBank

    @PlaneLoopRun:
        PUSH CurrentReadPlane
        CALL SetReadPlane
        PUSH CurrentWritePlane
        CALL SetWritePlane
        MOV AX,BlitTableLength
        MOV BlitTableCounter,AX
        MOV MaskOffset,OFFSET TMedSprite(MouseCursor).Mask
        LEA BX,BlitTable

        @CopyLoopInit:
            MOV DI,TBlit(SS:[BX]).VidOffset
            MOV CX,TBlit(SS:[BX]).RepeatCount
            MOV DX,TBlit(SS:[BX]).ScanSkip
            MOV RepeatCount,CX
            MOV ScanSkip,DX
            PUSH BX
            MOV CL,MouseRightShift
            MOV CH,MouseLeftShift
            CMP MouseClip,2
            JA @CopyLoopRun4

        @CopyLoopRun2:
        { Save screen image }
            MOV ES,Word(TGraphWin(GraphMode.Win[0])).Segment
            MOV SI,SaveOffset
            MOV AX,ES:[DI]
            MOV [SI],AX
            XCHG AL,AH

        { Apply the mask }
            MOV SI,MaskOffset
            MOV BX,[SI]
            SHR BX,CL
            NOT BX
            AND AX,BX

        { Apply the image }
            MOV SI,ImageOffset
            MOV BX,[SI]
            SHR BX,CL
            XOR AX,BX

        { Write to video memory }
            XCHG AL,AH
            MOV ES,Word(TGraphWin(GraphMode.Win[WriteWindow])).Segment
            MOV ES:[DI],AX

        { Advance to next scan line }
            ADD DI,DX
            ADD SaveOffset,2
            ADD MaskOffset,2
            ADD ImageOffset,2
            DEC RepeatCount
            JNZ @CopyLoopRun2
            JMP @CopyLoopDone

        @CopyLoopRun4:
        { Save screen image }
            MOV ES,Word(TGraphWin(GraphMode.Win[0])).Segment
            MOV SI,SaveOffset
            MOV AX,ES:[DI]
            MOV DX,ES:[DI+2]
            MOV [SI],AX
            MOV [SI+2],DX
            XCHG AL,AH
            XCHG DL,DH

        { Apply the mask }
            MOV SI,MaskOffset
            MOV BX,[SI]
            SHR BX,CL
            NOT BX
            AND AX,BX
            MOV BX,[SI]
            XCHG CH,CL
            SHL BX,CL
            NOT BX
            AND DX,BX

        { Apply the image }
            MOV SI,ImageOffset
            MOV BX,[SI]
            XCHG CH,CL
            SHR BX,CL
            OR AX,BX
            MOV BX,[SI]
            XCHG CH,CL
            SHL BX,CL
            OR DX,BX

        { Write to video memory }
            XCHG AL,AH
            XCHG DL,DH
            MOV ES,Word(TGraphWin(GraphMode.Win[WriteWindow])).Segment
            MOV ES:[DI],AX
            MOV ES:[DI+2],DX

        { Advance to next scan line }
            ADD DI,ScanSkip
            ADD SaveOffset,4
            ADD MaskOffset,2
            ADD ImageOffset,2
            XCHG CH,CL
            DEC RepeatCount
            JNZ @CopyLoopRun4

        @CopyLoopDone:
            POP BX
            DEC BlitTableCounter
            JZ @PlaneLoopDone

        @FlipBank:
            ADD BX,TYPE TBlit
            PUSH BX
            PUSH TBlit(SS:[BX]).VidBank
            PUSH TBlit(SS:[BX]).VidBank
            CALL SetReadBank
            CALL SetWriteBank
            POP BX
            JMP @CopyLoopInit

    @PlaneLoopDone:
        INC CurrentReadPlane
        SHL CurrentWritePlane,1
        CMP CurrentWritePlane,$10
        JE @Done
        CMP BlitTableLength,1
        JBE @PlaneLoopRun
        PUSH TBlit[0].VidBank
        PUSH TBlit[0].VidBank
        CALL SetReadBank
        CALL SetWriteBank
        JMP @PlaneLoopRun
@Done:
end;

procedure RestoreGMouse(X,Y:Integer); assembler;
var
    CurrentWritePlane:Word;
    BlitTable:array[0..3] of TBlit;
    BlitTableLength,BlitTableCounter:Word;
    MouseClip:Word;
asm
    SHR X,3
    AND X,$FFFE
    MOV AX,GraphMode.ScanLineSize
    SUB AX,X
    MOV MouseClip,AX

    LEA BX,BlitTable
    PUSH X
    PUSH Y
    PUSH 16
    PUSH SS
    PUSH BX
    PUSH 3
    CALL BuildBlitTable
    MOV BlitTableLength,AX

    @PlaneLoopInit:
        MOV CurrentWritePlane,1
        LEA BX,BlitTable
        PUSH TBlit(SS:[BX]).VidBank
        CALL SetWriteBank
        MOV SI,OFFSET MouseSave
        MOV ES,Word(TGraphWin(GraphMode.Win[WriteWindow])).Segment

    @PlaneLoopRun:
        PUSH CurrentWritePlane
        CALL SetWritePlane
        MOV AX,BlitTableLength
        MOV BlitTableCounter,AX
        LEA BX,BlitTable

        @CopyLoopInit:
            MOV DI,TBlit(SS:[BX]).VidOffset
            MOV CX,TBlit(SS:[BX]).RepeatCount
            MOV DX,TBlit(SS:[BX]).ScanSkip
            CMP MouseClip,2
            JA @CopyLoopRun4

        @CopyLoopRun2:
            MOV AX,[SI]
            MOV ES:[DI],AX
            ADD DI,DX
            ADD SI,2
            LOOP @CopyLoopRun2
            JMP @CopyLoopDone

        @CopyLoopRun4:
            MOV AX,[SI]
            MOV ES:[DI],AX
            MOV AX,[SI+2]
            MOV ES:[DI+2],AX
            ADD DI,DX
            ADD SI,4
            LOOP @CopyLoopRun4

        @CopyLoopDone:
            DEC BlitTableCounter
            JZ @PlaneLoopDone

        @FlipBank:
            ADD BX,TYPE TBlit
            PUSH BX
            PUSH TBlit(SS:[BX]).VidBank
            CALL SetWriteBank
            POP BX
            JMP @CopyLoopInit

    @PlaneLoopDone:
        SHL CurrentWritePlane,1
        CMP CurrentWritePlane,$10
        JE @Done
        CMP BlitTableLength,1
        JBE @PlaneLoopRun
        PUSH TBlit[0].VidBank
        CALL SetWriteBank
        JMP @PlaneLoopRun
@Done:
end;

procedure SetGMouseCursor(NewMouseCursor:PMedSprite);
var
    C,Y:Byte;
begin
    MouseCursor:=NewMouseCursor^;
    for Y:=0 to 15 do
    begin
        for C:=0 to 3 do MouseCursor.Image[C,Y]:=Swap(MouseCursor.Image[C,Y]);
        MouseCursor.Mask[Y]:=not Swap(MouseCursor.Mask[Y]);
    end;
end;

procedure ShowGMouse;
begin
    MouseVisible:=True;
    DrawGMouse(MouseCoord.X,MouseCoord.Y);
end;

procedure HideGMouse;
begin
    MouseVisible:=False;
    RestoreGMouse(MouseCoord.X,MouseCoord.Y);
end;

procedure UpdateGMouse(X,Y:Integer);
begin
    if MouseVisible then
    begin
        if (X<>MouseCoord.X) or (Y<>MouseCoord.Y) then
        begin
            RestoreGMouse(MouseCoord.X,MouseCoord.Y);
            DrawGMouse(X,Y);
            MouseCoord.X:=X;
            MouseCoord.Y:=Y;
        end;
    end else begin
        MouseVisible:=True;
        DrawGMouse(X,Y);
        MouseCoord.X:=X;
        MouseCoord.Y:=Y;
    end;
end;

{ --- Startup routine ----------------------------------------------------- }

begin
    SaveUpdateProc:=GraphModeUpdateProc;
    GraphModeUpdateProc:=UpdateProc;
end.
