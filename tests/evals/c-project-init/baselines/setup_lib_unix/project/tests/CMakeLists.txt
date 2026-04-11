cmake_minimum_required(VERSION 3.25)

project(tests LANGUAGES C)

include(logkit-utils)

# logkit_add_test(<module>)

if(LOGKIT_ENABLE_COVERAGE)
    find_program(LCOV_BIN lcov)
    find_program(GENHTML_BIN genhtml)
    if(LCOV_BIN AND GENHTML_BIN)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ctest --test-dir ${CMAKE_BINARY_DIR} -C $<CONFIG> --output-on-failure
            COMMAND ${LCOV_BIN} --capture --directory . --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info
            COMMAND ${GENHTML_BIN} ${CMAKE_BINARY_DIR}/coverage/coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage/html
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "generating html coverage report in coverage/html/index.html"
        )
    else()
        message(WARNING "lcov or genhtml not found; 'coverage' target not created.")
    endif()
endif()
