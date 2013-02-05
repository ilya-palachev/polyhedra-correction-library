#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran-4.6
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/EdgeList_io.o \
	${OBJECTDIR}/VertexInfo_intersection.o \
	${OBJECTDIR}/Facet.o \
	${OBJECTDIR}/Facet_intersection.o \
	${OBJECTDIR}/EdgeSet_intersection.o \
	${OBJECTDIR}/EdgeList.o \
	${OBJECTDIR}/Polyhedron.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/Facet_join_facets.o \
	${OBJECTDIR}/Polyhedron_join_points.o \
	${OBJECTDIR}/Polyhedron_deform.o \
	${OBJECTDIR}/VertexInfo_preprocess.o \
	${OBJECTDIR}/Polyhedron_intersection.o \
	${OBJECTDIR}/Facet_test.o \
	${OBJECTDIR}/Facet_io.o \
	${OBJECTDIR}/Facet_preprocess.o \
	${OBJECTDIR}/Polyhedron_intersection_j.o \
	${OBJECTDIR}/Polyhedron_figures.o \
	${OBJECTDIR}/Polyhedron_preprocess.o \
	${OBJECTDIR}/Polyhedron_join_facets.o \
	${OBJECTDIR}/FutureFacet.o \
	${OBJECTDIR}/Facet_deform_linear_vertmin.o \
	${OBJECTDIR}/EdgeSet_io.o \
	${OBJECTDIR}/Gauss_string.o \
	${OBJECTDIR}/FutureFacet_intersection.o \
	${OBJECTDIR}/Polyhedron_deform_linear_vertmin.o \
	${OBJECTDIR}/EdgeList_intersection.o \
	${OBJECTDIR}/FutureFacet_io.o \
	${OBJECTDIR}/Polyhedron_io.o \
	${OBJECTDIR}/EdgeSet.o \
	${OBJECTDIR}/Polyhedron_deform_linear.o \
	${OBJECTDIR}/VertexInfo_io.o \
	${OBJECTDIR}/Gauss_test.o \
	${OBJECTDIR}/array_operations.o \
	${OBJECTDIR}/Polyhedron_simplify.o \
	${OBJECTDIR}/Vector3d.o \
	${OBJECTDIR}/VertexInfo.o \
	${OBJECTDIR}/list_squares_method.o \
	${OBJECTDIR}/Polyhedron_deform_linear_control.o \
	${OBJECTDIR}/Polyhedron_test_consections.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/polyhedron_join_facets_2

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/polyhedron_join_facets_2: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -I octave-3.6.1 -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/polyhedron_join_facets_2 ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/EdgeList_io.o: EdgeList_io.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/EdgeList_io.o EdgeList_io.cpp

${OBJECTDIR}/VertexInfo_intersection.o: VertexInfo_intersection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/VertexInfo_intersection.o VertexInfo_intersection.cpp

${OBJECTDIR}/Facet.o: Facet.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet.o Facet.cpp

${OBJECTDIR}/Facet_intersection.o: Facet_intersection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet_intersection.o Facet_intersection.cpp

${OBJECTDIR}/EdgeSet_intersection.o: EdgeSet_intersection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/EdgeSet_intersection.o EdgeSet_intersection.cpp

${OBJECTDIR}/EdgeList.o: EdgeList.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/EdgeList.o EdgeList.cpp

${OBJECTDIR}/Polyhedron.o: Polyhedron.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron.o Polyhedron.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/Facet_join_facets.o: Facet_join_facets.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet_join_facets.o Facet_join_facets.cpp

${OBJECTDIR}/Polyhedron_join_points.o: Polyhedron_join_points.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_join_points.o Polyhedron_join_points.cpp

${OBJECTDIR}/Polyhedron_deform.o: Polyhedron_deform.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_deform.o Polyhedron_deform.cpp

${OBJECTDIR}/VertexInfo_preprocess.o: VertexInfo_preprocess.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/VertexInfo_preprocess.o VertexInfo_preprocess.cpp

