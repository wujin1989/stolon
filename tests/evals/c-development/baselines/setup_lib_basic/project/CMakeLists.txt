cmake_minimum_required(VERSION 3.16)

project(MY_UTILS VERSION 0.0.1 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(my-utils-utils)

option(MY_UTILS_ENABLE_TESTING "enable unit testing" ON)
option(MY_UTILS_ENABLE_ASAN "enable memory error detection" OFF)
option(MY_UTILS_ENABLE_TSAN  "enable data race detection" OFF)
option(MY_UTILS_ENABLE_UBSAN "enable undefined behavior detection" OFF)
option(MY_UTILS_ENABLE_DYNAMIC_LIBRARY "build dynamic library" OFF)
option(MY_UTILS_ENABLE_COVERAGE "enable code coverage reporting" OFF)

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
	# src/my-utils-<module>.c
)

if(WIN32)
	list(APPEND SRCS
	)
endif()

if(UNIX)
	list(APPEND SRCS
	)
endif()

if(MY_UTILS_ENABLE_DYNAMIC_LIBRARY)
    add_library(my-utils SHARED ${SRCS})
else()
    add_library(my-utils STATIC ${SRCS})
endif()

target_link_libraries(my-utils PRIVATE ${CMAKE_DL_LIBS})

my_utils_apply_sanitizer(my-utils MY_UTILS_ENABLE_ASAN address)
my_utils_apply_sanitizer(my-utils MY_UTILS_ENABLE_TSAN thread)
my_utils_apply_sanitizer(my-utils MY_UTILS_ENABLE_UBSAN undefined)

if(MY_UTILS_ENABLE_COVERAGE AND UNIX)
    target_compile_options(my-utils PRIVATE --coverage)
    target_link_options(my-utils PRIVATE --coverage)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION . FILES_MATCHING PATTERN "*")
install(TARGETS my-utils DESTINATION lib)

add_subdirectory(examples)

if(MY_UTILS_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
