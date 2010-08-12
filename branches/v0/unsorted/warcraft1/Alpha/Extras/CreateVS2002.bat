copy SDL.dll ..\
copy Tao.*.dll ..\
copy Tao.*.dll.config ..\

Prebuild /target vs2002 /file ..\prebuild.xml /pause
