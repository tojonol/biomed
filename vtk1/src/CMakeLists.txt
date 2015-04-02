cmake_minimum_required(VERSION 2.8)
 
PROJECT(Sphere)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(Sphere MACOSX_BUNDLE Sphere)
 
if(VTK_LIBRARIES)
  target_link_libraries(Sphere ${VTK_LIBRARIES})
else()
  target_link_libraries(Sphere vtkHybrid vtkWidgets)
endif()