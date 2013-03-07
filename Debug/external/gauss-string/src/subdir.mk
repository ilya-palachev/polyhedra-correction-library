################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../external/gauss-string/src/Gauss_string.cpp \
../external/gauss-string/src/Gauss_test.cpp 

OBJS += \
./external/gauss-string/src/Gauss_string.o \
./external/gauss-string/src/Gauss_test.o 

CPP_DEPS += \
./external/gauss-string/src/Gauss_string.d \
./external/gauss-string/src/Gauss_test.d 


# Each subdirectory must supply rules for building sources it contributes
external/gauss-string/src/%.o: ../external/gauss-string/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(PWD)/../include -I$(PWD)/../external/gauss-string/include -I$(PWD)/../external/array-operations/include -I$(PWD)/../external/lsm/include -I$(PWD)/../external/vector3d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


