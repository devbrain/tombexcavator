cmake_minimum_required(VERSION 3.17)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(src
        dst.cpp
        scriptHandler.cpp
        scriptHandler.h
        scriptHandlerCompiler.cpp
        scriptHandlerCompiler.h
        scriptHandlerDecompiler.cpp
        scriptHandlerDecompiler.h
        scriptOpcodesCompiler.cpp
        scriptOpcodesDecompiler.cpp
        stdafx.cpp
        stdafx.h)
