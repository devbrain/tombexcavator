program DN1Mod;

{$M 16384,483328,483328}

uses LevelEditor;

var
    Editor:PEditor;

begin
    New(Editor,Init);
    if Editor<>nil then
    begin
        Editor^.Run;
        Dispose(Editor,Done);
    end;
end.
