################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../external/array-operations/src/array_operations.cpp 

OBJS += \
./external/array-operations/src/array_operations.o 

CPP_DEPS += \
./external/array-operations/src/array_operations.d 


# Each subdirectory must supply rules for building sources it contributes
external/array-operations/src/%.o: ../external/array-operations/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(PWD)/../include -I$(PWD)/../external/gauss-string/include -I$(PWD)/../external/array-operations/include -I$(PWD)/../external/lsm/include -I$(PWD)/../external/vector3d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


