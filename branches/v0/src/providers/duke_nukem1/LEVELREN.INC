{ This table instructs the renderer how to draw each of the active sprites }

ActiveSprite:array[$3000..$3059] of TActiveSprite=(
    (Mode:atSolid11S; Anim:$01; Data:(siGBox,0)),                { Grey box - nothing }
    (Mode:atSolid11S; Anim:$01; Data:(siElevator,0)),            { Elevator }
    (Mode:atConveyor; Anim:$01; Data:(siConveyorLEnd,6)),        { Conveyor belt left end, moving left }
    (Mode:atSolid11S; Anim:$01; Data:(siConveyorREnd,6)),        { Conveyor belt right end, moving left }
    (Mode:atConveyor; Anim:$01; Data:(siConveyorLEnd,5)),        { Conveyor belt left end, moving right }
    (Mode:atSolid11S; Anim:$01; Data:(siConveyorREnd,5)),        { Conveyor belt right end, moving right }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siBoots)),          { Grey box - boots }
    (Mode:atMissile;  Anim:$01; Data:(siMissile,0)),             { Missile }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siHooks)),          { Grey box - grappling hooks }
    (Mode:atTrans11;  Anim:$01; Data:(siRFlamethrower,0)),       { Flamethrower, firing right }
    (Mode:atTrans11;  Anim:$01; Data:(siLFlamethrower,0)),       { Flamethrower, firing left }
    (Mode:atTrans12;  Anim:$01; Data:(siHoverTech,0)),           { Hovering techbot }
    (Mode:atTrans22;  Anim:$01; Data:(siDreadnoughtTech,0)),     { Dreadnought techbot }
    (Mode:atTrans21;  Anim:$01; Data:(siGunCarTech,0)),          { Gun car techbot }
    (Mode:atTrans22;  Anim:$01; Data:(siFirewheelTech,0)),       { Firewheel techbot }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siGun)),            { Grey Box - Duke gun powerup }
    (Mode:atTrans11;  Anim:$03; Data:(siSimpleTech,0)),          { Simple techbot }
    (Mode:atTrans22;  Anim:$01; Data:(siExit,0)),                { Exit door }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siDynamite)),       { Grey box - dynamite }
    (Mode:atTrans11;  Anim:$08; Data:(siSnakeTech,0)),           { Snake techbot }
    (Mode:atMirror;   Anim:$01; Data:(0,0)),                     { Mirror floor }
    (Mode:atSolid11D; Anim:$34; Data:(siRBox,siCola)),           { Red box - cola }
    (Mode:atTrans11;  Anim:$04; Data:(siRCrawlieTech,0)),        { Crawlie techbot, facing right }
    (Mode:atTrans11;  Anim:$04; Data:(siLCrawlieTech,0)),        { Crawlie techbot, facing left }
    (Mode:atSolid11D; Anim:$01; Data:(siRBox,siTurkey)),         { Red box - single turkey drumstick }
    (Mode:atBridge;   Anim:$01; Data:(siBreakBridge,Ord('B'))),  { Breakable bridge }
    (Mode:atTrans11;  Anim:$04; Data:(siHorizWave,0)),           { Horizontal electric waves }
    (Mode:atTrans12;  Anim:$01; Data:(siLFan,6)),                { Fan, facing left }
    (Mode:atTrans12;  Anim:$01; Data:(siLFan,5)),                { Fan, facing right }
    (Mode:atSolid11D; Anim:$01; Data:(siBBox,siFooty)),          { Blue box - football }
    (Mode:atSolid11D; Anim:$01; Data:(siBBox,siJoystick)),       { Blue box - joystick }
    (Mode:atSolid11D; Anim:$01; Data:(siBBox,siFloppy)),         { Blue box - floppy disk }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siRobohand)),       { Grey box - Robohand }
    (Mode:atTrans11;  Anim:$04; Data:(siVerticWave,0)),          { Vertical electric waves (circuit board door) }
    (Mode:atTrans42;  Anim:$01; Data:(siHeliTech,0)),            { Helicopter techbot }
    (Mode:atSolid11D; Anim:$01; Data:(siBBox,siBalloon)),        { Blue box - balloon }
    (Mode:atTrans11;  Anim:$01; Data:(siCameraTech,0)),          { Camera }
    (Mode:atTrans11;  Anim:$01; Data:(siDuplicatorUp,0)),        { Duplicator - copies from above, adds spikes }
    (Mode:atTrans11;  Anim:$01; Data:(siDuplicatorRight,0)),     { Duplicator - copies from right, adds 1/2 rock }
    (Mode:atTrans11;  Anim:$01; Data:(siDuplicatorLeft,0)),      { Duplicator - copies from left, fades to black }
    (Mode:atTrans11;  Anim:$01; Data:(siPrison,0)),              { Prison window, see thru }
    (Mode:atSolid11D; Anim:$09; Data:(siGBox,siNucleus)),        { Grey box - nucleus }
    (Mode:atTrans21;  Anim:$01; Data:(siACME,0)),                { ACME sign }
    (Mode:atReactor;  Anim:$05; Data:(siReactor,0)),             { Reactor }
    (Mode:atTrans11;  Anim:$01; Data:(siRetractableSpike,0)),    { Retractable floor spike }
    (Mode:atSolid11D; Anim:$03; Data:(siBBox,siFlag)),           { Blue box - flag }
    (Mode:atSolid11D; Anim:$03; Data:(siBBox,siRadio)),          { Blue box - radio }
    (Mode:atTrans33;  Anim:$01; Data:(siTeleport,Ord('1'))),     { Teleporter 1 }
    (Mode:atTrans33;  Anim:$01; Data:(siTeleport,Ord('2'))),     { Teleporter 2 }
    (Mode:atTrans11;  Anim:$01; Data:(siBounceMine,Ord('B'))),   { Bounce mine }
    (Mode:atTrans22;  Anim:$01; Data:(siDuke,0)),                { Duke himself }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siCircuitBoard)),   { Grey box - circuit board }
    (Mode:atSolid11S; Anim:$08; Data:(siCircuitBoardSlot,0)),    { Hand-in slot for circuit board }
    (Mode:atTrans31;  Anim:$B2; Data:(siRobohandSlot,0)),        { Hand-in slot for Robohand }
    (Mode:atBridge;   Anim:$01; Data:(siRedGirder,Ord('R'))),    { Extension bridge for Robohand }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siD)),              { Grey box - D }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siU)),              { Grey box - U }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siK)),              { Grey box - K }
    (Mode:atSolid11D; Anim:$01; Data:(siGBox,siE)),              { Grey box - E }
    (Mode:atTrans12;  Anim:$01; Data:(siBunnyTech,0)),           { Bunny techbot }
    (Mode:atTrans12;  Anim:$01; Data:(siFireballTech,0)),        { Fireball techbot }
    (Mode:atTrans11;  Anim:$01; Data:(siCycloneFence,0)),        { Cyclone fence, see thru }
    (Mode:atTrans11;  Anim:$01; Data:(siLeftWin,0)),             { Left 1/2 window, see thru }
    (Mode:atTrans11;  Anim:$01; Data:(siRightWin,0)),            { Right 1/2 window, see thru }
    (Mode:atSolid11S; Anim:$01; Data:(siSecretTip,0)),           { Secret tip message board }
    (Mode:atTrans22;  Anim:$01; Data:(siProtonTerminal,0)),      { Message terminal with Dr Proton }
    (Mode:atTrans22;  Anim:$01; Data:(siProton,Ord('X'))),       { Dr Proton, explodes when killed }
    (Mode:atTrans22;  Anim:$01; Data:(siProton,Ord('E'))),       { Dr Proton, escapes when killed }
    (Mode:atTrans11;  Anim:$01; Data:(siRedKey,0)),              { Red key }
    (Mode:atTrans11;  Anim:$01; Data:(siGreenKey,0)),            { Green key }
    (Mode:atTrans11;  Anim:$01; Data:(siBlueKey,0)),             { Blue key }
    (Mode:atTrans11;  Anim:$01; Data:(siPurpleKey,0)),           { Purple key }
    (Mode:atSolid11S; Anim:$01; Data:(siRedKeyhole,0)),          { Red keyhole }
    (Mode:atSolid11S; Anim:$01; Data:(siGreenKeyhole,0)),        { Green keyhole }
    (Mode:atSolid11S; Anim:$01; Data:(siBlueKeyhole,0)),         { Blue keyhole }
    (Mode:atSolid11S; Anim:$01; Data:(siPurpleKeyhole,0)),       { Purple keyhole }
    (Mode:atSolid11S; Anim:$01; Data:(siRedDoor,0)),             { Red door }
    (Mode:atSolid11S; Anim:$01; Data:(siGreenDoor,0)),           { Green door }
    (Mode:atSolid11S; Anim:$01; Data:(siBlueDoor,0)),            { Blue door }
    (Mode:atSolid11S; Anim:$01; Data:(siPurpleDoor,0)),          { Purple door }
    (Mode:atTrans11;  Anim:$01; Data:(siFooty,0)),               { Football }
    (Mode:atTrans11;  Anim:$01; Data:(siTurkey,0)),              { Single turkey stick }
    (Mode:atTrans11;  Anim:$34; Data:(siCola,0)),                { Cola }
    (Mode:atTrans11;  Anim:$01; Data:(siFloppy,0)),              { Floppy disk }
    (Mode:atTrans11;  Anim:$01; Data:(siJoystick,0)),            { Joystick }
    (Mode:atTrans11;  Anim:$03; Data:(siFlag,0)),                { Flag }
    (Mode:atTrans11;  Anim:$03; Data:(siRadio,0)),               { Radio }
    (Mode:atTrans11;  Anim:$01; Data:(siBounceMine,0)),          { Exploding mine }
    (Mode:atTrans11;  Anim:$01; Data:(siFloorSpikes,0)),         { Floor spikes }
    (Mode:atTrans11;  Anim:$01; Data:(siCeilingSpikes,0))        { Ceiling spikes }
    );

