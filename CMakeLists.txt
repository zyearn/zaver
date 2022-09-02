cmake_minimum_required (VERSION 2.6)
project (zaver)

file(GLOB SOURCES "src/*.c")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-g -O2 -Wall -ggdb3 -Wextra -pthread -DNDEBUG")
add_executable(zaver ${SOURCES})
add_subdirectory(tests)
