cmake_minimum_required(VERSION 3.16)

project(xapp VERSION 0.0.1 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(xapp-utils)

option(XAPP_ENABLE_TESTING "enable unit testing" ON)
option(XAPP_ENABLE_ASAN "enable memory error detection" OFF)
option(XAPP_ENABLE_TSAN  "enable data race detection" OFF)
option(XAPP_ENABLE_UBSAN "enable undefined behavior detection" OFF)
option(XAPP_ENABLE_COVERAGE "enable code coverage reporting" OFF)

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
	# src/xapp-<module>.c
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

add_executable(xapp ${SRCS})

target_link_libraries(xapp PRIVATE ${CMAKE_DL_LIBS})

xapp_apply_sanitizer(xapp XAPP_ENABLE_ASAN address)
xapp_apply_sanitizer(xapp XAPP_ENABLE_TSAN thread)
xapp_apply_sanitizer(xapp XAPP_ENABLE_UBSAN undefined)

if(XAPP_ENABLE_COVERAGE AND UNIX)
    target_compile_options(xapp PRIVATE --coverage)
    target_link_options(xapp PRIVATE --coverage)
endif()

install(TARGETS xapp DESTINATION bin)

if(XAPP_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
