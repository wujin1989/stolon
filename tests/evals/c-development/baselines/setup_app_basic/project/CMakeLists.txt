cmake_minimum_required(VERSION 3.16)

project(HELLO_WORLD VERSION 0.0.1 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(hello-world-utils)

option(HELLO_WORLD_ENABLE_TESTING "enable unit testing" ON)
option(HELLO_WORLD_ENABLE_ASAN "enable memory error detection" OFF)
option(HELLO_WORLD_ENABLE_TSAN  "enable data race detection" OFF)
option(HELLO_WORLD_ENABLE_UBSAN "enable undefined behavior detection" OFF)
option(HELLO_WORLD_ENABLE_COVERAGE "enable code coverage reporting" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)

if(WIN32)
	add_compile_options("/experimental:c11atomics")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(SRCS
	src/main.c
)

if(WIN32)
	list(APPEND SRCS
	)
endif()

if(UNIX)
	list(APPEND SRCS
	)
endif()

add_executable(hello-world ${SRCS})

target_link_libraries(hello-world PRIVATE ${CMAKE_DL_LIBS})

hello_world_apply_sanitizer(hello-world HELLO_WORLD_ENABLE_ASAN address)
hello_world_apply_sanitizer(hello-world HELLO_WORLD_ENABLE_TSAN thread)
hello_world_apply_sanitizer(hello-world HELLO_WORLD_ENABLE_UBSAN undefined)

if(HELLO_WORLD_ENABLE_COVERAGE AND UNIX)
    target_compile_options(hello-world PRIVATE --coverage)
    target_link_options(hello-world PRIVATE --coverage)
endif()

install(TARGETS hello-world DESTINATION bin)

if(HELLO_WORLD_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
