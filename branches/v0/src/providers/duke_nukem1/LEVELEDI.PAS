unit LevelEditor;

{ Level editing interface for DN1 editor }

{$DEFINE PersistentCopy}
{ $DEFINE PrintMemUsage}

interface

uses LevelWorkSpace,LevelRenderer,LevelManager,TextDisplay,
    Config,GSprite,GCore,Mouse,Keyboard,LevelMarker,SpriteManager;

{$I LEVELEDI.INC}

type
    TEditMenuMode=(emToolBar,emMenu,emBoth);

    PEditor=^TEditor;
    TEditor=object
        public
            constructor Init;
            procedure Run;
            destructor Done;
        private
        { ScreenMode: Array of available screen modes for GCore }
        { ScreenModeCount: Number of available modes }
        { CurrentScreenMode: Currenty selected screen mode }
            ScreenMode:array[1..255] of Word;
            ScreenModeCount,CurrentScreenMode:Word;
            ScreenSize:TGraphCoord;

        { Various graphical interface objects/variables }
            LevelRenderer:PLevelRenderer;
            ToolBar:PToolBar;
            StatusBar:PStatusBar;
            HelpBar:PHelpBar;
            LevelBar:PLevelBar;
            InputPrompt:PInputPrompt;
            EditMenu:PMenu;
            Button:array[0..7] of PButton;
            EditMenuMode:TEditMenuMode;
            MouseXDiv2:Boolean;
            MessageTimer:Byte;
            RendererFullScreen:Boolean;
            RendererNormalSize:TLevelCoord;

        { Variables used in event processing }
            Highlight,PrevRenClick:THighlight;
            HighlightSprite:Word;
            MouseStatus:TMouseStatus;
            ShiftKeys,NewWorkSpace,SourceWorkSpace:Byte;
            NewShapeThing,CurrentShapeThing:TShapeThing;
            EditorEvent,DragMode:TEditorEvent;
            DragWindow:array[0..1] of TLevelCoord;
            PrevToolClick:Boolean;

        { These flags indicate if specific screen elements need to be redrawn }
            RedrawRenderer,RedrawToolBar,RedrawStatusBar,RedrawHelpBar,
                RedrawLevelBar,RedrawMenu,RedrawButtons,RebuildRenderer:Boolean;

        { WorkSpace: Array of workspaces (0=Clipboard) }
        { CurrentWorkSpace: Current workspace }
        { Config: Configuration file data }
            WorkSpace:array[0..12] of PWorkSpace;
            CurrentWorkSpace:Word;
            Config:PConfig;

        { ScreenModeInit: Initialises list of available screen modes }
        { ScreenModeAdd: Adds a screen mode to the array }
        { ScreenModeSet: Sets a screen mode }
            procedure ScreenModeInit;
            procedure ScreenModeAdd(Mode:Word);
            procedure ScreenModeSet(NewScreenMode:Word);

        { ScreenUpdate: Updates required screen components }
        { ProcessMouse: Processes mouse input }
        { ProcessKeyboard: Processes keyboard input }
            procedure ScreenUpdate;
            procedure ProcessMouse;
            procedure ProcessKeyboard;
            procedure ProcessEvent;
            procedure IdleProc;
    end;

implementation

{ --- TEditor methods ----------------------------------------------------- }

constructor TEditor.Init;
var
    I:Word;
