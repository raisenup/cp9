cmake_minimum_required(VERSION 3.29)
project(cp9 C)

set(CMAKE_C_STANDARD 11)

add_executable(cp9 main.c
        lib/common.c
        lib/common.h
        lib/validation.c
        lib/validation.h
        lib/sort.c
        lib/sort.h)
