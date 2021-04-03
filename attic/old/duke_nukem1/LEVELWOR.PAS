unit LevelWorkSpace;

{ The LevelWorkSpace object is designed to provide editing operations on }
{ levels. Note that this object has nothing to do with screen operations. }

interface

uses LevelManager,LevelMarker;

type
{ Init: Creates new blank workspace }
{ Plot: Plots a single cell }
{ Remove: Erases a single cell }
{ Mark: Mark/Unmark/Toggle a single cell }
{ MarkRange: Mark/Unmark/Toggle a rectangular range of cells }
{ SetAnchor: Sets the anchor point (by default it is the first marked point) }
{ UnmarkAll: Clears all marks }
{ FillRange: Fills a selected range with a cell value }
{ FillSelection: Fills selection with with a cell value }
{ Copy: Copies selection from source workspace, clears selection }
{ Move: Moves selection from target workspace, clears selection }

    TMarkMode=(mmMark,mmUnmark,mmToggle);

    PWorkSpace=^TWorkSpace;
    TWorkSpace=object
        public
            ViewPos:TLevelCoord;
            Level:PLevel;
            SelectionEnabled:Boolean;
            Selection:PLevelMarker;
            Modified,Named:Boolean;
            Anchor:TLevelCoord;
            constructor Init;
            procedure Plot(X,Y:Integer;ShapeThing:TShapeThing);
            procedure Remove(X,Y:Integer);
            procedure Mark(X,Y:Integer;MarkMode:TMarkMode);
            procedure MarkRange(X1,Y1,X2,Y2:Integer;MarkMode:TMarkMode);
            procedure SetAnchor(X,Y:Integer);
            procedure UnmarkAll;
            procedure FillRange(X1,Y1,X2,Y2:Integer;ShapeThing:TShapeThing);
            procedure FillSelection(ShapeThing:TShapeThing);
            procedure Copy(Source:PWorkSpace;X,Y:Integer);
            procedure Move(Source:PWorkSpace;X,Y:Integer);
            function Save:TLevelError;
            function SaveAsFile(FileName:String):TLevelError;
            function SaveAsLevel(EpisodeIndex:TEpisodeIndex;LevelIndex:TLevelIndex):TLevelError;
            function LoadFile(FileName:String):TLevelError;
            function LoadLevel(EpisodeIndex:TEpisodeIndex;LevelIndex:TLevelIndex):TLevelError;
            function Reload:TLevelError;
            procedure CreateNew;
            destructor Done;
        private
            procedure SelectionCheck;
            procedure SelectionInitCheck(X,Y:Integer);
    end;

implementation

constructor TWorkSpace.Init;
begin
    New(Level,Init);
    New(Selection);
    Modified:=False;
    Named:=False;
    SelectionEnabled:=False;
    Level^.Clear;
end;

procedure TWorkSpace.Plot(X,Y:Integer;ShapeThing:TShapeThing);
begin
    Level^.Grid[Y,X]:=ShapeThing;
    Modified:=True;
end;

procedure TWorkSpace.Remove(X,Y:Integer);
begin
    Level^.Grid[Y,X]:=Level^.GetRestoreValue(X,Y);
    Modified:=True;
end;

procedure TWorkSpace.Mark(X,Y:Integer;MarkMode:TMarkMode);
begin
    case MarkMode of
        mmMark: begin
            SelectionInitCheck(X,Y);
            Selection^.SetMarker(X,Y,True);
        end;
        mmUnmark: Selection^.SetMarker(X,Y,False);
        mmToggle: begin
            SelectionInitCheck(X,Y);
            Selection^.SetMarker(X,Y,not Selection^.GetMarker(X,Y));
        end;
    end;
    SelectionCheck;
end;

procedure TWorkSpace.MarkRange(X1,Y1,X2,Y2:Integer;MarkMode:TMarkMode);
var
    Current:TLevelCoord;
begin
    if MarkMode in [mmMark,mmToggle] then SelectionInitCheck(X1,Y1);
    for Current.Y:=Y1 to Y2 do
        for Current.X:=X1 to X2 do
            case MarkMode of
                mmMark: Selection^.SetMarker(Current.X,Current.Y,True);
                mmUnmark: Selection^.SetMarker(Current.X,Current.Y,False);
                mmToggle: Selection^.SetMarker(Current.X,Current.Y,
                    not Selection^.GetMarker(Current.X,Current.Y));
            end;
    SelectionCheck;
