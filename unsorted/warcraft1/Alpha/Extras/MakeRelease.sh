#! /bin/sh
#
#       This is a script that create 2 release archive, one
#       with the binaries and one with the source.
#
#       This script is remarkably crappy.
#

DATA_FILES="Data/*.jpg Data/*.png prebuild.xml COPYING README"

BINARY_FILES="$DATA_FILES Tao.*.dll Tao.*.dll.config Alpha.dll Alpha-WC1.exe Alpha-Demo.exe SDL.dll"

SOURCE_FILES="$DATA_FILES Extras/Create* Extras/Clean* \
        Extras/Tao.*.dll Extras/Tao.*.dll.config Extras/SDL.dll \
        Alpha/*/*.cs Alpha-WC1/*.cs Alpha-Demo/*.cs"

RELEASE_NAME=alpha-snapshot-`date +%d-%m-%y`

echo Doing a fresh compile
./Extras/Clean.sh
./Extras/CreateMakefile.sh
make
cp Extras/SDL.dll ./

echo Binary release
rm -rf $RELEASE_NAME $RELEASE_NAME-bin.tar.gz
mkdir -p $RELEASE_NAME
cp --parents $BINARY_FILES $RELEASE_NAME
tar czvf $RELEASE_NAME-bin.tar.gz $RELEASE_NAME
rm -rf $RELEASE_NAME

echo Source release
rm -rf $RELEASE_NAME $RELEASE_NAME-src.tar.gz
mkdir -p $RELEASE_NAME
cp --parents $SOURCE_FILES $RELEASE_NAME
tar czvf $RELEASE_NAME-src.tar.gz $RELEASE_NAME
rm -rf $RELEASE_NAME

