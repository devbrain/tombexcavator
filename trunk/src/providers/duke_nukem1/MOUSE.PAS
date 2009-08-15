unit Mouse;

{$F+,O-}

{ This unit interfaces with the mouse }

interface

type

{ Enumerated and set types for mouse button number (mb = mouse button) }
    TButtonNumber=(mbLeft,mbRight,mbCentre,mb3,mb4,mb5,mb6,mb7,mb8,mb9,mbA,mbB,mdC,mbD,mbE,mbF);
    TButtonStatus=set of TButtonNumber;

{ Sub-routine call mask field positions (cm = call mask) }
    TCallMask=set of (cmMouseMoved,
        cmLeftPressed,cmLeftReleased,
        cmRightPressed,cmRightReleased,
        cmCentrePressed,cmCentreReleased,
        cm7,cm8,cm9,cmA,cmB,cmC,cmD,cmE,cmF);

{ Record used to return mouse info }
    PMouseInfo=^TMouseInfo;
    TMouseInfo=record
        Installed:Boolean;
        NumberOfButtons:Byte;
    end;

{ Record used to store mouse coordinates }
    PMouseCoord=^TMouseCoord;
    TMouseCoord=record
        X,Y:Integer;
    end;

{ Record used to return mouse status }
    PMouseStatus=^TMouseStatus;
    TMouseStatus=record
        ButtonStatus:TButtonStatus;
        Location:TMouseCoord;
    end;

{ Record used to return button press/release data }
    PMouseData=^TMouseData;
    TMouseData=record
        ButtonStatus:TButtonStatus;
        ButtonCount:Word;
        Location:TMouseCoord;
    end;

{ Record used to return info on mouse driver subroutine }
    PMouseSubRoutine=^TMouseSubRoutine;
    TMouseSubRoutine=record
        RoutineAddr:Pointer;
        CallMask:TCallMask;
    end;

function GetMouseInfo:PMouseInfo;
function GetMouseStatus:PMouseStatus;
function GetMousePressData(Button:TButtonNumber):PMouseData;
function GetMouseReleaseData(Button:TButtonNumber):PMouseData;
function GetMouseMotion:PMouseCoord;
function GetMouseStateSize:Word;
function GetMouseVideoPage:Byte;
function SwapMouseSubRoutine(RoutineAddr:Pointer;CallMask:TCallMask):PMouseSubRoutine;
function CheckMouseAction:Boolean;

procedure ShowMouseCursor;
procedure HideMouseCursor;
procedure SetMouseLocation(X,Y:Word);
procedure SetMouseWindow(X1,Y1,X2,Y2:Word);
procedure SetGraphicsCursor(var Bitmap;HotX,HotY:Integer);
procedure SetTextCursor(ScreenMask,CursorMask:Word);
procedure EnableMousePen;
procedure DisableMousePen;
procedure SetMouseRatio(X,Y:Word);
procedure SetMouseUpdateRegion(X1,Y1,X2,Y2:Word);
procedure SetMouseVideoPage(Page:Byte);
procedure SaveMouseState(var Buf);
procedure RestoreMouseState(var Buf);
procedure SetMouseSubRoutine(RoutineAddr:Pointer;CallMask:TCallMask);

implementation

{ --- Internal declarations ----------------------------------------------- }

var
{ Return structures for above functions }
    MouseInfo:TMouseInfo;
    MouseCoord:TMouseCoord;
    MouseStatus:TMouseStatus;
    MouseData:TMouseData;
    MouseSubRoutine:TMouseSubRoutine;

{ Save area for previous mouse status (Used by CheckMouseAction) }
    PrevMouseStatus:TMouseStatus;

{ --- User functions ------------------------------------------------------ }

function GetMouseInfo:PMouseInfo; assembler;
asm
    MOV AX,$0000
    INT $33
    CMP AX,$FFFF
    JE @MouseInstalled
    @MouseNotInstalled:
        MOV MouseInfo.Installed,False
        JMP @DONE
    @MouseInstalled:
        CMP BX,0; JG @@1
        DEC BX
        AND BX,$0003
    @@1:MOV MouseInfo.Installed,True
        MOV MouseInfo.NumberOfButtons,BL
@DONE:
    MOV AX,OFFSET MouseInfo
    MOV DX,SEG MouseInfo
end;

function GetMouseStatus:PMouseStatus; assembler;
asm
    MOV AX,$0003
    INT $33
    MOV MouseStatus.ButtonStatus,BX
    MOV MouseStatus.Location.X,CX
    MOV MouseStatus.Location.Y,DX
    MOV AX,OFFSET MouseStatus
    MOV DX,SEG MouseStatus
end;

function GetMousePressData(Button:TButtonNumber):PMouseData; assembler;
asm
    MOV AX,$0005
    MOV BX,WORD PTR Button
    INT $33
    MOV MouseData.ButtonStatus,AX
    MOV MouseData.ButtonCount,BX
    MOV MouseData.Location.X,CX
    MOV MouseData.Location.Y,DX
    MOV AX,OFFSET MouseData
    MOV DX,SEG MouseData
