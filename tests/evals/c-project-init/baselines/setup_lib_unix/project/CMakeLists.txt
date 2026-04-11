cmake_minimum_required(VERSION 3.25)

project(logkit LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(logkit-utils)

option(LOGKIT_ENABLE_TESTING "enable unit testing" OFF)
option(LOGKIT_ENABLE_ASAN "enable memory error detection" OFF)
option(LOGKIT_ENABLE_TSAN  "enable data race detection" OFF)
option(LOGKIT_ENABLE_UBSAN "enable undefined behavior detection" OFF)
option(LOGKIT_ENABLE_DYNAMIC_LIBRARY "build dynamic library" OFF)
option(LOGKIT_ENABLE_COVERAGE "enable code coverage reporting" OFF)

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

set(SRCS
)

list(APPEND SRCS
)

if(LOGKIT_ENABLE_DYNAMIC_LIBRARY)
    add_library(logkit SHARED ${SRCS})
else()
    add_library(logkit STATIC ${SRCS})
endif()

target_include_directories(logkit
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(logkit PRIVATE ${CMAKE_DL_LIBS})

logkit_apply_sanitizer(logkit LOGKIT_ENABLE_ASAN address)
logkit_apply_sanitizer(logkit LOGKIT_ENABLE_TSAN thread)
logkit_apply_sanitizer(logkit LOGKIT_ENABLE_UBSAN undefined)

if(LOGKIT_ENABLE_COVERAGE)
    target_compile_options(logkit PRIVATE --coverage)
    target_link_options(logkit PUBLIC --coverage)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION . FILES_MATCHING PATTERN "*")
install(TARGETS logkit DESTINATION lib)

add_subdirectory(examples)

if(LOGKIT_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
