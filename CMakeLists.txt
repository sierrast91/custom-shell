cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDART 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(myshell)

include_directories(inc)
file(GLOB SOURCES "./src/*.c")
add_executable(myshell main.c ${SOURCES})
