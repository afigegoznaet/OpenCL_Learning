cmake_minimum_required(VERSION 3.5)

project(Chap12 LANGUAGES C)
include_directories("C:/Intel/OpenCL/sdk/include")
#link_directories("C:/Intel/OpenCL/sdk/lib/x64")
link_directories("C:/Intel/NVIDIA GPU Computing SDK/OpenCL/common/lib/x64")
#add_library(OpenCL STATIC IMPORTED)
link_libraries(OpenCL.lib)

configure_file(matrix_mult.cl ${CMAKE_CURRENT_BINARY_DIR}/matrix_mult.cl COPYONLY)
configure_file(qr.cl ${CMAKE_CURRENT_BINARY_DIR}/qr.cl COPYONLY)
configure_file(transpose.cl ${CMAKE_CURRENT_BINARY_DIR}/transpose.cl COPYONLY)
configure_file(vec_reflect.cl ${CMAKE_CURRENT_BINARY_DIR}/vec_reflect.cl COPYONLY)

#add_executable(Chap12 matrix_mult.c)
add_executable(Chap12 qr.c)
#add_executable(Chap12 transpose.c)
#add_executable(Chap12 vec_reflect.c)
