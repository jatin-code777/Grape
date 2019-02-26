cmake_minimum_required(VERSION 2.6)
project(grape)
set( CMAKE_CXX_FLAGS "-O3")
add_library(BM BM.cpp)
add_library(call call.cpp)
add_library(argparser argparser.cpp)

