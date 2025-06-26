include(TargetHelpers.cmake)

function(enable_tests sandbox_target test_target_name engine_lib_target copy_assets_target)

    if(NOT ENABLE_TESTS AND NOT ENABLE_COVERAGE)
        message(STATUS "Tests/Coverage disabled")
        return()
    endif()

    if(NOT TARGET GTest::gtest)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        )

        FetchContent_MakeAvailable(googletest)
    endif()

    enable_testing()

    add_executable(${test_target_name} tests/${test_target_name}.cpp)
    target_link_libraries(${test_target_name} PRIVATE GTest::gtest_main)
    target_include_directories(${test_target_name} PRIVATE ${BANKSYSTEMLIB_INCLUDE})

    target_add_pedantic_flags(${engine_lib_target})
    target_add_pedantic_flags(${sandbox_target})
    target_add_pedantic_flags(${test_target_name})

    include(GoogleTest)
    gtest_discover_tests(${test_target_name})

    if(ENABLE_COVERAGE)
        if(NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
            message(STATUS "Cannot enable coverage for ${CMAKE_CXX_COMPILER_ID}")
            return()
        endif()
    endif()

    if(ENABLE_TESTS AND NOT ENABLE_COVERAGE)
        message(STATUS "Tests enabled")
        add_custom_target(run_tests
            COMMAND ${CMAKE_BINARY_DIR}/bin/${test_target_name}
            COMMENT "Running tests"
        )
        add_dependencies(${test_target_name} ${engine_lib_target})
        add_dependencies(run_tests ${test_target_name})
    endif()


    if(ENABLE_COVERAGE)
        message(STATUS "Code coverage enabled")
        set(COVERAGE_COMPILE_FLAGS -fprofile-instr-generate -fcoverage-mapping)
        set(COVERAGE_LINK_FLAGS -fprofile-instr-generate)
    endif()

    if(ENABLE_COVERAGE)

        target_compile_options(${engine_lib_target} PRIVATE ${COVERAGE_COMPILE_FLAGS})
        target_link_options(${engine_lib_target} PRIVATE ${COVERAGE_LINK_FLAGS})

        target_compile_options(${test_target_name} PRIVATE ${COVERAGE_COMPILE_FLAGS})
        target_link_options(${test_target_name} PRIVATE ${COVERAGE_LINK_FLAGS})

        set(PROFRAW_MAIN "default.profraw")
        set(PROFRAW_TESTS "tests.profraw")
        set(PROFDATA_FILE "coverage.profdata")

        add_custom_target(run_test_program
            COMMAND LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/bin/${PROFRAW_MAIN} ${CMAKE_BINARY_DIR}/bin/${sandbox_target}
            COMMENT "Running main program to generate ${PROFRAW_MAIN}"
        )

        add_custom_target(run_tests
            COMMAND LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/bin/${PROFRAW_TESTS} ${CMAKE_BINARY_DIR}/bin/${test_target_name}
            COMMENT "Running tests to generate ${PROFRAW_TESTS}"
        )

        add_custom_target(merge_coverage_data
            COMMAND llvm-profdata-19 merge -sparse -output=${CMAKE_BINARY_DIR}/bin/${PROFDATA_FILE} ${CMAKE_BINARY_DIR}/bin/${PROFRAW_MAIN} ${CMAKE_BINARY_DIR}/bin/${PROFRAW_TESTS}
            DEPENDS run_test_program run_tests
            COMMENT "Merging coverage data into ${PROFDATA_FILE}"
        )

        add_custom_target(generate_coverage_report
            COMMAND llvm-cov-19 show ${CMAKE_BINARY_DIR}/bin/${sandbox_target} -instr-profile=${CMAKE_BINARY_DIR}/bin/${PROFDATA_FILE} -format=html -output-dir=${CMAKE_SOURCE_DIR}/coverage_report --show-branches=count --show-expansions
            DEPENDS merge_coverage_data
            COMMENT "Generating HTML coverage report in ${CMAKE_SOURCE_DIR}/coverage_report"
        )

        add_dependencies(run_test_program ${sandbox_target})
        add_dependencies(run_tests ${test_target_name})
        add_dependencies(generate_coverage_report merge_coverage_data)
        add_dependencies(${test_target_name} ${copy_assets_target})

    endif()
endfunction(enable_tests)
