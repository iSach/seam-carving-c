cmake_minimum_required(VERSION 3.16)
project(image_slimming C)

set(CMAKE_C_STANDARD 99)

add_executable(image_slimming mainSlimming.c PNM.c slimming.c slimming.c)