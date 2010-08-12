copy SDL.dll ..\
copy Tao.*.dll ..\
copy Tao.*.dll.config ..\

Prebuild /target vs2003 /file ..\prebuild.xml /pause
