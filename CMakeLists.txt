cmake_minimum_required (VERSION 2.8)
project (Polyhedron-correction)

set (SOURCE_arrayOperations
./External/ArrayOperations/src/array_operations.cpp
)

set (SOURCE_gaussString
./External/GaussString/src/Gauss_string.cpp
./External/GaussString/src/Gauss_test.cpp
)

set (SOURCE_lsm
./External/LSM/src/list_squares_method.cpp
)

set (SOURCE_vector3d
./External/Vector3d/src/Vector3d.cpp
)

set (SOURCE_polyhedronCorrection
./Source/Edge/Edge_io.cpp
./Source/Edge/Edge.cpp
./Source/EdgeSet/EdgeSet_io.cpp
./Source/EdgeSet/EdgeSet_intersection.cpp
./Source/EdgeSet/EdgeSet.cpp
./Source/FutureFacet/FutureFacet_intersection.cpp
./Source/FutureFacet/FutureFacet_io.cpp
./Source/FutureFacet/FutureFacet.cpp
./Source/Polyhedron/Polyhedron_deform.cpp
./Source/Polyhedron/Polyhedron.cpp
./Source/Polyhedron/Polyhedron_intersection.cpp
./Source/Polyhedron/Polyhedron_correction_test.cpp
./Source/Polyhedron/Polyhedron_simplify.cpp
./Source/Polyhedron/Polyhedron_join_points.cpp
./Source/Polyhedron/Polyhedron_deform_linear.cpp
./Source/Polyhedron/Polyhedron_intersection_j.cpp
./Source/Polyhedron/Polyhedron_preprocess.cpp
./Source/Polyhedron/Polyhedron_correction.cpp
./Source/Polyhedron/Polyhedron_figures.cpp
./Source/Polyhedron/Polyhedron_join_facets.cpp
./Source/Polyhedron/Polyhedron_correction_preprocess.cpp
./Source/Polyhedron/Polyhedron_test_consections.cpp
./Source/Polyhedron/Polyhedron_io.cpp
./Source/SContour/SContour.cpp
./Source/SContour/SContour_io.cpp
./Source/EdgeList/EdgeList.cpp
./Source/EdgeList/EdgeList_intersection.cpp
./Source/EdgeList/EdgeList_io.cpp
./Source/VertexInfo/VertexInfo_intersection.cpp
./Source/VertexInfo/VertexInfo_preprocess.cpp
./Source/VertexInfo/VertexInfo_io.cpp
./Source/VertexInfo/VertexInfo.cpp
./Source/Facet/Facet_io.cpp
./Source/Facet/Facet_join_facets.cpp
./Source/Facet/Facet_intersection.cpp
./Source/Facet/Facet_test.cpp
./Source/Facet/Facet.cpp
./Source/Facet/Facet_preprocess.cpp
./Source/SideOfContour/SideOfContour_io.cpp
./Source/SideOfContour/SideOfContour.cpp
)

include_directories( 
${CMAKE_SOURCE_DIR}/Include/ 
${CMAKE_SOURCE_DIR}/External/ArrayOperations/include/ 
${CMAKE_SOURCE_DIR}/External/GaussString/include/ 
${CMAKE_SOURCE_DIR}/External/LSM/include/
${CMAKE_SOURCE_DIR}/External/Vector3d/include/)


add_library(arrayOperations SHARED ${SOURCE_arrayOperations})
add_library(gaussString SHARED ${SOURCE_gaussString})
add_library(lsm SHARED ${SOURCE_lsm})
add_library(vector3d SHARED ${SOURCE_vector3d})
add_library(polyhedronCorrection SHARED ${SOURCE_polyhedronCorrection})

target_link_libraries(polyhedronCorrection 
lapacke 
arrayOperations 
gaussString 
lsm 
vector3d
)

add_subdirectory(Tests)

