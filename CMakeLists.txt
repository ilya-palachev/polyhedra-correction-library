cmake_minimum_required (VERSION 2.8)
project (Polyhedron-correction)

# Set path for CMake
set(CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake"
    ${CMAKE_MODULE_PATH}
)

set (SOURCE_arrayOperations
	External/ArrayOperations/array_operations.cpp
)

set (SOURCE_gaussString
	External/GaussString/Gauss_string.cpp
	External/GaussString/Gauss_test.cpp
)

set (SOURCE_lsm
	External/LSM/LeastSquaresMethod.cpp
)

set (SOURCE_vector3d
	External/Vector3d/Vector3d.cpp
)

set (SOURCE_pcl
	Source/Analyzers/Clusterizer/Clusterizer.cpp
	Source/Analyzers/Clusterizer/ClusterNorm/ClusterNorm.cpp
	Source/Analyzers/Clusterizer/MatrixDistNorm/MatrixDistNorm.cpp
	Source/Analyzers/Clusterizer/SortedSetOfPairs/SortedSetOfPairs.cpp
	Source/Analyzers/Clusterizer/SpherePoint/SpherePoint.cpp
	Source/Analyzers/Clusterizer/TreeClusterNormNode/TreeClusterNormNode.cpp
	Source/Analyzers/Clusterizer/TreeClusterNorm/TreeClusterNorm.cpp
	Source/Analyzers/PAnalyzer/PAnalyzer.cpp
	Source/Analyzers/ShadowContourClusterizer/ShadowContourClusterizer.cpp
	Source/Analyzers/SizeCalculator/SizeCalculator.cpp
	Source/Analyzers/SizeCalculator/SortedDouble/SortedDouble.cpp
	Source/Common.cpp
	Source/Correctors/Coalescer/Coalescer.cpp
	Source/Correctors/EdgeReducer/EdgeReducer.cpp
	Source/Correctors/GlobalShadowCorrector/GlobalShadowCorrector.cpp
	Source/Correctors/GlobalShadowCorrector/GlobalShadowCorrector_derivateTest.cpp
	Source/Correctors/GlobalShadowCorrector/GlobalShadowCorrector_preprocess.cpp
	Source/Correctors/GlobalShadowCorrector/GSAssociator/GSAssociator.cpp
	Source/Correctors/Intersector/EdgeList/EdgeList.cpp
	Source/Correctors/Intersector/EdgeList/EdgeList_intersection.cpp
	Source/Correctors/Intersector/EdgeList/EdgeList_io.cpp
	Source/Correctors/Intersector/EdgeSetIntersected/EdgeSetIntersected.cpp
	Source/Correctors/Intersector/EdgeSetIntersected/EdgeSetIntersected_intersection.cpp
	Source/Correctors/Intersector/EdgeSetIntersected/EdgeSetIntersected_io.cpp
	Source/Correctors/Intersector/FacetIntersector.cpp
	Source/Correctors/Intersector/FutureFacet/FutureFacet.cpp
	Source/Correctors/Intersector/FutureFacet/FutureFacet_intersection.cpp
	Source/Correctors/Intersector/FutureFacet/FutureFacet_io.cpp
	Source/Correctors/Intersector/Intersector.cpp
	Source/Correctors/PCorrector/PCorrector.cpp
	Source/Correctors/PointShifterLinear/PointShifterLinear.cpp
	Source/Correctors/PointShifter/PointShifter.cpp
	Source/Correctors/PointShifterWeighted/PointShifterWeighted.cpp
	Source/Correctors/VertexGrouper/VertexGrouper.cpp
	Source/DataConstructors/EdgeConstructor/EdgeConstructor.cpp
	Source/DataConstructors/PDataConstructor/PDataConstructor.cpp
	Source/DataConstructors/ShadowContourConstructor/ShadowContourConstructor.cpp
	Source/DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor.cpp
	Source/DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractorByPlane.cpp
	Source/DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractorByPoints.cpp
	Source/DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractor.cpp
	Source/DataConstructors/SupportFunctionEstimationDataConstructor/SupportFunctionEstimationDataConstructor.cpp
	Source/DataContainers/EdgeData/EdgeContourAssociation/EdgeContourAssociation.cpp
	Source/DataContainers/EdgeData/EdgeData.cpp
	Source/DataContainers/EdgeData/Edge/Edge.cpp
	Source/DataContainers/EdgeData/Edge/Edge_io.cpp
	Source/DataContainers/ShadowContourData/SContour/SContour.cpp
	Source/DataContainers/ShadowContourData/SContour/SContour_io.cpp
	Source/DataContainers/ShadowContourData/ShadowContourData.cpp
	Source/DataContainers/ShadowContourData/SideOfContour/SideOfContour.cpp
	Source/DataContainers/ShadowContourData/SideOfContour/SideOfContour_io.cpp
	Source/DataContainers/SupportFunctionData/SupportFunctionData.cpp
	Source/DataContainers/SupportFunctionData/SupportFunctionDataItem.cpp
	Source/DataContainers/SupportFunctionData/SupportFunctionDataItemInfo.cpp
	Source/DataContainers/SupportFunctionEstimationData/SupportFunctionEstimationData.cpp
	Source/DataContainers/SupportFunctionEstimationData/SupportMatrix.cpp
	Source/DataContainers/SupportFunctionEstimationData/GardnerKiderlenSupportMatrix.cpp
	Source/Dumpers/GraphDumperGEXF/GraphDumperGEXF.cpp
	Source/KernelCGAL/dual.cpp
	Source/PCLDumper.cpp
	Source/Polyhedron/Facet/Facet_area.cpp
	Source/Polyhedron/Facet/Facet.cpp
	Source/Polyhedron/Facet/Facet_inertia.cpp
	Source/Polyhedron/Facet/Facet_io.cpp
	Source/Polyhedron/Facet/Facet_preprocess.cpp
	Source/Polyhedron/Facet/Facet_test.cpp
	Source/Polyhedron/Facet/Facet_verification.cpp
	Source/Polyhedron/Polyhedron_clusterize.cpp
	Source/Polyhedron/Polyhedron_coalesce_facets.cpp
	Source/Polyhedron/Polyhedron_correction.cpp
	Source/Polyhedron/Polyhedron.cpp
	Source/Polyhedron/Polyhedron_group_vertices.cpp
	Source/Polyhedron/Polyhedron_intersection.cpp
	Source/Polyhedron/Polyhedron_io.cpp
	Source/Polyhedron/Polyhedron_preprocess.cpp
	Source/Polyhedron/Polyhedron_shift_point.cpp
	Source/Polyhedron/Polyhedron_size.cpp
	Source/Polyhedron/Polyhedron_verification.cpp
	Source/Polyhedron/SpecificPolyhedrons/Cube.cpp
	Source/Polyhedron/SpecificPolyhedrons/CubeCutted.cpp
	Source/Polyhedron/SpecificPolyhedrons/Prism.cpp
	Source/Polyhedron/SpecificPolyhedrons/Pyramid.cpp
	Source/Polyhedron/Verifier/Verifier.cpp
	Source/Polyhedron/VertexInfo/VertexInfo.cpp
	Source/Polyhedron/VertexInfo/VertexInfo_intersection.cpp
	Source/Polyhedron/VertexInfo/VertexInfo_io.cpp
	Source/Polyhedron/VertexInfo/VertexInfo_preprocess.cpp
	Source/Polyhedron_3/Polyhedron_3.cpp
	Source/Recoverer/ClpSupportFunctionEstimator.cpp
	Source/Recoverer/Colouring.cpp
	Source/Recoverer/CPLEXSupportFunctionEstimator.cpp
	Source/Recoverer/CGALSupportFunctionEstimator.cpp
	Source/Recoverer/IpoptSupportFunctionEstimator.cpp
	Source/Recoverer/IpoptFinitePlanesFitter.cpp
	Source/Recoverer/GlpkSupportFunctionEstimator.cpp
	Source/Recoverer/GlpkSFELinearProgramBuilder.cpp
	Source/Recoverer/NaiveFacetJoiner.cpp
	Source/Recoverer/NativeSupportFunctionEstimator.cpp
	Source/Recoverer/Recoverer.cpp
	Source/Recoverer/SupportFunctionEstimator.cpp
	Source/Recoverer/TangientPointInformation.cpp
	Source/Recoverer/TrustedEdgesDetector.cpp
	Source/Recoverer/TsnnlsSupportFunctionEstimator.cpp
	Source/SparseMatrixEigen.cpp
	Source/TimeMeasurer/TimeMeasurer.cpp
	Source/TimeMeasurer/Timer.cpp
)

