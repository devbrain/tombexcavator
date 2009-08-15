unit GCore;

{ This unit is the core of the graphics system, and provides a common       }
{ direct buffer access interface for all EGA, VGA and VESA video modes, as  }
{ well as palette handling for 16 and 256 colour modes. }

{ The real mode version requires a 286+ processor }

interface

const
    MaxModes=256;

type
{ TGraphSubSystem is the graphic card type the mode belongs to }
{ TGraphModeType is the memory model (pixel format/addressing) }
{   Note: In the real mode version, all modes are banked }
{ TGraphPalette is the type of palette available }
{   None (do not use palette services), EGA 16 or 64 colour, }
{   Standard VGA 6-bit*3, VESA 8-bit*3 }
    TGraphSubSystem=(gsEGA,gsVGA,gsVESA);
    TGraphModeType=(gtPlane4,gtPacked8,gtPlane8,gtTrueColour);
    TGraphPalette=(gpNone,gpEGA16,gpEGA64,gpRGB);

    TGraphCoord=record
        X,Y:Integer;
    end;

    TTextCoord=record
        X,Y:Byte;
    end;

    TRGB=record
        Alpha,Red,Green,Blue:Byte;
    end;

    TRGBComp=record
        Size,Pos:Byte;
    end;

    TRGBField=record
        Red,Green,Blue,Alpha:TRGBComp;
    end;

{ Procedural types for common video functions that differ between modes }
    TGetRGB=procedure(var RGBArray:array of TRGB;Index,Count:Word);
    TSetRGB=procedure(var RGBArray:array of TRGB;Index,Count:Word;WaitForRetrace:Boolean);
    TSetDisplayOffset=procedure(DisplayOffset:Longint;WaitForRetrace:Boolean);
    TSetScanLineSize=procedure(ScanLineSize:Word);
    TPrintChar=procedure(CharOffset,Colour:Longint;C:Char);

{ Segment: 16-bit segment selector of window buffer }
{ Size: Size of window in bytes }
{ Index: Hardware window number (0=A, 1-B), used for bank switching calls }
{ Inverse: If this window also supports inverse operation (read<->write) }

{ Please be aware that it is possible for both the reading and writing }
{ window to be the same window, if this is case the index fields for both }
{ windows will be the same and changing the write window will also change }
{ the read window (and vice versa). Do not always assume that direct memory }
{ copying is available - please check the index fields or use a system }
{ memory buffer. }
    TGraphWin=record
        Segment:Word;
        Size:Longint;
        Index:Byte;
        Inverse:Boolean;
    end;

{ Size: X,Y coordinate sizes of screen. }
{ ColourBits: Number of bits used for colour. }
{ BitsPerPixel: Total bits per pixel, including alpha fields. }
{ GraphSubSystem: Graphics subsystem that this mode belongs to. }
{ GraphModeType: Memory model of video mode. }
{ Info: 16-bit word, subsystem-specific. Should not be used externally. }
    TGraphModeDesc=record
        Size:TGraphCoord;
        ColourBits,BitsPerPixel:Byte;
        GraphSubSystem:TGraphSubSystem;
        GraphModeType:TGraphModeType;
        Info:Word;
    end;

{ Win: Graphics windows (0=Read window, 1=Write window) }
{ WinGranularity: Granularity of each window in bytes }
{ MaxBank: Highest bank number (0=no banking needed) }
{ Palette: Palette type }
{ PlaneCount: Number of planes }
{ Plane: Status of read/write planes (0=Read plane, 1=Write Plane) }
{ RedField..BlueField: RGB Packing format, used only in truecolour modes }
{ UseAlpha: Alpha field of RGBA quads may be used }
{ UseText: Text services may be used (BIOS or otherwise) }
{ TextCell: Resolution of text characters }
{ ScanLineSize: Length of each scan line in bytes }
{ MaxScanLineSize: Maximum scan line length - 0 if unable to determine }
{ DisplayOffset: Pixel Location in video memory where image starts }
{ PlaneSize: Total bytes of video memory per plane }

{ IMPORTANT: For correct drawing operation, do not use the Size.X field to }
{ determine the horizontal size of the video buffer - use }
{ GraphMode.ScanLineSize. It is possible for these two variables to be }
{ different, as some video cards may set the scan line size to be a }
{ multiple of the window granularity to make bank switching easier, with }
{ extra bytes after each displayed portion for alignment purposes. }
    TGraphMode=record
        Win:array[0..1] of TGraphWin;
        WinGranularity:Longint;
        MaxBank:Word;
        Palette:TGraphPalette;
        PlaneCount:Byte;
        Plane:array[0..1] of Byte;
        PixelsPerUnit:Word;
        RGBField:TRGBField;
        UseAlpha,UseText:Boolean;
        TextCell:TTextCoord;
        ScanLineSize,MaxScanLineSize:Word;
        DisplayOffset,PlaneSize:Longint;
    end;

