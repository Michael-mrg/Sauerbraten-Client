project(sauer_spec)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE debug)
set(CMAKE_C_FLAGS "-W -Wall -std=c99 -Wno-unused -Werror")
set(SRCS stream.c stream.h demo.c demo.h main.c main.h)

add_executable(sauer_spec ${SRCS})
target_link_libraries(sauer_spec enet z)

