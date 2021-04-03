unit SpriteManager;

{ Sprite manager unit for DN1 editor }

interface

uses SpriteClass,SpriteFile,GSprite;

{ Sprite class indexes: }
{   $00: Empty/Flasher: BACK0 ($0000-$05FF, 48 sprites) }
{   $01: Backgrounds: BACK1..BACK3 ($0600-$17FF, 144 sprites) }
{   $02: Solids: SOLID0..SOLID3 ($1800-$2FFF, 192 sprites) }
{   $03: Man sprites (MAN0, 4 sprites) }
{   $04: Animation Sprites (ANIM0..ANIM5), 84 sprites }
{   $05: Object Sprites (OBJECT0..OBJECT3), 88 sprites }
{   $06: Extra user-defined inbuilt sprites, 32 sprites }

{ Sprite size notation: [X*Y*Frames] }

{ Backdrop indexes (DROP# file): }
{   0:  Blue/grey skyline, river in middle, broken road in front }
{   1:  Cave with lava falls and bubble cities }
{   2:  Industrial plant, blue floor, loading hook, large tower }
{   3:  Blue rock cavern }
{   4:  Not used (invalid) }
{   5:  Spaceport 10 with blue spaceship }
{   6:  Black (no associated filename) }
{   7:  Cyan mountains in back, industrial buildings in front }
{   8:  Moonscape with Earth in distance }
{   9:  Inside moon cavern with view of Earth }
{   10: Future city on green hills }
{   11: Starfield }
{   12: Edge of moon mountain with buildings, rail and lamp }
{   13: Blue sky with clouds and space ships }
type
    TBackdropIndex=0..13;

const
{$I SPRITEMA.INC}

    BackdropKey:array[0..3,0..12,0..1] of TBackdropIndex=(
        ((0,3),(0,3),(0,3),(7,3),(3,5),(7,7),(2,2),(1,2),(3,3),(5,2),(0,11),(0,3),(13,9)),
        ((0,3),(0,3),(0,3),(7,3),(3,5),(7,7),(2,2),(1,2),(3,3),(5,2),(0,11),(0,3),(13,9)),
        ((8,9),(8,9),(9,9),(8,5),(11,2),(3,9),(3,3),(5,8),(1,1),(5,2),(8,11),(8,9),(13,9)),
        ((0,10),(0,10),(10,3),(6,5),(12,12),(12,13),(1,1),(0,0),(2,6),(2,2),(13,2),(0,10),(13,9)));

    SpriteManagerOk:Boolean=False;

var
    SClass:array[0..6] of PMedSpriteClass;
    Backdrop:array[0..1] of PMedSpriteClass;
    Font:PSmallSpriteClass;

    AnimPulse:array[1..31] of Byte;
    FontMap:array[0..255] of Byte;

procedure SpriteManagerInit;
procedure SpriteManagerDone;
procedure LoadBackdrop(Primary,Secondary:TBackdropIndex);
function RunAnimPulse:Boolean;
procedure PrintChar(X,Y:Integer;Colour:Byte;Ch:Char);
function TranslateSpriteIndex(SpriteIndex:Word):PMedSprite;
function TranslateAnimPulse(Anim:Word):Word;

implementation

var
    CurrentBackdrop:array[0..1] of TBackdropIndex;

    SystemTimer:Longint absolute $0040:$006C;
    SystemTimerSave:Longint;

    Merge4Target:TMedSprite;

procedure LoadBackdrop(Primary,Secondary:TBackdropIndex);
var
    InputFile:TMedSpriteFile;
    BackdropString:String;
    I:Word;
begin
    if CurrentBackdrop[0]<>Primary then
    begin
        Backdrop[0]^.Reset;
        if Primary<>6 then
        begin
            Str(Primary,BackdropString);
            InputFile.Init('DROP'+BackdropString);
            InputFile.Load(0,129,Backdrop[0]);
            InputFile.Done;
        end else for I:=0 to 129 do Backdrop[0]^.Add(@smBlack);
        CurrentBackdrop[0]:=Primary;
    end;
    if CurrentBackdrop[1]<>Secondary then
    begin
        Backdrop[1]^.Reset;
        if Secondary<>6 then
        begin
            Str(Secondary,BackdropString);
            InputFile.Init('DROP'+BackdropString);
            InputFile.Load(0,129,Backdrop[1]);
            InputFile.Done;
        end else for I:=0 to 129 do Backdrop[1]^.Add(@smBlack);
        CurrentBackdrop[1]:=Secondary;
    end;
end;

function TranslateSpriteIndex(SpriteIndex:Word):PMedSprite;
begin
    TranslateSpriteIndex:=@SClass[Hi(SpriteIndex)]^.Sprite^[Lo(SpriteIndex)];
end;

function TranslateAnimPulse(Anim:Word):Word;
var
    Delay:Byte;
begin
    Delay:=(Anim and $30) shr 4+1;
    TranslateAnimPulse:=AnimPulse[(Anim and $0F)*Delay] div Delay;
end;

function RunAnimPulse:Boolean;
var
    I:Byte;
begin
    if SystemTimer<>SystemTimerSave then
    begin
        for I:=2 to High(AnimPulse) do AnimPulse[I]:=(AnimPulse[I]+1) mod I;
        SystemTimerSave:=SystemTimer;
        RunAnimPulse:=True;
    end else RunAnimPulse:=False;
end;

procedure PrintChar(X,Y:Integer;Colour:Byte;Ch:Char);
var
    Source,Target:TSmallSprite;
    L,C:Byte;
begin
    for C:=0 to 3 do
        if (Colour and ($10 shl C))=($10 shl C) then
            for L:=0 to 7 do Target.Image[C,L]:=$FF
                else for L:=0 to 7 do Target.Image[C,L]:=$00;
    Source:=Font^.Sprite^[FontMap[Ord(Ch)]];
    for C:=0 to 3 do
        if (Colour and ($01 shl C))<>($01 shl C) then
            for L:=0 to 7 do Source.Image[C,L]:=$00;
    Target.Merge(@Source);
    Target.Draw(X,Y);
end;

procedure SpriteManagerInit;
var
    InputFile:TMedSpriteFile;
    SmallFile:TSmallSpriteFile;
    I:Word;
    function InitMedFile(var InputFile:TMedSpriteFile;InputFileName:String):Boolean;
    begin
        if not InputFile.Init(InputFileName) then
        begin
            Writeln('Unable to find file ',InputFileName,'.DN?');
            Writeln('Check the DNxPATH entries in the DN1MOD.CFG file');
            SpriteManagerDone;
            InitMedFile:=False;
        end else InitMedFile:=True;
    end;
    function InitSmallFile(var InputFile:TSmallSpriteFile;InputFileName:String):Boolean;
    begin
        if not InputFile.Init(InputFileName) then
        begin
            Writeln('Unable to find file ',InputFileName,'.DN?');
            Writeln('Check the DNxPATH entries in the DN1MOD.CFG file');
            SpriteManagerDone;
            InitSmallFile:=False;
        end else InitSmallFile:=True;
    end;
    function Merge4(Source1,Source2,Source3,Source4:Word):PMedSprite;
    var
        Micro:TSmallSprite;
    begin
        Micro.Reduce(TranslateSpriteIndex(Source1));
        Merge4Target.Paste(@Micro,0,0);
        Micro.Reduce(TranslateSpriteIndex(Source2));
        Merge4Target.Paste(@Micro,1,0);
        Micro.Reduce(TranslateSpriteIndex(Source3));
        Merge4Target.Paste(@Micro,0,1);
        Micro.Reduce(TranslateSpriteIndex(Source4));
        Merge4Target.Paste(@Micro,1,1);
        Merge4:=@Merge4Target;
    end;
begin
    New(SClass[0],Init(48));
    New(SClass[1],Init(144));
    New(SClass[2],Init(192));
    New(SClass[3],Init(4));
    New(SClass[4],Init(84));
    New(SClass[5],Init(88));
    New(SClass[6],Init(32));
    New(Font,Init(105));

    if not InitMedFile(InputFile,'BACK0') then Exit;
    InputFile.Load(0,47,SClass[0]); InputFile.Done;
    if not InitMedFile(InputFile,'BACK1') then Exit;
    InputFile.Load(0,47,SClass[1]); InputFile.Done;
    if not InitMedFile(InputFile,'BACK2') then Exit;
    InputFile.Load(0,47,SClass[1]); InputFile.Done;
    if not InitMedFile(InputFile,'BACK3') then Exit;
    InputFile.Load(0,47,SClass[1]); InputFile.Done;
    if not InitMedFile(InputFile,'SOLID0') then Exit;
    InputFile.Load(0,47,SClass[2]); InputFile.Done;
    if not InitMedFile(InputFile,'SOLID1') then Exit;
    InputFile.Load(0,47,SClass[2]); InputFile.Done;
    if not InitMedFile(InputFile,'SOLID2') then Exit;
    InputFile.Load(0,47,SClass[2]); InputFile.Done;
    if not InitMedFile(InputFile,'SOLID3') then Exit;
    InputFile.Load(0,47,SClass[2]); InputFile.Done;
    if not InitMedFile(InputFile,'MAN0') then Exit;
    InputFile.Load(24,27,SClass[3]); InputFile.Done;

    if not InitMedFile(InputFile,'ANIM0') then Exit;
    InputFile.Load(5,5,SClass[4]);   { Hover techbot upper half [1*2*1] }
    InputFile.Load(9,9,SClass[4]);   { Hover techbot lower half }
    InputFile.Load(22,25,SClass[4]); { Dreadnought techbot [2*2*1] }
    InputFile.Load(38,39,SClass[4]); { Car techbot [2*1*1] }
    InputFile.Done;                  { 8 sprites }

    if not InitMedFile(InputFile,'ANIM1') then Exit;
    InputFile.Load(12,15,SClass[4]); { Firewheel techbot [2*2*1] }
    InputFile.Load(32,34,SClass[4]); { Simple techbot [1*1*3] }
    InputFile.Done;                  { 7 sprites }

    if not InitMedFile(InputFile,'ANIM2') then Exit;
    InputFile.Load(0,3,SClass[4]);   { Exit door [2*2*1] }
    InputFile.Load(17,17,SClass[4]); { Dynamite [1*1*1] }
    InputFile.Load(24,31,SClass[4]); { Snake techbot [1*1*8] }
    InputFile.Load(32,35,SClass[4]); { Coke can [1*1*4] }
    InputFile.Load(44,47,SClass[4]); { Crawlie techbot (left) [1*1*4] }
    InputFile.Load(40,43,SClass[4]); { Crawlie techbot (right) [1*1*4] }
    InputFile.Done;                  { 25 sprites }

    if not InitMedFile(InputFile,'ANIM3') then Exit;
    InputFile.Load(0,1,SClass[4]);   { Breakable bridge [2*1*1] }
    InputFile.Load(12,13,SClass[4]); { Fan (left) [1*2*1] }
    InputFile.Load(44,47,SClass[4]); { Helicopter (upper half) [4*2*1] }
    InputFile.Load(24,27,SClass[4]); { Helicopter (lower half) }
    InputFile.Done;                  { 12 sprites }

    if not InitMedFile(InputFile,'ANIM4') then Exit;
    InputFile.Load(8,10,SClass[4]);  { Camera [1*1*3] }
    InputFile.Load(11,11,SClass[4]); { Stone with wires sticking out [1*1*1] }
    InputFile.Load(12,12,SClass[4]); { Half rock [1*1*1] }
    InputFile.Load(13,13,SClass[4]); { Cavern inside edge [1*1*1] }
    InputFile.Load(14,14,SClass[4]); { Prison window [1*1*1] }
    InputFile.Load(20,28,SClass[4]); { Teleporter [3*3*1] }
    InputFile.Load(31,31,SClass[4]); { Bounce-mine [1*1*1] }
    InputFile.Load(36,37,SClass[4]); { Bunny techbot [1*2*1] }
    InputFile.Done;                  { 19 sprites }

    if not InitMedFile(InputFile,'ANIM5') then Exit;
    InputFile.Load(6,7,SClass[4]);   { Fireball techbot [1*2*1] }
    InputFile.Load(12,12,SClass[4]); { Cyclone fence [1*1*1] }
    InputFile.Load(13,14,SClass[4]); { Metal window [2*1*1] }
    InputFile.Load(18,19,SClass[4]); { TV screen (upper half) [2*2*1] }
    InputFile.Load(16,17,SClass[4]); { TV screen (lower half) }
    InputFile.Load(36,39,SClass[4]); { Dr Proton [2*2*1] }
    InputFile.Done;                  { 13 sprites }

    if not InitMedFile(InputFile,'OBJECT0') then Exit;
    InputFile.Load(0,0,SClass[5]);   { Grey box [1*1*1] }
    InputFile.Load(5,5,SClass[5]);   { Elevator [1*1*1] }
    InputFile.Load(10,10,SClass[5]); { Jump boots [1*1*1] }
    InputFile.Load(11,11,SClass[5]); { Missile nose [1*1*1] }
    InputFile.Load(12,12,SClass[5]); { Missile shaft [1*1*1] }
    InputFile.Load(14,14,SClass[5]); { Missile left fin [1*1*1] }
    InputFile.Load(13,13,SClass[5]); { Missile base [1*1*1] }
    InputFile.Load(15,15,SClass[5]); { Missile right fin [1*1*1] }
    InputFile.Load(18,18,SClass[5]); { Grappling hooks [1*1*1] }
    InputFile.Load(29,29,SClass[5]); { Left flamethrower [1*1*1] }
    InputFile.Load(24,24,SClass[5]); { Right flamethrower [1*1*1] }
    InputFile.Load(43,43,SClass[5]); { Duke's gun powerup [1*1*1] }
    InputFile.Load(44,44,SClass[5]); { Turkey 1x [1*1*1] }
    InputFile.Done;                  { 13 sprites }

    if not InitMedFile(InputFile,'OBJECT1') then Exit;
    InputFile.Load(0,3,SClass[5]);   { Blue horizontal laser [1*1*4] }
    InputFile.Load(15,18,SClass[5]); { Blue vertical laser [1*1*4] }
    InputFile.Load(8,8,SClass[5]);   { Footy [1*1*1] }
    InputFile.Load(9,9,SClass[5]);   { Joystick [1*1*1] }
    InputFile.Load(10,10,SClass[5]); { Floppy disk [1*1*1] }
    InputFile.Load(13,13,SClass[5]); { RoboHand [1*1*1] }
    InputFile.Load(14,14,SClass[5]); { Access card [1*1*1] }
    InputFile.Load(40,44,SClass[5]); { Nuclear reactor [1*1*5] }
    InputFile.Load(45,46,SClass[5]); { Hidden spike [1*1*2] }
    InputFile.Load(19,19,SClass[5]); { Balloon upper half [1*1*1] }
    InputFile.Load(20,22,SClass[5]); { Balloon string [1*1*3] }
    InputFile.Load(24,32,SClass[5]); { Nucleus [1*1*9] }
    InputFile.Load(33,34,SClass[5]); { ACME sign [2*1*1] }
    InputFile.Load(35,38,SClass[5]); { Blip text cursor [1*1*4] }
    InputFile.Load(47,49,SClass[5]); { Flag [1*1*3] }
    InputFile.Done;                  { 42 sprites }

    if not InitMedFile(InputFile,'OBJECT2') then Exit;
    InputFile.Load(0,0,SClass[5]);   { Blue box [1*1*1] }
    InputFile.Load(1,1,SClass[5]);   { Red box [1*1*1] }
    InputFile.Load(2,4,SClass[5]);   { Radio [1*1*3] }
    InputFile.Load(5,12,SClass[5]);  { Access card slot [1*1*8] }
    InputFile.Load(14,15,SClass[5]); { RoboHand slot [1*1*2] }
    InputFile.Load(16,17,SClass[5]); { RoboHand slot edges [1*1*2] }
    InputFile.Load(18,18,SClass[5]); { D [1*1*1] }
    InputFile.Load(19,19,SClass[5]); { U [1*1*1] }
    InputFile.Load(20,20,SClass[5]); { K [1*1*1] }
    InputFile.Load(21,21,SClass[5]); { E [1*1*1] }
    InputFile.Load(23,23,SClass[5]); { Secret tip [1*1*1] }
    InputFile.Load(24,24,SClass[5]); { Red key [1*1*1] }
    InputFile.Load(25,25,SClass[5]); { Green key [1*1*1] }
    InputFile.Load(26,26,SClass[5]); { Blue key [1*1*1] }
    InputFile.Load(27,27,SClass[5]); { Purple key [1*1*1] }
    InputFile.Load(28,28,SClass[5]); { Closed door [1*1*1] }
    InputFile.Load(37,37,SClass[5]); { Red keylock [1*1*1] }
    InputFile.Load(38,38,SClass[5]); { Green keylock [1*1*1] }
    InputFile.Load(39,39,SClass[5]); { Blue keylock [1*1*1] }
    InputFile.Load(40,40,SClass[5]); { Purple keylock [1*1*1] }
    InputFile.Load(48,48,SClass[5]); { Floor spike [1*1*1] }
    InputFile.Load(49,49,SClass[5]); { Ceiling spike [1*1*1] }
    InputFile.Done;                  { 33 sprites }

    SClass[6]^.Add(@smBlack);
    SClass[6]^.Add(@smError);
    SClass[6]^.Add(@smHighlight);
    SClass[6]^.Add(@smSelection);
    SClass[6]^.Add(@smSourceAnchor);
    SClass[6]^.Add(@smTargetAnchor);
    SClass[6]^.Add(@smRedDoor);
    SClass[6]^.Add(@smGreenDoor);
    SClass[6]^.Add(@smBlueDoor);
    SClass[6]^.Add(@smPurpleDoor);
    SClass[6]^.Add(Merge4(siBlack,siMissile,siMissile+2,siMissile+3));
    SClass[6]^.Add(Merge4(siHoverTech,siBlack,siHoverTech+1,siBlack));
    SClass[6]^.Add(Merge4(siDreadnoughtTech,siDreadnoughtTech+1,siDreadnoughtTech+2,siDreadnoughtTech+3));
    SClass[6]^.Add(Merge4(siBlack,siBlack,siGunCarTech,siGunCarTech+1));
    SClass[6]^.Add(Merge4(siFireWheelTech,siFireWheelTech+1,siFireWheelTech+2,siFireWheelTech+3));
    SClass[6]^.Add(Merge4(siLFan,siBlack,siLFan+1,siBlack));
    SClass[6]^.Add(Merge4(siHeliTech,siHeliTech+1,siHeliTech+4,siHeliTech+5));
    SClass[6]^.Add(Merge4(siACME,siACME+1,siBlack,siBlack));
    SClass[6]^.Add(Merge4(siTelePort,siTelePort+1,siTelePort+3,siTelePort+4));
    SClass[6]^.Add(Merge4(siDuke,siDuke+1,siDuke+2,siDuke+3));
    SClass[6]^.Add(Merge4(siBunnyTech,siBlack,siBunnyTech+1,siBlack));
    SClass[6]^.Add(Merge4(siFireBallTech,siBlack,siFireBallTech+1,siBlack));
    SClass[6]^.Add(Merge4(siProtonTerminal,siProtonTerminal+1,siProtonTerminal+2,siProtonTerminal+3));
    SClass[6]^.Add(Merge4(siProton,siProton+1,siProton+2,siProton+3));

    if not InitSmallFile(SmallFile,'FONT1') then Exit;
    SmallFile.Load(0,49,Font); SmallFile.Done;
    if not InitSmallFile(SmallFile,'FONT2') then Exit;
    SmallFile.Load(0,49,Font); SmallFile.Done;
    for I:=0 to 3 do Font^.Sprite^[100+I].Cut(TranslateSpriteIndex(siBlip+I),0,0);
    Font^.Sprite^[104]:=smBackSlash;

    for I:=0 to 255 do FontMap[I]:=10;
    for I:=1 to 10 do FontMap[I]:=I-1;
    for I:=11 to 15 do FontMap[I]:=I+84;
    for I:=32 to 90 do FontMap[I]:=I-22;
    for I:=96 to 121 do FontMap[I]:=I-28;
    for I:=128 to 131 do FontMap[I]:=I-28;
    FontMap[92]:=104;

    New(Backdrop[0],Init(130));
    New(Backdrop[1],Init(130));
    CurrentBackdrop[0]:=4;
    CurrentBackdrop[1]:=4;

    for I:=1 to High(AnimPulse) do AnimPulse[I]:=0;
    SystemTimerSave:=SystemTimer;
    SpriteManagerOk:=True;
end;

procedure SpriteManagerDone;
begin
    Dispose(SClass[0]);
    Dispose(SClass[1]);
    Dispose(SClass[2]);
    Dispose(SClass[3]);
    Dispose(SClass[4]);
    Dispose(SClass[5]);
    Dispose(SClass[6]);
    Dispose(Font);
end;

end.