begin
{ This is a fix for the problem of not being able to open read-only files }
    FileMode:=0;

    if not GetMouseInfo^.Installed then
    begin
        Writeln('Error: No mouse was detected.');
        Writeln('Check to see if your mouse driver is loaded.');
        Fail;
    end;

    New(Config,Init);
    for I:=1 to 3 do
    begin
        LevelManager.Path[I]:=Config^.Path[I];
        SpriteFile.Path[I]:=Config^.Path[I];
    end;

    DrawFlasher:=Config^.AnimFlash;
    DrawActive:=Config^.AnimActive;
    DrawBridge:=Config^.DrawBridge;
    DrawConveyor:=Config^.DrawConveyor;
    DrawMulti:=Config^.DrawMulti;

    SpriteManagerInit;
    if not SpriteManagerOk then Fail;

    for I:=0 to 12 do
    begin
        New(WorkSpace[I],Init);
        WorkSpace[I]^.ViewPos.X:=0;
        WorkSpace[I]^.ViewPos.Y:=0;
    end;
    CurrentWorkSpace:=1;

    New(LevelRenderer,Init(13,10));
    New(ToolBar,Init(16,0,4,10,4,192));
    New(StatusBar,Init(0,24,40,1));
    New(HelpBar,Init(0,21,40,2));
    New(LevelBar,Init(0,23,40,1));
    New(InputPrompt,Init(0,24,40,1));
    New(EditMenu,Init(28,0,13,15,@EditCommandMenu,16));
    New(Button[btLevelLeft],Init(0,20,#6));
    New(Button[btLevelRight],Init(25,20,#5));
    New(Button[btLevelUp],Init(26,0,#3));
    New(Button[btLevelDown],Init(26,19,#4));
    New(Button[btToolBarLeft],Init(31,2,#6));
    New(Button[btToolBarUp],Init(31,3,#3));
    New(Button[btToolBarDown],Init(31,18,#4));
    New(Button[btToolBarRight],Init(31,19,#5));

    ToolBar^.SetListName(0,'Flasher');
    ToolBar^.SetListName(1,'Backgrnd');
    ToolBar^.SetListName(2,'Solid');
    ToolBar^.SetListName(3,'Active');
    ToolBar^.AddIcon(0,$0000);
    ToolBar^.AddIcon(0,$0020);
    ToolBar^.AddIcon(0,$0080);
    ToolBar^.AddIcon(0,$0100);
    ToolBar^.AddIcon(0,$0180);
    ToolBar^.AddIcon(0,$0200);
    ToolBar^.AddIcon(0,$0280);
    ToolBar^.AddIcon(0,$0300);
    ToolBar^.AddIcon(0,$0380);
    ToolBar^.AddIcon(0,$0400);
    ToolBar^.AddIcon(0,$0420);
    ToolBar^.AddIcon(0,$0440);
    ToolBar^.AddIcon(0,$0460);
    ToolBar^.AddIcon(0,$0500);
    ToolBar^.AddIcon(0,$0580);
    for I:=0 to 143 do ToolBar^.AddIcon(1,$0600+I*32);
    for I:=0 to 191 do ToolBar^.AddIcon(2,$1800+I*32);
    for I:=0 to 89 do ToolBar^.AddIcon(3,$3000+I);

    LevelRenderer^.Assign(WorkSpace[CurrentWorkSpace]);
    LevelRenderer^.Build;
    HelpBar^.SetMessage(hmDefault);
    Highlight.Enabled:=False;
    PrevRenClick.Enabled:=False;
    PrevToolClick:=False;
    HighlightSprite:=siHighlight;

    ScreenModeInit;
    ScreenModeSet(CurrentScreenMode);
    ScreenUpdate;
    SetGMouseCursor(@smMouseCursor);

    EditorEvent:=eeNone;
    DragMode:=eeNone;
    CurrentShapeThing:=$0000;
    MessageTimer:=0;
end;

{ REMEMBER: Mouse cursor must be hidden whenever screen is written to }

procedure TEditor.ScreenModeInit;
var
    I:Word;
begin
    ScreenModeCount:=0;
    for I:=1 to GraphModeDescCount do
        if (GraphModeDesc[I].GraphModeType=gtPlane4)
            and (GraphModeDesc[I].Size.Y*2>=GraphModeDesc[I].Size.X) then
                ScreenModeAdd(I);
    CurrentScreenMode:=1;
    for I:=1 to ScreenModeCount do
        if (GraphModeDesc[ScreenMode[I]].Size.X=Config^.ScreenSize.X)
            and (GraphModeDesc[ScreenMode[I]].Size.Y=Config^.ScreenSize.Y) then
            CurrentScreenMode:=I;
end;

procedure TEditor.ScreenModeAdd(Mode:Word);
var
    I:Word;
    ModeFound:Boolean;
    function ScreenModeRank(Mode:Word):Longint;
    begin
        ScreenModeRank:=GraphModeDesc[Mode].Size.X*2048+GraphModeDesc[Mode].Size.Y;
    end;
begin
    ModeFound:=False;
    for I:=1 to ScreenModeCount do
        if ScreenModeRank(Mode)=ScreenModeRank(ScreenMode[I]) then
            ModeFound:=True;
    I:=ScreenModeCount;
    if not ModeFound then
    begin
        while I>0 do
        begin
            if ScreenModeRank(Mode)>ScreenModeRank(ScreenMode[I]) then Break
                else ScreenMode[I+1]:=ScreenMode[I];
            Dec(I);
        end;
        ScreenMode[I+1]:=Mode;
        Inc(ScreenModeCount);
    end;
end;

{ --- Rules for placing interface objects --- }

{ Level renderer needs to be a multiple of 13x10, though dimensions }
{   need to be even if zooming in should be attempted. Level renderer }
{   must leave at least 112 pixel columns for menu to be toggled and }
{   224 pixel columns for a seperate menu display. Also, at least 32 }
{   pixel rows must be left to make room for status bars. }

{ Toolbar: If level renderer left more than 224 pixel columns, then place }
{   toolbar so it's starting at renderer.X+menu.X+16 X pixels. The height }
{   of the toolbar extends from the top to 32 pixels short off the bottom }

{ Status bar should go on lowest screen row, and needs 40 8x8 characters }
{ Level bar should go on 2nd lowest screen row, and needs 40 8x8 characters }
{ Help bar should go on 3rd lowest screen row if it can be 80 characters }
{   long, otherwise needs to start from 4th lowest row and wrap 2x40 columns }
{ Scroll buttons should be placed appropiately relative to the level renderer }
{   and toolbar. }
{ Menu: If the renderer left a gap of at least 192 pixel columns, then }
{   place the menu 8 pixels after the end of the renderer horizontally. }
{   Otherwise place 104 pixels from the right of the screen. }

procedure TEditor.ScreenModeSet(NewScreenMode:Word);
var
    ScreenSize8,ScreenSize16,RenderWinSize,ToolBarSize,MenuSize,
        MenuPos:TLevelCoord;
begin
    SetGraphMode(ScreenMode[NewScreenMode]);
    CurrentScreenMode:=NewScreenMode;
    ScreenSize:=GraphModeDesc[GraphModeCurrent].Size;
    MouseXDiv2:=ScreenSize.X=320;
    if MouseXDiv2 then SetMouseWindow(0,0,ScreenSize.X*2-1,ScreenSize.Y-1)
        else SetMouseWindow(0,0,ScreenSize.X-1,ScreenSize.Y-1);

    ScreenSize.Y:=(ScreenSize.Y+6) and $FFF8;
    ScreenSize8.X:=ScreenSize.X div 8;
    ScreenSize8.Y:=ScreenSize.Y div 8;
    ScreenSize16.X:=ScreenSize.X div 16;
    ScreenSize16.Y:=ScreenSize.Y div 16;

    RenderWinSize.X:=((ScreenSize.X-112) div 208)*13;
    RenderWinSize.Y:=((ScreenSize.Y-32) div 160)*10;
    if RenderWinSize.X>13 then RenderWinSize.X:=RenderWinSize.X and $FFFE;
    RendererNormalSize:=RenderWinSize;
    RendererFullScreen:=False;

    if ScreenSize16.X-RenderWinSize.X>=12 then
    begin
        ToolBarSize.X:=ScreenSize16.X-RenderWinSize.X-8;
        ToolBarSize.Y:=ScreenSize16.Y-2;
        MenuSize.X:=14;
        MenuSize.Y:=(ScreenSize16.Y-2)*2;
        MenuPos.X:=RenderWinSize.X*2+1;
        MenuPos.Y:=0;
        EditMenuMode:=emBoth;
    end else begin
        ToolBarSize.X:=ScreenSize16.X-RenderWinSize.X-1;
        ToolBarSize.Y:=ScreenSize16.Y-2;
        MenuSize.X:=13;
        MenuSize.Y:=(ScreenSize16.Y-2)*2;
        MenuPos.X:=RenderWinSize.X*2+1;
        MenuPos.Y:=0;
        EditMenuMode:=emToolBar;
    end;

    LevelRenderer^.Resize(RenderWinSize.X,RenderWinSize.Y);
    ToolBar^.Resize(ScreenSize16.X-ToolBarSize.X,0,ToolBarSize.X,ToolBarSize.Y);
    EditMenu^.Resize(MenuPos.X,MenuPos.Y,MenuSize.X,MenuSize.Y);
    StatusBar^.Resize(0,ScreenSize8.Y-1,ScreenSize8.X,1);
    InputPrompt^.Resize(0,ScreenSize8.Y-1,ScreenSize8.X,1);
    LevelBar^.Resize(0,ScreenSize8.Y-2,ScreenSize8.X,1);
    if ScreenSize8.X<80 then HelpBar^.Resize(0,ScreenSize8.Y-4,ScreenSize8.X,2)
        else HelpBar^.Resize(0,ScreenSize8.Y-3,ScreenSize8.X,1);
    Button[btLevelLeft]^.Resize(0,RenderWinSize.Y*2);
    Button[btLevelRight]^.Resize(RenderWinSize.X*2-1,RenderWinSize.Y*2);
    Button[btLevelUp]^.Resize(RenderWinSize.X*2,0);
    Button[btLevelDown]^.Resize(RenderWinSize.X*2,RenderWinSize.Y*2-1);
    Button[btToolBarLeft]^.Resize(ScreenSize8.X-ToolBarSize.X*2-1,2);
    Button[btToolBarUp]^.Resize(ScreenSize8.X-ToolBarSize.X*2-1,3);
    Button[btToolBarDown]^.Resize(ScreenSize8.X-ToolBarSize.X*2-1,ScreenSize16.Y*2-6);
    Button[btToolBarRight]^.Resize(ScreenSize8.X-ToolBarSize.X*2-1,ScreenSize16.Y*2-5);

    RedrawRenderer:=True;
    RedrawStatusBar:=True;
    RedrawToolBar:=EditMenuMode in [emBoth,emToolBar];
    RedrawHelpBar:=True;
    RedrawLevelBar:=True;
    RedrawMenu:=EditMenuMode in [emBoth,emMenu];
    RedrawButtons:=True;
end;


procedure TEditor.ScreenUpdate;
var
    I:Word;
begin
    if RebuildRenderer then
    begin
        LevelRenderer^.Build;
        RebuildRenderer:=False;
    end;

    if RedrawRenderer then
    begin
        LevelRenderer^.Render;
        RedrawRenderer:=False;
    end else LevelRenderer^.Update;

    if RedrawToolBar then
    begin
        if EditMenuMode=emToolBar then
        begin
            EditMenu^.Erase;
            Button[btToolBarLeft]^.Render;
            Button[btToolBarUp]^.Render;
            Button[btToolBarDown]^.Render;
            Button[btToolBarRight]^.Render;
        end;
        ToolBar^.Render;
        RedrawToolBar:=False;
    end;

    if RedrawStatusBar then
    begin
        StatusBar^.Render(CurrentWorkSpace,WorkSpace[CurrentWorkSpace]^.Level^.Grid[Highlight.Pos.Y,Highlight.Pos.X],
        Highlight.Pos.X,Highlight.Pos.Y,Highlight.Enabled);
        RedrawStatusBar:=False;
    end else StatusBar^.Update(CurrentWorkSpace,WorkSpace[CurrentWorkSpace]^.Level^.Grid[Highlight.Pos.Y,Highlight.Pos.X],
        Highlight.Pos.X,Highlight.Pos.Y,Highlight.Enabled);

    if RedrawHelpBar then
    begin
        HelpBar^.Render;
        RedrawHelpBar:=False;
    end else HelpBar^.Update;

    if RedrawLevelBar then
    begin
        if WorkSpace[CurrentWorkSpace]^.Named then
        begin
            if WorkSpace[CurrentWorkSpace]^.Level^.EpisodeIndex>0 then
                LevelBar^.SetLevel(WorkSpace[CurrentWorkSpace]^.Level^.EpisodeIndex,
                WorkSpace[CurrentWorkSpace]^.Level^.LevelIndex)
                else LevelBar^.SetFileName(WorkSpace[CurrentWorkSpace]^.Level^.FileName);
        end else begin
            if CurrentWorkSpace=0 then LevelBar^.SetFileName('<Clipboard Area>')
                else LevelBar^.SetFileName('<Unnamed>');
        end;
        LevelBar^.Render;
        RedrawLevelBar:=False;
    end;

    if RedrawMenu then
    begin
        if EditMenuMode=emMenu then
        begin
            ToolBar^.Erase;
            Button[btToolBarLeft]^.Erase;
            Button[btToolBarUp]^.Erase;
            Button[btToolBarDown]^.Erase;
            Button[btToolBarRight]^.Erase;
        end;
        EditMenu^.Render;
        RedrawMenu:=False;
    end else if EditMenuMode in [emMenu,emBoth] then EditMenu^.Update;

    if RedrawButtons then
    begin
        for I:=0 to 7 do Button[I]^.Render;
        RedrawButtons:=False;
    end;
end;

procedure TEditor.ProcessMouse;
var
    Hover,NewMenuOption:Byte;

    procedure SwapVar(var A,B:Integer);
    var
        TempVar:Integer;
    begin
        TempVar:=A;
        A:=B;
        B:=TempVar;
    end;

    function RenDebounceCheck:Boolean;
    begin
        RenDebounceCheck:=(Highlight.Enabled<>PrevRenClick.Enabled) or (PrevRenClick.Pos.X<>Highlight.Pos.X)
            or (PrevRenClick.Pos.Y<>Highlight.Pos.Y);
    end;
begin
{ Update level highlight }
    LevelRenderer^.SetHighlightSprite(HighlightSprite);
    Highlight:=LevelRenderer^.Hover(MouseStatus.Location.X,MouseStatus.Location.Y)^;

{ Check if the mouse pointer is hovering over anything }
    if EditMenuMode in [emMenu,emBoth] then
    begin
        Hover:=EditMenu^.Hover(MouseStatus.Location.X,MouseStatus.Location.Y);
        if (MessageTimer=0) and not (DragMode in [eeMove,eeCopy]) then
        begin
            if (Hover in [0..EditMenuSize-1]) then HelpBar^.SetMessage(EditCommandHelp[Hover])
                else HelpBar^.SetMessage(hmDefault);
        end;
    end else if (MessageTimer=0) and not (DragMode in [eeMove,eeCopy]) then
        HelpBar^.SetMessage(hmDefault);

{ Now check if the user has clicked the left mouse button }
    if mbLeft in MouseStatus.ButtonStatus then
    begin
    { See what the user has actually clicked on }
    { Try the scroll buttons }
        if Button[btLevelLeft]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeLevelLeft else
        if Button[btLevelRight]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeLevelRight else
        if Button[btLevelUp]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeLevelUp else
        if Button[btLevelDown]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeLevelDown else
        if EditMenuMode in [emToolBar,emBoth] then
        begin
            if Button[btToolBarLeft]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then
                if not PrevToolClick then EditorEvent:=eeTilePrev;
            if Button[btToolBarRight]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then
                if not PrevToolClick then EditorEvent:=eeTileNext;
            if Button[btToolBarUp]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeTileUp else
            if Button[btToolBarDown]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeTileDown;
        end;
        PrevToolClick:=True;

    { Check for menu selection }
        if EditMenuMode in [emMenu,emBoth] then
        begin
            NewMenuOption:=EditMenu^.Click(MouseStatus.Location.X,MouseStatus.Location.Y);
            case NewMenuOption of
                ecUnmark: EditorEvent:=eeUnmarkAll;
                ecFill: EditorEvent:=eeFillSel;
                ecCopy: EditorEvent:=eeCopy;
                ecMove: EditorEvent:=eeMove;
                ecSave: EditorEvent:=eeSave;
                ecSaveAsLevel: EditorEvent:=eeSaveAsLevel;
                ecSaveAsFile: EditorEvent:=eeSaveAsFileName;
                ecLoadLevel: EditorEvent:=eeLoadLevel;
                ecLoadFile: EditorEvent:=eeLoadFileName;
                ecReload: EditorEvent:=eeReload;
                ecCreateNew: EditorEvent:=eeCreateNew;
                ecQuit: EditorEvent:=eeQuit;
            end;
        end;

    { Check for a workspace change }
        NewWorkSpace:=StatusBar^.Click(MouseStatus.Location.X,MouseStatus.Location.Y);
        if NewWorkSpace in [0..12] then EditorEvent:=eeWorkSpaceChange;

    { Check for a tile selection }
        if EditMenuMode in [emToolBar,emBoth] then
        begin
            NewShapeThing:=ToolBar^.Click(MouseStatus.Location.X,MouseStatus.Location.Y);
            if NewShapeThing<>$FFFF then EditorEvent:=eeTileSelect;
        end;

    { Check if user is clicking on the level itself }
        if Highlight.Enabled then
        begin
            case DragMode of
                eeNone: begin
                    if (ShiftKeys and kfShift)<>0 then
                    begin
                        if RenDebounceCheck then EditorEvent:=eePlotToggleSel;
                    end else begin
                        if (ShiftKeys and kfCtrl)<>0 then
                        begin
                            DragMode:=eeDragMark;
                            DragWindow[0]:=Highlight.Pos;
                        end else begin
                            if (ShiftKeys and kfAlt)<>0 then
                            begin
                                DragMode:=eeDragFill;
                                DragWindow[0]:=Highlight.Pos;
                            end else if RenDebounceCheck then EditorEvent:=eePlot;
                        end;
                    end;
                end;
                eeCopy: begin
                    DragWindow[0]:=Highlight.Pos;
                    DragMode:=eeNone;
                    EditorEvent:=eeCopyTo;
                end;
                eeMove: begin
                    DragWindow[0]:=Highlight.Pos;
                    DragMode:=eeNone;
                    EditorEvent:=eeMoveTo;
                end;
            end;
            PrevRenClick:=Highlight;
        end;
    end else if mbRight in MouseStatus.ButtonStatus then begin
    { Check the right mouse button too }

    { Try the scroll buttons }
        if Button[btLevelLeft]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeLevelFastLeft else
        if Button[btLevelRight]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeLevelFastRight else
        if Button[btLevelUp]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeLevelFastUp else
        if Button[btLevelDown]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeLevelFastDown;
        if EditMenuMode in [emToolBar,emBoth] then
        begin
            if Button[btToolBarUp]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeTilePgUp else
            if Button[btToolBarDown]^.Click(MouseStatus.Location.X,MouseStatus.Location.Y) then EditorEvent:=eeTilePgDn;
        end;

        { Try the level itself }
        if Highlight.Enabled then
        begin
            if DragMode=eeNone then
            begin
                if (ShiftKeys and kfShift)<>0 then
                begin
                    if RenDebounceCheck then EditorEvent:=eePlotAnchor;
                end else begin
                    if (ShiftKeys and kfCtrl)<>0 then
                    begin
                        DragMode:=eeDragUnmark;
                        DragWindow[0]:=Highlight.Pos;
                    end else begin
                        if (ShiftKeys and kfAlt)<>0 then
                        begin
                            DragMode:=eeDragErase;
                            DragWindow[0]:=Highlight.Pos;
                        end else if RenDebounceCheck then EditorEvent:=eePlotErase;
                    end;
                end;
            end;
            PrevRenClick:=Highlight;
        end;
    end else begin
    { User has let go of a mouse button (or is not pressing any) }
        if DragMode in [eeDragMark,eeDragFill,eeDragUnmark,eeDragErase] then
        begin
            if Highlight.Enabled then
            begin
                DragWindow[1]:=Highlight.Pos;
                EditorEvent:=DragMode;
                if DragWindow[1].X<DragWindow[0].X then SwapVar(DragWindow[0].X,DragWindow[1].X);
                if DragWindow[1].Y<DragWindow[0].Y then SwapVar(DragWindow[0].Y,DragWindow[1].Y);
                DragMode:=eeNone;
            end else DragMode:=eeNone;
        end;
        PrevRenClick.Enabled:=False;
        PrevToolClick:=False;
    end;
end;

procedure TEditor.ProcessKeyboard;
var
    KeyInput:Word;
begin
{ Now translate keyboard input }
    KeyInput:=GetKey;

    case UpCase(Chr(Lo(KeyInput))) of
        'F': EditorEvent:=eeToggleFlashers;
        'A': EditorEvent:=eeToggleActives;
        'B': EditorEvent:=eeToggleBridges;
        'C': EditorEvent:=eeToggleConveyors;
        'M': EditorEvent:=eeToggleMultis;
    end;

    case KeyInput of
        kbUp,kbGreyUp: EditorEvent:=eeLevelUp;
        kbDown,kbGreyDown: EditorEvent:=eeLevelDown;
        kbLeft,kbGreyLeft: EditorEvent:=eeLevelLeft;
        kbRight,kbGreyRight: EditorEvent:=eeLevelRight;
        kbHome,kbGreyHome: EditorEvent:=eeLevelUpLeft;
        kbEnd,kbGreyEnd: EditorEvent:=eeLevelDownLeft;
        kbPgUp,kbGreyPgUp: EditorEvent:=eeLevelUpRight;
        kbPgDn,kbGreyPgDn: EditorEvent:=eeLevelDownRight;

        kbCtrlUp,kbCtrlGreyUp: EditorEvent:=eeLevelFastUp;
        kbCtrlDown,kbCtrlGreyDown: EditorEvent:=eeLevelFastDown;
        kbCtrlLeft,kbCtrlGreyLeft: EditorEvent:=eeLevelFastLeft;
        kbCtrlRight,kbCtrlGreyRight: EditorEvent:=eeLevelFastRight;
        kbCtrlHome,kbCtrlGreyHome: EditorEvent:=eeLevelHome;
        kbCtrlEnd,kbCtrlGreyEnd: EditorEvent:=eeLevelEnd;
        kbCtrlPgUp,kbCtrlGreyPgUp: EditorEvent:=eeLevelPgUp;
        kbCtrlPgDn,kbCtrlGreyPgDn: EditorEvent:=eeLevelPgDn;

        kbAltUp: EditorEvent:=eeTileUp;
        kbAltDown: EditorEvent:=eeTileDown;
        kbAltLeft: EditorEvent:=eeTilePrev;
        kbAltRight: EditorEvent:=eeTileNext;
        kbAltHome: EditorEvent:=eeTileHome;
        kbAltEnd: EditorEvent:=eeTileEnd;
        kbAltPgUp: EditorEvent:=eeTilePgUp;
        kbAltPgDn: EditorEvent:=eeTilePgDn;
        kbAlt1..kbAlt0,kbAltA,kbAltB,kbAltC: begin
            EditorEvent:=eeWorkSpaceChange;
            case KeyInput of
                kbAlt0: NewWorkSpace:=10;
                kbAlt1: NewWorkSpace:=1;
                kbAlt2: NewWorkSpace:=2;
                kbAlt3: NewWorkSpace:=3;
                kbAlt4: NewWorkSpace:=4;
                kbAlt5: NewWorkSpace:=5;
                kbAlt6: NewWorkSpace:=6;
                kbAlt7: NewWorkSpace:=7;
                kbAlt8: NewWorkSpace:=8;
                kbAlt9: NewWorkSpace:=9;
                kbAltA: NewWorkSpace:=11;
                kbAltB: NewWorkSpace:=12;
                kbAltC: NewWorkSpace:=0;
            end;
        end;
        kbGreyPlus: EditorEvent:=eeZoomIn;
        kbGreyMinus: EditorEvent:=eeZoomOut;
        kbCtrlLSQBracket: EditorEvent:=eeScreenModePrev;
        kbCtrlRSQBracket: EditorEvent:=eeScreenModeNext;
        kbTab: EditorEvent:=eeFullScreenToggle;
        kbCtrlU: EditorEvent:=eeUnmarkAll;
        kbCtrlF: EditorEvent:=eeFillSel;
        kbCtrlC: EditorEvent:=eeCopy;
        kbCtrlM: EditorEvent:=eeMove;
        kbF2: EditorEvent:=eeSave;
        kbF3: EditorEvent:=eeLoadLevel;
        kbF4: EditorEvent:=eeLoadFileName;
        kbF5: EditorEvent:=eeSaveAsLevel;
        kbF6: EditorEvent:=eeSaveAsFileName;
        kbF7: EditorEvent:=eeReload;
        kbF9: EditorEvent:=eeCreateNew;
        kbF10: EditorEvent:=eeQuit;
        kbSpace: EditorEvent:=eeMenuToggle;
        kbEnter: EditorEvent:=eeGrab;
        kbEsc: EditorEvent:=eeAbort;
    end;
end;

procedure TEditor.ProcessEvent;
var
    LevelCode:Word;
    LevelFileName:String;
    Error:TLevelError;
    NewRendererHeight:Integer;

    function LoadFileCheck(Error:TLevelError):Boolean;
    begin
        case Error of
            leOk: begin
                LevelRenderer^.Assign(WorkSpace[CurrentWorkSpace]);
                RebuildRenderer:=True;
                RedrawRenderer:=True;
                RedrawLevelBar:=True;
            end;
            leBadFileName: begin
                HelpBar^.SetMessage(hmNotFound);
                MessageTimer:=MessageDelay;
            end;
            leCorruptedFile: begin
                HelpBar^.SetMessage(hmFileError);
                MessageTimer:=MessageDelay;
            end;
        end;
    end;

    function SaveFileCheck(Error:TLevelError):Boolean;
    begin
        case Error of
            leOk: begin
                SaveFileCheck:=True;
                LevelRenderer^.Assign(WorkSpace[CurrentWorkSpace]);
                RebuildRenderer:=True;
                RedrawRenderer:=True;
                RedrawLevelBar:=True;
            end;
            leBadFileName: begin
                HelpBar^.SetMessage(hmBadName);
                MessageTimer:=MessageDelay;
                SaveFileCheck:=False;
            end;
            leNoSpace: begin
                HelpBar^.SetMessage(hmNoSpace);
                MessageTimer:=MessageDelay;
                SaveFileCheck:=False;
            end;
        end;
    end;

    function ConfirmAbandon(Prompt:String):Boolean;
    var
        NewFileName:String;
    begin
        if WorkSpace[CurrentWorkSpace]^.Modified then
        begin
            case InputPrompt^.GetYesNoCancel(Prompt) of
                irYes: begin
                    if not WorkSpace[CurrentWorkSpace]^.Named then
                    begin
                        NewFileName:=InputPrompt^.GetFileName('Save as file: ');
                        if NewFileName<>'' then
                            ConfirmAbandon:=SaveFileCheck(WorkSpace[CurrentWorkSpace]^.SaveAsFile(NewFileName))
                            else ConfirmAbandon:=False;
                    end else ConfirmAbandon:=SaveFileCheck(WorkSpace[CurrentWorkSpace]^.Save);
                end;
                irNo: ConfirmAbandon:=True;
                irCancel: ConfirmAbandon:=False;
            end;
        end else ConfirmAbandon:=True;
    end;

    procedure Quit;
    begin
        EditorEvent:=eeQuit;
        CurrentWorkSpace:=1;
        while CurrentWorkSpace<=12 do
        begin
            if WorkSpace[CurrentWorkSpace]^.Modified then
            begin
                LevelRenderer^.Assign(WorkSpace[CurrentWorkSpace]);
                LevelRenderer^.SetViewPos;
                RebuildRenderer:=True;
                RedrawRenderer:=True;
                RedrawLevelBar:=True;
                ScreenUpdate;
                if not ConfirmAbandon('Exit: Save this file? (Yes/No/Cancel) ') then Break;
            end;
            Inc(CurrentWorkSpace);
        end;
        if CurrentWorkSpace<13 then
        begin
            EditorEvent:=eeNone;
            RedrawStatusBar:=True;
        end;
    end;
begin
{ Now act on the events }
    case EditorEvent of
        eeLevelUp..eeLevelPgDn: begin
            case EditorEvent of
                eeLevelUp: Dec(WorkSpace[CurrentWorkSpace]^.ViewPos.Y);
                eeLevelDown: Inc(WorkSpace[CurrentWorkSpace]^.ViewPos.Y);
                eeLevelLeft: Dec(WorkSpace[CurrentWorkSpace]^.ViewPos.X);
                eeLevelRight: Inc(WorkSpace[CurrentWorkSpace]^.ViewPos.X);
                eeLevelUpLeft: begin
                    Dec(WorkSpace[CurrentWorkSpace]^.ViewPos.X);
                    Dec(WorkSpace[CurrentWorkSpace]^.ViewPos.Y);
                end;
                eeLevelDownLeft: begin
                    Dec(WorkSpace[CurrentWorkSpace]^.ViewPos.X);
                    Inc(WorkSpace[CurrentWorkSpace]^.ViewPos.Y);
                end;
                eeLevelUpRight: begin
                    Inc(WorkSpace[CurrentWorkSpace]^.ViewPos.X);
                    Dec(WorkSpace[CurrentWorkSpace]^.ViewPos.Y);
                end;
                eeLevelDownRight: begin
                    Inc(WorkSpace[CurrentWorkSpace]^.ViewPos.X);
                    Inc(WorkSpace[CurrentWorkSpace]^.ViewPos.Y);
                end;
                eeLevelFastUp: Dec(WorkSpace[CurrentWorkSpace]^.ViewPos.Y,10);
                eeLevelFastDown: Inc(WorkSpace[CurrentWorkSpace]^.ViewPos.Y,10);
                eeLevelFastLeft: Dec(WorkSpace[CurrentWorkSpace]^.ViewPos.X,13);
                eeLevelFastRight: Inc(WorkSpace[CurrentWorkSpace]^.ViewPos.X,13);
                eeLevelHome: begin
                    WorkSpace[CurrentWorkSpace]^.ViewPos.X:=0;
                    WorkSpace[CurrentWorkSpace]^.ViewPos.Y:=0;
                end;
                eeLevelEnd: begin
                    WorkSpace[CurrentWorkSpace]^.ViewPos.X:=0;
                    WorkSpace[CurrentWorkSpace]^.ViewPos.Y:=89;
                end;
                eeLevelPgUp: begin
                    WorkSpace[CurrentWorkSpace]^.ViewPos.X:=127;
                    WorkSpace[CurrentWorkSpace]^.ViewPos.Y:=0;
                end;
                eeLevelPgDn: begin
                    WorkSpace[CurrentWorkSpace]^.ViewPos.X:=127;
                    WorkSpace[CurrentWorkSpace]^.ViewPos.Y:=89;
                end;
            end;
            LevelRenderer^.SetViewPos;
            RedrawRenderer:=True;
        end;
        eeTileUp..eeTileSelect: begin
            if EditMenuMode in [emToolBar,emBoth] then
            begin
                case EditorEvent of
                    eeTileUp: ToolBar^.Scroll(tsUp);
                    eeTileDown: ToolBar^.Scroll(tsDown);
                    eeTileHome: ToolBar^.Scroll(tsHome);
                    eeTileEnd: ToolBar^.Scroll(tsEnd);
                    eeTilePgUp: ToolBar^.Scroll(tsPgUp);
                    eeTilePgDn: ToolBar^.Scroll(tsPgDn);
                    eeTileNext: ToolBar^.Scroll(tsNext);
                    eeTilePrev: ToolBar^.Scroll(tsPrev);
                    eeTileSelect: CurrentShapeThing:=NewShapeThing;
                end;
                RedrawToolBar:=True;
            end;
        end;
        eeGrab: begin
            if Highlight.Enabled then
            begin
                CurrentShapeThing:=ToolBar^.SetHighlight(
                    WorkSpace[CurrentWorkSpace]^.Level^.Grid[Highlight.Pos.Y,Highlight.Pos.X]);
                if EditMenuMode in [emToolBar,emBoth] then RedrawToolBar:=True;
            end;
        end;
        eeWorkSpaceChange: begin
            CurrentWorkSpace:=NewWorkSpace;
            LevelRenderer^.Assign(WorkSpace[CurrentWorkSpace]);
            LevelRenderer^.SetViewPos;
            RebuildRenderer:=True;
            RedrawRenderer:=True;
            RedrawLevelBar:=True;
        end;
        eeZoomIn: begin
            LevelRenderer^.ZoomIn;
            LevelRenderer^.SetViewPos;
            RedrawRenderer:=True;
        end;
        eeZoomOut: begin
            LevelRenderer^.ZoomOut;
            LevelRenderer^.SetViewPos;
            RedrawRenderer:=True;
        end;
        eeMenuToggle: begin
            case EditMenuMode of
                emMenu: begin
                    EditMenuMode:=emToolBar;
                    RedrawToolBar:=True;
                end;
                emToolBar: begin
                    EditMenuMode:=emMenu;
                    RedrawMenu:=True;
                end;
            end;
        end;
        eeScreenModePrev: begin
            if CurrentScreenMode>1 then
                ScreenModeSet(CurrentScreenMode-1);
        end;
        eeScreenModeNext: begin
            if CurrentScreenMode<ScreenModeCount then
                ScreenModeSet(CurrentScreenMode+1);
        end;
        eeFullScreenToggle: begin
            LevelRenderer^.Erase;
            Button[btLevelLeft]^.Erase;
            Button[btLevelRight]^.Erase;
            Button[btLevelDown]^.Erase;
            if RendererFullScreen then NewRendererHeight:=RendererNormalSize.Y
                else NewRendererHeight:=((ScreenSize.Y-32) div 16) and $FFFE;
            RendererFullScreen:=not RendererFullScreen;
            LevelRenderer^.Resize(RendererNormalSize.X,NewRendererHeight);
            Button[btLevelLeft]^.Resize(0,NewRendererHeight*2);
            Button[btLevelRight]^.Resize(RendererNormalSize.X*2-1,NewRendererHeight*2);
            Button[btLevelDown]^.Resize(RendererNormalSize.X*2,NewRendererHeight*2-1);
            RedrawRenderer:=True;
            RedrawButtons:=True;
            LevelRenderer^.SetViewPos;
        end;
        eeUnmarkAll: begin
            WorkSpace[CurrentWorkSpace]^.UnmarkAll;
            RedrawRenderer:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeFillSel: begin
            WorkSpace[CurrentWorkSpace]^.FillSelection(CurrentShapeThing);
            RebuildRenderer:=True;
            RedrawRenderer:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeCopy: begin
            if WorkSpace[CurrentWorkSpace]^.SelectionEnabled then
            begin
                EditMenu^.SetSelect(ecCopy);
                DragMode:=eeCopy;
                SourceWorkSpace:=CurrentWorkSpace;
                HighlightSprite:=siTargetAnchor;
                HelpBar^.SetMessage(hmCopy);
            end else EditMenu^.SetSelect($FF);
        end;
        eeMove: begin
            if WorkSpace[CurrentWorkSpace]^.SelectionEnabled then
            begin
                EditMenu^.SetSelect(ecMove);
                DragMode:=eeMove;
                SourceWorkSpace:=CurrentWorkSpace;
                HighlightSprite:=siTargetAnchor;
                HelpBar^.SetMessage(hmMove);
            end else EditMenu^.SetSelect($FF);
        end;
        eeCopyTo: begin
            WorkSpace[0]^.Copy(WorkSpace[SourceWorkSpace],DragWindow[0].X,DragWindow[0].Y);
            WorkSpace[CurrentWorkSpace]^.Copy(WorkSpace[0],DragWindow[0].X,DragWindow[0].Y);
            RebuildRenderer:=True;
            RedrawRenderer:=True;
        {$IFDEF PersistentCopy}
            DragMode:=eeCopy;
            SourceWorkSpace:=CurrentWorkSpace;
        {$ELSE}
            HighlightSprite:=siHighlight;
            EditMenu^.SetSelect($FF);
        {$ENDIF}
        end;
        eeMoveTo: begin
            WorkSpace[0]^.Move(WorkSpace[SourceWorkSpace],DragWindow[0].X,DragWindow[0].Y);
            WorkSpace[CurrentWorkSpace]^.Move(WorkSpace[0],DragWindow[0].X,DragWindow[0].Y);
            RebuildRenderer:=True;
            RedrawRenderer:=True;
        {$IFDEF PersistentCopy}
            DragMode:=eeMove;
            SourceWorkSpace:=CurrentWorkSpace;
        {$ELSE}
            HighlightSprite:=siHighlight;
            EditMenu^.SetSelect($FF);
        {$ENDIF}
        end;
        eeSave: begin
            if not WorkSpace[CurrentWorkSpace]^.Named then
            begin
                LevelFileName:=InputPrompt^.GetFileName('Save as file: ');
                if LevelFileName<>'' then SaveFileCheck(WorkSpace[CurrentWorkSpace]^.SaveAsFile(LevelFileName));
            end else SaveFileCheck(WorkSpace[CurrentWorkSpace]^.Save);
            RedrawStatusBar:=True;
            RedrawLevelBar:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeLoadLevel: begin
            if ConfirmAbandon('Save changes? (Yes/No/Cancel) ') then
            begin
                LevelCode:=InputPrompt^.GetLevel('Level to load: ');
                if LevelCode<>$0000 then
                begin
                    if LoadFileCheck(WorkSpace[CurrentWorkSpace]^.LoadLevel(Hi(LevelCode),Lo(LevelCode))) then
                    begin
                        WorkSpace[CurrentWorkSpace]^.ViewPos.X:=0;
                        WorkSpace[CurrentWorkSpace]^.ViewPos.Y:=0;
                    end;
                end;
            end;
            RedrawStatusBar:=True;
            RedrawLevelBar:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeLoadFileName: begin
            if ConfirmAbandon('Save changes? (Yes/No/Cancel) ') then
            begin
                LevelFileName:=InputPrompt^.GetFileName('File to load: ');
                if LevelFileName<>'' then
                begin
                    if LoadFileCheck(WorkSpace[CurrentWorkSpace]^.LoadFile(LevelFileName)) then
                    begin
                        WorkSpace[CurrentWorkSpace]^.ViewPos.X:=0;
                        WorkSpace[CurrentWorkSpace]^.ViewPos.Y:=0;
                    end;
                end;
            end;
            RedrawStatusBar:=True;
            RedrawLevelBar:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeSaveAsLevel: begin
            LevelCode:=InputPrompt^.GetLevel('Save as level: ');
            if LevelCode<>$0000 then SaveFileCheck(WorkSpace[CurrentWorkSpace]^.SaveAsLevel(Hi(LevelCode),Lo(LevelCode)));
            RedrawStatusBar:=True;
            RedrawLevelBar:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeSaveAsFileName: begin
            LevelFileName:=InputPrompt^.GetFileName('Save as file: ');
            if LevelFileName<>'' then SaveFileCheck(WorkSpace[CurrentWorkSpace]^.SaveAsFile(LevelFileName));
            RedrawStatusBar:=True;
            RedrawLevelBar:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeReload: begin
            if WorkSpace[CurrentWorkSpace]^.Modified
                and WorkSpace[CurrentWorkSpace]^.Named
                and (InputPrompt^.GetYesNoCancel('Revert to previously saved file? (Y/N) ')=irYes) then
                LoadFileCheck(WorkSpace[CurrentWorkSpace]^.Reload);
            RedrawStatusBar:=True;
            RedrawLevelBar:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeCreateNew: begin
            if ConfirmAbandon('Save changes? (Yes/No/Cancel) ') then
            begin
                WorkSpace[CurrentWorkSpace]^.CreateNew;
                LevelRenderer^.Assign(WorkSpace[CurrentWorkSpace]);
                RebuildRenderer:=True;
                RedrawRenderer:=True;
                RedrawLevelBar:=True;
                WorkSpace[CurrentWorkSpace]^.ViewPos.X:=0;
                WorkSpace[CurrentWorkSpace]^.ViewPos.Y:=0;
            end;
            RedrawStatusBar:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeQuit: begin
            Quit;
            RedrawStatusBar:=True;
            EditMenu^.SetSelect($FF);
        end;
        eeToggleFlashers: begin
            DrawFlasher:=not DrawFlasher;
            RebuildRenderer:=True;
            RedrawRenderer:=True;
        end;
        eeToggleActives: begin
            DrawActive:=not DrawActive;
            RebuildRenderer:=True;
            RedrawRenderer:=True;
        end;
        eeToggleBridges: begin
            DrawBridge:=not DrawBridge;
            RedrawRenderer:=True;
        end;
        eeToggleConveyors: begin
            DrawConveyor:=not DrawConveyor;
            RedrawRenderer:=True;
        end;
        eeToggleMultis: begin
            DrawMulti:=not DrawMulti;
            RedrawRenderer:=True;
        end;
        eePlot: begin
            if Highlight.Enabled then
            begin
                WorkSpace[CurrentWorkSpace]^.Plot(Highlight.Pos.X,Highlight.Pos.Y,CurrentShapeThing);
                RebuildRenderer:=True;
                RedrawRenderer:=True;
            end;
        end;
        eePlotErase: begin
            if Highlight.Enabled then
            begin
                WorkSpace[CurrentWorkSpace]^.Remove(Highlight.Pos.X,Highlight.Pos.Y);
                RebuildRenderer:=True;
                RedrawRenderer:=True;
            end;
        end;
        eePlotToggleSel: begin
            if Highlight.Enabled then
            begin
                WorkSpace[CurrentWorkSpace]^.Mark(Highlight.Pos.X,Highlight.Pos.Y,mmToggle);
                RedrawRenderer:=True;
            end;
        end;
        eePlotAnchor: begin
            if Highlight.Enabled then
            begin
                WorkSpace[CurrentWorkSpace]^.SetAnchor(Highlight.Pos.X,Highlight.Pos.Y);
                RedrawRenderer:=True;
            end;
        end;
        eeDragMark: begin
            WorkSpace[CurrentWorkSpace]^.MarkRange(DragWindow[0].X,DragWindow[0].Y,DragWindow[1].X,DragWindow[1].Y,mmMark);
            RedrawRenderer:=True;
        end;
        eeDragUnmark: begin
            WorkSpace[CurrentWorkSpace]^.MarkRange(DragWindow[0].X,DragWindow[0].Y,DragWindow[1].X,DragWindow[1].Y,mmUnmark);
            RedrawRenderer:=True;
        end;
        eeDragFill: begin
            WorkSpace[CurrentWorkSpace]^.FillRange(DragWindow[0].X,DragWindow[0].Y,
                DragWindow[1].X,DragWindow[1].Y,CurrentShapeThing);
            RebuildRenderer:=True;
            RedrawRenderer:=True;
        end;
        eeDragErase: begin
            WorkSpace[CurrentWorkSpace]^.FillRange(DragWindow[0].X,DragWindow[0].Y,
                DragWindow[1].X,DragWindow[1].Y,$0000);
            RebuildRenderer:=True;
            RedrawRenderer:=True;
        end;
        eeAbort: begin
            if DragMode in [eeCopy,eeMove] then
            begin
                DragMode:=eeNone;
                HighlightSprite:=siHighlight;
                EditMenu^.SetSelect($FF);
            end else if WorkSpace[CurrentWorkSpace]^.SelectionEnabled then begin
                WorkSpace[CurrentWorkSpace]^.SelectionEnabled:=False;
                RedrawRenderer:=True;
            end else Quit;
        end;
    end;

    if EditorEvent<>eeQuit then EditorEvent:=eeNone;
end;

procedure TEditor.IdleProc;
begin
    repeat
        MouseStatus:=GetMouseStatus^;
        ShiftKeys:=KeyFlags and $0F;
        if MouseXDiv2 then MouseStatus.Location.X:=MouseStatus.Location.X div 2;
        UpdateGMouse(MouseStatus.Location.X,MouseStatus.Location.Y);
    until RunAnimPulse;
end;

procedure TEditor.Run;
begin

{ Main editor loop }
    repeat
        IdleProc;
        if MessageTimer>0 then Dec(MessageTimer);

        ProcessMouse;
        ProcessKeyboard;
        HideGMouse;
        ScreenUpdate;
        ProcessEvent;

    until EditorEvent=eeQuit;
end;

destructor TEditor.Done;
var
    I:Byte;
begin
    ResetGraphMode;

{$IFDEF PrintMemUsage}
    Writeln('MemAvail=',MemAvail);
    Writeln('MaxAvail=',MaxAvail);
{$ENDIF}
    Config^.ScreenSize:=GraphModeDesc[ScreenMode[CurrentScreenMode]].Size;
    Config^.AnimFlash:=DrawFlasher;
    Config^.AnimActive:=DrawActive;
    Config^.DrawBridge:=DrawBridge;
    Config^.DrawConveyor:=DrawConveyor;
    Config^.DrawMulti:=DrawMulti;
    Dispose(Config,Done);

    SpriteManagerDone;

    for I:=0 to 12 do Dispose(WorkSpace[I],Done);

    Dispose(LevelRenderer,Done);
    Dispose(ToolBar,Done);

    Writeln('DN1Mod v1.00 - Duke Nukem 1 Level Editor');
    Writeln('Copyright (C) 1997, 2000 Bryan Rodgers <rodgersb@ses.curtin.edu.au>');
    Writeln;
    Writeln('This program is free software; you can redistribute it and/or modify');
    Writeln('it under the terms of the GNU General Public License as published by');
    Writeln('the Free Software Foundation; either version 2 of the License, or');
    Writeln('(at your option) any later version.');
    Writeln;
    Writeln('This program is distributed in the hope that it will be useful,');
    Writeln('but WITHOUT ANY WARRANTY; without even the implied warranty of');
    Writeln('MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the');
    Writeln('GNU General Public License for more details.');
    Writeln;
    Writeln('You should have received a copy of the GNU General Public License');
    Writeln('along with this program; if not, write to the Free Software');
    Writeln('Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.');
    Writeln;
end;

end.
