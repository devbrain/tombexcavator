unit LevelRenderer;

{ Rendering code for displaying levels/toolbars on the screen }

interface

uses LevelWorkSpace,LevelManager,LevelMarker,SpriteManager,GSprite;

type
{ bsNormal - Backdrop is subdivided into 13x10 16x16 pixel squares }
{ bsDouble - Backdrop is subdivided into 26x20 16x16 pixel squares }
{ bsQuad - Backdrop is subdivided into 52x40 16x16 pixel squares }
{ rsHalf - Level cells are rendered as 8x8 pixel squares }
{ rsNormal - Level cells are rendered as 16x16 pixel squares }
{ rsDouble - Level cells are rendered as 32x32 pixel squares }
    TBackdropScale=(bsNormal,bsDouble,bsQuad);
    TRenderScale=(rsHalf,rsNormal,rsDouble);

    PHighlight=^THighlight;
    THighlight=record
        Enabled:Boolean;
        Pos:TLevelCoord;
    end;

{ Please note that LevelRenderer actually renders a workspace rather than a }
{ level object - the WorkSpace object includes selection coordinates that }
{ the level object does not understand. The workspace the level renderer }
{ object is assigned to must not change its level or selection pointers, as }
{ during the assignment these pointers are copied over and stored }
{ internally to speed up drawing and eliminate a level of pointer recursion. }

{ Init: Specify the size of level rendering window, and also copies some }
{   data pointers over from the given workspace object. }
{ Resize: Resizes screen size of level rendering window. }
{ Assign: Assigns a level object to the rendering object }
{ SetViewPos: Places viewing offset at X,Y relative to top-left corner }
{   of level as specified by the ViewPos field in the WorkSpace object,}
{   and stores corrected coordinates (if they span beyond the level) in the }
{   WorkSpace object. We pinch the viewing coordinates from the TWorkSpace }
{   object because this saves the TEditor object having to constantly copy }
{   and update both the render and workspace objects. }
{ Hover: Pass this method the screen coordinates of the mouse pointer }
{   as is. Hover will then do the rest, which includes marking the }
{   highlight variables for displaying the highlight marker and returning the }
{   actual highlighted coordinates in the level for use by the level editor. }
{   If the mouse pointer is not on the level, the Enabled field of the }
{   returned THighlight record will be false, and the X,Y fields are not valid. }
{ SetZoom attempts to set zoom level to requested level, and returns corrected }
{   zoom level if not valid. Values for zoom are: 0=Half, 1=Normal, 2=Double. }
{ Build: Builds marker tables, must be called if the level data changes }
{   or the rendering object is assigned a new level. }
{ Render draws the entire screen. Must be called if the selection }
{   information changes in any way. }
{ Update only updates the animated components, as well as the highlight }
{   marker. }

    PLevelRenderer=^TLevelRenderer;
    TLevelRenderer=object
        public
            constructor Init(SizeX,SizeY:Integer);
            procedure Resize(SizeX,SizeY:Integer);
            procedure Assign(NewWorkSpace:PWorkSpace);
            procedure SetViewPos;
            procedure SetHighlightSprite(NewHighlightSprite:Word);
            function Hover(X,Y:Integer):PHighlight;
            function Click(X,Y:Integer):PHighlight;
            procedure ZoomIn;
            procedure ZoomOut;
            procedure Build;
            procedure Render;
            procedure Update;
            procedure Erase;
            destructor Done;
        private
        { WinSize maintains the size of the screen window in rendering units, }
        {   either 8x8, 16x16 or 32x32 as according to the RenderScale field. }
        { BackdropScale indicates the zooming level of the backdrop to }
        {   accommodate higher screen resolutions: either 1x, 2x or 4x }
            WinSize:TLevelCoord;
            RenderScale:TRenderScale;
            BackdropScale:TBackdropScale;

        { Target workspace to render and some pre-copied data pointers }
        { which we pinch off the workspace object to reduce pointer derefs. }
            WorkSpace:PWorkSpace;
            Level:PLevel;
            Selection:PLevelMarker;
            ViewPos:TLevelCoord;

        { Bit-field arrays to assist in rendering multi-square sprites. }
        { The update bitfield indicates what cells get updated. }
            Multi,BreakBridge,RoboBridge,Conveyor,UpdateAnim:PLevelMarker;


        { Highlight manages the current location of the highlight marker }
        { PrevHighlight maintains the location of the last highlight bar }
        {   that was drawn on screen. }
        { Anchor holds the position of the anchor as copied from the }
        {   workspace object by the Render procedure. }
        { PrevHighlightSave and HighlightSave record the update bitmap fields }
        {   that the highlight marker is currently over. }
        { HighlightSprite determines the sprite that is used to draw the }
        {   highlight marker. }
            PrevHighlight,Highlight,Anchor:THighlight;
            PrevHighlightSave,HighlightSave:Boolean;
            HighlightSprite:Word;

        { RenderCell renders an individual cell }
        { RenderBack renders the background of the specified cell }
        { RenderReflection renders a reflection from the specifed cell }
            procedure RenderCell(X,Y:Integer;var Target:TMedSprite);
            procedure RenderBack(X,Y:Integer;var Target:TMedSprite);
            procedure RenderReflection(X,Y,Depth:Integer;var Target:TMedSprite);
    end;

