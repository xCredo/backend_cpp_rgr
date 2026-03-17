cmake_minimum_required(VERSION 3.14)
project(SmallVector VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Library as INTERFACE (header-only)
add_library(SmallVector INTERFACE)
target_include_directories(SmallVector INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Options for sanitizers
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_USAN "Enable UndefinedBehaviorSanitizer" OFF)
option(ENABLE_VALGRIND "Enable Valgrind tests" OFF)

# Sanitizer configuration
if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

if(ENABLE_USAN)
    add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=undefined)
endif()

# Clang-tidy configuration
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE} -warnings-as-errors=*)
    endif()
endif()

# Enable testing
enable_testing()

# Subdirectories
add_subdirectory(tests)
add_subdirectory(examples)
