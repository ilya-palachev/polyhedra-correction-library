################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Edge.cpp \
../EdgeList.cpp \
../EdgeList_intersection.cpp \
../EdgeList_io.cpp \
../EdgeSet.cpp \
../EdgeSet_intersection.cpp \
../EdgeSet_io.cpp \
../Edge_io.cpp \
../Facet.cpp \
../Facet_intersection.cpp \
../Facet_io.cpp \
../Facet_join_facets.cpp \
../Facet_preprocess.cpp \
../Facet_test.cpp \
../FutureFacet.cpp \
../FutureFacet_intersection.cpp \
../FutureFacet_io.cpp \
../Gauss_string.cpp \
../Gauss_test.cpp \
../Polyhedron.cpp \
../Polyhedron_correction.cpp \
../Polyhedron_correction_preprocess.cpp \
../Polyhedron_correction_test.cpp \
../Polyhedron_deform.cpp \
../Polyhedron_deform_linear.cpp \
../Polyhedron_figures.cpp \
../Polyhedron_intersection.cpp \
../Polyhedron_intersection_j.cpp \
../Polyhedron_io.cpp \
../Polyhedron_join_facets.cpp \
../Polyhedron_join_points.cpp \
../Polyhedron_preprocess.cpp \
../Polyhedron_simplify.cpp \
../Polyhedron_test_consections.cpp \
../SContour.cpp \
../SContour_io.cpp \
../SideOfContour.cpp \
../SideOfContour_io.cpp \
../Vector3d.cpp \
../VertexInfo.cpp \
../VertexInfo_intersection.cpp \
../VertexInfo_io.cpp \
../VertexInfo_preprocess.cpp \
../array_operations.cpp \
../list_squares_method.cpp \
../main.cpp 

OBJS += \
./Edge.o \
./EdgeList.o \
./EdgeList_intersection.o \
./EdgeList_io.o \
./EdgeSet.o \
./EdgeSet_intersection.o \
./EdgeSet_io.o \
./Edge_io.o \
./Facet.o \
./Facet_intersection.o \
./Facet_io.o \
./Facet_join_facets.o \
./Facet_preprocess.o \
./Facet_test.o \
./FutureFacet.o \
./FutureFacet_intersection.o \
./FutureFacet_io.o \
./Gauss_string.o \
./Gauss_test.o \
./Polyhedron.o \
./Polyhedron_correction.o \
./Polyhedron_correction_preprocess.o \
./Polyhedron_correction_test.o \
./Polyhedron_deform.o \
./Polyhedron_deform_linear.o \
./Polyhedron_figures.o \
./Polyhedron_intersection.o \
./Polyhedron_intersection_j.o \
./Polyhedron_io.o \
./Polyhedron_join_facets.o \
./Polyhedron_join_points.o \
./Polyhedron_preprocess.o \
./Polyhedron_simplify.o \
./Polyhedron_test_consections.o \
./SContour.o \
./SContour_io.o \
./SideOfContour.o \
./SideOfContour_io.o \
./Vector3d.o \
./VertexInfo.o \
./VertexInfo_intersection.o \
./VertexInfo_io.o \
./VertexInfo_preprocess.o \
./array_operations.o \
./list_squares_method.o \
./main.o 

CPP_DEPS += \
./Edge.d \
./EdgeList.d \
./EdgeList_intersection.d \
./EdgeList_io.d \
./EdgeSet.d \
./EdgeSet_intersection.d \
./EdgeSet_io.d \
./Edge_io.d \
./Facet.d \
./Facet_intersection.d \
./Facet_io.d \
./Facet_join_facets.d \
./Facet_preprocess.d \
./Facet_test.d \
./FutureFacet.d \
./FutureFacet_intersection.d \
./FutureFacet_io.d \
./Gauss_string.d \
./Gauss_test.d \
./Polyhedron.d \
./Polyhedron_correction.d \
./Polyhedron_correction_preprocess.d \
./Polyhedron_correction_test.d \
./Polyhedron_deform.d \
./Polyhedron_deform_linear.d \
./Polyhedron_figures.d \
./Polyhedron_intersection.d \
./Polyhedron_intersection_j.d \
./Polyhedron_io.d \
./Polyhedron_join_facets.d \
./Polyhedron_join_points.d \
./Polyhedron_preprocess.d \
./Polyhedron_simplify.d \
./Polyhedron_test_consections.d \
./SContour.d \
./SContour_io.d \
./SideOfContour.d \
./SideOfContour_io.d \
./Vector3d.d \
./VertexInfo.d \
./VertexInfo_intersection.d \
./VertexInfo_io.d \
./VertexInfo_preprocess.d \
./array_operations.d \
./list_squares_method.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


