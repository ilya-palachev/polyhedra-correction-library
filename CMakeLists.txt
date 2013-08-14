cmake_minimum_required (VERSION 2.8)
project (Polyhedron-correction)

file(GLOB_RECURSE SOURCE_arrayOperations      "./External/ArrayOperations/*.cpp")
file(GLOB_RECURSE SOURCE_gaussString          "./External/GaussString/*.cpp")
file(GLOB_RECURSE SOURCE_lsm                  "./External/LSM/*.cpp")
file(GLOB_RECURSE SOURCE_vector3d             "./External/Vector3d/*.cpp")
file(GLOB_RECURSE SOURCE_pcl                  "./Source/*.cpp")

file(GLOB_RECURSE HEADERS_pcl "./Source/*.h")
file(GLOB_RECURSE HEADERS_external "./External/*.h")
set (HEADERS_all "")
list (APPEND HEADERS_all ${HEADERS_pcl})
list (APPEND HEADERS_all ${HEADERS_external})

set (INCLUDE_DIRS_all "")
foreach (_headerFile ${HEADERS_all})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND INCLUDE_DIRS_all ${_dir})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS_all)
include_directories(${INCLUDE_DIRS_all})

if (CMAKE_COMPILER_IS_GNUCXX)
	set(COMMON_FLAGS "-std=c++11")
endif ()

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

if (${CMAKE_BUILD_TYPE} STREQUAL "Valgrind")
	set(CMAKE_BUILD_TYPE "Release")
	set(COMMON_FLAGS "${COMMON_FLAGS} -g")
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "Valgrind")
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

add_subdirectory(Tests)

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
