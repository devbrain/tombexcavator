#! /bin/sh

cp -f Extras/Tao.*.dll Extras/Tao.*.dll.config ./

mono Extras/Prebuild.exe /target monodev
