cmake_minimum_required(VERSION 3.25)

project(netkit LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(netkit-utils)

option(NETKIT_ENABLE_TESTING "enable unit testing" OFF)
option(NETKIT_ENABLE_ASAN "enable memory error detection" OFF)
option(NETKIT_ENABLE_TSAN  "enable data race detection" OFF)
option(NETKIT_ENABLE_UBSAN "enable undefined behavior detection" OFF)
option(NETKIT_ENABLE_DYNAMIC_LIBRARY "build dynamic library" OFF)
option(NETKIT_ENABLE_COVERAGE "enable code coverage reporting" OFF)

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

add_compile_options("/experimental:c11atomics")

set(SRCS
    src/netkit-err.c
    src/heap.c
    src/op.c
    src/netkit-loop.c
    src/netkit-tcp.c
    src/netkit-udp.c
    src/netkit-timer.c
)

if(NETKIT_ENABLE_DYNAMIC_LIBRARY)
    add_library(netkit SHARED ${SRCS})
else()
    add_library(netkit STATIC ${SRCS})
endif()

target_include_directories(netkit
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(netkit PRIVATE ws2_32 mswsock)

netkit_apply_sanitizer(netkit NETKIT_ENABLE_ASAN address)
netkit_apply_sanitizer(netkit NETKIT_ENABLE_TSAN thread)
netkit_apply_sanitizer(netkit NETKIT_ENABLE_UBSAN undefined)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION . FILES_MATCHING PATTERN "*")
install(TARGETS netkit DESTINATION lib)

add_subdirectory(examples)

if(NETKIT_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
