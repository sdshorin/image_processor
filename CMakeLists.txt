cmake_minimum_required(VERSION 3.17)
project(image_processor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra -Werror")

add_executable(image_processor
    image_processor.cpp
    arg_parser.cpp
    filters.cpp
    image.cpp
    fabric.cpp
)
