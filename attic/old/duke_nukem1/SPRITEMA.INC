{ Sprite index constants for SClass[] array }
{ For use with the TranslateSpriteIndex() function }

{ Solid sprites }
    siConveyorLEnd=$021D;
    siConveyorREnd=$0223;
    siConveyor=$0220;
    siRedGirder=$02BF;

{ Man sprites }
    siDuke=$0300;

{ Animation sprites }
    siHoverTech=$0400;
    siDreadnoughtTech=$0402;
    siGunCarTech=$0406;
    siFirewheelTech=$0408;
    siSimpleTech=$040C;
    siExit=$040F;
    siDynamite=$0413;
    siSnakeTech=$0414;
    siCola=$041C;
    siLCrawlieTech=$0420;
    siRCrawlieTech=$0424;
    siBreakBridge=$0428;
    siLFan=$042A;
    siHeliTech=$042C;
    siCameraTech=$0434;
    siDuplicatorUp=$0437;
    siDuplicatorRight=$0438;
    siDuplicatorLeft=$0439;
    siPrison=$043A;
    siTeleport=$043B;
    siBounceMine=$0444;
    siBunnyTech=$0445;
    siFireballTech=$0447;
    siCycloneFence=$0449;
    siLeftWin=$044A;
    siRightWin=$044B;
    siProtonTerminal=$044C;
    siProton=$0450;

{ Object sprites }
    siGBox=$0500;
    siElevator=$0501;
    siBoots=$0502;
    siMissile=$0503;
    siHooks=$0508;
    siLFlamethrower=$0509;
    siRFlamethrower=$050A;
    siGun=$050B;
    siTurkey=$050C;
    siHorizWave=$050D;
    siVerticWave=$0511;
    siFooty=$0515;
    siJoystick=$0516;
    siFloppy=$0517;
    siRobohand=$0518;
    siCircuitBoard=$0519;
    siReactor=$051A;
    siRetractableSpike=$051F;
    siBalloon=$0521;
    siNucleus=$0525;
    siACME=$052E;
    siBlip=$0530;
    siFlag=$0534;
    siBBox=$0537;
    siRBox=$0538;
    siRadio=$0539;
    siCircuitBoardSlot=$053C;
    siRobohandSlot=$0544;
    siRobohandSlotEdges=$0546;
    siD=$0548;
    siU=$0549;
    siK=$054A;
    siE=$054B;
    siSecretTip=$054C;
    siRedKey=$054D;
    siGreenKey=$054E;
    siBlueKey=$054F;
    siPurpleKey=$0550;
    siClosedDoor=$0551;
    siRedKeyhole=$0552;
    siGreenKeyhole=$0553;
    siBlueKeyhole=$0554;
    siPurpleKeyhole=$0555;
    siFloorSpikes=$0556;
    siCeilingSpikes=$0557;

{ Special user defined sprites }
    siBlack=$0600;
    siError=$0601;
    siHighlight=$0602;
    siSelection=$0603;
    siSourceAnchor=$0604;
    siTargetAnchor=$0605;
    siRedDoor=$0606;
    siGreenDoor=$0607;
    siBlueDoor=$0608;
    siPurpleDoor=$0609;

    siMicroMissile=$060A;
    siMicroHoverTech=$060B;
    siMicroDreadnoughtTech=$060C;
    siMicroGunCarTech=$060D;
    siMicroFireWheelTech=$060E;
    siMicroLFan=$060F;
    siMicroHeliTech=$0610;
    siMicroACME=$0611;
    siMicroTeleport=$0612;
    siMicroDuke=$0613;
    siMicroBunnyTech=$0614;
    siMicroFireBallTech=$0615;
    siMicroProtonTerminal=$0616;
    siMicroProton=$0617;

