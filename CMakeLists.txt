cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(biomed)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(SRGMain srg.cxx)

target_link_libraries(SRGMain ${ITK_LIBRARIES})

