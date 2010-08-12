#! /bin/sh

rm -rf Alpha.dll
rm -rf Alpha.dll.mdb
rm -rf Alpha.pdb
rm -rf Alpha.dll.manifest
rm -rf Alpha/Alpha.mdp

rm -rf Alpha-WC1.exe
rm -rf Alpha-WC1.exe.mdb
rm -rf Alpha-WC1.pdb
rm -rf Alpha-WC1.exe.manifest
rm -rf Alpha-WC1/Alpha-WC1.mdp

rm -rf Alpha-Demo.exe
rm -rf Alpha-Demo.exe.mdb
rm -rf Alpha-Demo.pdb
rm -rf Alpha-Demo.exe.manifest
rm -rf Alpha-Demo/Alpha-Demo.mdp

rm -rf SDL.dll
rm -rf Tao.*.dll
rm -rf Tao.*.dll.config
rm -rf Makefile

mono Extras/Prebuild.exe /clean all
