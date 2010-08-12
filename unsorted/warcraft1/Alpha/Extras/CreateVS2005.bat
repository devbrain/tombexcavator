copy SDL.dll ..\
copy Tao.*.dll ..\
copy Tao.*.dll.config ..\

Prebuild /target vs2005 /file ..\prebuild.xml /pause