{ Scroll direction for toolbar }
    TToolBarScroll=(tsNone,tsUp,tsDown,tsHome,tsEnd,tsPgUp,tsPgDn,tsNext,tsPrev,tsLocateCurrent);

    PIconArray=^TIconArray;
    TIconArray=array[0..255] of Word;

    PIconList=^TIconList;
    TIconList=record
        Name:String[8];
        Size,Length:Word;
        Icon:PIconArray;
    end;

    PIconListArray=^TIconListArray;
    TIconListArray=array[0..255] of TIconList;

{ The TToolBar object represents multiple lists of selectable sprite tiles }

{ Init: Defines dimensions of the toolbar, and initialises tiles to blanks }
{ Resize: Resizes dimensions of toolbar on the screen. }
{ SetListName: Sets name of specified list }
{ Add: Adds a new tile icon to the specified list }
{ Render: Draws toolbar on screen }
{ Erase: Erases toolbar off screen }
{ Scroll: Performs scrolling operations on the toolbar }
{ SetHighlight: Highlights given ShapeThing code (used by grab function) }
{ Click: Pass in coordinates of the mouse pointer - the Click method will }
{   translate them into an appropiate toolbar selection, and return the }
{   translated ShapeThing value. Returns $FFFF if click not valid. }
    PToolBar=^TToolBar;
    TToolBar=object
        public
            constructor Init(OffsetX,OffsetY,SizeX,SizeY:Integer;ListCount,ListSize:Word);
            procedure Resize(OffsetX,OffsetY,SizeX,SizeY:Integer);
            procedure SetListName(List:Word;NewName:String);
            procedure AddIcon(List:Word;NewShapeThing:TShapeThing);
            procedure Render;
            procedure Erase;
            procedure Scroll(Dir:TToolBarScroll);
            function SetHighlight(NewShapeThing:TShapeThing):TShapeThing;
            function Click(X,Y:Integer):TShapeThing;
            destructor Done;
        private
        { IconList: array of lists of icons }
        { HighlightIcon: Index of icon that is highlighted }
        { HighlightList: List of icon that is highlighted }
        { ScrollIcon: Icon at top of screen (must be multiple of WinSize.X) }
        { ScrollList: Current list that is visible on screen }
        { WinPos: Location on screen of toolbar }
        { WinSize: Size of toolbar on screen }
            IconList:PIconListArray;
            IconListSize:Word;
            HighlightIcon,HighlightList:Integer;
            ScrollIcon,ScrollList:Integer;
            WinPos,WinSize:TLevelCoord;
            procedure RenderIcon(ShapeThing:TShapeThing;var Target:TMedSprite);
    end;

var
{ Global animation flags }
    DrawFlasher,DrawActive:Boolean;
    DrawBridge,DrawConveyor,DrawMulti:Boolean;

implementation

type
    TActiveSpriteMode=(atSolid11S,atSolid11D,
        atTrans11,atTrans21,atTrans12,atTrans22,atTrans31,atTrans33,atTrans42,
        atMissile,atReactor,atMirror,atBridge,atConveyor);

    TActiveSprite=record
        Mode:TActiveSpriteMode;
        Anim:Word;
        Data:array[0..1] of Word;
    end;

    TMultiCheck=record
        GridOffset:TLevelCoord;
        SpriteOffset:array[Low(TActiveSpriteMode)..High(TActiveSpriteMode)] of Word;
    end;

const
{$I LEVELREN.INC}

    HexChar:array[0..15] of Char='0123456789ABCDEF';

    clToolBarName=$07;
    clToolBarShapeThing=$07;

var
    ReturnHighlight:THighlight;

{ --- Useful internal functions ------------------------------------------- }

{ Pastes a character onto the top left of the sprite }
procedure MergeChar(Data:Word;var Target:TMedSprite);
var
    Source:TMedSprite;
begin
    if Data<>0 then
    begin
        Source:=TransSprite;
        Source.Paste(@Font^.Sprite^[FontMap[Data]],0,0);
        Target.Merge(@Source);
    end;
end;

{ Translates a word into a hexadecimal string }
function Word2Hex(N:Word):String;
var
    Count:Byte;
begin
    Word2Hex[0]:=#4;
    for Count:=0 to 3 do
        Word2Hex[4-Count]:=HexChar[(N shr (Count*4)) and $000F];
end;

{ --- TLevelRenderer methods ---------------------------------------------- }

