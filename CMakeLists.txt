# opencl compatible
cmake_minimum_required(VERSION 3.28)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

add_executable(cpp main.cpp
        helper_functions.h
        helper_functions.cpp
        multi_thread.h
        multi_thread_implementation.cpp
        opencl_implementation.h
        opencl_implementation.cpp
        global_variables.h)

target_link_libraries(cpp ${OpenCL_LIBRARY})