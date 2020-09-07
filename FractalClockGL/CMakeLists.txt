# CMakeList.txt : CMake project for FractalClock, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.

add_executable (FractalClock "Main.cpp" "Program.cpp" "Program.h" "Clock.cpp" "Clock.h" "SubClock.cpp" "SubClock.h" "Maths/Maths.h" "Maths/Vec2.h" "FractalClock.cpp" "FractalClock.h" "deps/glad/glad.c" "deps/glad/glad.h" "Shader.cpp" "Shader.h" "Drawable.cpp" "Drawable.h")

add_custom_command(TARGET FractalClock PRE_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/copy.bat ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(TARGET FractalClock PRE_BUILD COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/generate_shader_header.py ${CMAKE_CURRENT_SOURCE_DIR})


target_include_directories(FractalClock PUBLIC deps)

#Reduce binary size.
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
	set_property(TARGET FractalClock APPEND PROPERTY COMPILE_OPTIONS /NODEFAULTLIB:library)
endif()

target_link_libraries(FractalClock ${GLFW_LIBRARIES})



#file (COPY Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
