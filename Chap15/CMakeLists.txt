cmake_minimum_required(VERSION 3.5)

project(Chap16 LANGUAGES C)
include_directories("C:/Intel/OpenCL/sdk/include" "D:/Workspace/glew-2.1.0/include" "D:/Workspace/freeglut/include" )
#link_directories("C:/Intel/OpenCL/sdk/lib/x64")
link_directories("C:/Intel/NVIDIA GPU Computing SDK/OpenCL/common/lib/x64" "D:/Workspace/glew-2.1.0/lib/Release/x64" "D:/Workspace/freeglut/lib/x64")
#add_library(OpenCL STATIC IMPORTED)
link_libraries(OpenCL.lib glew32.lib freeglut.lib)

configure_file(sphere.cl ${CMAKE_CURRENT_BINARY_DIR}/sphere.cl COPYONLY)
configure_file(sphere.frag ${CMAKE_CURRENT_BINARY_DIR}/sphere.frag COPYONLY)
configure_file(sphere.vert ${CMAKE_CURRENT_BINARY_DIR}/sphere.vert COPYONLY)


configure_file(basic_interop.cl ${CMAKE_CURRENT_BINARY_DIR}/basic_interop.cl COPYONLY)
configure_file(basic_interop.frag ${CMAKE_CURRENT_BINARY_DIR}/basic_interop.frag COPYONLY)
configure_file(basic_interop.vert ${CMAKE_CURRENT_BINARY_DIR}/basic_interop.vert COPYONLY)

#add_executable(Chap16 basic_interop.c)
add_executable(Chap16 sphere.c)
