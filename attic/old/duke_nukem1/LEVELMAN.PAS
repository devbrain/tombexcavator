unit LevelManager;

{ Level file I/O interface for DN1 editor }
{ Also provides a couple of useful level functions }

interface

type
{ Episode code mappings: }
{   0:  Custom level }
{   1:  Shrapnel City (.DN1) }
{   2:  Mission Moonbase (.DN2) }
{   3:  Trapped in the future (.DN3) }
    TEpisodeIndex=0..3;

{ Level code mappings for each episode: }
{   0:      User defined level }
{   1..10:  Levels 1..10 (WORLDAL1, WORLDAL3..WORLDALB) }
{   11:     Intermission level (WORLDAL2) }
{   12:     Demo level (WORLDALC) }
    TLevelIndex=0..12;

{ Level coordinates }
    TLevelCoord=record
        X,Y:Integer;
    end;

{ Level error codes }
    TLevelError=(leOk,leBadFileName,leCorruptedFile,leNoSpace);

{ Shape & thing codes for level cells }
    TShapeType=(stNone,stFlasher,stBack,stSolid,stActive);
    TShapeThing=Word;

{ Sprite pulling direction codes }
    TPullMode=(pmNone,pmLeft,pmRight,pmUp,pmDown,pmThru);

{ TLevel object }
    PLevel=^TLevel;
    TLevel=object
        public
        { --- Level data fields --- }
            Grid:array[0..89,0..127] of TShapeThing;
            FileName:String;
            EpisodeIndex:TEpisodeIndex;
            LevelIndex:TLevelIndex;

        { --- Level I/O related functions --- }
            constructor Init;
            procedure AssignFileName(NewFileName:String);
            procedure AssignLevel(NewEpisodeIndex:TEpisodeIndex;NewLevelIndex:TLevelIndex);
            function Load:TLevelError;
            function Save:TLevelError;

        { --- Other utility functions --- }
        { Clear: Clears whole grid, sets all elements to zero }
        { GetPullValue: Returns the background sprite for the object at the }
        {   specified grid location }
        { GetClipValue: Returns cell value at (X,Y) unless (X,Y) is out of }
        {   bounds, in which case the value Error is returned instead }
        { GetShapeValue: Returns type of value at cell (X,Y) }
        { GetRestoreValue: Returns background value for active sprites and }
        {   zero for anything else, effectively deleting the sprite at (X,Y) }
            procedure Clear;
            function GetPullValue(X,Y:Integer):TShapeThing;
            function GetClipValue(X,Y:Integer;Error:TShapeThing):TShapeThing;
            function GetShapeValue(X,Y:Integer):TShapeType;
            function GetRestoreValue(X,Y:Integer):TShapeThing;
    end;

var
    Path:array[1..3] of String;

{ Returns validity of X,Y coordinate pair }
function ValidLevelCoord(X,Y:Integer):Boolean;

const
{$I LEVELMAN.INC}

implementation

const
    EpisodeIndexChar:array[Low(TEpisodeIndex)..High(TEpisodeIndex)] of Char='X123';
    LevelIndexChar:array[Low(TLevelIndex)..High(TLevelIndex)]of Char='X13456789AB2C';

function ValidLevelCoord(X,Y:Integer):Boolean;
begin
    ValidLevelCoord:=(X>=0) and (X<128) and (Y>=0) and (Y<90);
end;

constructor TLevel.Init;
begin
    Clear;
    AssignFileName('');
end;

procedure TLevel.AssignFileName(NewFileName:String);
begin
    FileName:=NewFileName;
    EpisodeIndex:=0;
    LevelIndex:=0;
end;

procedure TLevel.AssignLevel(NewEpisodeIndex:TEpisodeIndex;NewLevelIndex:TLevelIndex);
begin
    if NewEpisodeIndex in [1..3] then
        FileName:=Path[NewEpisodeIndex]+'WORLDAL'+LevelIndexChar[NewLevelIndex]+'.DN'+EpisodeIndexChar[NewEpisodeIndex]
    else FileName:='WORLDAL'+LevelIndexChar[NewLevelIndex]+'.DNX';
    EpisodeIndex:=NewEpisodeIndex;
    LevelIndex:=NewLevelIndex;
