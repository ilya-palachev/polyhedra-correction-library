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
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/Facet.o \
	${OBJECTDIR}/EdgeList.o \
	${OBJECTDIR}/Polyhedron.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/VertexInfo_preprocess.o \
	${OBJECTDIR}/Facet_io.o \
	${OBJECTDIR}/Polyhedron_figures.o \
	${OBJECTDIR}/Facet_preprocess.o \
	${OBJECTDIR}/Polyhedron_preprocess.o \
	${OBJECTDIR}/Polyhedron_io.o \
	${OBJECTDIR}/array_operations.o \
	${OBJECTDIR}/VertexInfo_io.o \
	${OBJECTDIR}/Vector3d.o \
	${OBJECTDIR}/list_squares_method.o \
	${OBJECTDIR}/VertexInfo.o


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
	"${MAKE}"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/polyhedron_join_facets_2

dist/Debug/GNU-Linux-x86/polyhedron_join_facets_2: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/polyhedron_join_facets_2 ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/Facet.o: Facet.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet.o Facet.cpp

${OBJECTDIR}/EdgeList.o: EdgeList.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/EdgeList.o EdgeList.cpp

${OBJECTDIR}/Polyhedron.o: Polyhedron.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron.o Polyhedron.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/VertexInfo_preprocess.o: VertexInfo_preprocess.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/VertexInfo_preprocess.o VertexInfo_preprocess.cpp

${OBJECTDIR}/Facet_io.o: Facet_io.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet_io.o Facet_io.cpp

${OBJECTDIR}/Polyhedron_figures.o: Polyhedron_figures.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_figures.o Polyhedron_figures.cpp

${OBJECTDIR}/Facet_preprocess.o: Facet_preprocess.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Facet_preprocess.o Facet_preprocess.cpp

${OBJECTDIR}/Polyhedron_preprocess.o: Polyhedron_preprocess.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_preprocess.o Polyhedron_preprocess.cpp

${OBJECTDIR}/Polyhedron_io.o: Polyhedron_io.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Polyhedron_io.o Polyhedron_io.cpp

${OBJECTDIR}/array_operations.o: array_operations.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/array_operations.o array_operations.cpp

${OBJECTDIR}/VertexInfo_io.o: VertexInfo_io.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/VertexInfo_io.o VertexInfo_io.cpp

${OBJECTDIR}/Vector3d.o: Vector3d.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/Vector3d.o Vector3d.cpp

${OBJECTDIR}/list_squares_method.o: list_squares_method.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/list_squares_method.o list_squares_method.cpp

${OBJECTDIR}/VertexInfo.o: VertexInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/VertexInfo.o VertexInfo.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/polyhedron_join_facets_2

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
