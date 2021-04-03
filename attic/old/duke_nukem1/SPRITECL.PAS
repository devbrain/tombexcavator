unit SpriteClass;

{ Sprite class objects for DN1 editor }

interface

uses GSprite;

type
{ Arrays of pointers to sprites }
    PSmallSpriteArray=^TSmallSpriteArray;
    PMedSpriteArray=^TMedSpriteArray;
    TSmallSpriteArray=array[0..255] of TSmallSprite;
    TMedSpriteArray=array[0..255] of TMedSprite;

{ Sprite classes (objects that hold arrays of sprites) }
{ Sprite: Pointer to array of sprites (Max 255) }
{ Size: Maximum size of sprite array }
{ Length: Number of allocated sprites in sprite array }
{ Init: Allocates sprite array }
{ Add: Adds a sprite object }
{ Reset: Clears away all added sprite objects }
{ Done: Deallocates sprite array }
    PSmallSpriteClass=^TSmallSpriteClass;
    TSmallSpriteClass=object
        Sprite:PSmallSpriteArray;
        Size,Length:Word;
        constructor Init(NewSize:Word);
        procedure Add(Source:PSmallSprite);
        procedure Reset;
        destructor Done;
    end;

    PMedSpriteClass=^TMedSpriteClass;
    TMedSpriteClass=object
        Sprite:PMedSpriteArray;
        Size,Length:Word;
        constructor Init(NewSize:Word);
        procedure Add(Source:PMedSprite);
        procedure Reset;
        destructor Done;
    end;

implementation

{ --- TSmallSpriteClass methods ------------------------------------------- }

constructor TSmallSpriteClass.Init(NewSize:Word);
begin
    GetMem(Sprite,NewSize*SizeOf(TSmallSprite));
    Size:=NewSize;
    Length:=0;
end;

procedure TSmallSpriteClass.Add(Source:PSmallSprite);
begin
    if Length<Size then
    begin
        Sprite^[Length]:=Source^;
        Inc(Length);
    end;
end;

procedure TSmallSpriteClass.Reset;
begin
    Length:=0;
end;

destructor TSmallSpriteClass.Done;
begin
    FreeMem(Sprite,Size*SizeOf(TSmallSprite));
end;

{ --- TMedSpriteClass methods --------------------------------------------- }

constructor TMedSpriteClass.Init(NewSize:Word);
begin
    GetMem(Sprite,NewSize*SizeOf(TMedSprite));
    Size:=NewSize;
    Length:=0;
end;

procedure TMedSpriteClass.Add(Source:PMedSprite);
begin
    if Length<Size then
    begin
        Sprite^[Length]:=Source^;
        Inc(Length);
    end;
end;

procedure TMedSpriteClass.Reset;
begin
    Length:=0;
end;

destructor TMedSpriteClass.Done;
begin
    FreeMem(Sprite,Size*SizeOf(TMedSprite));
end;

end.