end;

function TLevel.Load:TLevelError;
var
    InputFile:File;
    Result:Word;
begin
    Assign(InputFile,FileName);
    Reset(InputFile,1);
    if IOResult=0 then
    begin
        BlockRead(InputFile,Grid,SizeOf(Grid),Result);
        Close(InputFile);
        if Result=SizeOf(Grid) then Load:=leOk else Load:=leCorruptedFile;
    end else Load:=leBadFileName;
end;

function TLevel.Save:TLevelError;
var
    OutputFile:File;
    Result:Word;
begin
    Assign(OutputFile,FileName);
    Rewrite(OutputFile,1);
    if IOResult=0 then
    begin
        BlockWrite(OutputFile,Grid,SizeOf(Grid),Result);
        Close(OutputFile);
        if Result=SizeOf(Grid) then Save:=leOk else Save:=leNoSpace;
    end else Save:=leBadFileName;
end;

procedure TLevel.Clear;
var
    Current:TLevelCoord;
begin
    for Current.Y:=0 to 89 do
        for Current.X:=0 to 127 do
            Grid[Current.Y,Current.X]:=0;
end;

{ Try and resolve background values for specified grid cell. For inactive }
{ objects, the background value is the cell value itself. For active }
{ objects, the pull values are used to try and trace around the level grid }
{ until a inactive object is found. A recursive method is used. If the }
{ trace process ends up going off the edge of the level, the warning sprite }
{ ($FFFF) should be used. Cells that are tested are temporarily marked as }
{ $FFFF to prevent infinite recursion. }

function TLevel.GetPullValue(X,Y:Integer):TShapeThing;
var
    SaveShapeThing:TShapeThing;
begin
    case Grid[Y,X] of
        $0000..$2FFF: GetPullValue:=Grid[Y,X];
        $3000..$3059: begin
            SaveShapeThing:=Grid[Y,X];
            Grid[Y,X]:=$FFFF;
            GetPullValue:=$FFFF;
            case PullMode[SaveShapeThing] of
                pmNone: GetPullValue:=SaveShapeThing;
                pmLeft: if X>0 then GetPullValue:=GetPullValue(X-1,Y);
                pmRight: if X<127 then GetPullValue:=GetPullValue(X+1,Y);
                pmUp: if Y>0 then GetPullValue:=GetPullValue(X,Y-1);
                pmDown: if Y<89 then GetPullValue:=GetPullValue(X,Y+1);
                pmThru: GetPullValue:=$0000;
            end;
            Grid[Y,X]:=SaveShapeThing;
        end;
        else GetPullValue:=$FFFF;
    end;
end;

function TLevel.GetClipValue(X,Y:Integer;Error:TShapeThing):TShapeThing;
begin
    if ValidLevelCoord(X,Y) then GetClipValue:=Grid[Y,X]
        else GetClipValue:=Error;
end;

function TLevel.GetShapeValue(X,Y:Integer):TShapeType;
begin
    if ValidLevelCoord(X,Y) then case Grid[Y,X] of
        $0000..$05FF: GetShapeValue:=stFlasher;
        $0600..$17FF: GetShapeValue:=stBack;
        $1800..$2FFF: GetShapeValue:=stSolid;
        $3000..$3059: GetShapeValue:=stActive;
        else GetShapeValue:=stNone;
    end else GetShapeValue:=stNone;
end;

function TLevel.GetRestoreValue(X,Y:Integer):TShapeThing;
var
    ShapeThing:TShapeThing;
begin
    if ValidLevelCoord(X,Y) then
    begin
        case Grid[Y,X] of
            $3000..$3059: begin
                ShapeThing:=GetPullValue(X,Y);
                if ShapeThing=$FFFF then GetRestoreValue:=$0000 else
                    GetRestoreValue:=ShapeThing;
            end;
            else GetRestoreValue:=$0000;
        end;
    end else GetRestoreValue:=$0000;
end;

end.
