unit TextDisplay;

{ Text/Font display unit }

interface

uses SpriteManager,Keyboard,Mouse;

const
    DataSize=256;

type
{ Pointer to a string }
    PString=^String;

{ All text elements are 8x8 pixels }
    TTextCoord=record
        X,Y:Integer;
    end;

{ Abstract text object, has a location, size and string length. }
    PTextBar=^TTextBar;
    TTextBar=object
        Data:array[0..DataSize-1] of Char;
        Attr:array[0..DataSize-1] of Byte;
        WinPos,WinSize:TTextCoord;
        constructor Init(PosX,PosY,SizeX,SizeY:Integer);
        procedure Resize(PosX,PosY,SizeX,SizeY:Integer);
        procedure AssignData(Start:Word;S:String);
        procedure AssignChar(Start,FillLength:Word;NewChar:Char);
        procedure AssignAttr(Start,FillLength:Word;NewAttr:Byte);
        procedure Fill(Start,FillLength:Word;NewAttr:Byte;NewChar:Char);
        procedure Render;
        procedure Erase;
    end;

{ Status bar at bottom of screen }
    PStatusBar=^TStatusBar;
    TStatusBar=object(TTextBar)
        public
            constructor Init(PosX,PosY,SizeX,SizeY:Integer);
            procedure Resize(PosX,PosY,SizeX,SizeY:Integer);
            procedure Render(WorkSpace,ShapeThing:Word;X,Y:Integer;Highlight:Boolean);
            procedure Update(WorkSpace,ShapeThing:Word;X,Y:Integer;Highlight:Boolean);
            function Click(X,Y:Integer):Byte;
        private
            PrevWorkSpace,PrevShapeThing:Word;
            PrevX,PrevY:Integer;
            PrevHighlight:Boolean;
    end;

{ Help bar at bottom of screen }
    PHelpBar=^THelpBar;
    THelpBar=object(TTextBar)
        public
            constructor Init(PosX,PosY,SizeX,SizeY:Integer);
            procedure SetMessage(const S:String);
            procedure Update;
        private
            Message,PrevMessage:PString;
    end;