{ User defined sprite images }
    smBlack:TMedSprite=(Image:(
        ($0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000),
        ($0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000),
        ($0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000),
        ($0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000));
       Mask:($0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000));

    smError:TMedSprite=(Image:(
        ($0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000),
        ($0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000),
        ($0000,$0044,$0028,$0010,$0028,$0044,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000),
        ($0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000));
       Mask:($0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000,$0000));

    smHighlight:TMedSprite=(Image:(
        ($FFFF,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$FFFF),
        ($AAAA,$0100,$0080,$0100,$0080,$0100,$0080,$0100,$0080,$0100,$0080,$0100,$0080,$0100,$0080,$5555),
        ($AAAA,$0100,$0080,$0100,$0080,$0100,$0080,$0100,$0080,$0100,$0080,$0100,$0080,$0100,$0080,$5555),
        ($FFFF,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$0180,$FFFF));
       Mask:($0000,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$FE7F,$0000));

    smSelection:TMedSprite=(Image:(
        ($0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555),
        ($0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555),
        ($0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555,$0000,$5555),
        ($AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555));
       Mask:($5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA,$5555,$AAAA));

    smSourceAnchor:TMedSprite=(Image:(
        ($A00A,$0000,$4005,$0000,$8282,$0820,$2288,$0821,$2288,$0820,$8282,$0000,$4005,$0000,$A00A,$0000),
        ($E00F,$0000,$C007,$0000,$8283,$0AA0,$2AA8,$2AA9,$2AA8,$0AA0,$8283,$0000,$C007,$0000,$E00F,$0000),
        ($A00A,$0000,$4005,$0000,$8282,$0820,$2288,$0821,$2288,$0820,$8282,$0000,$4005,$0000,$A00A,$0000),
        ($E00F,$0000,$C007,$0000,$8283,$0AA0,$2AA8,$2AA9,$2AA8,$0AA0,$8283,$0000,$C007,$0000,$E00F,$0000));
       Mask:($1FF0,$FFFF,$3FF8,$FFFF,$7D7C,$F55F,$D557,$D556,$D557,$F55F,$7D7C,$FFFF,$3FF8,$FFFF,$1FF0,$FFFF));

    smTargetAnchor:TMedSprite=(Image:(
        ($A00A,$0000,$4005,$0000,$8282,$0820,$2288,$0821,$2288,$0820,$8282,$0000,$4005,$0000,$A00A,$0000),
        ($E00F,$0000,$C007,$0000,$8283,$0AA0,$2AA8,$2AA9,$2AA8,$0AA0,$8283,$0000,$C007,$0000,$E00F,$0000),
        ($E00F,$0000,$C007,$0000,$8283,$0AA0,$2AA8,$2AA9,$2AA8,$0AA0,$8283,$0000,$C007,$0000,$E00F,$0000),
        ($E00F,$0000,$C007,$0000,$8283,$0AA0,$2AA8,$2AA9,$2AA8,$0AA0,$8283,$0000,$C007,$0000,$E00F,$0000));
       Mask:($1FF0,$FFFF,$3FF8,$FFFF,$7D7C,$F55F,$D557,$D556,$D557,$F55F,$7D7C,$FFFF,$3FF8,$FFFF,$1FF0,$FFFF));

    smRedDoor:TMedSprite=(Image:(
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$F6EE,$FADE,$C082,$FADE,$F6EE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$F6EE,$FADE,$C082,$FADE,$F6EE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181));
        Mask:(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));

    smGreenDoor:TMedSprite=(Image:(
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$F6EE,$FADE,$C082,$FADE,$F6EE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$F6EE,$FADE,$C082,$FADE,$F6EE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181));
        Mask:(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));

    smBlueDoor:TMedSprite=(Image:(
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$F6EE,$FADE,$C082,$FADE,$F6EE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$F6EE,$FADE,$C082,$FADE,$F6EE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181));
        Mask:(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));

    smPurpleDoor:TMedSprite=(Image:(
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$F6EE,$FADE,$C082,$FADE,$F6EE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE,$FEFE),
        ($8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181,$8181));
        Mask:(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0));

    smMouseCursor:TMedSprite=(Image:(
        ($00C0,$00A0,$0090,$0088,$0084,$0082,$0081,$8080,$4080,$2080,$E083,$0082,$0091,$00A9,$00C9,$0006),
        ($00C0,$00A0,$0090,$0088,$0084,$0082,$0081,$8080,$4080,$2080,$E083,$0082,$0091,$00A9,$00C9,$0006),
        ($00C0,$00A0,$0090,$0088,$0084,$0082,$0081,$8080,$4080,$2080,$E083,$0082,$0091,$00A9,$00C9,$0006),
        ($00C0,$00A0,$0090,$0088,$0084,$0082,$0081,$8080,$4080,$2080,$E083,$0082,$0091,$00A9,$00C9,$0006));
        Mask:($FF3F,$FF1F,$FF0F,$FF07,$FF03,$FF01,$FF00,$7F00,$3F00,$1F00,$1F00,$FF01,$FF00,$FF10,$FF30,$FFF9));

    smBackSlash:TSmallSprite=(Image:(
        ($00,$60,$30,$18,$0C,$06,$00,$00),
        ($00,$60,$30,$18,$0C,$06,$00,$00),
        ($00,$60,$30,$18,$0C,$06,$00,$00),
        ($00,$60,$30,$18,$0C,$06,$00,$00));
        Mask:($FF,$9F,$8F,$C7,$E3,$F1,$F9,$FF));
