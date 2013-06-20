cmake_minimum_required (VERSION 2.8)
project (Polyhedron-correction)

file(GLOB_RECURSE SOURCE_arrayOperations      "./External/ArrayOperations/src/*.cpp")
file(GLOB_RECURSE SOURCE_gaussString          "./External/GaussString/src/*.cpp")
file(GLOB_RECURSE SOURCE_lsm                  "./External/LSM/src/*.cpp")
file(GLOB_RECURSE SOURCE_vector3d             "./External/Vector3d/src/*.cpp")
file(GLOB_RECURSE SOURCE_polyhedronCorrection "./Source/*.cpp")

include_directories( 
   ${CMAKE_SOURCE_DIR}/Include/ 
   ${CMAKE_SOURCE_DIR}/External/ArrayOperations/include/ 
   ${CMAKE_SOURCE_DIR}/External/GaussString/include/ 
   ${CMAKE_SOURCE_DIR}/External/LSM/include/
   ${CMAKE_SOURCE_DIR}/External/Vector3d/include/
)

add_library(arrayOperations SHARED ${SOURCE_arrayOperations})
add_library(gaussString SHARED ${SOURCE_gaussString})
add_library(lsm SHARED ${SOURCE_lsm})
add_library(vector3d SHARED ${SOURCE_vector3d})
add_library(polyhedronCorrection SHARED ${SOURCE_polyhedronCorrection})

target_link_libraries(polyhedronCorrection 
   arrayOperations 
   gaussString 
   lsm 
   vector3d
)

add_subdirectory(Tests)

