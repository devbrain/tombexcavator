unit LevelMarker;

{ Marking grid system for levels: Each TLevelMarker object maintains a }
{ 128x90x1-bit array. Set, reset, test and clear operations are provided. }

interface

type
{ Method description: }
{   GetMarker: Returns true or false if bit at (X,Y) is set/clear }
{   SetMarker: Sets/Clears bit at (X,Y) }
{   Clear: Sets entire array to zero }
    PLevelMarker=^TLevelMarker;
    TLevelMarker=object
        public
            function GetMarker(X,Y:Integer):Boolean;
            procedure SetMarker(X,Y:Integer;B:Boolean);
            procedure Clear;
        private
            Grid:array[0..89,0..15] of Byte;
    end;

implementation

{ --- TLevelMarker methods ------------------------------------------------ }

procedure TLevelMarker.Clear;
var
    X,Y:Integer;
begin
    for Y:=0 to 89 do for X:=0 to 15 do Grid[Y,X]:=0;
end;

function TLevelMarker.GetMarker(X,Y:Integer):Boolean;
begin
    if (X>=0) and (X<=127) and (Y>=0) and (Y<=127) then
        GetMarker:=(Grid[Y,X shr 3] and (1 shl (X and $07)))<>$00
        else GetMarker:=False;
end;

procedure TLevelMarker.SetMarker(X,Y:Integer;B:Boolean);
begin
    if (X>=0) and (X<=127) and (Y>=0) and (Y<=127) then
    begin
        case B of
            True: Grid[Y,X shr 3]:=Grid[Y,X shr 3] or (1 shl (X and $07));
            False: Grid[Y,X shr 3]:=Grid[Y,X shr 3] and not (1 shl (X and $07));
        end;
    end;
end;

end.
