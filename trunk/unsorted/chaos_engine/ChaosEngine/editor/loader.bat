rem @echo off

rem
rem  Chaos Engine run script
rem
rem  written by JOTD
rem

set OLDCD=%CD%

set MP_ROOT_DIR=%CD%

rem increase heap size if necessary

set HEAP_SIZE=100m


start javaw -classpath "%MP_ROOT_DIR%\bin" -DROOT_DIR="%MP_ROOT_DIR%" -Xmx%HEAP_SIZE% %*
cd %OLDCD%
