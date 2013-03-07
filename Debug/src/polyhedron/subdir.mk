################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/polyhedron/Polyhedron.cpp \
../src/polyhedron/Polyhedron_correction.cpp \
../src/polyhedron/Polyhedron_correction_preprocess.cpp \
../src/polyhedron/Polyhedron_correction_test.cpp \
../src/polyhedron/Polyhedron_deform.cpp \
../src/polyhedron/Polyhedron_deform_linear.cpp \
../src/polyhedron/Polyhedron_figures.cpp \
../src/polyhedron/Polyhedron_intersection.cpp \
../src/polyhedron/Polyhedron_intersection_j.cpp \
../src/polyhedron/Polyhedron_io.cpp \
../src/polyhedron/Polyhedron_join_facets.cpp \
../src/polyhedron/Polyhedron_join_points.cpp \
../src/polyhedron/Polyhedron_preprocess.cpp \
../src/polyhedron/Polyhedron_simplify.cpp \
../src/polyhedron/Polyhedron_test_consections.cpp 

OBJS += \
./src/polyhedron/Polyhedron.o \
./src/polyhedron/Polyhedron_correction.o \
./src/polyhedron/Polyhedron_correction_preprocess.o \
./src/polyhedron/Polyhedron_correction_test.o \
./src/polyhedron/Polyhedron_deform.o \
./src/polyhedron/Polyhedron_deform_linear.o \
./src/polyhedron/Polyhedron_figures.o \
./src/polyhedron/Polyhedron_intersection.o \
./src/polyhedron/Polyhedron_intersection_j.o \
./src/polyhedron/Polyhedron_io.o \
./src/polyhedron/Polyhedron_join_facets.o \
./src/polyhedron/Polyhedron_join_points.o \
./src/polyhedron/Polyhedron_preprocess.o \
./src/polyhedron/Polyhedron_simplify.o \
./src/polyhedron/Polyhedron_test_consections.o 

CPP_DEPS += \
./src/polyhedron/Polyhedron.d \
./src/polyhedron/Polyhedron_correction.d \
./src/polyhedron/Polyhedron_correction_preprocess.d \
./src/polyhedron/Polyhedron_correction_test.d \
./src/polyhedron/Polyhedron_deform.d \
./src/polyhedron/Polyhedron_deform_linear.d \
./src/polyhedron/Polyhedron_figures.d \
./src/polyhedron/Polyhedron_intersection.d \
./src/polyhedron/Polyhedron_intersection_j.d \
./src/polyhedron/Polyhedron_io.d \
./src/polyhedron/Polyhedron_join_facets.d \
./src/polyhedron/Polyhedron_join_points.d \
./src/polyhedron/Polyhedron_preprocess.d \
./src/polyhedron/Polyhedron_simplify.d \
./src/polyhedron/Polyhedron_test_consections.d 


# Each subdirectory must supply rules for building sources it contributes
src/polyhedron/%.o: ../src/polyhedron/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(PWD)/../include -I$(PWD)/../external/gauss-string/include -I$(PWD)/../external/array-operations/include -I$(PWD)/../external/lsm/include -I$(PWD)/../external/vector3d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


