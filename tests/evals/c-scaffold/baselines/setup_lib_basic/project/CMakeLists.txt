
cmake_minimum_required(VERSION 3.16)

project(xlib VERSION 0.0.1 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(xlib-utils)

option(XLIB_ENABLE_TESTING "enable unit testing" ON)
option(XLIB_ENABLE_ASAN "enable memory error detection" OFF)
option(XLIB_ENABLE_TSAN  "enable data race detection" OFF)
option(XLIB_ENABLE_UBSAN "enable undefined behavior detection" OFF)
option(XLIB_ENABLE_DYNAMIC_LIBRARY "build dynamic library" OFF)
option(XLIB_ENABLE_COVERAGE "enable code coverage reporting" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)

if(WIN32)
	add_compile_options("/experimental:c11atomics")
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set(SRCS
	# TODO: add source files
	# src/xlib-<module>.c
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

if(XLIB_ENABLE_DYNAMIC_LIBRARY)
    add_library(xlib SHARED ${SRCS})
else()
    add_library(xlib STATIC ${SRCS})
endif()

target_link_libraries(xlib PRIVATE ${CMAKE_DL_LIBS})

xlib_apply_sanitizer(xlib XLIB_ENABLE_ASAN address)
xlib_apply_sanitizer(xlib XLIB_ENABLE_TSAN thread)
xlib_apply_sanitizer(xlib XLIB_ENABLE_UBSAN undefined)

if(XLIB_ENABLE_COVERAGE AND UNIX)
    target_compile_options(xlib PRIVATE --coverage)
    target_link_options(xlib PRIVATE --coverage)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION . FILES_MATCHING PATTERN "*")
install(TARGETS xlib DESTINATION lib)

add_subdirectory(examples)

if(XLIB_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
