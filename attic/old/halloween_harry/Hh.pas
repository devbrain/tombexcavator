uses crt,nTools;
type _fr=record
      name:string[12];
      pos,size:longint;
     end;
var fr:array[0..1024]of _fr;
    f:file;
    l,c:word;
    s,nm:string;
begin
 writeln('Halloween Harry file extractor 1.0     (c) 1999 by John Doe');
 if paramCount<1 then begin
  writeln('use: hh [filename]');
  halt;
 end;
 nm:=paramStr(1);
 writeln('file ',nm);
 c:=0;
 s:=copy(nm,1,pos('.',nm)-1);
 if not existpath(s) then mkdir(s);
 assign(f,s+'.fat'); reset(f,1);
 repeat
  with fr[c] do begin
   blockread(f,name[0],13);
   blockread(f,pos,4);
   blockread(f,size,4);
  end;
  inc(c);
 until eof(f);
 close(f);
 assign(f,nm+s); reset(f,1);
 for l:=0 to c-1 do begin
  with fr[l] do begin
   writeln('extracing ',name,'...');
   seek(f,pos);
   CopyFileTo(f,nm+'\'+name,size);
  end;
 end;
 close(f);
end.