{ Level bar which indicates what level/file you're editing }
    PLevelBar=^TLevelBar;
    TLevelBar=object(TTextBar)
        procedure SetLevel(Episode,Level:Word);
        procedure SetFileName(S:String);
    end;

{ Type of menu option: Title (bold but not selectable), }
{   Option (selectable), Blank (not selectable), Shaded (greyed-out option) }
    TMenuOptionType=(mtTitle,mtOption,mtBlank,mtShaded);

{ Menu option record for TMenu object }
    TMenuOption=record
        Name:String[15];
        OptionType:TMenuOptionType;
        Code:Byte;
    end;

{ Menu object lists }
    PMenuOptionList=^TMenuOptionList;
    TMenuOptionList=array[0..255] of TMenuOption;

{ Menu object }
    PMenu=^TMenu;
    TMenu=object
        public
            constructor Init(PosX,PosY,SizeX,SizeY:Integer;
                NewList:PMenuOptionList;NewListLength:Byte);
            procedure Resize(PosX,PosY,SizeX,SizeY:Integer);
            procedure Render;
            procedure Update;
            procedure Erase;
            procedure SetSelect(NewSelect:Byte);
            procedure ShadeToggle(Option:Byte);
            function Hover(X,Y:Integer):Byte;
            function Click(X,Y:Integer):Byte;
        private
            WinPos,WinSize:TTextCoord;
            List:PMenuOptionList;
            ListLength:Byte;
            Highlight,Select:Byte;
            PrevHighlight,PrevSelect:Byte;
            procedure RenderOption(Option:Byte);
    end;

{ Responses from TInputPrompt.GetYesNoCancel }
    TInputPromptResponse=(irYes,irNo,irCancel);

{ Input dialog prompt object }
    PInputPrompt=^TInputPrompt;
    TInputPrompt=object(TTextBar)
        public
            function GetFileName(Prompt:String):String;
            function GetLevel(Prompt:String):Word;
            function GetYesNoCancel(Prompt:String):TInputPromptResponse;
        private
            function GetChar(Offset:Integer):Char;
    end;

{ Scroll button object }
    PButton=^TButton;
    TButton=object
        public
            constructor Init(PosX,PosY:Integer;C:Char);
            procedure Resize(PosX,PosY:Integer);
            procedure Render;
            procedure Erase;
            function Click(X,Y:Integer):Boolean;
        private
            WinPos:TTextCoord;
            Data:Char;
    end;

implementation

const
    HexChar:array[0..15] of Char='0123456789ABCDEF';
    WorkSpaceChar:array[0..12] of Char='C1234567890AB';

    clStatusBarText=$1F;
    clStatusBarNumber=$1B;
    clStatusBarWorkSpaceInactive=$17;
    clStatusBarWorkSpaceActive=$1F;
    clHelpBarText=$0A;
    clLevelBarText=$1F;
    clLevelBarNumber=$1B;
    clMenuTitle=$09;
    clMenuBlank=$08;
    clMenuOptionInactive=$0F;
    clMenuOptionActive=$0A;
    clMenuOptionHighlightInactive=$1F;
    clMenuOptionHighlightActive=$1A;
    clInputPrompt=$1F;
    clInputText=$1B;
    clButton=$07;

{ --- Useful internal routines -------------------------------------------- }

function Word2Hex(N:Word):String;
var
    I:Byte;
begin
    Word2Hex[0]:=#4;
    for I:=0 to 3 do Word2Hex[4-I]:=HexChar[(N shr (I*4)) and $000F];
end;

{ --- TTextBar methods ---------------------------------------------------- }

constructor TTextBar.Init(PosX,PosY,SizeX,SizeY:Integer);
var
    I:Word;
begin
    WinPos.X:=PosX;
    WinPos.Y:=PosY;
    WinSize.X:=SizeX;
    WinSize.Y:=SizeY;
    Fill(0,DataSize,0,#0);
end;

procedure TTextBar.Resize(PosX,PosY,SizeX,SizeY:Integer);
begin
    WinPos.X:=PosX;
    WinPos.Y:=PosY;
    WinSize.X:=SizeX;
    WinSize.Y:=SizeY;
end;

procedure TTextBar.AssignData(Start:Word;S:String);
var
    I:Word;
begin
    if Length(S)>0 then for I:=0 to Length(S)-1 do Data[Start+I]:=S[I+1];
end;

procedure TTextBar.AssignChar(Start,FillLength:Word;NewChar:Char);
var
    I:Word;
begin
    for I:=0 to FillLength-1 do Data[Start+I]:=NewChar;
end;

procedure TTextBar.AssignAttr(Start,FillLength:Word;NewAttr:Byte);
var
    I:Word;
begin
    for I:=0 to FillLength-1 do Attr[Start+I]:=NewAttr;
end;

procedure TTextBar.Fill(Start,FillLength:Word;NewAttr:Byte;NewChar:Char);
var
    I:Word;
begin
    for I:=0 to FillLength-1 do
    begin
        Attr[Start+I]:=NewAttr;
        Data[Start+I]:=NewChar;
    end;
end;

procedure TTextBar.Render;
var
    Screen:TTextCoord;
    I:Word;
begin
    for I:=0 to WinSize.Y*WinSize.X-1 do
    begin
        Screen.X:=WinPos.X+I mod WinSize.X;
        Screen.Y:=WinPos.Y+I div WinSize.X;
        PrintChar(Screen.X,Screen.Y,Attr[I],Data[I]);
    end;
end;

procedure TTextBar.Erase;
var
    Screen:TTextCoord;
    I:Word;
begin
    for I:=0 to WinSize.Y*WinSize.X-1 do
    begin
        Screen.X:=WinPos.X+I mod WinSize.X;
        Screen.Y:=WinPos.Y+I div WinSize.X;
        PrintChar(Screen.X,Screen.Y,0,#0);
    end;
end;

{ --- TStatusBar methods -------------------------------------------------- }

constructor TStatusBar.Init(PosX,PosY,SizeX,SizeY:Integer);
begin
    inherited Init(PosX,PosY,SizeX,SizeY);
    Resize(PosX,PosY,SizeX,SizeY);
    PrevWorkSpace:=1;
    PrevHighlight:=False;
end;

procedure TStatusBar.Resize(PosX,PosY,SizeX,SizeY:Integer);
begin
    inherited Resize(PosX,PosY,SizeX,SizeY);
    Fill(0,DataSize,clStatusBarText,#0);
    AssignData(0,'Workspace');
    AssignData(12,'C1234567890AB');
    AssignData(WinSize.X-14,'S');
    AssignData(WinSize.X-8,'X');
    AssignData(WinSize.X-3,'Y');
    AssignAttr(10,1,clStatusBarNumber);
    AssignAttr(WinSize.X-13,4,clStatusBarNumber);
    AssignAttr(WinSize.X-7,3,clStatusBarNumber);
    AssignAttr(WinSize.X-2,2,clStatusBarNumber);
end;

procedure TStatusBar.Render(WorkSpace,ShapeThing:Word;X,Y:Integer;Highlight:Boolean);
var
    ShapeHex,XStr,YStr:String[4];
begin
    AssignData(10,WorkSpaceChar[WorkSpace]);
    AssignAttr(12,13,clStatusBarWorkSpaceInactive);
    AssignAttr(12+WorkSpace,1,clStatusBarWorkSpaceActive);
    if Highlight then
    begin
        ShapeHex:=Word2Hex(ShapeThing);
        Str(X:3,XStr);
        Str(Y:2,YStr);
    end else begin
        ShapeHex:='    ';
        XStr:='   ';
        YStr:='  ';
    end;
    AssignData(WinSize.X-13,ShapeHex);
    AssignData(WinSize.X-7,XStr);
    AssignData(WinSize.X-2,YStr);
    inherited Render;
    PrevWorkSpace:=WorkSpace;
    PrevShapeThing:=ShapeThing;
    PrevX:=X;
    PrevY:=Y;
    PrevHighlight:=Highlight;
end;

procedure TStatusBar.Update(WorkSpace,ShapeThing:Word;X,Y:Integer;Highlight:Boolean);
begin
    if (PrevWorkSpace<>WorkSpace) or (PrevShapeThing<>ShapeThing)
        or (PrevX<>X) or (PrevY<>Y) or (PrevHighlight<>Highlight) then
        Render(WorkSpace,ShapeThing,X,Y,Highlight);
end;

function TStatusBar.Click(X,Y:Integer):Byte;
begin
    X:=X div 8;
    Y:=Y div 8;
    if (X>=WinPos.X+12) and (X<=WinPos.X+24) and (Y=WinPos.Y) then
        Click:=X-WinPos.X-12 else Click:=$FF;
end;

{ --- THelpBar methods ---------------------------------------------------- }

constructor THelpBar.Init(PosX,PosY,SizeX,SizeY:Integer);
begin
    inherited Init(PosX,PosY,SizeX,SizeY);
    Message:=nil;
    PrevMessage:=nil;
end;

procedure THelpBar.SetMessage(const S:String);
begin
    Fill(0,DataSize,clHelpBarText,#0);
    AssignData(0,S);
    Message:=@S;
end;

procedure THelpBar.Update;
begin
    if Message<>PrevMessage then
    begin
        Render;
        PrevMessage:=Message;
    end;
end;

{ --- TLevelBar methods --------------------------------------------------- }

procedure TLevelBar.SetLevel(Episode,Level:Word);
var
    EpisodeStr,LevelStr:String[4];
    I:Word;
begin
    Fill(0,DataSize,clLevelBarText,#0);
    Str(Episode:1,EpisodeStr);
    Str(Level:2,LevelStr);
    AssignAttr(8,1,clLevelBarNumber);
    AssignAttr(16,2,clLevelBarNumber);
    AssignData(0,'Episode');
    AssignData(10,'Level');
    AssignData(8,EpisodeStr);
    AssignData(16,LevelStr);
end;

procedure TLevelBar.SetFileName(S:String);
var
    I:Word;
begin
    Fill(0,DataSize,clLevelBarText,#0);
    AssignAttr(6,155,clLevelBarNumber);
    AssignData(0,'File');
    AssignData(6,S);
end;

{ --- TMenu methods ------------------------------------------------------- }

constructor TMenu.Init(PosX,PosY,SizeX,SizeY:Integer;
    NewList:PMenuOptionList;NewListLength:Byte);
begin
    WinPos.X:=PosX;
    WinPos.Y:=PosY;
    WinSize.X:=SizeX;
    WinSize.Y:=SizeY;
    List:=NewList;
    ListLength:=NewListLength;
    Highlight:=$FF;
    Select:=$FF;
    PrevHighlight:=$FF;
    PrevSelect:=$FF;
end;

procedure TMenu.Resize(PosX,PosY,SizeX,SizeY:Integer);
begin
    WinPos.X:=PosX;
    WinPos.Y:=PosY;
    WinSize.X:=SizeX;
    WinSize.Y:=SizeY;
end;

procedure TMenu.Render;
var
    I:Byte;
begin
    for I:=0 to ListLength-1 do RenderOption(I);
    PrevHighlight:=Highlight;
    PrevSelect:=Select;
end;

procedure TMenu.Update;
begin
    if PrevHighlight<>Highlight then
    begin
        RenderOption(PrevHighlight);
        RenderOption(Highlight);
    end;
    if PrevSelect<>Select then
    begin
        RenderOption(PrevSelect);
        RenderOption(Select);
    end;
    PrevHighlight:=Highlight;
    PrevSelect:=Select;
end;

procedure TMenu.SetSelect(NewSelect:Byte);
begin
    Select:=NewSelect;
end;

procedure TMenu.Erase;
var
    Screen:TTextCoord;
begin
    for Screen.Y:=0 to WinSize.Y-1 do
        for Screen.X:=0 to WinSize.X-1 do
            PrintChar(Screen.X+WinPos.X,Screen.Y+WinPos.Y,0,#0);
end;

procedure TMenu.ShadeToggle(Option:Byte);
begin
    case List^[Option].OptionType of
        mtOption: List^[Option].OptionType:=mtShaded;
        mtShaded: List^[Option].OptionType:=mtOption;
    end;
end;

function TMenu.Hover(X,Y:Integer):Byte;
var
    NewHighlight:Byte;
begin
    X:=X div 8;
    Y:=Y div 8;
    if (X>=WinPos.X) and (X<WinPos.X+WinSize.X)
        and (Y>=WinPos.Y) and (Y<WinPos.Y+ListLength) then
    begin
        NewHighlight:=Y-WinPos.Y;
        if List^[NewHighlight].OptionType=mtOption then
        begin
            Hover:=List^[NewHighlight].Code;
            Highlight:=NewHighlight;
        end else begin
            Hover:=$FF;
            Highlight:=$FF;
        end;
    end else begin
        Hover:=$FF;
        Highlight:=$FF;
    end;
end;

function TMenu.Click(X,Y:Integer):Byte;
var
    NewSelect:Word;
begin
    X:=X div 8;
    Y:=Y div 8;
    if (X>=WinPos.X) and (X<WinPos.X+WinSize.X)
        and (Y>=WinPos.Y) and (Y<WinPos.Y+ListLength) then
    begin
        NewSelect:=Y-WinPos.Y;
        if List^[NewSelect].OptionType=mtOption then
        begin
            Click:=List^[NewSelect].Code;
            Select:=NewSelect;
        end else Click:=$FF;
    end else Click:=$FF;
end;

procedure TMenu.RenderOption(Option:Byte);
var
    Attr:Byte;
    TextBar:TTextBar;
begin
    if Option<>$FF then
    begin
        TextBar.Init(WinPos.X,WinPos.Y+Option,WinSize.X,1);
        TextBar.AssignChar(0,DataSize,#0);
        TextBar.AssignData(0,List^[Option].Name);
        case List^[Option].OptionType of
            mtTitle: TextBar.AssignAttr(0,DataSize,clMenuTitle);
            mtBlank,mtShaded: TextBar.AssignAttr(0,DataSize,clMenuBlank);
            mtOption: begin
                if Highlight=Option then
                begin
                    if Select=Option then Attr:=clMenuOptionHighlightActive else Attr:=clMenuOptionHighlightInactive;
                end else begin
                    if Select=Option then Attr:=clMenuOptionActive else Attr:=clMenuOptionInactive;
                end;
                TextBar.AssignAttr(0,DataSize,Attr);
            end;
        end;
        TextBar.Render;
    end;
end;

{ --- TInputPrompt methods ------------------------------------------------ }

function TInputPrompt.GetFileName(Prompt:String):String;
var
    InputLength,InputOffset,I:Byte;
    S:String;
    Key:Char;
begin
    HideMouseCursor;
    InputLength:=0;
    InputOffset:=Length(Prompt);
    AssignAttr(0,InputOffset,clInputPrompt);
    AssignAttr(InputOffset,DataSize-InputOffset,clInputText);
    AssignChar(InputOffset,DataSize-InputOffset,#0);
    AssignData(0,Prompt);

    repeat
        Render;
        Key:=GetChar(InputOffset+InputLength);
        case Key of
            #13: Break;
            #8: begin
                if InputLength>0 then
                begin
                    Data[InputOffset+InputLength-1]:=#0;
                    Dec(InputLength);
                end;
            end;
            #27: begin
                InputLength:=0;
                Break;
            end;
            else begin
                if InputLength<WinSize.X-InputOffset then
                begin
                    Data[InputOffset+InputLength]:=Key;
                    Inc(InputLength);
                end;
            end;
        end;
    until False;

    S:='';
    for I:=1 to InputLength do S:=S+Data[InputOffset+I-1];
    GetFileName:=S;
    ShowMouseCursor;
end;

function TInputPrompt.GetLevel(Prompt:String):Word;
var
    Key:Char;
    InputLength,InputOffset,DisplayOffset,I:Byte;
    LevelStr:String[2];
    Episode,Level:Byte;
    Code:Integer;
begin
    HideMouseCursor;
    InputLength:=0;
    InputOffset:=Length(Prompt);
    Fill(0,DataSize,clInputPrompt,#0);
    AssignAttr(InputOffset+8,1,clInputText);
    AssignAttr(InputOffset+16,2,clInputText);
    AssignData(0,Prompt);
    AssignData(InputOffset,'Episode');
    AssignData(InputOffset+10,'Level');

    repeat
        repeat
            Render;
            case InputLength of
                0: DisplayOffset:=8;
                1..3: DisplayOffset:=15+InputLength;
            end;
            Key:=GetChar(InputOffset+DisplayOffset);
            case Key of
                #13: Break;
                #8: begin
                    if InputLength in [1..3] then
                    begin
                        case InputLength of
                            1: Data[InputOffset+8]:=#0;
                            2..3: Data[InputOffset+14+InputLength]:=#0;
                        end;
                        Dec(InputLength);
                    end;
                end;
                #27: begin
                    InputLength:=0;
                    Break;
                end;
                '0'..'9': begin
                    if InputLength<3 then
                    begin
                        Data[InputOffset+DisplayOffset]:=Key;
                        Inc(InputLength);
                    end;
                end;
            end;
        until False;
        if InputLength>=2 then
        begin
            if Data[InputOffset+8] in ['1'..'3'] then
            begin
                Episode:=Ord(Data[InputOffset+8])-48;
                LevelStr:='';
                for I:=0 to InputLength-2 do LevelStr:=LevelStr+Data[InputOffset+16+I];
                Val(LevelStr,Level,Code);
                if (Code=0) and (Level in [1..12]) then
                begin
                    GetLevel:=Episode shl 8+Level;
                    Break;
                end;
            end;
        end else if InputLength=0 then begin
            GetLevel:=0;
            Break;
        end;
    until False;
    ShowMouseCursor;
end;

function TInputPrompt.GetYesNoCancel(Prompt:String):TInputPromptResponse;
var
    InputOffset:Byte;
    Key:Char;
begin
    HideMouseCursor;
    InputOffset:=Length(Prompt);
    Fill(0,DataSize,clInputPrompt,#0);
    AssignData(0,Prompt);

    Render;
    repeat
        Key:=GetChar(InputOffset);
        case Key of
            'Y','y': begin
                GetYesNoCancel:=irYes;
                Break;
            end;
            'N','n': begin
                GetYesNoCancel:=irNo;
                Break;
            end;
            #27,'C','c': begin
                GetYesNoCancel:=irCancel;
                Break;
            end;
        end;
    until False;
    ShowMouseCursor;
end;

function TInputPrompt.GetChar(Offset:Integer):Char;
var
    Screen:TTextCoord;
    Key:Word;
begin
    Screen.X:=Offset mod WinSize.X+WinPos.X;
    Screen.Y:=Offset div WinSize.X+WinPos.Y;
    repeat
        RunAnimPulse;
        PrintChar(Screen.X,Screen.Y,$1F,Chr(128+AnimPulse[4]));
        Key:=GetKey;
    until (Lo(Key)<>0) and (Lo(Key)<>$E0);
    GetChar:=Chr(Lo(Key));
end;

{ --- TButton methods ----------------------------------------------------- }

constructor TButton.Init(PosX,PosY:Integer;C:Char);
begin
    WinPos.X:=PosX;
    WinPos.Y:=PosY;
    Data:=C;
end;

procedure TButton.Resize(PosX,PosY:Integer);
begin
    WinPos.X:=PosX;
    WinPos.Y:=PosY;
end;

procedure TButton.Render;
begin
    PrintChar(WinPos.X,WinPos.Y,clButton,Data);
end;

procedure TButton.Erase;
begin
    PrintChar(WinPos.X,WinPos.Y,$00,#0);
end;

function TButton.Click(X,Y:Integer):Boolean;
begin
    if (WinPos.X=X div 8) and (WinPos.Y=Y div 8) then Click:=True
        else Click:=False;
end;

end.
