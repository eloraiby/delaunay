cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(delaunay C)

set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

add_library(
    delaunay
    SHARED
    delaunay.c
    delaunay.h
    )
