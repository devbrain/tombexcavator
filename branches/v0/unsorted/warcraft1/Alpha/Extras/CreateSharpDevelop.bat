copy SDL.dll ..\
copy Tao.*.dll ..\
copy Tao.*.dll.config ..\

Prebuild /target sharpdev /file ..\prebuild.xml /pause