set (SOURCE_tests
	Tests/buildCluster.cpp
	Tests/experimentCGAL_convexHullDynamic.cpp
	Tests/experimentCGAL_convexHullStatic.cpp
	Tests/gaussStringTest.cpp
	Tests/globalCorrection.cpp
	Tests/GlobalCorrectionReal.cpp
	Tests/intersectCubeCutted.cpp
	Tests/intersectPolyhedron.cpp
	Tests/joinFacets.cpp
	Tests/joinFacetsMultiple.cpp
	Tests/moveVertexGetStatistics.cpp
	Tests/moveVertexLinear2.cpp
	Tests/moveVertexW.cpp
	Tests/polyhedron_prog_incr_builder.cpp
	Tests/printTemplatePolyhedrons.cpp
	Tests/Recoverer_test.cpp
	Tests/ShadowContourCluterizer_test.cpp
	Tests/shadowContoursConstruction.cpp
)

set (INCLUDE_DIRS_all
	${CMAKE_SOURCE_DIR}/Source
	${CMAKE_SOURCE_DIR}/External/ArrayOperations
	${CMAKE_SOURCE_DIR}/External/GaussString
	${CMAKE_SOURCE_DIR}/External/LSM
	${CMAKE_SOURCE_DIR}/External/Vector3d
	${CMAKE_SOURCE_DIR}/External/CGAL
	/usr/local/include/eigen3
	/usr/local/include/coin/
	/home/ilya/git/opensource/Ipopt-3.12.1/Ipopt/src/Algorithm
	/home/ilya/git/opensource/Ipopt-3.12.1/Ipopt/src/Common
	/home/ilya/git/opensource/Ipopt-3.12.1/Ipopt/src/LinAlg
	/home/ilya/git/opensource/Ipopt-3.12.1/Ipopt/src/Interfaces
)

