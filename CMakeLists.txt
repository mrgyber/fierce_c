cmake_minimum_required(VERSION 3.10)
project(dns C)

set(CMAKE_C_STANDARD 11)

add_executable(dns
        dict/5000.txt.c
        dns/config.c
        dns/config.h
        dns/dns_request.c
        dns/dns_request.h
        dns/threads.c
        dns/threads.h
        dns/main.c)

if (WIN32)
    target_link_libraries(dns ws2_32)
    add_custom_command(TARGET dns POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/lib/libwinpthread-1.dll"
            $<TARGET_FILE_DIR:dns>)
endif ()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(dns Threads::Threads)
