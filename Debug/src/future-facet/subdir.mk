################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/future-facet/FutureFacet.cpp \
../src/future-facet/FutureFacet_intersection.cpp \
../src/future-facet/FutureFacet_io.cpp 

OBJS += \
./src/future-facet/FutureFacet.o \
./src/future-facet/FutureFacet_intersection.o \
./src/future-facet/FutureFacet_io.o 

CPP_DEPS += \
./src/future-facet/FutureFacet.d \
./src/future-facet/FutureFacet_intersection.d \
./src/future-facet/FutureFacet_io.d 


# Each subdirectory must supply rules for building sources it contributes
src/future-facet/%.o: ../src/future-facet/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(PWD)/../include -I$(PWD)/../external/gauss-string/include -I$(PWD)/../external/array-operations/include -I$(PWD)/../external/lsm/include -I$(PWD)/../external/vector3d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