constructor TLevelRenderer.Init(SizeX,SizeY:Integer);
begin
    New(Multi);
    New(BreakBridge);
    New(RoboBridge);
    New(Conveyor);
    New(UpdateAnim);
    WinSize.X:=SizeX;
    WinSize.Y:=SizeY;
    RenderScale:=rsNormal;
    case WinSize.X div 13 of
        1: BackdropScale:=bsNormal;
        2..3: BackdropScale:=bsDouble;
        4..7: BackdropScale:=bsQuad;
    end;
    Highlight.Enabled:=False;
    Anchor.Enabled:=False;
    PrevHighlight.Enabled:=False;
    HighlightSprite:=siHighlight;
end;

procedure TLevelRenderer.Resize(SizeX,SizeY:Integer);
begin
    WinSize.X:=SizeX;
    WinSize.Y:=SizeY;
    RenderScale:=rsNormal;
    case WinSize.X div 13 of
        1: BackdropScale:=bsNormal;
        2..3: BackdropScale:=bsDouble;
        4..7: BackdropScale:=bsQuad;
    end;
    SetViewPos;
end;

function TLevelRenderer.Hover(X,Y:Integer):PHighlight;
var
    ScreenCell:TLevelCoord;
begin
    case RenderScale of
        rsHalf: begin
            ScreenCell.X:=X div 8;
            ScreenCell.Y:=Y div 8;
        end;
        rsNormal: begin
            ScreenCell.X:=X div 16;
            ScreenCell.Y:=Y div 16;
        end;
        rsDouble: begin
            ScreenCell.X:=X div 32;
            ScreenCell.Y:=Y div 32;
        end;
    end;
    if (ScreenCell.X>=0) and (ScreenCell.X<WinSize.X)
        and (ScreenCell.Y>=0) and (ScreenCell.Y<WinSize.Y) then
    begin
        Highlight.Pos.X:=ScreenCell.X+WorkSpace^.ViewPos.X;
        Highlight.Pos.Y:=ScreenCell.Y+WorkSpace^.ViewPos.Y;
        Highlight.Enabled:=True;
    end else Highlight.Enabled:=False;
    ReturnHighlight:=Highlight;
    Hover:=@ReturnHighlight;
end;

function TLevelRenderer.Click(X,Y:Integer):PHighlight;
begin
    Click:=Hover(X,Y);
end;

procedure TLevelRenderer.ZoomIn;
begin
    if (BackdropScale>bsNormal) and (RenderScale<rsDouble) then
    begin
        Dec(BackdropScale);
        Inc(RenderScale);
        WinSize.X:=WinSize.X div 2;
        WinSize.Y:=WinSize.Y div 2;
    end;
end;

procedure TLevelRenderer.ZoomOut;
begin
    if (BackdropScale<bsQuad) and (RenderScale>rsHalf) then
    begin
        Inc(BackdropScale);
        Dec(RenderScale);
        WinSize.X:=WinSize.X*2;
        WinSize.Y:=WinSize.Y*2;
    end;
end;

destructor TLevelRenderer.Done;
begin
    Dispose(UpdateAnim);
    Dispose(Conveyor);
    Dispose(RoboBridge);
    Dispose(BreakBridge);
    Dispose(Multi);
end;

procedure TLevelRenderer.Assign(NewWorkSpace:PWorkSpace);
begin
    WorkSpace:=NewWorkSpace;
    Level:=WorkSpace^.Level;
    LoadBackdrop(BackdropKey[Level^.EpisodeIndex,Level^.LevelIndex,0],
        BackdropKey[Level^.EpisodeIndex,Level^.LevelIndex,1]);
end;

procedure TLevelRenderer.SetViewPos;
begin
    if (WorkSpace^.ViewPos.X<0) then WorkSpace^.ViewPos.X:=0;
    if (WorkSpace^.ViewPos.X>128-WinSize.X) then
        WorkSpace^.ViewPos.X:=128-WinSize.X;
    if (WorkSpace^.ViewPos.Y<0) then WorkSpace^.ViewPos.Y:=0;
    if (WorkSpace^.ViewPos.Y>90-WinSize.Y) then
        WorkSpace^.ViewPos.Y:=90-WinSize.Y;
end;

procedure TLevelRenderer.SetHighlightSprite(NewHighlightSprite:Word);
begin
    HighlightSprite:=NewHighlightSprite;
end;

procedure TLevelRenderer.Build;
var
    X,Y,I:Integer;
    ShapeThing,CurrentShapeThing,Background:TShapeThing;
    A:TActiveSprite;
    procedure MarkUpdateCell(X,Y:Integer;Anim:Word);
    begin
        if ((Anim and $0F)>1) and DrawActive then UpdateAnim^.SetMarker(X,Y,True);
    end;
    procedure MarkMultiCell(X,Y:Integer;Anim:Word);
    begin
        Multi^.SetMarker(X,Y,True);
        if ((Anim and $0F)>1) and DrawActive then UpdateAnim^.SetMarker(X,Y,True);
    end;
