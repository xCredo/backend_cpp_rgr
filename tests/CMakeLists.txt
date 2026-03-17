cmake_minimum_required(VERSION 3.14)

# Find system Catch2
find_package(Catch2 REQUIRED)

add_executable(run_tests test_small_vector.cpp)
target_link_libraries(run_tests PRIVATE SmallVector Catch2::Catch2WithMain)

# Enable testing
enable_testing()
add_test(NAME SmallVectorTests COMMAND run_tests)

# Valgrind target
if(ENABLE_VALGRIND)
    find_program(VALGRIND_EXE NAMES valgrind)
    if(VALGRIND_EXE)
        add_test(
            NAME SmallVectorValgrind
            COMMAND ${VALGRIND_EXE} --leak-check=full --error-exitcode=1 $<TARGET_FILE:run_tests>
        )
    endif()
endif()

# Run tests
if(ENABLE_TESTS)
    add_test(NAME SmallVectorTests COMMAND run_tests)
endif()
