{ Shape and Thing enumerations for active objects }
    asGBoxNothing=$3000;          { Grey box - nothing }
    asElevator=$3001;             { Elevator }
    asLConveyorLEnd=$3002;        { Conveyor belt left end, moving left }
    asLConveyorREnd=$3003;        { Conveyor belt right end, moving left }
    asRConveyorLEnd=$3004;        { Conveyor belt left end, moving right }
    asRConveyorREnd=$3005;        { Conveyor belt right end, moving right }
    asGBoxBoots=$3006;            { Grey box - boots }
    asMissile=$3007;              { Missile }
    asGBoxHooks=$3008;            { Grey box - grappling hooks }
    asRFlamethrower=$3009;        { Flamethrower, firing right }
    asLFlamethrower=$300A;        { Flamethrower, firing left }
    asHoverTech=$300B;            { Hovering techbot }
    asDreadnoughtTech=$300C;      { Dreadnought techbot }
    asGunCarTech=$300D;           { Gun car techbot }
    asFireWheel=$300E;            { Firewheel techbot }
    asGun=$300F;                  { Duke gun powerup }
    asSimpleTech=$3010;           { Simple techbot }
    asExit=$3011;                 { Exit door }
    asGBoxDynamite=$3012;         { Grey box - dynamite }
    asSnakeTech=$3013;            { Snake techbot }
    asMirror=$3014;               { Mirror floor }
    asRBoxCola=$3015;             { Red box - cola }
    asRCrawlieTech=$3016;         { Crawlie techbot, facing right }
    asLCrawlieTech=$3017;         { Crawlie techbot, facing left }
    asRBoxTurkey=$3018;           { Red box - single turkey drumstick }
    asBreakBridge=$3019;          { Breakable bridge }
    asHorizWaves=$301A;           { Horizontal electric waves }
    asLFan=$301B;                 { Fan, facing left }
    asRFan=$301C;                 { Fan, facing right }
    asBBoxFooty=$301D;            { Blue box - football }
    asBBoxJoystick=$301E;         { Blue box - joystick }
    asBBoxFloppy=$301F;           { Blue box - floppy disk }
    asGBoxRobohand=$3020;         { Grey box - Robohand }
    asVerticWaves=$3021;          { Vertical electric waves that form circuit board door }
    asHeliTech=$3022;             { Helicopter }
    asBBoxBalloon=$3023;          { Blue box - balloon }
    asCameraTech=$3024;           { Camera }
    asDuplicatorUp=$3025;         { Duplicator - copies from above, adds spikes }
    asDuplicatorRight=$3026;      { Duplicator - copies from right, adds 1/2 rock }
    asDuplicatorLeft=$3027;       { Duplicator - copies from left, fades to black }
    asPrison=$3028;               { Prison window, see thru }
    asGBoxNucleus=$3029;          { Grey box - nucleus }
    asACME=$302A;                 { ACME sign }
    asReactor=$302B;              { Reactor }
    asRetractableSpike=$302C;     { Retractable floor spike }
    asBBoxFlag=$302D;             { Blue box - flag }
    asBBoxRadio=$302E;            { Blue box - radio }
    asTeleporter1=$302F;          { Teleporter 1 }
    asTeleporter2=$3030;          { Teleporter 2 }
    asBounceMineTech=$3031;       { Bounce mine }
    asDuke=$3032;                 { Duke himself }
    asGBoxCircuitBoard=$3033;     { Grey box - circuit board }
    asCircuitBoardSlot=$3034;     { Hand-in slot for circuit board }
    asRobohandSlot=$3035;         { Hand-in slot for Robohand }
    asRobohandExtension=$3036;    { Extension point for Robohand }
    asGBoxD=$3037;                { Grey box - D }
    asGBoxU=$3038;                { Grey box - U }
    asGBoxK=$3039;                { Grey box - K }
    asGBoxE=$303A;                { Grey box - E }
    asBunnyTech=$303B;            { Bunny techbot }
    asFireballTech=$303C;         { Fireball techbot }
    asCycloneFence=$303D;         { Cyclone fence, see thru }
    asLeftWin=$303E;              { Left 1/2 window, see thru }
    asRightWin=$303F;             { Right 1/2 window, see thru }
    asSecretTip=$3040;            { Secret tip message board }
    asProtonTerminal=$3041;       { Message terminal with Dr Proton }
    asProtonExplode=$3042;        { Dr Proton, explodes when killed }
    asProtonEscape=$3043;         { Dr Proton, escapes when killed }
    asRedKey=$3044;               { Red key }
    asGreenKey=$3045;             { Green key }
    asBlueKey=$3046;              { Blue key }
    asPurpleKey=$3047;            { Purple key }
    asRedKeyhole=$3048;           { Red keyhole }
    asGreenKeyhole=$3049;         { Green keyhole }
    asBlueKeyhole=$304A;          { Blue keyhole }
    asPurpleKeyhole=$304B;        { Purple keyhole }
    asRedDoor=$304C;              { Red door }
    asGreenDoor=$304D;            { Green door }
    asBlueDoor=$304E;             { Blue door }
    asPurpleDoor=$304F;           { Purple door }
    asFooty=$3050;                { Football }
    asTurkey=$3051;               { Single turkey stick }
    asCola=$3052;                 { Cola }
    asFloppy=$3053;               { Floppy disk }
    asJoystick=$3054;             { Joystick }
    asFlag=$3055;                 { Flag }
    asRadio=$3056;                { Radio }
    asExplodeMineTech=$3057;      { Exploding mine }
    asFloorSpikes=$3058;          { Floor spikes }
    asCeilingSpikes=$3059;        { Ceiling spikes }

    asError=$FFFF;                { Invalid ShapeThing code }

    asRedGirder=$2FE0;            { Red girder, needed for RoboHand }
                                  {   extension to run into. }

