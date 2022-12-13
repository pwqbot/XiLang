cmake_minimum_required(VERSION 3.17)

project(
  templte
  VERSION 0.0.1
  LANGUAGES CXX)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    -Wall -Wextra -Wshadow -pedantic \
    -Wnon-virtual-dtor \
    -Wconversion \
    -Wunused \
    -Wlogical-op \
    -Woverloaded-virtual \
    -Wcast-align \
    -Wdouble-promotion \
    -Wnull-dereference \
    -Werror \
")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(lib)

include(CTest)
enable_testing()
add_subdirectory(tests)
