################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/facet/Facet.cpp \
../src/facet/Facet_intersection.cpp \
../src/facet/Facet_io.cpp \
../src/facet/Facet_join_facets.cpp \
../src/facet/Facet_preprocess.cpp \
../src/facet/Facet_test.cpp 

OBJS += \
./src/facet/Facet.o \
./src/facet/Facet_intersection.o \
./src/facet/Facet_io.o \
./src/facet/Facet_join_facets.o \
./src/facet/Facet_preprocess.o \
./src/facet/Facet_test.o 

CPP_DEPS += \
./src/facet/Facet.d \
./src/facet/Facet_intersection.d \
./src/facet/Facet_io.d \
./src/facet/Facet_join_facets.d \
./src/facet/Facet_preprocess.d \
./src/facet/Facet_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/facet/%.o: ../src/facet/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(PWD)/../include -I$(PWD)/../external/gauss-string/include -I$(PWD)/../external/array-operations/include -I$(PWD)/../external/lsm/include -I$(PWD)/../external/vector3d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