{ Pull directions for active objects }
    PullMode:array[$3000..$3059] of TPullMode=(
        pmLeft,     { Grey box - nothing }
        pmLeft,     { Elevator }
        pmLeft,     { Conveyor belt left end, moving left }
        pmRight,    { Conveyor belt right end, moving left }
        pmLeft,     { Conveyor belt left end, moving right }
        pmRight,    { Conveyor belt right end, moving right }
        pmLeft,     { Grey box - boots }
        pmLeft,     { Missile }
        pmLeft,     { Grey box - grappling hooks }
        pmRight,    { Flamethrower, firing right }
        pmLeft,     { Flamethrower, firing left }
        pmLeft,     { Hovering techbot }
        pmLeft,     { Dreadnought techbot }
        pmLeft,     { Gun car techbot }
        pmLeft,     { Firewheel techbot }
        pmLeft,     { Duke gun powerup }
        pmLeft,     { Simple techbot }
        pmLeft,     { Exit door }
        pmLeft,     { Grey box - dynamite }
        pmLeft,     { Snake techbot }
        pmLeft,     { Mirror floor }
        pmLeft,     { Red box - cola }
        pmRight,    { Crawlie techbot, facing right }
        pmLeft,     { Crawlie techbot, facing left }
        pmLeft,     { Red box - single turkey drumstick }
        pmUp,       { Breakable bridge }
        pmUp,       { Horizontal electric waves }
        pmLeft,     { Fan, facing left }
        pmRight,    { Fan, facing right }
        pmLeft,     { Blue box - football }
        pmLeft,     { Blue box - joystick }
        pmLeft,     { Blue box - floppy disk }
        pmLeft,     { Grey box - Robohand }
        pmLeft,     { Vertical electric waves that form circuit board door }
        pmLeft,     { Helicopter }
        pmLeft,     { Blue box - balloon }
        pmDown,     { Camera }
        pmUp,       { Duplicator - copies from above, adds spikes }
        pmLeft,     { Duplicator - copies from right, adds 1/2 rock }
        pmRight,    { Duplicator - copies from left, fades to black }
        pmThru,     { Prison window, see thru }
        pmLeft,     { Grey box - nucleus }
        pmLeft,     { ACME sign }
        pmLeft,     { Reactor }
        pmUp,       { Retractable floor spike }
        pmLeft,     { Blue box - flag }
        pmLeft,     { Blue box - radio }
        pmLeft,     { Teleporter 1 }
        pmLeft,     { Teleporter 2 }
        pmLeft,     { Bounce mine }
        pmLeft,     { Duke himself }
        pmLeft,     { Grey box - circuit board }
        pmLeft,     { Hand-in slot for circuit board }
        pmLeft,     { Hand-in slot for Robohand }
        pmLeft,     { Extension point for Robohand }
        pmLeft,     { Grey box - D }
        pmLeft,     { Grey box - U }
        pmLeft,     { Grey box - K }
        pmLeft,     { Grey box - E }
        pmLeft,     { Bunny techbot }
        pmLeft,     { Fireball techbot }
        pmThru,     { Cyclone fence, see thru }
        pmThru,     { Left 1/2 window, see thru }
        pmThru,     { Right 1/2 window, see thru }
        pmLeft,     { Secret tip message board }
        pmLeft,     { Message terminal with Dr Proton }
        pmLeft,     { Dr Proton, explodes when killed }
        pmLeft,     { Dr Proton, escapes when killed }
        pmLeft,     { Red key }
        pmLeft,     { Green key }
        pmLeft,     { Blue key }
        pmLeft,     { Purple key }
        pmLeft,     { Red keyhole }
        pmLeft,     { Green keyhole }
        pmLeft,     { Blue keyhole }
        pmLeft,     { Purple keyhole }
        pmNone,     { Red door }
        pmNone,     { Green door }
        pmNone,     { Blue door }
        pmNone,     { Purple door }
        pmLeft,     { Football }
        pmLeft,     { Single turkey stick }
        pmLeft,     { Cola }
        pmLeft,     { Floppy disk }
        pmLeft,     { Joystick }
        pmLeft,     { Flag }
        pmLeft,     { Radio }
        pmLeft,     { Exploding mine }
        pmUp,       { Floor spikes }
        pmDown      { Ceiling spikes }
        );
