program CK6Patch;

{ Contains patch information for versions of Keen 6 }

{$A+,B+,E-,F-,G-,I-,P+,Q-,R-,S-,T-,V-,X+}
{$M 2304,0,0}

{$DEFINE KEEN6}
{$DEFINE KEEN4_ARCH}

uses Loader,Parser,Patcher,Misc,Dos,Strings;

const
    { The search pattern to use when looking for the game executable }
    ExecutableFileSpec='KEEN6*.EXE';

    { The name of the CKPatch program for this episode }
    ProgramName='CK6PATCH';

    { The extension that the game uses }
    Ext='CK6';

    { Number of levels featured in this game }
    NumLevels=18;

type
    { Enumerations for each of the resources }
    TDataResource=(
        drAudioHed, { AUDIOHED, index of sound effects in AUDIO.CK6 }
        drEGAHead,  { EGAHEAD, index of sprite tiles in EGAGRAPH.CK6 }
        drMapHead,  { MAPHEAD, index of levels in GAMEMAPS.CK6 }
        drAudioDct, { AUDIODCT, huffman tree for decompressing AUDIO.CK6 }
        drEGADict   { EGADICT, huffman tree for decompressing EGAGRAPH.CK6 }
        );

    { Contains image offsets of each resource }
    PDataResourceOffset=^TDataResourceOffset;
    TDataResourceOffset=
        array[Low(TDataResource)..High(TDataResource)] of Longint;

    { Contains byte lengths of each resource }
    PDataResourceLength=^TDataResourceLength;
    TDataResourceLength=
        array[Low(TDataResource)..High(TDataResource)] of Longint;

    { Contains the maximum length of the name of each level }
    PLevelStringLength=^TLevelStringLength;
    TLevelStringLength=array[0..NumLevels] of Word;

    { The version information structure for Keen 6 }
    PVersion=^TVersion;
    TVersion=record
        { The version ID string }
        ID:String[7];

        { The uncompressed length of the game executable image }
        ImageLength:Longint;

        { The LZEXE-compressed length of the game executable image }
        LZEXEImageLength:Longint;

        { Image offsets of each of the data resources }
        DataResourceOffset:PDataResourceOffset;

        { Byte lengths of each of the data resources }
        DataResourceLength:PDataResourceLength;

        { Image offset of the level name string array }
        LevelNameOffset:Longint;

        { Maximum lengths of each of the level name strings }
        LevelNameLength:PLevelStringLength;

        { Image offset of the level entry string array }
        LevelEntryOffset:Longint;

        { Maximum lengths of each of the level entry strings }
        LevelEntryLength:PLevelStringLength;
    end;

const
    { Resource image offsets for version 1.0 }
    DataResourceOffset_10:TDataResourceOffset=
        ($1F820,$1FB20,$23C50,$35912,$35D12);

    { Resource byte lengths for version 1.0 }
    DataResourceLength_10:TDataResourceLength=
        (768,16688,402,1024,1024);

    { Resource image offsets for version 1.4 }
    DataResourceOffset_14:TDataResourceOffset=
        ($20C50,$20F50,$25080,$36EEE,$372EE);

    { Level name maximum lengths for version 1.0 }
    LevelNameLength_10:TLevelStringLength=
        (16,32,16,32,32,16,32,16,32,16,16,16,32,32,32,32,16,32,16);

    { Level entry name maximum lengths for version 1.0 }
    LevelEntryLength_10:TLevelStringLength=
        (32,48,48,48,48,48,48,32,48,48,48,80,48,64,64,64,48,48,48);

    { Versions currently supported are 1.0 and 1.4 }
    Version:array[0..1] of TVersion=(
        (
            { Keen 6 version 1.0 }
            ID:'1.0';
            ImageLength:254768;
            LZEXEImageLength:104906;
            DataResourceOffset:@DataResourceOffset_10;
            DataResourceLength:@DataResourceLength_10;
            LevelNameOffset:$1DA70;
            LevelNameLength:@LevelNameLength_10;
            LevelEntryOffset:$1DC40;
            LevelEntryLength:@LevelEntryLength_10
        ),(
            { Keen 6 version 1.4, level name/entry lengths same as v1.0 }
            ID:'1.4';
            ImageLength:260432;
            LZEXEImageLength:107687;
            DataResourceOffset:@DataResourceOffset_14;
            DataResourceLength:@DataResourceLength_10;
            LevelNameOffset:$1EF40;
            LevelNameLength:@LevelNameLength_10;
            LevelEntryOffset:$1F110;
            LevelEntryLength:@LevelEntryLength_10
        ));

{$I CKXPATCH.INC}

begin
    Main;
end.
