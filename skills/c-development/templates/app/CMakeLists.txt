cmake_minimum_required(VERSION 3.16)

project({PROJECT} LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include({project}-utils)

option({PROJECT}_ENABLE_TESTING "enable unit testing" ON)
option({PROJECT}_ENABLE_ASAN "enable memory error detection" OFF)
option({PROJECT}_ENABLE_TSAN  "enable data race detection" OFF)
option({PROJECT}_ENABLE_UBSAN "enable undefined behavior detection" OFF)
option({PROJECT}_ENABLE_COVERAGE "enable code coverage reporting" OFF)

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
	# TODO: add source files
	# src/{project}-<module>.c
	# src/main.c
)

if(WIN32)
	list(APPEND SRCS
		# TODO: add Windows platform sources
	)
endif()

if(UNIX)
	list(APPEND SRCS
		# TODO: add Unix platform sources
	)
endif()

add_executable({project} ${SRCS})

target_link_libraries({project} PRIVATE ${CMAKE_DL_LIBS})

{project}_apply_sanitizer({project} {PROJECT}_ENABLE_ASAN address)
{project}_apply_sanitizer({project} {PROJECT}_ENABLE_TSAN thread)
{project}_apply_sanitizer({project} {PROJECT}_ENABLE_UBSAN undefined)

if({PROJECT}_ENABLE_COVERAGE AND UNIX)
    target_compile_options({project} PRIVATE --coverage)
    target_link_options({project} PRIVATE --coverage)
endif()

install(TARGETS {project} DESTINATION bin)

if({PROJECT}_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