end;

function GetMouseReleaseData(Button:TButtonNumber):PMouseData; assembler;
asm
    MOV AX,$0006
    MOV BX,WORD PTR Button
    INT $33
    MOV MouseData.ButtonStatus,AX
    MOV MouseData.ButtonCount,BX
    MOV MouseData.Location.X,CX
    MOV MouseData.Location.Y,DX
    MOV AX,OFFSET MouseData
    MOV DX,SEG MouseData
end;

function GetMouseMotion:PMouseCoord; assembler;
asm
    MOV AX,$000B
    INT $33
    MOV MouseCoord.X,CX
    MOV MouseCoord.Y,DX
    MOV AX,OFFSET MouseCoord
    MOV DX,SEG MouseCoord
end;

function GetMouseStateSize:Word; assembler;
asm
    MOV AX,$0015
    INT $33
    MOV AX,BX
end;

function GetMouseVideoPage:Byte; assembler;
asm
    MOV AX,$001E
    INT $33
    MOV AL,BL
end;

function SwapMouseSubRoutine(RoutineAddr:Pointer;CallMask:TCallMask):PMouseSubRoutine; assembler;
asm
    MOV AX,$0014
    MOV CX,CallMask
    LES DX,RoutineAddr
    INT $33
    MOV MouseSubRoutine.CallMask,CX
    MOV WORD PTR MouseSubRoutine.RoutineAddr[0],DX
    MOV WORD PTR MouseSubRoutine.RoutineAddr[2],ES
    MOV AX,OFFSET MouseSubRoutine
    MOV DX,SEG MouseSubRoutine
end;

function CheckMouseAction:Boolean;
var
    CurrentMouseStatus:TMouseStatus;
begin
    CurrentMouseStatus:=GetMouseStatus^;
    if (CurrentMouseStatus.Location.X<>PrevMouseStatus.Location.X)
        or (CurrentMouseStatus.Location.Y<>PrevMouseStatus.Location.Y)
        or (CurrentMouseStatus.ButtonStatus<>PrevMouseStatus.ButtonStatus) then
    begin
        CheckMouseAction:=True;
        PrevMouseStatus:=CurrentMouseStatus;
    end else CheckMouseAction:=False;
end;

{ --- User procedures ----------------------------------------------------- }

procedure ShowMouseCursor; assembler;
asm
    MOV AX,$0001
    INT $33
end;

procedure HideMouseCursor; assembler;
asm
    MOV AX,$0002
    INT $33
end;

procedure SetMouseLocation(X,Y:Word); assembler;
asm
    MOV AX,$0004
    MOV CX,X
    MOV DX,Y
    INT $33
end;

procedure SetMouseWindow(X1,Y1,X2,Y2:Word); assembler;
asm
    MOV AX,$0007
    MOV CX,X1
    MOV DX,X2
    INT $33
    MOV AX,$0008
    MOV CX,Y1
    MOV DX,Y2
    INT $33
end;

procedure SetGraphicsCursor(var Bitmap;HotX,HotY:Integer); assembler;
asm
    MOV AX,$0009
    MOV BX,HotX
    MOV CX,HotY
    LES DX,Bitmap
    INT $33
end;

procedure SetTextCursor(ScreenMask,CursorMask:Word); assembler;
asm
    MOV AX,$000A
    MOV BX,$0000
    MOV CX,ScreenMask
    MOV DX,CursorMask
    INT $33
end;

procedure EnableMousePen; assembler;
asm
    MOV AX,$000D
    INT $33
end;

procedure DisableMousePen; assembler;
asm
    MOV AX,$000E
    INT $33
end;

procedure SetMouseRatio(X,Y:Word); assembler;
asm
    MOV AX,$000F
    MOV CX,X
    MOV DX,Y
    INT $33
end;

procedure SetMouseUpdateRegion(X1,Y1,X2,Y2:Word); assembler;
asm
    MOV AX,$0010
    MOV CX,X1
    MOV DX,Y1
    MOV SI,X2
    MOV DI,Y2
    INT $33
end;

procedure SetMouseVideoPage(Page:Byte); assembler;
asm
    MOV AX,$001D
    MOV BL,Page
    MOV BH,0
    INT $33
end;

procedure SaveMouseState(var Buf); assembler;
asm
    CALL GetMouseStateSize
    MOV BX,AX
    MOV AX,$0016
    LES DX,Buf
    INT $33
end;

procedure RestoreMouseState(var Buf); assembler;
asm
    CALL GetMouseStateSize
    MOV BX,AX
    MOV AX,$0017
    LES DX,Buf
    INT $33
end;

procedure SetMouseSubRoutine(RoutineAddr:Pointer;CallMask:TCallMask); assembler;
asm
    MOV AX,$000C
    MOV CX,CallMask
    LES DX,RoutineAddr
    INT $33
end;

{ --- Startup routine ----------------------------------------------------- }

begin
    PrevMouseStatus:=GetMouseStatus^;
end.
