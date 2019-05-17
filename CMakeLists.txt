cmake_minimum_required(VERSION 3.10)
project(c_chat C)

set(CMAKE_C_STANDARD 11)

find_package(Threads)

include_directories(src)
include_directories(src/utils)

add_executable(c_chat
        src/utils/alist.c src/utils/alist.h
        src/utils/sockutil.c src/utils/sockutil.h
        src/common.c src/common.h
        src/server.c src/server.h
        src/client.c src/client.h
        src/termin.c src/termin.h
        src/main.c)

target_link_libraries(c_chat ${CMAKE_THREAD_LIBS_INIT})