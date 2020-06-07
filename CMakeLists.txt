cmake_minimum_required(VERSION 3.10)
project(fierce_c C)

set(CMAKE_C_STANDARD 11)

add_executable(fierce_c
        dict/5000.txt.c
        fierce_c/config.c
        fierce_c/config.h
        fierce_c/dns_request.c
        fierce_c/dns_request.h
        fierce_c/threads.c
        fierce_c/threads.h
        fierce_c/main.c)

if (WIN32)
    target_link_libraries(fierce_c ws2_32)
    add_custom_command(TARGET fierce_c POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/lib/libwinpthread-1.dll"
            $<TARGET_FILE_DIR:fierce_c>)
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(fierce_c Threads::Threads)