begin
    Level:=WorkSpace^.Level;
    Multi^.Clear;
    BreakBridge^.Clear;
    RoboBridge^.Clear;
    Conveyor^.Clear;
    UpdateAnim^.Clear;
    for Y:=0 to 89 do
    begin
        for X:=0 to 127 do
        begin
            ShapeThing:=Level^.Grid[Y,X];
            Background:=Level^.GetPullValue(X,Y);
            case ShapeThing of
                $0021..$05FF: if DrawFlasher then UpdateAnim^.SetMarker(X,Y,True);
                $3000..$3059: begin
                    A:=ActiveSprite[ShapeThing];
                    MarkUpdateCell(X,Y,A.Anim);
                    if (Background>=$0000) and (Background<=$05FF) and DrawFlasher then
                        UpdateAnim^.SetMarker(X,Y,True);
                    case A.Mode of
                        atTrans21: MarkMultiCell(X+1,Y,A.Anim);
                        atTrans12: MarkMultiCell(X,Y-1,A.Anim);
                        atTrans22: begin
                            MarkMultiCell(X+1,Y,A.Anim);
                            MarkMultiCell(X,Y-1,A.Anim);
                            MarkMultiCell(X+1,Y-1,A.Anim);
                        end;
                        atTrans31: begin
                            MarkMultiCell(X-1,Y,A.Anim);
                            MarkMultiCell(X+1,Y,A.Anim);
                        end;
                        atTrans33: begin
                            MarkMultiCell(X+1,Y,A.Anim);
                            MarkMultiCell(X+2,Y,A.Anim);
                            MarkMultiCell(X,Y-1,A.Anim);
                            MarkMultiCell(X+1,Y-1,A.Anim);
                            MarkMultiCell(X+2,Y-1,A.Anim);
                            MarkMultiCell(X,Y-2,A.Anim);
                            MarkMultiCell(X+1,Y-2,A.Anim);
                            MarkMultiCell(X+2,Y-2,A.Anim);
                        end;
                        atTrans42: begin
                            MarkMultiCell(X+1,Y,A.Anim);
                            MarkMultiCell(X+2,Y,A.Anim);
                            MarkMultiCell(X+3,Y,A.Anim);
                            MarkMultiCell(X,Y-1,A.Anim);
                            MarkMultiCell(X+1,Y-1,A.Anim);
                            MarkMultiCell(X+2,Y-1,A.Anim);
                            MarkMultiCell(X+3,Y-1,A.Anim);
                        end;
                        atMissile: begin
                            MarkMultiCell(X-1,Y,A.Anim);
                            MarkMultiCell(X+1,Y,A.Anim);
                            MarkMultiCell(X,Y-1,A.Anim);
                            MarkMultiCell(X,Y-2,A.Anim);
                            MarkMultiCell(X,Y-3,A.Anim);
                        end;
                        atReactor: begin
                            MarkMultiCell(X,Y-1,A.Anim);
                            MarkMultiCell(X,Y-2,A.Anim);
                            MarkMultiCell(X,Y-3,A.Anim);
                        end;
                        atMirror: begin
                            UpdateAnim^.SetMarker(X,Y,True);
                            UpdateAnim^.SetMarker(X,Y+1,True);
                            Multi^.SetMarker(X,Y+1,True);
                        end;
                        atBridge: begin
                            I:=X;
                            repeat
                                case ShapeThing of
                                    asBreakBridge: if (I-X) mod 2=0 then BreakBridge^.SetMarker(I,Y,True);
                                    asRobohandExtension: RoboBridge^.SetMarker(I,Y,True);
                                end;
                                Inc(I);
                                case ShapeThing of
                                    asBreakBridge: if Level^.GetShapeValue(I,Y) in [stNone,stSolid] then Break;
                                    asRobohandExtension: if Level^.GetClipValue(I,Y,asRedGirder)=asRedGirder then Break;
                                end;
                            until False;
                        end;
                        atConveyor: begin
                            I:=X+1;
                            repeat
                                CurrentShapeThing:=Level^.GetClipValue(I,Y,asLConveyorREnd);
                                if (CurrentShapeThing=asLConveyorREnd)
                                    or (CurrentShapeThing=asRConveyorREnd) then Break;
                                Conveyor^.SetMarker(I,Y,True);
                                Inc(I);
                            until False;
                        end;
                    end;
                end;
            end;
        end;
    end;
    if PrevHighlight.Enabled then
    begin
        PrevHighlightSave:=UpdateAnim^.GetMarker(PrevHighlight.Pos.X,PrevHighlight.Pos.Y);
        UpdateAnim^.SetMarker(PrevHighlight.Pos.X,PrevHighlight.Pos.Y,True);
    end;
    if Highlight.Enabled then
    begin
        HighlightSave:=UpdateAnim^.GetMarker(Highlight.Pos.X,Highlight.Pos.Y);
        UpdateAnim^.SetMarker(Highlight.Pos.X,Highlight.Pos.Y,True);
    end;
end;

