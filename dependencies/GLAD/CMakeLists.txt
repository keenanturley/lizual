cmake_minimum_required(VERSION 3.16)

project(glad VERSION 0.1.36 LANGUAGES C)

add_library(glad STATIC src/gl.c)

target_include_directories(glad PUBLIC include)