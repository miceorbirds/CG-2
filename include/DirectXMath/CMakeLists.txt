cmake_minimum_required(VERSION 3.17) #required version of cmake
project(DirectXMath)
set(CMAKE_CXX_STANDARD 17) # specify the C++ standard


add_library(DirectXMath STATIC Inc/DirectXMath.h)
set_target_properties(DirectXMath PROPERTIES LINKER_LANGUAGE CXX)