${OBJECTDIR}/Polyhedron_intersection.o: Polyhedron_intersection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_intersection.o Polyhedron_intersection.cpp

${OBJECTDIR}/Facet_test.o: Facet_test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet_test.o Facet_test.cpp

${OBJECTDIR}/Facet_io.o: Facet_io.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet_io.o Facet_io.cpp

${OBJECTDIR}/Facet_preprocess.o: Facet_preprocess.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet_preprocess.o Facet_preprocess.cpp

${OBJECTDIR}/Polyhedron_intersection_j.o: Polyhedron_intersection_j.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_intersection_j.o Polyhedron_intersection_j.cpp

${OBJECTDIR}/Polyhedron_figures.o: Polyhedron_figures.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_figures.o Polyhedron_figures.cpp

${OBJECTDIR}/Polyhedron_preprocess.o: Polyhedron_preprocess.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_preprocess.o Polyhedron_preprocess.cpp

${OBJECTDIR}/Polyhedron_join_facets.o: Polyhedron_join_facets.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_join_facets.o Polyhedron_join_facets.cpp

${OBJECTDIR}/FutureFacet.o: FutureFacet.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/FutureFacet.o FutureFacet.cpp

${OBJECTDIR}/Facet_deform_linear_vertmin.o: Facet_deform_linear_vertmin.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet_deform_linear_vertmin.o Facet_deform_linear_vertmin.cpp

${OBJECTDIR}/EdgeSet_io.o: EdgeSet_io.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/EdgeSet_io.o EdgeSet_io.cpp

${OBJECTDIR}/Gauss_string.o: Gauss_string.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Gauss_string.o Gauss_string.cpp

${OBJECTDIR}/FutureFacet_intersection.o: FutureFacet_intersection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/FutureFacet_intersection.o FutureFacet_intersection.cpp

${OBJECTDIR}/Polyhedron_deform_linear_vertmin.o: Polyhedron_deform_linear_vertmin.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_deform_linear_vertmin.o Polyhedron_deform_linear_vertmin.cpp

${OBJECTDIR}/EdgeList_intersection.o: EdgeList_intersection.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/EdgeList_intersection.o EdgeList_intersection.cpp

${OBJECTDIR}/FutureFacet_io.o: FutureFacet_io.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/FutureFacet_io.o FutureFacet_io.cpp

${OBJECTDIR}/Polyhedron_io.o: Polyhedron_io.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_io.o Polyhedron_io.cpp

${OBJECTDIR}/EdgeSet.o: EdgeSet.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/EdgeSet.o EdgeSet.cpp

${OBJECTDIR}/Polyhedron_deform_linear.o: Polyhedron_deform_linear.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_deform_linear.o Polyhedron_deform_linear.cpp

${OBJECTDIR}/VertexInfo_io.o: VertexInfo_io.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/VertexInfo_io.o VertexInfo_io.cpp

${OBJECTDIR}/Gauss_test.o: Gauss_test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Gauss_test.o Gauss_test.cpp

${OBJECTDIR}/array_operations.o: array_operations.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/array_operations.o array_operations.cpp

${OBJECTDIR}/Polyhedron_simplify.o: Polyhedron_simplify.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_simplify.o Polyhedron_simplify.cpp

${OBJECTDIR}/Vector3d.o: Vector3d.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Vector3d.o Vector3d.cpp

${OBJECTDIR}/VertexInfo.o: VertexInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/VertexInfo.o VertexInfo.cpp

${OBJECTDIR}/list_squares_method.o: list_squares_method.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/list_squares_method.o list_squares_method.cpp

${OBJECTDIR}/Polyhedron_deform_linear_control.o: Polyhedron_deform_linear_control.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_deform_linear_control.o Polyhedron_deform_linear_control.cpp

${OBJECTDIR}/Polyhedron_test_consections.o: Polyhedron_test_consections.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_test_consections.o Polyhedron_test_consections.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/polyhedron_join_facets_2

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
