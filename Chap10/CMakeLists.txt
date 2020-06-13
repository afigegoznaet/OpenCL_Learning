cmake_minimum_required(VERSION 3.5)

project(Chap10 LANGUAGES C)
include_directories("C:/Intel/OpenCL/sdk/include")
#link_directories("C:/Intel/OpenCL/sdk/lib/x64")
link_directories("C:/Intel/NVIDIA GPU Computing SDK/OpenCL/common/lib/x64")
#add_library(OpenCL STATIC IMPORTED)
link_libraries(OpenCL.lib)

configure_file(blank.cl ${CMAKE_CURRENT_BINARY_DIR}/blank.cl COPYONLY)
configure_file(reduction.cl ${CMAKE_CURRENT_BINARY_DIR}/reduction.cl COPYONLY)
configure_file(reduction_complete.cl ${CMAKE_CURRENT_BINARY_DIR}/reduction_complete.cl COPYONLY)

#add_executable(Chap10 reduction.c)
#add_executable(Chap10 reduction_complete.c)
add_executable(Chap10 wg_test.c)