var
{ GraphModeCurrent is the current video mode (0 if nothing set) }
{ GraphModeDescCount is the highest index in the array that has valid info }
{ GraphModeDesc is an array of brief descriptions of video modes }
    GraphModeCurrent:Word;
    GraphModeDescCount:Word;
    GraphModeDesc:array[1..MaxModes] of TGraphModeDesc;

{ GraphMode holds full information on the current video mode }
    GraphMode:TGraphMode;

{ GetRGB and SetRGB are aliases of the RGB palette functions, since the VGA }
{ and VESA BIOS require them to be loaded in different ways. }
    GetRGB:TGetRGB;
    SetRGB:TSetRGB;

{ SetDisplayOffset sets the display start to the pixel offset in video }
{ memory (this is the byte offset multiplied by pixels per byte). }
    SetDisplayOffset:TSetDisplayOffset;

{ SetScanLineSize sets the logical scan line length of the video card (in }
{ bytes). Check the GraphMode.ScanLineSize variable afterwards for what }
{ the actual length is. }
    SetScanLineSize:TSetScanLineSize;

{ PrintChar prints the specified text character at the specified text }
{ location using BIOS tty calls. No action is taken if BIOS tty calls are }
{ not available for this mode, however check the GraphMode.UseText }
{ variable for availability of BIOS tty calls. It is also possible for }
{ these tty functions to be emulated by this unit. }
    PrintChar:TPrintChar;

{ This procedural pointer will get called whenever there is a mode set, or }
{ if any information in the GraphMode variable changes (such as the scan }
{ line size). To chain onto this list, please save the current contents of }
{ this pointer in a procedural pointer, then when your routine is called, }
{ jump to or call the pointer value you saved. }
    GraphModeUpdateProc:procedure;

