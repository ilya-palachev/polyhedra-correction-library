set(SOURCES_buildCluster buildCluster.cpp)
add_executable(buildCluster ${SOURCES_buildCluster})
target_link_libraries(buildCluster pcl)

set(SOURCES_globalCorrection globalCorrection.cpp)
add_executable(globalCorrection ${SOURCES_globalCorrection})
target_link_libraries(globalCorrection pcl)

set(SOURCES_gaussStringTest gaussStringTest.cpp)
add_executable(gaussStringTest ${SOURCES_gaussStringTest})
target_link_libraries(gaussStringTest pcl gaussString)

set(SOURCES_intersectCubeCutted intersectCubeCutted.cpp)
add_executable(intersectCubeCutted ${SOURCES_intersectCubeCutted})
target_link_libraries(intersectCubeCutted pcl)

set(SOURCES_intersectPolyhedron intersectPolyhedron.cpp)
add_executable(intersectPolyhedron ${SOURCES_intersectPolyhedron})
target_link_libraries(intersectPolyhedron pcl)

set(SOURCES_joinFacets joinFacets.cpp)
add_executable(joinFacets ${SOURCES_joinFacets})
target_link_libraries(joinFacets pcl)

set(SOURCES_joinFacetsMultiple joinFacetsMultiple.cpp)
add_executable(joinFacetsMultiple ${SOURCES_joinFacetsMultiple})
target_link_libraries(joinFacetsMultiple pcl)

set(SOURCES_moveVertexGetStatistics moveVertexGetStatistics.cpp)
add_executable(moveVertexGetStatistics ${SOURCES_moveVertexGetStatistics})
target_link_libraries(moveVertexGetStatistics pcl)

set(SOURCES_moveVertexLinear2 moveVertexLinear2.cpp)
add_executable(moveVertexLinear2 ${SOURCES_moveVertexLinear2})
target_link_libraries(moveVertexLinear2 pcl)

set(SOURCES_moveVertexW moveVertexW.cpp)
add_executable(moveVertexW ${SOURCES_moveVertexW})
target_link_libraries(moveVertexW pcl)

set(SOURCES_printTemplatePolyhedrons printTemplatePolyhedrons.cpp)
add_executable(printTemplatePolyhedrons ${SOURCES_printTemplatePolyhedrons})
target_link_libraries(printTemplatePolyhedrons pcl)
