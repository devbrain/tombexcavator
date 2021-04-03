@echo off

vasmm68k -phxass -no-opt -Fhunk -o z2.o z2.asm
vlink -S -s -o ..\z2 z2.o

del z2.o
