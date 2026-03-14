# Updated to explicitly support your modern CMake version 4.2
cmake_minimum_required(VERSION 4.2)

project(CocoUI VERSION 0.1.0 LANGUAGES CXX)

# Disable compiler-specific extensions (e.g., gnu++14) to ensure strict standard compliance
set(CMAKE_CXX_EXTENSIONS OFF)

# ==============================================================================
# 1. LLVM TOOLS CONFIGURATION (Clang-Tidy & Clang-Format)
# ==============================================================================

# Crucial for clang-tidy and LSP (Language Server Protocol) to work in VSCode/Neovim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -- CLANG-TIDY INTEGRATION --
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    # This automatically runs clang-tidy every time you compile a target
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-fix") 
else()
    message(WARNING "clang-tidy not found! Static analysis will be disabled.")
endif()

# -- CLANG-FORMAT INTEGRATION --
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXE}")
    
    # Gather all C++ source and header files in the project
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        "src/*.[ch]pp"
        "src/*.cc"
        "include/*.[ch]pp"
    )
    
    # Create a custom target to format the code manually
    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_CXX_SOURCE_FILES}
        COMMENT "Formatting C++ source code with clang-format..."
    )
else()
    message(WARNING "clang-format not found! 'format' target will not be available.")
endif()

# ==============================================================================
# 2. DEPENDENCIES (Raylib)
# ==============================================================================
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5 
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# ==============================================================================
# 3. COCOUI CORE (Target: Embedded Microcontroller)
# ==============================================================================
add_library(cocoui_core STATIC src/core.cpp)
target_include_directories(cocoui_core PUBLIC include)

# Force strict C++14 for the core framework (Microcontroller compatibility)
target_compile_features(cocoui_core PUBLIC cxx_std_14)

# ==============================================================================
# 4. PC SIMULATOR (Target: Desktop / Debugging)
# ==============================================================================
add_executable(cocoui_simulator src/simulator/main.cpp)
target_link_libraries(cocoui_simulator PRIVATE cocoui_core raylib)

# Allow modern C++20 features strictly for the PC simulator
target_compile_features(cocoui_simulator PRIVATE cxx_std_20)