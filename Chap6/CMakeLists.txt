cmake_minimum_required(VERSION 3.5)

project(Chap6 LANGUAGES C)
include_directories("C:/Intel/OpenCL/sdk/include")
include_directories("D:/Workspace/libpng/include")
include_directories("D:/Workspace/zlib")
#link_directories("C:/Intel/OpenCL/sdk/lib/x64")
link_directories("C:/Intel/NVIDIA GPU Computing SDK/OpenCL/common/lib/x64")
link_directories("D:/Workspace/libpng/lib")
link_directories("D:/Workspace/zlib")
#add_library(OpenCL STATIC IMPORTED)
link_libraries(OpenCL.lib libpng16_static.lib zlibstatic.lib)

configure_file(interp.cl ${CMAKE_CURRENT_BINARY_DIR}/interp.cl COPYONLY)
configure_file(interp_gnu.cl ${CMAKE_CURRENT_BINARY_DIR}/interp_gnu.cl COPYONLY)
configure_file(simple_image.cl ${CMAKE_CURRENT_BINARY_DIR}/simple_image.cl COPYONLY)

configure_file(blank.png ${CMAKE_CURRENT_BINARY_DIR}/blank.png COPYONLY)
configure_file(input.png ${CMAKE_CURRENT_BINARY_DIR}/input.png COPYONLY)
configure_file(Lenna.png ${CMAKE_CURRENT_BINARY_DIR}/Lenna.png COPYONLY)
configure_file(Lenna_1.png ${CMAKE_CURRENT_BINARY_DIR}/Lenna_1.png COPYONLY)

#add_executable(Chap6 interp.c)
add_executable(Chap6 interp_gnu.c)
#add_executable(Chap6 simple_image.c)

