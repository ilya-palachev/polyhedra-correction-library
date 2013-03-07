################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/vertexinfo/VertexInfo.cpp \
../src/vertexinfo/VertexInfo_intersection.cpp \
../src/vertexinfo/VertexInfo_io.cpp \
../src/vertexinfo/VertexInfo_preprocess.cpp 

OBJS += \
./src/vertexinfo/VertexInfo.o \
./src/vertexinfo/VertexInfo_intersection.o \
./src/vertexinfo/VertexInfo_io.o \
./src/vertexinfo/VertexInfo_preprocess.o 

CPP_DEPS += \
./src/vertexinfo/VertexInfo.d \
./src/vertexinfo/VertexInfo_intersection.d \
./src/vertexinfo/VertexInfo_io.d \
./src/vertexinfo/VertexInfo_preprocess.d 


# Each subdirectory must supply rules for building sources it contributes
src/vertexinfo/%.o: ../src/vertexinfo/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(PWD)/../include -I$(PWD)/../external/gauss-string/include -I$(PWD)/../external/array-operations/include -I$(PWD)/../external/lsm/include -I$(PWD)/../external/vector3d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


