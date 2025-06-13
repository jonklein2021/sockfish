cmake_minimum_required(VERSION 3.12)
project(MyCppProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wno-parentheses -pedantic -O3)

# List common CXX files
file(GLOB_RECURSE COMMON_SOURCES
    backend/*.cpp
    engine/*.cpp
    game/cli/*.cpp
    game/gui/*.cpp
)

# Add headers
include_directories(common backend engine game/cli game/gui)

# SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Executables
add_executable(main main.cpp ${COMMON_SOURCES})
target_link_libraries(main PUBLIC sfml-graphics sfml-window sfml-system)

add_executable(test test.cpp ${COMMON_SOURCES})
target_link_libraries(test PUBLIC sfml-graphics sfml-window sfml-system)