end;

procedure TWorkSpace.SetAnchor(X,Y:Integer);
begin
    Anchor.X:=X;
    Anchor.Y:=Y;
end;

procedure TWorkSpace.UnmarkAll;
var
    Current:TLevelCoord;
begin
    SelectionEnabled:=False;
end;

procedure TWorkSpace.SelectionCheck;
var
    Current:TLevelCoord;
    SelectionStatus:Boolean;
begin
    SelectionStatus:=False;
    for Current.Y:=0 to 89 do
        for Current.X:=0 to 127 do
            SelectionStatus:=SelectionStatus or Selection^.GetMarker(Current.X,Current.Y);
    if not SelectionStatus then SelectionEnabled:=False;
end;

procedure TWorkSpace.SelectionInitCheck(X,Y:Integer);
begin
    if not SelectionEnabled then
    begin
        Selection^.Clear;
        Anchor.X:=X;
        Anchor.Y:=Y;
        SelectionEnabled:=True;
    end;
end;

procedure TWorkSpace.FillRange(X1,Y1,X2,Y2:Integer;ShapeThing:TShapeThing);
var
    Current:TLevelCoord;
begin
    for Current.Y:=Y1 to Y2 do
        for Current.X:=X1 to X2 do
            Level^.Grid[Current.Y,Current.X]:=ShapeThing;
    Modified:=True;
end;

procedure TWorkSpace.FillSelection(ShapeThing:TShapeThing);
var
    L:TLevelCoord;
begin
    if SelectionEnabled then
    begin
        for L.Y:=0 to 89 do
            for L.X:=0 to 127 do
                if Selection^.GetMarker(L.X,L.Y) then
                    Level^.Grid[L.Y,L.X]:=ShapeThing;
        Modified:=True;
    end;
end;

procedure TWorkSpace.Copy(Source:PWorkSpace;X,Y:Integer);
var
    SourceCell,TargetCell:TLevelCoord;
begin
    if Source^.SelectionEnabled then
    begin
        Selection^.Clear;
        SelectionEnabled:=True;
        Anchor.X:=X;
        Anchor.Y:=Y;
        for SourceCell.Y:=0 to 89 do
            for SourceCell.X:=0 to 127 do
                if Source^.Selection^.GetMarker(SourceCell.X,SourceCell.Y) then
                begin
                    TargetCell.X:=SourceCell.X+X-Source^.Anchor.X;
                    TargetCell.Y:=SourceCell.Y+Y-Source^.Anchor.Y;
                    if ValidLevelCoord(TargetCell.X,TargetCell.Y) then
                    begin
                        Level^.Grid[TargetCell.Y,TargetCell.X]:=
                            Source^.Level^.Grid[SourceCell.Y,SourceCell.X];
                        Selection^.SetMarker(TargetCell.X,TargetCell.Y,True);
                    end;
                end;
        Source^.SelectionEnabled:=False;
    end;
    Modified:=True;
end;

procedure TWorkSpace.Move(Source:PWorkSpace;X,Y:Integer);
var
    SourceCell,TargetCell:TLevelCoord;
begin
    if Source^.SelectionEnabled then
    begin
        Selection^.Clear;
        SelectionEnabled:=True;
        Anchor.X:=X;
        Anchor.Y:=Y;
        for SourceCell.Y:=0 to 89 do
            for SourceCell.X:=0 to 127 do
                if Source^.Selection^.GetMarker(SourceCell.X,SourceCell.Y) then
                begin
                    TargetCell.X:=SourceCell.X+X-Source^.Anchor.X;
                    TargetCell.Y:=SourceCell.Y+Y-Source^.Anchor.Y;
                    if ValidLevelCoord(TargetCell.X,TargetCell.Y) then
                    begin
                        Level^.Grid[TargetCell.Y,TargetCell.X]:=
                            Source^.Level^.Grid[SourceCell.Y,SourceCell.X];
                        Source^.Level^.Grid[SourceCell.Y,SourceCell.X]:=$0000;
                        Selection^.SetMarker(TargetCell.X,TargetCell.Y,True);
                    end;
                end;
        Source^.SelectionEnabled:=False;
    end;
    Modified:=True;
    Source^.Modified:=True;
end;

function TWorkSpace.Save:TLevelError;
var
    Error:TLevelError;
