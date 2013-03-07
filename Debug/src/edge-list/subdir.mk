################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/edge-list/EdgeList.cpp \
../src/edge-list/EdgeList_intersection.cpp \
../src/edge-list/EdgeList_io.cpp 

OBJS += \
./src/edge-list/EdgeList.o \
./src/edge-list/EdgeList_intersection.o \
./src/edge-list/EdgeList_io.o 

CPP_DEPS += \
./src/edge-list/EdgeList.d \
./src/edge-list/EdgeList_intersection.d \
./src/edge-list/EdgeList_io.d 


# Each subdirectory must supply rules for building sources it contributes
src/edge-list/%.o: ../src/edge-list/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(PWD)/../include -I$(PWD)/../external/gauss-string/include -I$(PWD)/../external/array-operations/include -I$(PWD)/../external/lsm/include -I$(PWD)/../external/vector3d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