procedure TLevelRenderer.RenderReflection(X,Y,Depth:Integer;var Target:TMedSprite);
var
    Source:array[0..1] of TMedSprite;
    procedure Interleave(Depth:Integer;const Source:array of TMedSprite;var Target:TMedSprite);
    var
        ShiftOffset,L,C:Word;
    begin
        if DrawActive then ShiftOffset:=AnimPulse[MirrorAnimSpeed] div (MirrorAnimSpeed div 2)
            else ShiftOffset:=0;
        for L:=2-2*Depth to 15 do
            for C:=0 to 3 do
                Target.Image[C,L]:=Source[(15-L) div 8].Image[C,15-(L mod 8)*2-ShiftOffset];
    end;
begin
    RenderCell(X,(Y+88-Depth*3) mod 90,Source[0]);
    RenderCell(X,(Y+89-Depth*3) mod 90,Source[1]);
    Interleave(Depth,Source,Target);
end;

procedure TLevelRenderer.RenderCell(X,Y:Integer;var Target:TMedSprite);
var
    ShapeThing:TShapeThing;
    I,SpriteOffset,AnimOffset,CurrentAnimOffset:Word;
    A:TActiveSprite;
begin
    ShapeThing:=Level^.Grid[Y,X];
    case ShapeThing of
        $0000..$2FFF: RenderBack(X,Y,Target);
        $3000..$3059: begin
            A:=ActiveSprite[ShapeThing];
            if DrawActive then AnimOffset:=TranslateAnimPulse(A.Anim)
                else AnimOffset:=0;
            case A.Mode of
                atSolid11S,atBridge,atConveyor: begin
                    Target:=TranslateSpriteIndex(A.Data[0]+AnimOffset)^;
                    MergeChar(A.Data[1],Target);
                end;
                atSolid11D: begin
                    Target:=TranslateSpriteIndex(A.Data[0])^;
                    Target.Merge(TranslateSpriteIndex(A.Data[1]+AnimOffset));
                end;
                atTrans11..atReactor: begin
                    RenderBack(X,Y,Target);
                    Target.Merge(TranslateSpriteIndex(A.Data[0]
                        +AnimOffset*MultiSize[A.Mode]
                        +MultiCheck[0].SpriteOffset[A.Mode]));
                    MergeChar(A.Data[1],Target);
                end;
                atMirror: begin
                    RenderBack(X,Y,Target);
                    RenderReflection(X,Y,0,Target);
                end;
                else Target:=TranslateSpriteIndex(siError)^;
            end;
        end;
        else Target:=TranslateSpriteIndex(siError)^;
    end;

    if DrawBridge then case ShapeThing of
        asRobohandExtension,asBreakBridge:
        else begin
            if BreakBridge^.GetMarker(X-1,Y) then Target:=TranslateSpriteIndex(siBreakBridge+1)^;
            if BreakBridge^.GetMarker(X,Y) then Target:=TranslateSpriteIndex(siBreakBridge)^;
            if RoboBridge^.GetMarker(X,Y) then Target:=TranslateSpriteIndex(siRedGirder)^;
        end;
    end;
    if DrawConveyor and Conveyor^.GetMarker(X,Y) then Target:=TranslateSpriteIndex(siConveyor)^;
    if DrawMulti and Multi^.GetMarker(X,Y) then
    begin
        case Level^.GetClipValue(X,Y-1,$FFFF) of
            asMirror: RenderReflection(X,Y,1,Target);
        end;
        for I:=1 to 12 do
        begin
            ShapeThing:=Level^.GetClipValue(X+MultiCheck[I].GridOffset.X,
                Y+MultiCheck[I].GridOffset.Y,$0000);
            if (ShapeThing>=$3000) and (ShapeThing<=$3059) then
            begin
                A:=ActiveSprite[ShapeThing];
                SpriteOffset:=MultiCheck[I].SpriteOffset[A.Mode];
                if SpriteOffset<>$FFFF then
                begin
                    if DrawActive then AnimOffset:=TranslateAnimPulse(A.Anim) else AnimOffset:=0;
                    if (A.Anim and $80)=0 then
                        Target.Merge(TranslateSpriteIndex(A.Data[0]+AnimOffset*MultiSize[A.Mode]+SpriteOffset))
                        else Target.Merge(TranslateSpriteIndex(A.Data[0]+SpriteOffset));
                end;
            end;
        end;
    end;

    if Highlight.Enabled and (X=Highlight.Pos.X) and (Y=Highlight.Pos.Y) then
        Target.Merge(TranslateSpriteIndex(HighlightSprite));
    if Anchor.Enabled then
    begin
        if Selection^.GetMarker(X,Y) then Target.Merge(TranslateSpriteIndex(siSelection));
        if (X=Anchor.Pos.X) and (Y=Anchor.Pos.Y) then Target.Merge(TranslateSpriteIndex(siSourceAnchor));
    end;
end;

