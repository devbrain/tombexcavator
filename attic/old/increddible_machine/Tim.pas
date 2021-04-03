{ The Inredible Machine resource extractor v1.0 }
{ (c) 1999 by John Doe                          }
{ mail me:    john_doe@talknet.de               }
{ Public Domain Souce Code                      }

uses crt,nTools;
var f:file;
    cname,outdir,name,res:string;
    size:longint;
begin
 writeln('The Inredible Machine resource extractor 1.0     (c) 1999 by John Doe');
 if(paramCount=0)then begin
  writeln('use: tim [resource file] (output dir)');
  writeln('if output directory is not given all files are');
  writeln('extracted into the current directory.');
  halt;
 end;
 cname:=paramStr(1);
 if(paramCount=2)then outdir:=paramStr(2) else outdir:='.\';
 if(outdir[length(outdir)]<>'\')then outdir:=outdir+'\';
 assign(f,cname); reset(f,1);
 repeat
  blockread(f,name[1],13); name[0]:=#13;
  name:=Copy(name,1,pos(#0,name)-1);
  blockread(f,size,4);
  writeln('Extracting ',name,' (',size,' bytes)...');
  CopyFileTo(f,outdir+name,size);
 until eof(f);
 close(f);
end.