# Common compiler flags used for build:
set(COMMON_FLAGS "-g")
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

set(CMAKE_SHARED_LINKER_FLAGS "-L/usr/local/lib")
set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/lib")

add_library(arrayOperations SHARED ${SOURCE_arrayOperations})
add_library(gaussString SHARED ${SOURCE_gaussString})
add_library(lsm SHARED ${SOURCE_lsm})
add_library(vector3d SHARED ${SOURCE_vector3d})
add_library(pcl SHARED ${SOURCE_pcl})

option(ENABLE_TSNNLS "Enable tsnnls solver (does not work)" OFF)
option(ENABLE_IPOPT "Enable Ipopt solver" OFF)
option(ENABLE_GLPK "Enable Glpk solver" OFF)
option(ENABLE_GRAPH_DUMPER "Enable LibGEXF-based graph dumper" OFF)
option(DISABLE_CGAL_DEBUG "Disable all CGAL internal checks" OFF)

target_link_libraries(pcl 
	pthread
	arrayOperations
	gaussString
	lsm
	vector3d
	CGAL
	gmp
	mpfr
	tbb
	tbbmalloc
	boost_system
)

if (ENABLE_TSNNLS)
	target_link_libraries(pcl tsnnls)
	add_definitions(-DUSE_TSNNLS)
	message("!!! TSNNLS solver is enabled !!!")
endif()

if (ENABLE_IPOPT)
	target_link_libraries(pcl dl ipopt coinmumps coinhsl blas lapack)
	add_definitions(-DUSE_IPOPT)
	add_definitions(-DHAVE_CSTDDEF)
	add_definitions(-DFORTRAN_INTEGER_TYPE=int)
	message("!!! Ipopt solver is enabled !!!")
endif()

if (ENABLE_GLPK)
	target_link_libraries(pcl glpk)
	add_definitions(-DUSE_GLPK)
	message("!!! Glpk solver is enabled !!!")
endif()

if (ENABLE_CLP)
	target_link_libraries(pcl Clp)
	add_definitions(-DUSE_CLP)
	message("!!! Clp solver is enabled !!!")
endif()

if (ENABLE_CPLEX)
	add_definitions(-DUSE_CPLEX)
	message("!!! CPLEX solver is enabled !!!")
endif()

if (ENABLE_GRAPH_DUMPER)
	target_link_libraries(pcl gexf xml2)
	add_definitions(-DUSE_GRAPH_DUMPER)
	list (APPEND INCLUDE_DIRS_all "/usr/include/libgexf")
	list (APPEND INCLUDE_DIRS_all "/usr/include/libxml2")
endif()

if (DISABLE_CGAL_DEBUG)
	add_definitions(-DCGAL_NDEBUG)
	message("!!! all CGAL checks are disabled.")
endif()

add_definitions(-DSOURCE_DIR="${CMAKE_SOURCE_DIR}")

include_directories(${INCLUDE_DIRS_all})

foreach(test_source_file ${SOURCE_tests})
	get_filename_component(executable_name ${test_source_file} NAME_WE)
	message("Adding executable ${executable_name}")
	add_executable(${executable_name} ${test_source_file})
	target_link_libraries(${executable_name} pcl gaussString)
endforeach()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Valgrind")
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

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Gtest")
	add_definitions(-DGTEST_BUILD)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "GtestDebug")
	set(CMAKE_BUILD_TYPE "Gtest")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Gtest")
	set(CMAKE_BUILD_TYPE "Debug")
	set(COMMON_FLAGS "${COMMON_FLAGS} -g")
	set (SOURCE_gtests
		Source/DataContainers/SupportFunctionData/SupportFunctionDataItemInfo_test.cpp
		Source/DataContainers/SupportFunctionData/SupportFunctionDataItem_test.cpp
		Source/DataContainers/SupportFunctionData/SupportFunctionData_test.cpp
		Source/DataContainers/SupportFunctionEstimationData/SupportFunctionEstimationData_test.cpp
		Source/DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractorByPlane_test.cpp
		Source/DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataItemExtractorByPoints_test.cpp
		Source/DataConstructors/SupportFunctionDataConstructor/SupportFunctionDataConstructor_test.cpp
		UnitTests/AllGtestsDriver.cpp
	)
	add_executable(gtests ${SOURCE_gtests})
	target_link_libraries(pcl gtest)
	target_link_libraries(gtests pcl)
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
