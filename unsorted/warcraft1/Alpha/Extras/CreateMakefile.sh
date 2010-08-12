#! /bin/sh

cp -f Extras/Tao.*.dll Extras/Tao.*.dll.config ./

mono Extras/Prebuild.exe /target makefile
if [ -f Makefile ]
then
    rm -rf Makefile
fi
mv Alpha.make Makefile
