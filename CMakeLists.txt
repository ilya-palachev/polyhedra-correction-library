cmake_minimum_required (VERSION 2.8)
project (Polyhedron-correction)

# Set path for CMake
set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    ${CMAKE_MODULE_PATH}
)

file(GLOB_RECURSE SOURCE_arrayOperations      "./External/ArrayOperations/*.cpp")
file(GLOB_RECURSE SOURCE_gaussString          "./External/GaussString/*.cpp")
file(GLOB_RECURSE SOURCE_lsm                  "./External/LSM/*.cpp")
file(GLOB_RECURSE SOURCE_vector3d             "./External/Vector3d/*.cpp")
file(GLOB_RECURSE SOURCE_pcl                  "./Source/*.cpp")

file(GLOB_RECURSE SOURCE_tests                "./Tests/*.cpp")

set (HEADERS_all "")
list (APPEND HEADERS_all ${HEADERS_arrayOperations})
list (APPEND HEADERS_all ${HEADERS_gaussString})
list (APPEND HEADERS_all ${HEADERS_lsm})
list (APPEND HEADERS_all ${HEADERS_Vector3d})
list (APPEND HEADERS_all ${HEADERS_pcl})

set (INCLUDE_DIRS_all "")
foreach (_headerFile ${HEADERS_all})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS_all ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS_all)

list (APPEND INCLUDE_DIRS_all "${CMAKE_SOURCE_DIR}/Source")
list (APPEND INCLUDE_DIRS_all "${CMAKE_SOURCE_DIR}/External/ArrayOperations")
list (APPEND INCLUDE_DIRS_all "${CMAKE_SOURCE_DIR}/External/GaussString")
list (APPEND INCLUDE_DIRS_all "${CMAKE_SOURCE_DIR}/External/LSM")
list (APPEND INCLUDE_DIRS_all "${CMAKE_SOURCE_DIR}/External/Vector3d")
list (APPEND INCLUDE_DIRS_all "/usr/include/eigen3")

# Common compiler flags used for build:
set(COMMON_FLAGS "")
# Enable C++11:
set(COMMON_FLAGS "${COMMON_FLAGS} -std=c++11")
# Enable the most strict compiler errors:
set(COMMON_FLAGS "${COMMON_FLAGS} -Wall -Werror -pedantic")
# Enable rounding math option (required by CGAL):
# TODO: For ICC we need to set -fp-model here.
set(COMMON_FLAGS "${COMMON_FLAGS} -frounding-math")

message("Compiler is ${CMAKE_C_COMPILER}")
if (${CMAKE_C_COMPILER} MATCHES ".*clang")
	message("Clang compiler detected")
	set(COMMON_FLAGS "-Wno-error=gnu-zero-variadic-macro-arguments")
endif()

add_library(arrayOperations SHARED ${SOURCE_arrayOperations})
add_library(gaussString SHARED ${SOURCE_gaussString})
add_library(lsm SHARED ${SOURCE_lsm})
add_library(vector3d SHARED ${SOURCE_vector3d})
add_library(pcl SHARED ${SOURCE_pcl})

option(ENABLE_IPOPT "Enable Ipopt solver" OFF)
option(ENABLE_GRAPH_DUMPER "Enable LibGEXF-based graph dumper" OFF)

target_link_libraries(pcl 
   arrayOperations 
   gaussString 
   lsm 
   vector3d
   CGAL
   gmp
   tsnnls
)

if (ENABLE_IPOPT)
	target_link_libraries(pcl ipopt coinmumps coinhsl)
	add_definitions(-DUSE_IPOPT)
	message("!!! Ipopt solver is enabled !!!")
endif()

if (ENABLE_GRAPH_DUMPER)
	target_link_libraries(pcl gexf xml2)
	add_definitions(-DUSE_GRAPH_DUMPER)
	list (APPEND INCLUDE_DIRS_all "/usr/include/libgexf")
	list (APPEND INCLUDE_DIRS_all "/usr/include/libxml2")
endif()

include_directories(${INCLUDE_DIRS_all})

foreach(test_source_file ${SOURCE_tests})
	get_filename_component(executable_name ${test_source_file} NAME_WE)
	message("Adding executable ${executable_name}")
	add_executable(${executable_name} ${test_source_file})
	target_link_libraries(${executable_name} pcl gaussString)
endforeach()

if (${CMAKE_BUILD_TYPE} STREQUAL "Valgrind")
	set(CMAKE_BUILD_TYPE "Release")
	set(COMMON_FLAGS "${COMMON_FLAGS} -g")
	set(cat_config ${CMAKE_SOURCE_DIR}/Scripts/cat_config.sh)
	set(debug_exceptions_config "${CMAKE_SOURCE_DIR}/config/debug_exceptions.conf")
	set(debug_exceptions "")
	exec_program(${cat_config} ARGS ${debug_exceptions_config} OUTPUT_VARIABLE debug_exceptions)

	message("Debug exceptions:")
	foreach(souce_file ${debug_exceptions})
		set(souce_file "${CMAKE_SOURCE_DIR}/${souce_file}")
		message("   ${souce_file}")
		set_source_files_properties(${souce_file} PROPERTIES COMPILE_FLAGS -UNDEBUG)
	endforeach()
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")

add_subdirectory(Documentation)

# add custom target distclean
# cleans and removes cmake generated files etc.
# Jan Woetzel 04/2003
#

if (UNIX)
  add_custom_target (distclean @echo cleaning for source distribution)
  set(DISTCLEANED
   cmake.depends
   cmake.check_depends
   CMakeCache.txt
   cmake.check_cache
   *.cmake
   Makefile
   core core.*
   gmon.out
   *~
  )
  
  add_custom_command (
    DEPENDS clean
    COMMENT "distribution clean"
    COMMAND rm
    ARGS    -Rf CMakeTmp ${DISTCLEANED}
    TARGET  distclean
  )
endif (UNIX)
