cmake_minimum_required (VERSION 2.8)
project (polyhedron-correction)

set (SOURCE_arrayOperations
./external/array-operations/src/array_operations.cpp
)

set (SOURCE_gaussString
./external/gauss-string/src/Gauss_string.cpp
./external/gauss-string/src/Gauss_test.cpp
)

set (SOURCE_lsm
./external/lsm/src/list_squares_method.cpp
)

set (SOURCE_vector3d
./external/vector3d/src/Vector3d.cpp
)

set (SOURCE_polyhedronCorrection
./src/edge/Edge_io.cpp
./src/edge/Edge.cpp
./src/edge-set/EdgeSet_io.cpp
./src/edge-set/EdgeSet_intersection.cpp
./src/edge-set/EdgeSet.cpp
./src/future-facet/FutureFacet_intersection.cpp
./src/future-facet/FutureFacet_io.cpp
./src/future-facet/FutureFacet.cpp
./src/polyhedron/Polyhedron_deform.cpp
./src/polyhedron/Polyhedron.cpp
./src/polyhedron/Polyhedron_intersection.cpp
./src/polyhedron/Polyhedron_correction_test.cpp
./src/polyhedron/Polyhedron_simplify.cpp
./src/polyhedron/Polyhedron_join_points.cpp
./src/polyhedron/Polyhedron_deform_linear.cpp
./src/polyhedron/Polyhedron_intersection_j.cpp
./src/polyhedron/Polyhedron_preprocess.cpp
./src/polyhedron/Polyhedron_correction.cpp
./src/polyhedron/Polyhedron_figures.cpp
./src/polyhedron/Polyhedron_join_facets.cpp
./src/polyhedron/Polyhedron_correction_preprocess.cpp
./src/polyhedron/Polyhedron_test_consections.cpp
./src/polyhedron/Polyhedron_io.cpp
./src/scontour/SContour.cpp
./src/scontour/SContour_io.cpp
./src/edge-list/EdgeList.cpp
./src/edge-list/EdgeList_intersection.cpp
./src/edge-list/EdgeList_io.cpp
./src/vertexinfo/VertexInfo_intersection.cpp
./src/vertexinfo/VertexInfo_preprocess.cpp
./src/vertexinfo/VertexInfo_io.cpp
./src/vertexinfo/VertexInfo.cpp
./src/facet/Facet_io.cpp
./src/facet/Facet_join_facets.cpp
./src/facet/Facet_intersection.cpp
./src/facet/Facet_test.cpp
./src/facet/Facet.cpp
./src/facet/Facet_preprocess.cpp
./src/side-of-contour/SideOfContour_io.cpp
./src/side-of-contour/SideOfContour.cpp
)

include_directories( 
${CMAKE_SOURCE_DIR}/include/ 
${CMAKE_SOURCE_DIR}/external/array-operations/include/ 
${CMAKE_SOURCE_DIR}/external/gauss-string/include/ 
${CMAKE_SOURCE_DIR}/external/lsm/include/
${CMAKE_SOURCE_DIR}/external/vector3d/include/)


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
polyhedronCorrection
)

add_subdirectory(tests)

