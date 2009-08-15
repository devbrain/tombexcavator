unit Config;

{ Configuration file manager }

interface

uses GCore;

type
    PConfig=^TConfig;
    TConfig=object
        public
            ScreenSize:TGraphCoord;
            AnimFlash,AnimActive,DrawBridge,DrawConveyor,DrawMulti:Boolean;
            Path:array[1..3] of String;
            constructor Init;
            destructor Done;
        private
            procedure Load(InputFileName:String);
            procedure Save(OutputFileName:String);
    end;

implementation

const
    DefaultConfigFile='DN1MOD.CFG';

{ --- TConfig methods ----------------------------------------------------- }

constructor TConfig.Init;
begin
    ScreenSize.X:=640;
    ScreenSize.Y:=350;
    AnimFlash:=True;
    AnimActive:=True;
    DrawBridge:=True;
    DrawConveyor:=True;
    DrawMulti:=True;
    Path[1]:='.';
    Path[2]:='.';
    Path[3]:='.';
    Load(DefaultConfigFile);
end;

destructor TConfig.Done;
begin
    Save(DefaultConfigFile);
end;

procedure TConfig.Load(InputFileName:String);
var
    InputFile:Text;
    Option:String;

    function ReadOption(var F:Text):String;
    var
        S:String;
        C:Char;
    begin
        S:='';
        C:=#0;
        while not Eof(F) do
        begin
            Read(F,C);
            if not (C in [#13,' ']) then S:=S+UpCase(C) else Break;
        end;
        ReadOption:=S;
    end;
    function AppendBackslash(S:String):String;
    begin
        if (Length(S)>0) and (S[Length(S)]<>'\') then S:=S+'\';
        AppendBackslash:=S;
    end;
begin
    Assign(InputFile,InputFileName);
    Reset(InputFile);
    if IOResult=0 then
    begin
        while not Eof(InputFile) do
        begin
            Option:=ReadOption(InputFile);
            if Option='SCREENSIZE' then Readln(InputFile,ScreenSize.X,ScreenSize.Y)
                else if Option='ANIMFLASH' then Readln(InputFile,Byte(AnimFlash))
                else if Option='ANIMACTIVE' then Readln(InputFile,Byte(AnimActive))
                else if Option='DRAWBRIDGE' then Readln(InputFile,Byte(DrawBridge))
                else if Option='DRAWCONVEYOR' then Readln(InputFile,Byte(DrawConveyor))
                else if Option='DRAWMULTI' then Readln(InputFile,Byte(DrawMulti))
                else if Option='DN1PATH' then Readln(InputFile,Path[1])
                else if Option='DN2PATH' then Readln(InputFile,Path[2])
                else if Option='DN3PATH' then Readln(InputFile,Path[3]);
        end;
        Path[1]:=AppendBackslash(Path[1]);
        Path[2]:=AppendBackslash(Path[2]);
        Path[3]:=AppendBackslash(Path[3]);
        Close(InputFile);
    end;
end;

procedure TConfig.Save(OutputFileName:String);
var
    OutputFile:Text;
begin
    Assign(OutputFile,OutputFileName);
    Rewrite(OutputFile);
    if IOResult=0 then
    begin
        Writeln(OutputFile,'ScreenSize ',ScreenSize.X,' ',ScreenSize.Y);
        Writeln(OutputFile,'AnimFlash ',Byte(AnimFlash));
        Writeln(OutputFile,'AnimActive ',Byte(AnimActive));
        Writeln(OutputFile,'DrawBridge ',Byte(DrawBridge));
        Writeln(OutputFile,'DrawConveyor ',Byte(DrawConveyor));
        Writeln(OutputFile,'DrawMulti ',Byte(DrawMulti));
        Writeln(OutputFile,'DN1Path ',Path[1]);
        Writeln(OutputFile,'DN2Path ',Path[2]);
        Writeln(OutputFile,'DN3Path ',Path[3]);
        Close(OutputFile);
    end;
end;

end.