{ This table helps the renderer draw multi-square sprites properly }
    Z=$FFFF; { Indicates no sprite drawn for this offset }

MultiSize:array[Low(TActiveSpriteMode)..High(TActiveSpriteMode)] of Word=(
    1,1,1,2,2,4,1,9,8,5,1,1,1,1);

MultiCheck:array[0..12] of TMultiCheck=(
    (GridOffset:(X: 0; Y: 0); SpriteOffset:(0,0,0,0,1,2,0,6,4,3,0,0,0,0)), { Square itself }
    (GridOffset:(X: 0; Y: 1); SpriteOffset:(Z,Z,Z,Z,0,0,Z,3,0,1,0,Z,Z,Z)), { 1 square below }
    (GridOffset:(X:-1; Y: 0); SpriteOffset:(Z,Z,Z,1,Z,3,3,7,5,4,Z,Z,Z,Z)), { 1 square left }
    (GridOffset:(X:-1; Y: 1); SpriteOffset:(Z,Z,Z,Z,Z,1,Z,4,1,Z,Z,Z,Z,Z)), { 1 square below/left }
    (GridOffset:(X: 0; Y: 2); SpriteOffset:(Z,Z,Z,Z,Z,Z,Z,0,Z,1,0,Z,Z,Z)), { 2 squares below }
    (GridOffset:(X:-1; Y: 2); SpriteOffset:(Z,Z,Z,Z,Z,Z,Z,1,Z,Z,Z,Z,Z,Z)), { 2 squares below, 1 square left }
    (GridOffset:(X:-2; Y: 2); SpriteOffset:(Z,Z,Z,Z,Z,Z,Z,2,Z,Z,Z,Z,Z,Z)), { 2 squares below, 2 squares left }
    (GridOffset:(X:-2; Y: 1); SpriteOffset:(Z,Z,Z,Z,Z,Z,Z,5,2,Z,Z,Z,Z,Z)), { 1 square below, 2 squares left }
    (GridOffset:(X:-2; Y: 0); SpriteOffset:(Z,Z,Z,Z,Z,Z,Z,8,6,Z,Z,Z,Z,Z)), { 2 squares left }
    (GridOffset:(X: 0; Y: 3); SpriteOffset:(Z,Z,Z,Z,Z,Z,Z,Z,Z,0,Z,Z,Z,Z)), { 3 squares below }
    (GridOffset:(X:-3; Y: 1); SpriteOffset:(Z,Z,Z,Z,Z,Z,Z,Z,3,Z,Z,Z,Z,Z)), { 3 squares left, 1 square below }
    (GridOffset:(X:-3; Y: 0); SpriteOffset:(Z,Z,Z,Z,Z,Z,Z,Z,7,Z,Z,Z,Z,Z)), { 3 squares left }
    (GridOffset:(X: 1; Y: 0); SpriteOffset:(Z,Z,Z,Z,Z,Z,2,Z,Z,2,Z,Z,Z,Z)));{ 1 square right }

