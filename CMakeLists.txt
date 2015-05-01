cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(biomed)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

include_directories("include/Segmentation")

set(SEG_HEADERS include/Segmentation/DICOMImage.h include/Segmentation/Region.h include/Segmentation/SeededRegionGrower.h)

add_library(SeededRegionGrower src/Segmentation/SeededRegionGrower.cxx src/Segmentation/Region.cxx src/Segmentation/RatedVox.cxx)

add_executable(SRGMain src/Segmentation/SRGDriver.cxx)
target_link_libraries(SRGMain SeededRegionGrower ${ITK_LIBRARIES})

add_executable(MCMain src/MeshConstruction/MeshConstruction.cxx)
target_link_libraries(MCMain SeededRegionGrower ${ITK_LIBRARIES})

add_executable(MorphOpen src/MeshConstruction/MorphOpen.cxx)
target_link_libraries(MorphOpen SeededRegionGrower ${ITK_LIBRARIES})