procedure TLevelRenderer.RenderBack(X,Y:Integer;var Target:TMedSprite);
var
    ShapeThing:TShapeThing;
    Back:TLevelCoord;
    AnimOffset:Word;
    Source:TSmallSprite;
begin
    ShapeThing:=Level^.GetPullValue(X,Y);
    case ShapeThing of
        $0000,$0020: begin
            AnimOffset:=ShapeThing shr 5;
            case BackdropScale of
                bsNormal: begin
                    Back.X:=(X-ViewPos.X+130) mod 13;
                    Back.Y:=(Y-ViewPos.Y+100) mod 10;
                    Target:=Backdrop[AnimOffset]^.Sprite^[Back.Y*13+Back.X];
                end;
                bsDouble: begin
                    Back.X:=(X-ViewPos.X+130) mod 26;
                    Back.Y:=(Y-ViewPos.Y+100) mod 20;
                    Source.Cut(@Backdrop[AnimOffset]^.Sprite^[(Back.Y shr 1)*13+Back.X shr 1],
                        Back.X and 1,Back.Y and 1);
                    Target.Enlarge(@Source);
                end;
                bsQuad: begin
                    Back.X:=(X-ViewPos.X+260) mod 52;
                    Back.Y:=(Y-ViewPos.Y+200) mod 40;
                    Source.Cut(@Backdrop[AnimOffset]^.Sprite^[(Back.Y shr 2)*13+Back.X shr 2],
                        (Back.X and 2) shr 1,(Back.Y and 2) shr 1);
                    Target.Enlarge(@Source);
                    Source.Cut(@Target,Back.X and 1,Back.Y and 1);
                    Target.Enlarge(@Source);
                end;
            end;
        end;
        $0000..$05FF: begin
            if DrawFlasher then AnimOffset:=AnimPulse[4] else AnimOffset:=0;
            Target:=SClass[0]^.Sprite^[(ShapeThing shr 5)+AnimOffset];
        end;
        $0600..$17FF: Target:=SClass[1]^.Sprite^[(ShapeThing-$0600) shr 5];
        $1800..$2FFF: Target:=SClass[2]^.Sprite^[(ShapeThing-$1800) shr 5];
        else Target:=TranslateSpriteIndex(siError)^;
    end;
end;

procedure TLevelRenderer.Render;
var
    STarget:TSmallSprite;
    MTarget:TMedSprite;
    LTarget:TLargeSprite;
    X,Y:Integer;
begin
    Level:=WorkSpace^.Level;
    Selection:=WorkSpace^.Selection;
    ViewPos:=WorkSpace^.ViewPos;
    if Highlight.Enabled then
    begin
        HighlightSave:=UpdateAnim^.GetMarker(Highlight.Pos.X,Highlight.Pos.Y);
        UpdateAnim^.SetMarker(Highlight.Pos.X,Highlight.Pos.Y,True);
    end;
    Anchor.Enabled:=WorkSpace^.SelectionEnabled;
    if Anchor.Enabled then Anchor.Pos:=WorkSpace^.Anchor;

    case RenderScale of
        rsNormal: begin
            for Y:=0 to WinSize.Y-1 do
                for X:=0 to WinSize.X-1 do
                begin
                    RenderCell(ViewPos.X+X,ViewPos.Y+Y,MTarget);
                    MTarget.Draw(X,Y);
                end;
        end;
        rsHalf: begin
            for Y:=0 to WinSize.Y-1 do
                for X:=0 to WinSize.X-1 do
                begin
                    RenderCell(ViewPos.X+X,ViewPos.Y+Y,MTarget);
                    STarget.Reduce(@MTarget);
                    STarget.Draw(X,Y);
                end;
        end;
        rsDouble: begin
            for Y:=0 to WinSize.Y-1 do
                for X:=0 to WinSize.X-1 do
                begin
                    RenderCell(ViewPos.X+X,ViewPos.Y+Y,MTarget);
                    LTarget.Enlarge(@MTarget);
                    LTarget.Draw(X,Y);
                end;
        end;
    end;

    if PrevHighlight.Enabled then
        UpdateAnim^.SetMarker(PrevHighlight.Pos.X,PrevHighlight.Pos.Y,PrevHighlightSave);

    PrevHighlight:=Highlight;
    PrevHighlightSave:=HighlightSave;
end;

procedure TLevelRenderer.Update;
var
    STarget:TSmallSprite;
    MTarget:TMedSprite;
    LTarget:TLargeSprite;
    X,Y:Integer;
    ShapeThing:TShapeThing;
    A:TActiveSprite;
