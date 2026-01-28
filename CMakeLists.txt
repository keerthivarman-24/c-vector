cmake_minimum_required(VERSION 4.0)
project(c_vector C)

set(CMAKE_C_STANDARD 23)

add_executable(c_vector
        main.c
        include/vector.h)
