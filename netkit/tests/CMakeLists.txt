cmake_minimum_required(VERSION 3.25)

project(tests LANGUAGES C)

include(netkit-utils)

# netkit_add_test(<module>)
netkit_add_test(err)
netkit_add_test(heap)
netkit_add_test(loop)
netkit_add_test(timer)
netkit_add_test(tcp)
netkit_add_test(udp)

if(NETKIT_ENABLE_COVERAGE)
    find_program(OPENCPPCOVERAGE_BIN OpenCppCoverage)
    if(OPENCPPCOVERAGE_BIN)
        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/src" SRC_DIR_NATIVE)
        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/tests" TESTS_DIR_NATIVE)
        file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/coverage" COVERAGE_OUTPUT_DIR_NATIVE)

        add_custom_target(coverage
            COMMAND ${OPENCPPCOVERAGE_BIN}
                "--sources=${SRC_DIR_NATIVE}"
                "--excluded_sources=${TESTS_DIR_NATIVE}"
                "--export_type=html:${COVERAGE_OUTPUT_DIR_NATIVE}"
                "--cover_children"
                "--"
                ctest.exe --test-dir "${CMAKE_BINARY_DIR}" -C $<CONFIG> --output-on-failure
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "generating HTML coverage report in coverage/index.html"
            USES_TERMINAL
        )
    else()
        message(WARNING "OpenCppCoverage not found. install it and add to PATH.")
    endif()
endif()