begin
    if Highlight.Enabled then
    begin
        HighlightSave:=UpdateAnim^.GetMarker(Highlight.Pos.X,Highlight.Pos.Y);
        UpdateAnim^.SetMarker(Highlight.Pos.X,Highlight.Pos.Y,True);
    end;

    case RenderScale of
        rsNormal: begin
            for Y:=0 to WinSize.Y-1 do
                for X:=0 to WinSize.X-1 do
                begin
                    if UpdateAnim^.GetMarker(ViewPos.X+X,ViewPos.Y+Y) then
                    begin
                        RenderCell(ViewPos.X+X,ViewPos.Y+Y,MTarget);
                        MTarget.Draw(X,Y);
                    end;
                end;
        end;
        rsHalf: begin
            for Y:=0 to WinSize.Y-1 do
                for X:=0 to WinSize.X-1 do
                begin
                    if UpdateAnim^.GetMarker(ViewPos.X+X,ViewPos.Y+Y) then
                    begin
                         RenderCell(ViewPos.X+X,ViewPos.Y+Y,MTarget);
                         STarget.Reduce(@MTarget);
                         STarget.Draw(X,Y);
                    end;
                end;
        end;
        rsDouble: begin
            for Y:=0 to WinSize.Y-1 do
                for X:=0 to WinSize.X-1 do
                begin
                    if UpdateAnim^.GetMarker(ViewPos.X+X,ViewPos.Y+Y) then
                    begin
                        RenderCell(ViewPos.X+X,ViewPos.Y+Y,MTarget);
                        LTarget.Enlarge(@MTarget);
                        LTarget.Draw(X,Y);
                    end;
                end;
        end;
    end;

    if PrevHighlight.Enabled then
        UpdateAnim^.SetMarker(PrevHighlight.Pos.X,PrevHighlight.Pos.Y,PrevHighlightSave);

    PrevHighlight:=Highlight;
    PrevHighlightSave:=HighlightSave;
end;

procedure TLevelRenderer.Erase;
var
    Screen:TLevelCoord;
    STarget:TSmallSprite;
    MTarget:TMedSprite;
    LTarget:TLargeSprite;
begin
    case RenderScale of
        rsHalf: begin
            STarget.Clear;
            for Screen.Y:=0 to WinSize.Y-1 do
                for Screen.X:=0 to WinSize.X-1 do
                    STarget.Draw(Screen.X,Screen.Y);
        end;
        rsNormal: begin
            MTarget.Clear;
            for Screen.Y:=0 to WinSize.Y-1 do
                for Screen.X:=0 to WinSize.X-1 do
                    MTarget.Draw(Screen.X,Screen.Y);
        end;
        rsDouble: begin
            LTarget.Clear;
            for Screen.Y:=0 to WinSize.Y-1 do
                for Screen.X:=0 to WinSize.X-1 do
                    LTarget.Draw(Screen.X,Screen.Y);
        end;
    end;
end;

{ --- TToolBar methods ---------------------------------------------------- }

constructor TToolBar.Init(OffsetX,OffsetY,SizeX,SizeY:Integer;ListCount,ListSize:Word);
var
    I:Word;
begin
    WinPos.X:=OffsetX;
    WinPos.Y:=OffsetY;
    WinSize.X:=SizeX;
    WinSize.Y:=SizeY;
    HighlightIcon:=0;
    HighlightList:=0;
    ScrollIcon:=0;
    ScrollList:=0;
    IconListSize:=ListCount;
    GetMem(IconList,SizeOf(TIconList)*IconListSize);
    for I:=0 to IconListSize-1 do
    begin
        GetMem(IconList^[I].Icon,SizeOf(Word)*ListSize);
        IconList^[I].Name:='';
        IconList^[I].Size:=ListSize;
        IconList^[I].Length:=0;
    end;
end;

procedure TToolBar.Resize(OffsetX,OffsetY,SizeX,SizeY:Integer);
begin
    WinPos.X:=OffsetX;
    WinPos.Y:=OffsetY;
    WinSize.X:=SizeX;
    WinSize.Y:=SizeY;
    Dec(ScrollIcon,ScrollIcon mod WinSize.X);
    Scroll(tsNone);
end;

procedure TToolBar.SetListName(List:Word;NewName:String);
begin
    IconList^[List].Name:=NewName;
end;

procedure TToolBar.AddIcon(List:Word;NewShapeThing:TShapeThing);
begin
    if IconList^[List].Length<IconList^[List].Size then
    begin
        IconList^[List].Icon^[IconList^[List].Length]:=NewShapeThing;
        Inc(IconList^[List].Length);
    end;
end;

procedure TToolBar.Render;
var
    Screen:TLevelCoord;
    CurrentIcon,ScreenIcon,I:Integer;
    Target:TMedSprite;
    HexString:String[4];
