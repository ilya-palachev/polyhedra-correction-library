################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/scontour/SContour.cpp \
../src/scontour/SContour_io.cpp 

OBJS += \
./src/scontour/SContour.o \
./src/scontour/SContour_io.o 

CPP_DEPS += \
./src/scontour/SContour.d \
./src/scontour/SContour_io.d 


# Each subdirectory must supply rules for building sources it contributes
src/scontour/%.o: ../src/scontour/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(PWD)/../include -I$(PWD)/../external/gauss-string/include -I$(PWD)/../external/array-operations/include -I$(PWD)/../external/lsm/include -I$(PWD)/../external/vector3d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


