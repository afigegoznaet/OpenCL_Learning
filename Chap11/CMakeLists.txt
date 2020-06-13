cmake_minimum_required(VERSION 3.5)

project(Chap11 LANGUAGES C)
include_directories("C:/Intel/OpenCL/sdk/include")
#link_directories("C:/Intel/OpenCL/sdk/lib/x64")
link_directories("C:/Intel/NVIDIA GPU Computing SDK/OpenCL/common/lib/x64")
#add_library(OpenCL STATIC IMPORTED)
link_libraries(OpenCL.lib)

configure_file(bsort.cl ${CMAKE_CURRENT_BINARY_DIR}/bsort.cl COPYONLY)
configure_file(bsort8.cl ${CMAKE_CURRENT_BINARY_DIR}/bsort8.cl COPYONLY)
configure_file(radix_sort8.cl ${CMAKE_CURRENT_BINARY_DIR}/radix_sort8.cl COPYONLY)
configure_file(string_search.cl ${CMAKE_CURRENT_BINARY_DIR}/string_search.cl COPYONLY)
configure_file(kafka.txt ${CMAKE_CURRENT_BINARY_DIR}/kafka.txt COPYONLY)

#add_executable(Chap11 bsort.c)
#add_executable(Chap11 bsort8.c)
#add_executable(Chap11 radix_sort8.c)
add_executable(Chap11 string_search.c)
