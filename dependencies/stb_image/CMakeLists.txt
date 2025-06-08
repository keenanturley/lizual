cmake_minimum_required(VERSION 3.16)

project(stb_image VERSION 0.1.36 LANGUAGES C)

add_library(stb_image STATIC stb_image.cpp)

target_include_directories(stb_image PUBLIC .)
