cmake_minimum_required(VERSION 3.28)
project(toralize C)

set(CMAKE_C_STANDARD 17)

add_library(toralize SHARED
        toralize.c
        toralize.h
)

set_target_properties(toralize PROPERTIES PREFIX "")

target_compile_definitions(toralize PRIVATE _GNU_SOURCE)

target_link_libraries(toralize dl)
