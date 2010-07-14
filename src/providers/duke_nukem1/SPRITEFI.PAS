unit SpriteFile;

{$I-}

{ Sprite file I/O unit for DN1 editor }

interface

uses SpriteClass,GSprite;

type
{ Abstract sprite file object }
    TSpriteFile=object
        InputFile:File;
        Size:Byte;
        constructor Init(InputFileName:String);
        destructor Done;
    end;

{ Sprite file objects (objects that manage sprite files) }

{ InputFile: Input file variable }
{ Size: Number of sprites in file }
{ Init: Opens given filename. }
{   Note: Give filename only, will automatically append .DNx to it }
{   The x is automatically determined by trying to open .DN1, then .DN2,}
{   then .DN3, and attempts stop if succesful. Otherwise the constructor }
{   will fail. }
{ Load: Adds a range of sprites from this file to a sprite class }
{ Done: Closes the file }
    PSmallSpriteFile=^TSmallSpriteFile;
    TSmallSpriteFile=object(TSpriteFile)
        constructor Init(InputFileName:String);
        procedure Load(Start,Stop:Byte;Dest:PSmallSpriteClass);
    end;

    PMedSpriteFile=^TMedSpriteFile;
    TMedSpriteFile=object(TSpriteFile)
        constructor Init(InputFileName:String);
        procedure Load(Start,Stop:Byte;Dest:PMedSpriteClass);
    end;

var
    Path:array[1..3] of String;

implementation

{ --- Internal structures ------------------------------------------------- }

type
{ Header of sprite files }
    TSpriteFileHeader=record
        Count,X,Y:Byte;
    end;

{ --- TSpriteFile methods ------------------------------------------------- }

constructor TSpriteFile.Init(InputFileName:String);
var
    Episode:Byte;
begin
    Episode:=1;
    while Episode<=3 do
    begin
        Assign(InputFile,Path[Episode]+InputFileName+'.DN'+Chr(Episode+48));
        Reset(InputFile,1);
        if IOResult=0 then Break else Inc(Episode);
    end;
    if Episode=4 then Fail;
end;

destructor TSpriteFile.Done;
begin
    Close(InputFile);
end;

{ --- TSmallSpriteFile methods -------------------------------------------- }

constructor TSmallSpriteFile.Init(InputFileName:String);
var
    Header:TSpriteFileHeader;
begin
    if inherited Init(InputFileName) then
    begin
        BlockRead(InputFile,Header,SizeOf(Header));
        if (Header.X=1) and (Header.Y=8) then Size:=Header.Count else Fail;
    end else Fail;
end;

procedure TSmallSpriteFile.Load(Start,Stop:Byte;Dest:PSmallSpriteClass);
var
    I:Word;
    S:TSmallSprite;
begin
    Seek(InputFile,3+Start*40);
    for I:=Start to Stop do
    begin
        S.Load(InputFile);
        Dest^.Add(@S);
    end;
end;

{ --- TMedSpriteFile methods ---------------------------------------------- }

constructor TMedSpriteFile.Init(InputFileName:String);
var
    Header:TSpriteFileHeader;
begin
    if inherited Init(InputFileName) then
    begin
        BlockRead(InputFile,Header,SizeOf(Header));
        if (Header.X=2) and (Header.Y=16) then Size:=Header.Count
            else if (Header.X=0) and (Header.Y=0) then Size:=130
                else Fail;
    end else Fail;
end;

procedure TMedSpriteFile.Load(Start,Stop:Byte;Dest:PMedSpriteClass);
var
    I:Word;
    S:TMedSprite;
begin
    Seek(InputFile,3+Start*160);
    for I:=Start to Stop do
    begin
        S.Load(InputFile);
        Dest^.Add(@S);
    end;
end;

end.