begin
    for ScreenIcon:=0 to (WinSize.Y-1)*WinSize.X-1 do
    begin
        Screen.X:=ScreenIcon mod WinSize.X;
        Screen.Y:=ScreenIcon div WinSize.X+1;
        CurrentIcon:=ScreenIcon+ScrollIcon;
        if CurrentIcon<IconList^[ScrollList].Length then
        begin
            RenderIcon(IconList^[ScrollList].Icon^[CurrentIcon],Target);
            if (CurrentIcon=HighlightIcon) and (ScrollList=HighlightList) then
                Target.Merge(TranslateSpriteIndex(siHighlight));
        end else Target.Clear;
        Target.Draw(WinPos.X+Screen.X,WinPos.Y+Screen.Y);
    end;
    HexString:=Word2Hex(IconList^[HighlightList].Icon^[HighlightIcon]);
    for I:=0 to 7 do PrintChar(WinPos.X*2+I,WinPos.Y*2,clToolBarName,#0);
    for I:=0 to Length(IconList^[ScrollList].Name)-1 do
        PrintChar(WinPos.X*2+I,WinPos.Y*2,clToolBarName,IconList^[ScrollList].Name[I+1]);
    for I:=0 to 3 do PrintChar(WinPos.X*2+I,WinPos.Y*2+1,clToolBarShapeThing,HexString[I+1]);
end;

procedure TToolBar.Erase;
var
    Screen:TLevelCoord;
    Target:TMedSprite;
begin
    Target.Clear;
    for Screen.Y:=0 to WinSize.Y-1 do
        for Screen.X:=0 to WinSize.X-1 do
            Target.Draw(WinPos.X+Screen.X,WinPos.Y+Screen.Y);
end;

procedure TToolBar.Scroll(Dir:TToolBarScroll);
begin
    case Dir of
        tsUp: Dec(ScrollIcon,WinSize.X);
        tsDown: Inc(ScrollIcon,WinSize.X);
        tsHome: ScrollIcon:=0;
        tsEnd: ScrollIcon:=IconList^[ScrollList].Length-1;
        tsPgUp: Dec(ScrollIcon,WinSize.Y*WinSize.X);
        tsPgDn: Inc(ScrollIcon,WinSize.Y*WinSize.X);
        tsNext: Inc(ScrollList);
        tsPrev: Dec(ScrollList);
        tsLocateCurrent: begin
            ScrollList:=HighlightList;
            ScrollIcon:=HighlightIcon;
        end;
    end;
    if ScrollList>=IconListSize then ScrollList:=0;
    if ScrollList<0 then ScrollList:=IconListSize-1;
    if ScrollIcon>IconList^[ScrollList].Length-WinSize.X*(WinSize.Y-1) then
        ScrollIcon:=IconList^[ScrollList].Length-WinSize.X*(WinSize.Y-1);
    if ScrollIcon<0 then ScrollIcon:=0;
    Dec(ScrollIcon,ScrollIcon mod WinSize.X);
end;

function TToolBar.SetHighlight(NewShapeThing:TShapeThing):TShapeThing;
var
    I,J:Integer;
begin
    SetHighlight:=IconList^[HighlightList].Icon^[HighlightIcon];
    for J:=0 to IconListSize-1 do
        for I:=0 to IconList^[J].Length-1 do
            if IconList^[J].Icon^[I]=NewShapeThing then
            begin
                HighlightList:=J;
                HighlightIcon:=I;
                SetHighlight:=NewShapeThing;
                Scroll(tsLocateCurrent);
            end;
end;

function TToolBar.Click(X,Y:Integer):TShapeThing;
var
    NewHighlightIcon:Integer;
begin
    X:=(X div 16)-WinPos.X;
    Y:=(Y div 16)-WinPos.Y;
    if (X>=0) and (X<WinSize.X) and (Y>=1) and (Y<WinSize.Y) then
    begin
        NewHighlightIcon:=ScrollIcon+(Y-1)*WinSize.X+X;
        if NewHighlightIcon<IconList^[ScrollList].Length then
        begin
            HighlightList:=ScrollList;
            HighlightIcon:=NewHighlightIcon;
            Click:=IconList^[HighlightList].Icon^[HighlightIcon];
        end else Click:=$FFFF;
    end else Click:=$FFFF;
end;

procedure TToolBar.RenderIcon(ShapeThing:TShapeThing;var Target:TMedSprite);
var
    A:TActiveSprite;
begin
    case ShapeThing of
        $0000..$05FF: Target:=SClass[0]^.Sprite^[ShapeThing shr 5];
        $0600..$17FF: Target:=SClass[1]^.Sprite^[(ShapeThing-$0600) shr 5];
        $1800..$2FFF: Target:=SClass[2]^.Sprite^[(ShapeThing-$1800) shr 5];
        $3000..$3059: begin
            A:=ToolBarSprite[ShapeThing];
            case A.Mode of
                atSolid11S: begin
                    Target:=TranslateSpriteIndex(A.Data[0])^;
                    MergeChar(A.Data[1],Target);
                end;
                atSolid11D: begin
                    Target:=TranslateSpriteIndex(A.Data[0])^;
                    Target.Merge(TranslateSpriteIndex(A.Data[1]));
                end;
            end;
        end;
    end;
end;

destructor TToolBar.Done;
var
    I:Word;
begin
    for I:=0 to IconListSize-1 do
        FreeMem(IconList^[I].Icon,SizeOf(Word)*IconList^[I].Size);
    FreeMem(IconList,SizeOf(TIconList)*IconListSize);
end;

end.