begin
    Error:=Level^.Save;
    if Error=leOk then Modified:=False;
    Save:=Error;
end;

function TWorkSpace.SaveAsFile(FileName:String):TLevelError;
var
    Error:TLevelError;
    OldFileName:String;
    OldEpisodeIndex:TEpisodeIndex;
    OldLevelIndex:TLevelIndex;
begin
    OldFileName:=Level^.FileName;
    OldEpisodeIndex:=Level^.EpisodeIndex;
    OldLevelIndex:=Level^.LevelIndex;
    Level^.AssignFileName(FileName);
    Error:=Level^.Save;
    if Error=leOk then
    begin
        Modified:=False;
        Named:=True;
    end else begin
        if (OldEpisodeIndex=0) and (OldLevelIndex=0) then Level^.AssignFileName(OldFileName)
            else Level^.AssignLevel(OldEpisodeIndex,OldLevelIndex);
    end;
    SaveAsFile:=Error;
end;

function TWorkSpace.SaveAsLevel(EpisodeIndex:TEpisodeIndex;LevelIndex:TLevelIndex):TLevelError;
var
    Error:TLevelError;
    OldFileName:String;
    OldEpisodeIndex:TEpisodeIndex;
    OldLevelIndex:TLevelIndex;
begin
    OldFileName:=Level^.FileName;
    OldEpisodeIndex:=Level^.EpisodeIndex;
    OldLevelIndex:=Level^.LevelIndex;
    Level^.AssignLevel(EpisodeIndex,LevelIndex);
    Error:=Level^.Save;
    if Error=leOk then
    begin
        Modified:=False;
        Named:=True;
    end else begin
        if (OldEpisodeIndex=0) and (OldLevelIndex=0) then Level^.AssignFileName(OldFileName)
            else Level^.AssignLevel(OldEpisodeIndex,OldLevelIndex);
    end;
    SaveAsLevel:=Error;
end;

function TWorkSpace.LoadFile(FileName:String):TLevelError;
var
    Error:TLevelError;
    OldFileName:String;
    OldEpisodeIndex:TEpisodeIndex;
    OldLevelIndex:TLevelIndex;
begin
    OldFileName:=Level^.FileName;
    OldEpisodeIndex:=Level^.EpisodeIndex;
    OldLevelIndex:=Level^.LevelIndex;
    Level^.AssignFileName(FileName);
    Error:=Level^.Load;
    if Error=leOk then
    begin
        SelectionEnabled:=False;
        Modified:=False;
        Named:=True;
    end else begin
        if (OldEpisodeIndex=0) and (OldLevelIndex=0) then Level^.AssignFileName(OldFileName)
            else Level^.AssignLevel(OldEpisodeIndex,OldLevelIndex);
    end;
    LoadFile:=Error;
end;

function TWorkSpace.LoadLevel(EpisodeIndex:TEpisodeIndex;LevelIndex:TLevelIndex):TLevelError;
var
    Error:TLevelError;
    OldFileName:String;
    OldEpisodeIndex:TEpisodeIndex;
    OldLevelIndex:TLevelIndex;
begin
    OldFileName:=Level^.FileName;
    OldEpisodeIndex:=Level^.EpisodeIndex;
    OldLevelIndex:=Level^.LevelIndex;
    Level^.AssignLevel(EpisodeIndex,LevelIndex);
    Error:=Level^.Load;
    if Error=leOk then
    begin
        SelectionEnabled:=False;
        Modified:=False;
        Named:=True;
    end else begin
        if (OldEpisodeIndex=0) and (OldLevelIndex=0) then Level^.AssignFileName(OldFileName)
            else Level^.AssignLevel(OldEpisodeIndex,OldLevelIndex);
    end;
    LoadLevel:=Error;
end;

function TWorkSpace.Reload:TLevelError;
var
    Error:TLevelError;
begin
    Error:=Level^.Load;
    if Error=leOk then
    begin
        SelectionEnabled:=False;
        Modified:=False;
        Named:=True;
    end;
    Reload:=Error;
end;

procedure TWorkSpace.CreateNew;
begin
    Modified:=False;
    Named:=False;
    SelectionEnabled:=False;
    Level^.Clear;
    Level^.AssignLevel(0,0);
end;

destructor TWorkSpace.Done;
begin
    Dispose(Selection);
    Dispose(Level);
end;

end.
