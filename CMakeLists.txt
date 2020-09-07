# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("FractalClockGL")

find_package(GLFW REQUIRED)

include_directories(${GLFW_INCLUDE_DIRS})

add_subdirectory("FractalClockGL")