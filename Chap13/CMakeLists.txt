cmake_minimum_required(VERSION 3.5)

project(Chap1 LANGUAGES C)
include_directories("C:/Intel/OpenCL/sdk/include")
#link_directories("C:/Intel/OpenCL/sdk/lib/x64")
link_directories("C:/Intel/NVIDIA GPU Computing SDK/OpenCL/common/lib/x64")
#add_library(OpenCL STATIC IMPORTED)
link_libraries(OpenCL.lib)

configure_file(conj_grad.cl ${CMAKE_CURRENT_BINARY_DIR}/conj_grad.cl COPYONLY)
configure_file(steep_desc.cl ${CMAKE_CURRENT_BINARY_DIR}/steep_desc.cl COPYONLY)
configure_file(bcsstk05.mtx ${CMAKE_CURRENT_BINARY_DIR}/bcsstk05.mtx COPYONLY)

#add_executable(Chap13 conj_grad.c mmio.c mmio.h)
#add_executable(Chap13 read_mm.c mmio.c mmio.h)
add_executable(Chap13 steep_desc.c mmio.c mmio.h)
