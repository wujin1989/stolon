cmake_minimum_required(VERSION 3.25)

project(svcmon LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(svcmon-utils)

option(SVCMON_ENABLE_TESTING "enable unit testing" OFF)
option(SVCMON_ENABLE_ASAN "enable memory error detection" OFF)
option(SVCMON_ENABLE_TSAN  "enable data race detection" OFF)
option(SVCMON_ENABLE_UBSAN "enable undefined behavior detection" OFF)
option(SVCMON_ENABLE_COVERAGE "enable code coverage reporting" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)

add_compile_options("/experimental:c11atomics")

set(SRCS
)

add_library(svcmon_lib STATIC ${SRCS})

target_include_directories(svcmon_lib
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(svcmon_lib PRIVATE ${CMAKE_DL_LIBS})

svcmon_apply_sanitizer(svcmon_lib SVCMON_ENABLE_ASAN address)
svcmon_apply_sanitizer(svcmon_lib SVCMON_ENABLE_TSAN thread)
svcmon_apply_sanitizer(svcmon_lib SVCMON_ENABLE_UBSAN undefined)

add_executable(svcmon src/main.c)
target_link_libraries(svcmon PRIVATE svcmon_lib)

svcmon_apply_sanitizer(svcmon SVCMON_ENABLE_ASAN address)
svcmon_apply_sanitizer(svcmon SVCMON_ENABLE_TSAN thread)
svcmon_apply_sanitizer(svcmon SVCMON_ENABLE_UBSAN undefined)

install(TARGETS svcmon DESTINATION bin)

if(SVCMON_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
