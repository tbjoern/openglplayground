cmake_minimum_required (VERSION 2.6)
project(openglplayground)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 17)

include_directories(library ${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
add_subdirectory(src)
