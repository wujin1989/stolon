cmake_minimum_required(VERSION 3.25)

project(xprobe LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(xprobe-utils)

option(XPROBE_ENABLE_TESTING "enable unit testing" OFF)
option(XPROBE_ENABLE_ASAN "enable memory error detection" OFF)
option(XPROBE_ENABLE_TSAN  "enable data race detection" OFF)
option(XPROBE_ENABLE_UBSAN "enable undefined behavior detection" OFF)
option(XPROBE_ENABLE_COVERAGE "enable code coverage reporting" OFF)

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

set(SRCS
)

if(WIN32)
	list(APPEND SRCS
	)
endif()

if(UNIX)
	list(APPEND SRCS
	)
endif()

add_library(xprobe_lib STATIC ${SRCS})

target_include_directories(xprobe_lib
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(xprobe_lib PRIVATE ${CMAKE_DL_LIBS})

xprobe_apply_sanitizer(xprobe_lib XPROBE_ENABLE_ASAN address)
xprobe_apply_sanitizer(xprobe_lib XPROBE_ENABLE_TSAN thread)
xprobe_apply_sanitizer(xprobe_lib XPROBE_ENABLE_UBSAN undefined)

if(XPROBE_ENABLE_COVERAGE AND UNIX)
    target_compile_options(xprobe_lib PRIVATE --coverage)
    target_link_options(xprobe_lib PUBLIC --coverage)
endif()

add_executable(xprobe src/main.c)
target_link_libraries(xprobe PRIVATE xprobe_lib)

xprobe_apply_sanitizer(xprobe XPROBE_ENABLE_ASAN address)
xprobe_apply_sanitizer(xprobe XPROBE_ENABLE_TSAN thread)
xprobe_apply_sanitizer(xprobe XPROBE_ENABLE_UBSAN undefined)

install(TARGETS xprobe DESTINATION bin)

if(XPROBE_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