ToolBarSprite:array[$3000..$3059] of TActiveSprite=(
    (Mode:atSolid11S; Anim:$00; Data:(siGBox,0)),                { Grey box - nothing }
    (Mode:atSolid11S; Anim:$00; Data:(siElevator,0)),            { Elevator }
    (Mode:atSolid11S; Anim:$00; Data:(siConveyorLEnd,6)),        { Conveyor belt left end, moving left }
    (Mode:atSolid11S; Anim:$00; Data:(siConveyorREnd,6)),        { Conveyor belt right end, moving left }
    (Mode:atSolid11S; Anim:$00; Data:(siConveyorLEnd,5)),        { Conveyor belt left end, moving right }
    (Mode:atSolid11S; Anim:$00; Data:(siConveyorREnd,5)),        { Conveyor belt right end, moving right }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siBoots)),          { Grey box - boots }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroMissile,0)),        { Missile }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siHooks)),          { Grey box - grappling hooks }
    (Mode:atSolid11S; Anim:$00; Data:(siRFlamethrower,0)),       { Flamethrower, firing right }
    (Mode:atSolid11S; Anim:$00; Data:(siLFlamethrower,0)),       { Flamethrower, firing left }
    (Mode:atSolid11S; Anim:$00; Data:(siHoverTech,0)),           { Hovering techbot }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroDreadnoughtTech,0)),{ Dreadnought techbot }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroGunCarTech,0)),     { Gun car techbot }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroFirewheelTech,0)),  { Firewheel techbot }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siGun)),            { Grey Box - Duke gun powerup }
    (Mode:atSolid11S; Anim:$00; Data:(siSimpleTech,0)),          { Simple techbot }
    (Mode:atSolid11S; Anim:$00; Data:(siExit,0)),                { Exit door }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siDynamite)),       { Grey box - dynamite }
    (Mode:atSolid11S; Anim:$00; Data:(siSnakeTech,0)),           { Snake techbot }
    (Mode:atSolid11S; Anim:$00; Data:(siBlack,Ord('M'))),        { Mirror floor }
    (Mode:atSolid11D; Anim:$00; Data:(siRBox,siCola)),           { Red box - cola }
    (Mode:atSolid11S; Anim:$00; Data:(siRCrawlieTech,0)),        { Crawlie techbot, facing right }
    (Mode:atSolid11S; Anim:$00; Data:(siLCrawlieTech,0)),        { Crawlie techbot, facing left }
    (Mode:atSolid11D; Anim:$00; Data:(siRBox,siTurkey)),         { Red box - single turkey drumstick }
    (Mode:atSolid11S; Anim:$00; Data:(siBreakBridge,Ord('B'))),  { Breakable bridge }
    (Mode:atSolid11S; Anim:$00; Data:(siHorizWave,0)),           { Horizontal electric waves }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroLFan,6)),           { Fan, facing left }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroLFan,5)),           { Fan, facing right }
    (Mode:atSolid11D; Anim:$00; Data:(siBBox,siFooty)),          { Blue box - football }
    (Mode:atSolid11D; Anim:$00; Data:(siBBox,siJoystick)),       { Blue box - joystick }
    (Mode:atSolid11D; Anim:$00; Data:(siBBox,siFloppy)),         { Blue box - floppy disk }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siRobohand)),       { Grey box - Robohand }
    (Mode:atSolid11S; Anim:$00; Data:(siVerticWave,0)),          { Vertical electric waves (circuit board door) }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroHeliTech,0)),       { Helicopter techbot }
    (Mode:atSolid11D; Anim:$00; Data:(siBBox,siBalloon)),        { Blue box - balloon }
    (Mode:atSolid11S; Anim:$00; Data:(siCameraTech,0)),          { Camera }
    (Mode:atSolid11S; Anim:$00; Data:(siDuplicatorUp,0)),        { Duplicator - copies from above, adds spikes }
    (Mode:atSolid11S; Anim:$00; Data:(siDuplicatorRight,0)),     { Duplicator - copies from right, adds 1/2 rock }
    (Mode:atSolid11S; Anim:$00; Data:(siDuplicatorLeft,0)),      { Duplicator - copies from left, fades to black }
    (Mode:atSolid11S; Anim:$00; Data:(siPrison,0)),              { Prison window, see thru }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siNucleus)),        { Grey box - nucleus }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroACME,0)),           { ACME sign }
    (Mode:atSolid11S; Anim:$00; Data:(siReactor,0)),             { Reactor }
    (Mode:atSolid11S; Anim:$00; Data:(siRetractableSpike,0)),    { Retractable floor spike }
    (Mode:atSolid11D; Anim:$00; Data:(siBBox,siFlag)),           { Blue box - flag }
    (Mode:atSolid11D; Anim:$00; Data:(siBBox,siRadio)),          { Blue box - radio }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroTeleport,Ord('1'))),{ Teleporter 1 }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroTeleport,Ord('2'))),{ Teleporter 2 }
    (Mode:atSolid11S; Anim:$00; Data:(siBounceMine,Ord('B'))),   { Bounce mine }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroDuke,0)),           { Duke himself }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siCircuitBoard)),   { Grey box - circuit board }
    (Mode:atSolid11S; Anim:$00; Data:(siCircuitBoardSlot,0)),    { Hand-in slot for circuit board }
    (Mode:atSolid11S; Anim:$00; Data:(siRobohandSlot,0)),        { Hand-in slot for Robohand }
    (Mode:atSolid11S; Anim:$00; Data:(siRedGirder,Ord('R'))),    { Extension bridge for Robohand }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siD)),              { Grey box - D }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siU)),              { Grey box - U }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siK)),              { Grey box - K }
    (Mode:atSolid11D; Anim:$00; Data:(siGBox,siE)),              { Grey box - E }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroBunnyTech,0)),      { Bunny techbot }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroFireBallTech,0)),   { Fireball techbot }
    (Mode:atSolid11S; Anim:$00; Data:(siCycloneFence,0)),        { Cyclone fence, see thru }
    (Mode:atSolid11S; Anim:$00; Data:(siLeftWin,0)),             { Left 1/2 window, see thru }
    (Mode:atSolid11S; Anim:$00; Data:(siRightWin,0)),            { Right 1/2 window, see thru }
    (Mode:atSolid11S; Anim:$00; Data:(siSecretTip,0)),           { Secret tip message board }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroProtonTerminal,0)), { Message terminal with Dr Proton }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroProton,Ord('X'))),  { Dr Proton, explodes when killed }
    (Mode:atSolid11S; Anim:$00; Data:(siMicroProton,Ord('E'))),  { Dr Proton, escapes when killed }
    (Mode:atSolid11S; Anim:$00; Data:(siRedKey,0)),              { Red key }
    (Mode:atSolid11S; Anim:$00; Data:(siGreenKey,0)),            { Green key }
    (Mode:atSolid11S; Anim:$00; Data:(siBlueKey,0)),             { Blue key }
    (Mode:atSolid11S; Anim:$00; Data:(siPurpleKey,0)),           { Purple key }
    (Mode:atSolid11S; Anim:$00; Data:(siRedKeyhole,0)),          { Red keyhole }
    (Mode:atSolid11S; Anim:$00; Data:(siGreenKeyhole,0)),        { Green keyhole }
    (Mode:atSolid11S; Anim:$00; Data:(siBlueKeyhole,0)),         { Blue keyhole }
    (Mode:atSolid11S; Anim:$00; Data:(siPurpleKeyhole,0)),       { Purple keyhole }
    (Mode:atSolid11S; Anim:$00; Data:(siRedDoor,0)),             { Red door }
    (Mode:atSolid11S; Anim:$00; Data:(siGreenDoor,0)),           { Green door }
    (Mode:atSolid11S; Anim:$00; Data:(siBlueDoor,0)),            { Blue door }
    (Mode:atSolid11S; Anim:$00; Data:(siPurpleDoor,0)),          { Purple door }
    (Mode:atSolid11S; Anim:$00; Data:(siFooty,0)),               { Football }
    (Mode:atSolid11S; Anim:$00; Data:(siTurkey,0)),              { Single turkey stick }
    (Mode:atSolid11S; Anim:$00; Data:(siCola,0)),                { Cola }
    (Mode:atSolid11S; Anim:$00; Data:(siFloppy,0)),              { Floppy disk }
    (Mode:atSolid11S; Anim:$00; Data:(siJoystick,0)),            { Joystick }
    (Mode:atSolid11S; Anim:$00; Data:(siFlag,0)),                { Flag }
    (Mode:atSolid11S; Anim:$00; Data:(siRadio,0)),               { Radio }
    (Mode:atSolid11S; Anim:$00; Data:(siBounceMine,0)),          { Exploding mine }
    (Mode:atSolid11S; Anim:$00; Data:(siFloorSpikes,0)),         { Floor spikes }
    (Mode:atSolid11S; Anim:$00; Data:(siCeilingSpikes,0))        { Ceiling spikes }
    );

MirrorAnimSpeed=4;

TransSprite:TMedSprite=(
    Image:((0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
        (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
        (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0),
        (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));
    Mask:($FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,
        $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF));
