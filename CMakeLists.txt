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
Source/MatrixDistNorm/MatrixDistNorm.cpp
Source/SideOfContour/SideOfContour_io.cpp
Source/SideOfContour/SideOfContour.cpp
Source/SpecificPolyhedrons/Cube.cpp
Source/SpecificPolyhedrons/Prism.cpp
Source/SpecificPolyhedrons/Pyramid.cpp
Source/SpecificPolyhedrons/CubeCutted.cpp
Source/TreeClusterNormNode/TreeClusterNormNode.cpp
Source/SpherePoint/SpherePoint.cpp
Source/VertexInfo/VertexInfo_intersection.cpp
Source/VertexInfo/VertexInfo_preprocess.cpp
Source/VertexInfo/VertexInfo_io.cpp
Source/VertexInfo/VertexInfo.cpp
Source/Correctors/PCorrector/PCorrector.cpp
Source/Correctors/Coalescer/Coalescer.cpp
Source/Correctors/GlobalShadeCorrector/GlobalShadeCorrector_preprocess.cpp
Source/Correctors/GlobalShadeCorrector/GlobalShadeCorrector_derivateTest.cpp
Source/Correctors/GlobalShadeCorrector/GSAssociator/GSAssociator.cpp
Source/Correctors/GlobalShadeCorrector/GlobalShadeCorrector.cpp
Source/Correctors/Intersector/FacetIntersector.cpp
Source/Correctors/Intersector/Intersector.cpp
Source/Correctors/PointShifter/PointShifter.cpp
Source/Correctors/PointShifterWeighted/PointShifterWeighted.cpp
Source/Correctors/PointShifterLinear/PointShifterLinear.cpp
Source/Polyhedron/Polyhedron_intertia.cpp
Source/Polyhedron/Polyhedron.cpp
Source/Polyhedron/Polyhedron_intersection.cpp
Source/Polyhedron/Polyhedron_simplify.cpp
Source/Polyhedron/Polyhedron_join_points.cpp
Source/Polyhedron/Polyhedron_volume.cpp
Source/Polyhedron/Polyhedron_hierarchical_clustering.cpp
Source/Polyhedron/Polyhedron_shift_point.cpp
Source/Polyhedron/Polyhedron_preprocess.cpp
Source/Polyhedron/Polyhedron_correction.cpp
Source/Polyhedron/Polyhedron_coalesce_facets.cpp
Source/Polyhedron/Polyhedron_test_consections.cpp
Source/Polyhedron/Polyhedron_io.cpp
Source/Polyhedron/Polyhedron_cluster.cpp
Source/Polyhedron/Polyhedron_verification.cpp
Source/EdgeContourAssociation/EdgeContourAssociation.cpp
Source/EdgeContourAssociation/EdgeContourAssociation_test.cpp
Source/ClusterNorm/ClusterNorm.cpp
Source/EdgeList/EdgeList.cpp
Source/EdgeList/EdgeList_intersection.cpp
Source/EdgeList/EdgeList_io.cpp
Source/EdgeSet/EdgeSet_io.cpp
Source/EdgeSet/EdgeSet_intersection.cpp
Source/EdgeSet/EdgeSet.cpp
Source/DataContainers/PData/PData.cpp
Source/DataContainers/ShadeContourData/ShadeContourData.cpp
Source/DataContainers/EdgeData/EdgeData.cpp
Source/Facet/Facet_io.cpp
Source/Facet/Facet_area.cpp
Source/Facet/Facet_verification.cpp
Source/Facet/Facet_test.cpp
Source/Facet/Facet_inertia.cpp
Source/Facet/Facet.cpp
Source/Facet/Facet_preprocess.cpp
Source/DataConstructors/EdgeConstructor/EdgeConstructor.cpp
Source/DataConstructors/ShadeContourConstructor/ShadeContourConstructor.cpp
Source/DataConstructors/PDataConstructor/PDataConstructor.cpp
Source/FutureFacet/FutureFacet_intersection.cpp
Source/FutureFacet/FutureFacet_io.cpp
Source/FutureFacet/FutureFacet.cpp
Source/TreeClusterNorm/TreeClusterNorm.cpp
Source/SContour/SContour.cpp
Source/SContour/SContour_io.cpp
Source/Edge/Edge_io.cpp
Source/Edge/Edge.cpp
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
arrayOperations 
gaussString 
lsm 
vector3d
)

add_subdirectory(Tests)

