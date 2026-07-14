cmake_minimum_required(VERSION 3.20)

project(GQuest
    VERSION 0.1
    DESCRIPTION "GQuest development environment"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

add_subdirectory(libs)
add_subdirectory(src)