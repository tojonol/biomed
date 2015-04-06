cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(biomed)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_library(SeededRegionGrower src/Segmentation/SeededRegionGrower.cxx src/Segmentation/Region.cxx src/Segmentation/RatedVox.cxx)

add_executable(SRGMain src/Segmentation/SRGDriver.cxx)

target_link_libraries(SRGMain SeededRegionGrower ${ITK_LIBRARIES})

