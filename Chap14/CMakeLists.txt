cmake_minimum_required(VERSION 3.5)

project(Chap14 LANGUAGES C)
include_directories("C:/Intel/OpenCL/sdk/include")
link_directories("C:/Intel/OpenCL/sdk/lib/x64")
#link_directories("C:/Intel/NVIDIA GPU Computing SDK/OpenCL/common/lib/x64")
#add_library(OpenCL STATIC IMPORTED)
link_libraries(OpenCL.lib)

configure_file(rdft.cl ${CMAKE_CURRENT_BINARY_DIR}/rdft.cl COPYONLY)
configure_file(fft.cl ${CMAKE_CURRENT_BINARY_DIR}/fft.cl COPYONLY)

#add_executable(Chap14 rdft.c )
add_executable(Chap14 fft.c  )