{ SetGraphMode sets the graph mode to the mode associated with the mode }
{   descriptor that is at the given index in the array. Please do not }
{   hard code index numbers in your program, as the mode numbers will most
{   likely be different from machine to machine. Use the given search }
{   function here. No action is taken if the index is out of range. Returns }
{   true if the mode set was successful, false if not. }
{ ResetGraphMode sets the video card back into text mode }
function SetGraphMode(GraphModeIndex:Word):Boolean;
procedure ResetGraphMode;

{ SetReadBank and SetWriteBank set the video card's read and write window }
{ banks respectively. Give the number of the bank you want, starting from }
{ zero, and the given window will then be offset to the bank number }
{ multiplied by the window granularity. Current read/write banks are stored }
{ in GraphMode.Win[].Pos. }
procedure SetReadBank(Bank:Word);
procedure SetWriteBank(Bank:Word);

{ GetReadBank and GetWriteBank return the current bank of the read and write }
{ windows respectively. }
function GetReadBank:Word;
function GetWriteBank:Word;

{ SetReadPlane and SetWritePlane set the video card's read and write planes }
{ for video modes that need them. The read plane is specified as a number }
{ between 0..3 (because only one plane can be read at a time), and the }
{ write planes are specified as a 4-bit mask, with each bit from d0..d3 }
{ set if that plane is to be written to. Other bits are ignored. }
procedure SetReadPlane(Plane:Byte);
procedure SetWritePlane(Plane:Byte);

{ SetPalette and GetPalette load the EGA/VGA palette registers with the }
{   given array of values, starting from the given index and repeating using }
{   the given count. GetPalette is only available on VGA cards, please check }
{   the video subsystem before using this procedure. }
procedure SetPalette(var PalArray:array of Byte;Index,Count:Byte);
procedure GetPalette(var PalArray:array of Byte;Index,Count:Byte);

{ Prints a list of modes to the given output stream, useful for debugging. }
procedure WriteModes(var Output:Text);

{ Searches the array of mode descriptors for a mode descriptor with the }
{ requested resolution and colour depth. Returns 0 if nothing found. }
function GetGraphModeDesc(X,Y:Integer;ColourBits:Byte):Word;

implementation

{ --- Internal constants/structures/variables ----------------------------- }

const
{ VESA VBEInfo valid signatures }
    VESASignature=$41534556;
    VBE2Signature=$32454256;

{ VESA BIOS success status }
    vsOk=$004F;

type
    TMemLong=record
        Lo,Hi:Word;
    end;

    PVideoModeArray=^TVideoModeArray;
    TVideoModeArray=array[0..32759] of Word;

    TMemoryModel=(mmText,mmCGA,mmHercules,mmPlanar,mmPackedPixel,
        mmNonChain4,mmDirectColour,mmYUV);

    TVBEInfo=record
        VBESignature:Longint;
        VBEVersion:Word;
        OEMString:PChar;
        Capabilities:Longint;
        VideoModeArray:PVideoModeArray;
        TotalMemory:Word;
        OEMSoftwareRev:Word;
        OEMVendorName:PChar;
        OEMProductName:PChar;
        OEMProductRev:PChar;
        Reserved:array[0..477] of Byte;
    end;

    TModeInfo=record
        ModeAttributes:Word;
        WinAttribute:array[0..1] of Byte;
        WinGranularity:Word;
        WinSize:Word;
        WinSegment:array[0..1] of Word;
        WinFuncPtr:Pointer;
        BytesPerScanLine:Word;
        Size:TGraphCoord;
        Cell:TTextCoord;
        NumberOfPlanes:Byte;
        BitsPerPixel:Byte;
        NumberOfBanks:Byte;
        MemoryModel:TMemoryModel;
        BankSize:Byte;
        NumberOfImagePages:Byte;
        Reserved1:Byte;
        RGBField:TRGBField;
        DirectColourModeInfo:Byte;
        Reserved2:array[0..215] of Byte;
    end;

const
{ String equivalents of enumerations (for WriteModes routine) }
    GraphSubSystemStr:array[Low(TGraphSubSystem)..High(TGraphSubSystem)]
        of String[7]=('gsEGA','gsVGA','gsVESA');
    GraphModeTypeStr:array[Low(TGraphModeType)..High(TGraphModeType)]
        of String[15]=('gtPlane4','gtPacked8','gtPlane8','gtTrueColour');

{ Blank TGraphMode structure for easy initialisation }
    BlankGraphMode:TGraphMode=(
        Win:((Segment:0;Size:0;Index:0;Inverse:False),
            (Segment:0;Size:0;Index:0;Inverse:False));
        WinGranularity:0;
        MaxBank:0;
        Palette:gpNone;
        PlaneCount:0;
        Plane:(0,0);
        PixelsPerUnit:0;
        RGBField:(Red:(Size:0;Pos:0);Green:(Size:0;Pos:0);
            Blue:(Size:0;Pos:0);Alpha:(Size:0;Pos:0));
        UseAlpha:False;
        UseText:False;
        TextCell:(X:0;Y:0);
        ScanLineSize:0;
        MaxScanLineSize:0;
        DisplayOffset:0;
        PlaneSize:0);

{ Pre-initialised standard EGA/VGA TGraphMode structure }
    StdEGAVGAGraphMode:TGraphMode=(
        Win:((Segment:$A000;Size:65536;Index:0;Inverse:True),
            (Segment:$A000;Size:65536;Index:0;Inverse:True));
        WinGranularity:65536;
        MaxBank:0;
        Palette:gpEGA16;
        PlaneCount:4;
        Plane:(0,0);
        PixelsPerUnit:0;
        RGBField:(Red:(Size:0;Pos:0);Green:(Size:0;Pos:0);
            Blue:(Size:0;Pos:0);Alpha:(Size:0;Pos:0));
        UseAlpha:False;
        UseText:True;
        TextCell:(X:0;Y:0);
        ScanLineSize:0;
        MaxScanLineSize:255;
        DisplayOffset:0;
        PlaneSize:65536);

var

{ Storage for VESA query structures }
    VBEInfo:TVBEInfo;
    ModeInfo:TModeInfo;

{ Masking value for VGA display offset to prevent divide by zero }
    VGA_DisplayOffsetMask:Word;

{ In VESA paletted modes, shift factor to be applied before loading palette }
{ and after reading palette on RGB triples. }
    VESA_RGBShift:Byte;

{ Storage for pointer to bank-switching function }
    VESA_BankFunction:Pointer;

{ --- Blank uninitialised driver routines --------------------------------- }

{ These routines are pointed to if a specific video function is not }
{ available, to prevent a nil pointer being dereferenced. Note however that }
{ it is still good practice to not call these functions anyway, and you }
{ should use this unit as if these routines were going to be taken out }
{ tommorrow. Optionally these routines may also print a warning message. }
procedure NIL_GetRGB(var RGBArray:array of TRGB;Index,Count:Word); far; begin end;
procedure NIL_SetRGB(var RGBArray:array of TRGB;Index,Count:Word;WaitForRetrace:Boolean); far; begin end;
procedure NIL_SetDisplayOffset(DisplayOffset:Longint;WaitForRetrace:Boolean); far; begin end;
procedure NIL_SetScanLineSize(ScanLineSize:Word); far; begin end;
procedure NIL_PrintChar(CharOffset,Colour:Longint;C:Char); far; begin end;
procedure NIL_BankFunction; far; assembler; asm MOV DX,0 end;

{ End of update procedure chain }
procedure NIL_UpdateProc; far; begin end;

{ --- EGA/VGA internal driver routines ------------------------------------ }

{ Returns true if an EGA card is detected }

function DetectEGA:Boolean; assembler;
asm
    MOV AH,$12
    MOV BL,$10
    INT $10
    CMP BL,$10
    JNE @True
    @False:
        MOV AL,False
        JMP @DONE
    @True:
        MOV AL,True
    @DONE:
end;

{ Returns true if a VGA card is detected }

function DetectVGA:Boolean; assembler;
asm
    MOV AX,$1A00
    INT $10
    CMP BL,$08
    JE @True
    @False:
        MOV AL,False
        JMP @DONE
    @True:
        MOV AL,True
    @DONE:
end;

{ Sets a EGA/VGA BIOS video mode }

procedure SetBIOSMode(Mode:Byte); assembler;
asm
    MOV AH,$00
    MOV AL,Mode
    INT $10
end;

{ Standard VGA RGB palette loading function }

procedure VGA_SetRGB(var RGBArray:array of TRGB;Index,Count:Word;WaitForRetrace:Boolean); far; assembler;
asm
    PUSH DS
    LDS SI,RGBArray
    CLD

    MOV DX,$3C8
    MOV AX,Index
    OUT DX,AL
    MOV CX,Count

    CMP WaitForRetrace,True
    JNE @LoadPalette
    MOV DX,$3DA
    @W1:IN AL,DX; TEST AL,$08; JNZ @W1
    @W2:IN AL,DX; TEST AL,$08; JZ @W2

    @LoadPalette:
        MOV DX,$3C9

    @LoadPaletteLoop:
        INC SI
        LODSB; SHR AL,2; OUT DX,AL
        LODSB; SHR AL,2; OUT DX,AL
        LODSB; SHR AL,2; OUT DX,AL
        LOOP @LoadPaletteLoop
        POP DS
end;

{ Standard VGA RGB palette reading function }

procedure VGA_GetRGB(var RGBArray:array of TRGB;Index,Count:Word); far; assembler;
asm
    LES DI,RGBArray
    CLD

    MOV DX,$3C8
    MOV AX,Index
    OUT DX,AL
    MOV DX,$3C9
    MOV CX,Count

    @SavePaletteLoop:
        INC DI
        IN AL,DX; SHL AL,2; STOSB
        IN AL,DX; SHL AL,2; STOSB
        IN AL,DX; SHL AL,2; STOSB
        LOOP @SavePaletteLoop
end;

{ Standard VGA display start function }

procedure VGA_SetDisplayOffset(DisplayOffset:Longint;WaitForRetrace:Boolean); far; assembler;
asm
    CMP WaitForRetrace,True
    JNE @SetDisplayStart
    MOV DX,$3DA
    @W1:IN AL,DX; TEST AL,$08; JNZ @W1
    @W2:IN AL,DX; TEST AL,$08; JZ @W2

    @SetDisplayStart:
    { Calculate/record video memory offset and panning shift values }
        MOV AX,TMemLong(DisplayOffset).Lo
        MOV DX,TMemLong(DisplayOffset).Hi
        AND DX,VGA_DisplayOffsetMask
        MOV TMemLong(GraphMode.DisplayOffset).Lo,AX
        MOV TMemLong(GraphMode.DisplayOffset).Hi,DX
        DIV GraphMode.PixelsPerUnit
        MOV BX,DX   { Panning offset in BX }
        MOV CX,AX   { Memory offset in CX }
    { Write to VGA display start and panning registers }
        MOV DX,$3D4
        MOV AL,$0D; MOV AH,CL; OUT DX,AX
        MOV AL,$0C; MOV AH,CH; OUT DX,AX

    { NOTE: Need to find out how to pan at individual pixel offsets for 16 colour modes }
end;

{ Standard VGA set scan line length function }

procedure VGA_SetScanLineSize(ScanLineSize:Word); far; assembler;
asm
    MOV AL,Byte(ScanLineSize)
    MOV AH,0
    MOV GraphMode.ScanLineSize,AX
    MOV DX,$3D4
    MOV AH,AL
    MOV AL,$13
    OUT DX,AX
    CALL DWORD PTR GraphModeUpdateProc
end;

{ --- VESA internal driver routines --------------------------------------- }

{ Retrieves any available VESA BIOS info }

procedure GetVBEInfo(var VBEInfo:TVBEInfo); assembler;
asm
    MOV AX,$4F00
    LES DI,VBEInfo
    INT $10
end;

{ Retrieves information on a given VESA BIOS video mode }

procedure GetModeInfo(var ModeInfo:TModeInfo;Mode:Word); assembler;
asm
    LES DI,ModeInfo
    CLD
    MOV AX,0
    MOV CX,(TYPE TModeInfo)/2
    REP STOSW

    LES DI,ModeInfo
    MOV AX,$4F01
    MOV CX,Mode
    INT $10
end;

{ Sets a VESA BIOS video mode }

procedure SetVESAMode(Mode:Word); assembler;
asm
    MOV AX,$4F02
    MOV BX,Mode
    INT $10
end;

{ Returns true if function $4F09 is supported }

function VESAPaletteTest:Boolean; assembler;
var
    RGB:TRGB;
asm
    MOV AX,SS
    MOV ES,AX
    LEA DI,RGB
    MOV AX,$4F09
    MOV BL,$01
    MOV CX,1
    MOV DX,0
    INT $10
    CMP AX,vsOk; JNE @L1
    MOV AL,True; JMP @L2
@L1:MOV AL,False
@L2:
end;

{ Sets DAC width to maximum possible (8-bits), return resulting bit size }

function VESAMaxDAC:Byte; assembler;
asm
    MOV AX,$4F08
    MOV BX,$0800
    INT $10
    MOV AL,BH
end;

{ Queries maximum scan line size, returns it or 0 if not reported }

function GetMaxScanLineSize:Word; assembler;
asm
    MOV AX,$4F06
    MOV BL,$03
    INT $10
    CMP AX,vsOk; JNE @L1
    MOV AX,CX; JMP @L2
@L1:MOV AX,0
@L2:
end;

{ Bank switches thru the BIOS (emulates a far call bank-switching routine if
{ the underlying VESA BIOS does not provide one). }

procedure VESA_IntBankFunction; far; assembler;
asm
    MOV AX,$4F05
    INT $10
end;

procedure VESA_SetRGB(var RGBArray:array of TRGB;Index,Count:Word;WaitForRetrace:Boolean); far; assembler;
var
    VESAPal:array[0..1023] of Byte;
asm
    PUSH DS
    CMP VESA_RGBShift,0
    JE @QuickLoad

    @Convert:
        MOV CL,VESA_RGBShift
        LDS SI,RGBArray
        MOV AX,SS
        MOV ES,AX
        LEA DI,VESAPal
        MOV BX,Count
        CLD

        @ConvertLoop:
            INC SI
            INC DI
            LODSB; SHR AL,CL; STOSB
            LODSB; SHR AL,CL; STOSB
            LODSB; SHR AL,CL; STOSB
            DEC BX
            JNZ @ConvertLoop
            LEA DI,VESAPal
            JMP @LoadPalette

    @QuickLoad:
        LES DI,RGBArray

    @LoadPalette:
        MOV AX,$4F09
        MOV CX,Count
        MOV DX,Index
        CMP WaitForRetrace,True; JNE @L1
        MOV BL,$80; JMP @L2
    @L1:MOV BL,$00
    @L2:INT $10
        POP DS
end;

procedure VESA_GetRGB(var RGBArray:array of TRGB;Index,Count:Word); far; assembler;
asm
    MOV AX,$4F09
    MOV BL,$01
    MOV CX,Count
    MOV DX,Index
    LES DI,RGBArray
    INT $10

    CMP VESA_RGBShift,0; JE @Done

    @Convert:
        LES DI,RGBArray
        MOV BX,Count
        MOV CL,VESA_RGBShift

    @ConvertLoop:
        SHR TRGB(ES:[DI]).Red,CL
        SHR TRGB(ES:[DI]).Blue,CL
        SHR TRGB(ES:[DI]).Green,CL
        ADD DI,TYPE TRGB
        DEC BX
        JNZ @ConvertLoop

    @Done:
end;

procedure VESA_SetDisplayOffset(DisplayOffset:Longint;WaitForRetrace:Boolean); far; assembler;
asm
    MOV AX,GraphMode.ScanLineSize
    MUL GraphMode.PixelsPerUnit
    MOV BX,AX
    MOV AX,TMemLong(DisplayOffset).Lo
    MOV DX,TMemLong(DisplayOffset).Hi
    DIV BX
    MOV CX,DX
    MOV DX,AX
    MOV AX,$4F07
    MOV BX,$0000
    INT $10

    MOV AX,$4F07
    MOV BX,$0001
    INT $10
    MOV BX,DX
    MOV AX,GraphMode.ScanLineSize
    MUL GraphMode.PixelsPerUnit
    MUL BX
    ADD AX,CX
    ADC DX,0
    MOV TMemLong(GraphMode.DisplayOffset).Lo,AX
    MOV TMemLong(GraphMode.DisplayOffset).Hi,DX
end;

procedure VESA_SetScanLineSize(ScanLineSize:Word); far; assembler;
asm
    MOV AX,ScanLineSize
    MUL GraphMode.PixelsPerUnit
    MOV CX,AX
    MOV AX,$4F06
    MOV BL,$00
    INT $10
    CMP AX,vsOk; JNE @Done

    MOV AX,BX
    MOV DX,0
    DIV GraphMode.PixelsPerUnit
    MOV GraphMode.ScanLineSize,AX
@Done:
    CALL DWORD PTR GraphModeUpdateProc
end;

{ --- Internal initialisation routines ------------------------------------ }

{ Adds a graphics mode descriptor to the array of graphics mode }

procedure AddGraphModeDesc(X,Y:Integer;ColourBits,BitsPerPixel:Byte;
    GraphSubSystem:TGraphSubSystem;GraphModeType:TGraphModeType;Info:Word);
begin
    Inc(GraphModeDescCount);
    if GraphModeDescCount<=MaxModes then
    begin
        GraphModeDesc[GraphModeDescCount].Size.X:=X;
        GraphModeDesc[GraphModeDescCount].Size.Y:=Y;
        GraphModeDesc[GraphModeDescCount].ColourBits:=ColourBits;
        GraphModeDesc[GraphModeDescCount].BitsPerPixel:=BitsPerPixel;
        GraphModeDesc[GraphModeDescCount].GraphSubSystem:=GraphSubSystem;
        GraphModeDesc[GraphModeDescCount].GraphModeType:=GraphModeType;
        GraphModeDesc[GraphModeDescCount].Info:=Info;
    end;
end;

{ Autodetection routine that builds up graphics mode arrays }

procedure AutoDetect;
var
    CurrentModeIndex:Word;
begin
    if DetectEGA then
    begin
        AddGraphModeDesc(320,200,4,4,gsEGA,gtPlane4,$000D);
        AddGraphModeDesc(640,200,4,4,gsEGA,gtPlane4,$000E);
        AddGraphModeDesc(640,350,4,4,gsEGA,gtPlane4,$0010);
        if DetectVGA then
        begin
            AddGraphModeDesc(640,480,4,4,gsVGA,gtPlane4,$0012);
        end;
    end;

    VBEInfo.VBESignature:=VBE2Signature;
    GetVBEInfo(VBEInfo);
    if VBEInfo.VBESignature=VESASignature then
    begin
        CurrentModeIndex:=0;
        while VBEInfo.VideoModeArray^[CurrentModeIndex]<>$FFFF do
        begin
            GetModeInfo(ModeInfo,VBEInfo.VideoModeArray^[CurrentModeIndex]);
            if ModeInfo.ModeAttributes and $5B=$1B then
            begin
                if (ModeInfo.BitsPerPixel=4)
                    and (ModeInfo.MemoryModel=mmPlanar)
                    and (ModeInfo.NumberOfPlanes=4) then
                    AddGraphModeDesc(ModeInfo.Size.X,ModeInfo.Size.Y,4,4,gsVESA,gtPlane4,
                VBEInfo.VideoModeArray^[CurrentModeIndex]);
            end;
            Inc(CurrentModeIndex);
        end;
    end;
end;

{ --- User routines ------------------------------------------------------- }

function SetGraphMode(GraphModeIndex:Word):Boolean;
var
    i:Word;
begin
    if (GraphModeIndex>0) and (GraphModeIndex<=GraphModeDescCount) then
    begin
    { Initially assign do-nothing functions }
        GetRGB:=NIL_GetRGB;
        SetRGB:=NIL_SetRGB;
        SetDisplayOffset:=NIL_SetDisplayOffset;
        SetScanLineSize:=NIL_SetScanLineSize;
        PrintChar:=NIL_PrintChar;
        VESA_BankFunction:=@NIL_BankFunction;

        case GraphModeDesc[GraphModeIndex].GraphSubSystem of
            gsEGA,gsVGA: begin
                GraphMode:=StdEGAVGAGraphMode;

                case GraphModeDesc[GraphModeIndex].Info of
                    $000D: begin
                        GraphMode.PixelsPerUnit:=8;
                        GraphMode.TextCell.X:=8;
                        GraphMode.TextCell.Y:=8;
                        GraphMode.ScanLineSize:=40;
                        VGA_DisplayOffsetMask:=$0007;
                    end;
                    $000E: begin
                        GraphMode.PixelsPerUnit:=8;
                        GraphMode.TextCell.X:=8;
                        GraphMode.TextCell.Y:=8;
                        GraphMode.ScanLineSize:=80;
                        VGA_DisplayOffsetMask:=$0007;
                    end;
                    $0010: begin
                        GraphMode.Palette:=gpEGA64;
                        GraphMode.PixelsPerUnit:=8;
                        GraphMode.TextCell.X:=8;
                        GraphMode.TextCell.Y:=14;
                        GraphMode.ScanLineSize:=80;
                        VGA_DisplayOffsetMask:=$0007;
                    end;
                    $0012: begin
                        GraphMode.Palette:=gpRGB;
                        GraphMode.PixelsPerUnit:=8;
                        GraphMode.TextCell.X:=8;
                        GraphMode.TextCell.Y:=16;
                        GraphMode.ScanLineSize:=80;
                        VGA_DisplayOffsetMask:=$0007;
                        GetRGB:=VGA_GetRGB;
                        SetRGB:=VGA_SetRGB;
                    end;
                end;

                SetDisplayOffset:=VGA_SetDisplayOffset;
                SetScanLineSize:=VGA_SetScanLineSize;
                SetBIOSMode(GraphModeDesc[GraphModeIndex].Info);
            end;
            gsVESA: begin
            { Retrieve mode information from VESA BIOS }
                GraphMode:=BlankGraphMode;
                GetModeInfo(ModeInfo,GraphModeDesc[GraphModeIndex].Info);

            { Translate window info into quick access structure }
            { GraphMode.Win[0] holds the reading window }
            { GraphMode.Win[1] holds the writing window }
                for i:=1 downto 0 do
                begin
                    if ModeInfo.WinAttribute[i] and $03=$03 then GraphMode.Win[0].Index:=i;
                    if ModeInfo.WinAttribute[1-i] and $05=$05 then GraphMode.Win[1].Index:=1-i;
                end;
                for i:=0 to 1 do
                begin
                    GraphMode.Win[i].Segment:=ModeInfo.WinSegment[GraphMode.Win[i].Index];
                    GraphMode.Win[i].Size:=Longint(ModeInfo.WinSize)*1024;
                end;

            { Determine if read/write windows can do reverse operation }
                if ModeInfo.WinAttribute[GraphMode.Win[0].Index] and $05=$05 then GraphMode.Win[0].Inverse:=True;
                if ModeInfo.WinAttribute[GraphMode.Win[1].Index] and $03=$03 then GraphMode.Win[1].Inverse:=True;

            { Calculate window granularity and assign bank-switching function }
                GraphMode.WinGranularity:=Longint(ModeInfo.WinGranularity)*1024;
                if ModeInfo.WinFuncPtr<>nil then VESA_BankFunction:=ModeInfo.WinFuncPtr
                    else VESA_BankFunction:=@VESA_IntBankFunction;

            { Determine palette type (always RGB) }
                GraphMode.Palette:=gpRGB;

            { Determine if tty services are available }
                if ModeInfo.ModeAttributes and $04=$04 then
                begin
                    GraphMode.UseText:=True;
                    GraphMode.TextCell.X:=ModeInfo.Cell.X;
                    GraphMode.TextCell.Y:=ModeInfo.Cell.Y;
                end;

            { Determine if alpha byte can be used }
                if ModeInfo.DirectColourModeInfo and $02=$02 then GraphMode.UseAlpha:=True;

            { Calculate scan line and plane sizes, and get total # of banks }
                GraphMode.ScanLineSize:=ModeInfo.BytesPerScanLine;
                GraphMode.PlaneSize:=(VBEInfo.TotalMemory div ModeInfo.NumberOfPlanes)*65536;
                GraphMode.MaxBank:=GraphMode.PlaneSize div GraphMode.WinGranularity-1;

            { Get plane information, if any }
                case GraphModeDesc[GraphModeIndex].GraphModeType of
                    gtPlane4: begin
                        GraphMode.PlaneCount:=4;
                        GraphMode.PixelsPerUnit:=8;
                    end;
                end;

            { Set the video mode }
                SetVESAMode(GraphModeDesc[GraphModeIndex].Info);

            { If the VESA BIOS supports its own palette function, use them, }
            { otherwise fall back on writing to standard VGA RGB registers. }
                if VESAPaletteTest then
                begin
                    if (ModeInfo.DirectColourModeInfo and $01=$01) then VESA_RGBShift:=8-VESAMaxDAC;
                    SetRGB:=VESA_SetRGB;
                    GetRGB:=VESA_GetRGB;
                end else begin
                    SetRGB:=VGA_SetRGB;
                    GetRGB:=VGA_GetRGB;
                end;

            { Assign display start and scan line functions }
                SetDisplayOffset:=VESA_SetDisplayOffset;
                SetScanLineSize:=VESA_SetScanLineSize;

            { Determine maximum scan line length, store zero if VESA BIOS }
            { doesn't support this function (proceed with caution). }
                GraphMode.MaxScanLineSize:=GetMaxScanLineSize;
            end;
        end;

        GraphModeCurrent:=GraphModeIndex;
        SetGraphMode:=True;
        GraphModeUpdateProc;
    end else SetGraphMode:=False;
end;

procedure ResetGraphMode;
begin
    if GraphModeCurrent<>0 then
    begin
        SetBIOSMode($0003);
        GraphModeCurrent:=0;
        GraphMode:=BlankGraphMode;
        GetRGB:=NIL_GetRGB;
        SetRGB:=NIL_SetRGB;
        SetDisplayOffset:=NIL_SetDisplayOffset;
        SetScanLineSize:=NIL_SetScanLineSize;
        PrintChar:=NIL_PrintChar;
        VESA_BankFunction:=@NIL_BankFunction;
        GraphModeUpdateProc;
    end;
end;

procedure SetReadBank(Bank:Word); assembler;
asm
    MOV BH,$00
    MOV BL,TGraphWin(GraphMode.Win[0*TYPE TGraphWin]).Index
    MOV DX,Bank
    CALL DWORD PTR VESA_BankFunction
end;

procedure SetWriteBank(Bank:Word); assembler;
asm
    MOV BH,$00
    MOV BL,TGraphWin(GraphMode.Win[1*TYPE TGraphWin]).Index
    MOV DX,Bank
    CALL DWORD PTR VESA_BankFunction
end;

function GetReadBank:Word; assembler;
asm
    MOV BH,$01
    MOV BL,TGraphWin(GraphMode.Win[0*TYPE TGraphWin]).Index
    CALL DWORD PTR VESA_BankFunction
    MOV AX,DX
end;

function GetWriteBank:Word; assembler;
asm
    MOV BH,$01
    MOV BL,TGraphWin(GraphMode.Win[1*TYPE TGraphWin]).Index
    CALL DWORD PTR VESA_BankFunction
    MOV AX,DX
end;

procedure SetReadPlane(Plane:Byte); assembler;
asm
    MOV AL,$04
    MOV AH,Plane
    MOV Byte(GraphMode.Plane[0]),AH
    MOV DX,$03CE
    OUT DX,AX
end;

procedure SetWritePlane(Plane:Byte); assembler;
asm
    MOV AL,$02
    MOV AH,Plane
    MOV Byte(GraphMode.Plane[1]),AH
    MOV DX,$03C4
    OUT DX,AX
end;

procedure SetPalette(var PalArray:array of Byte;Index,Count:Byte); assembler;
asm
    LES DI,PalArray
    MOV AX,$1000
    MOV BL,Index
    MOV CL,Count
    @SetPaletteLoop:
        MOV BH,ES:[DI]
        INT $10
        INC BL
        INC DI
        DEC CL
        JNZ @SetPaletteLoop
end;

procedure GetPalette(var PalArray:array of Byte;Index,Count:Byte); assembler;
asm
    LES DI,PalArray
    MOV AX,$1007
    MOV BL,Index
    MOV CL,Count
    @GetPaletteLoop:
        INT $10
        MOV ES:[DI],BH
        INC BL
        INC DI
        DEC CL
        JNZ @GetPaletteLoop
end;

procedure WriteModes(var Output:Text);
var
    i:Word;
begin
    for i:=1 to GraphModeDescCount do
        Writeln(Output,
            i:3,
            ': X=',GraphModeDesc[i].Size.X,
            ' Y=',GraphModeDesc[i].Size.Y,
            ' ColourBits=',GraphModeDesc[i].ColourBits,
            ' BitsPerPixel=',GraphModeDesc[i].BitsPerPixel,
            ' GSS=',GraphSubSystemStr[GraphModeDesc[i].GraphSubSystem],
            ' GMT=',GraphModeTypeStr[GraphModeDesc[i].GraphModeType],
            ' Info=',GraphModeDesc[i].Info);
end;

function GetGraphModeDesc(X,Y:Integer;ColourBits:Byte):Word;
var
    i:Word;
begin
    GetGraphModeDesc:=0;
    for i:=GraphModeDescCount downto 1 do
        if (GraphModeDesc[i].Size.X=X)
            and (GraphModeDesc[i].Size.Y=Y)
            and (GraphModeDesc[i].ColourBits=ColourBits) then
        begin
            GetGraphModeDesc:=i;
            Break;
        end;
end;

{ --- Startup routine ----------------------------------------------------- }

begin
    GraphModeDescCount:=0;
    GraphModeCurrent:=0;
    GraphMode:=BlankGraphMode;
    GetRGB:=NIL_GetRGB;
    SetRGB:=NIL_SetRGB;
    SetDisplayOffset:=NIL_SetDisplayOffset;
    SetScanLineSize:=NIL_SetScanLineSize;
    PrintChar:=NIL_PrintChar;
    GraphModeUpdateProc:=NIL_UpdateProc;
    VESA_BankFunction:=@NIL_BankFunction;
    AutoDetect;
end.
