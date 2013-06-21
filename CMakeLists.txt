cmake_minimum_required (VERSION 2.8)
project (Polyhedron-correction)

file(GLOB_RECURSE SOURCE_arrayOperations      "./External/ArrayOperations/*.cpp")
file(GLOB_RECURSE SOURCE_gaussString          "./External/GaussString/*.cpp")
file(GLOB_RECURSE SOURCE_lsm                  "./External/LSM/*.cpp")
file(GLOB_RECURSE SOURCE_vector3d             "./External/Vector3d/*.cpp")
file(GLOB_RECURSE SOURCE_pcl                  "./Source/*.cpp")

file(GLOB_RECURSE HEADERS_pcl "./Source/*.h")
file(GLOB_RECURSE HEADERS_external "./External/*.h")
set (HEADERS_all "${HEADERS_pcl} ${HEADERS_external}")

set (INCLUDE_DIRS_all "")
foreach (_headerFile ${HEADERS_all})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS_all ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS_all)
include_directories(${INCLUDE_DIRS_all})

add_library(arrayOperations SHARED ${SOURCE_arrayOperations})
add_library(gaussString SHARED ${SOURCE_gaussString})
add_library(lsm SHARED ${SOURCE_lsm})
add_library(vector3d SHARED ${SOURCE_vector3d})
add_library(pcl SHARED ${SOURCE_pcl})

target_link_libraries(pcl 
   arrayOperations 
   gaussString 
   lsm 
   vector3d
)

add_subdirectory(Tests)

