################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/side-of-contour/SideOfContour.cpp \
../src/side-of-contour/SideOfContour_io.cpp 

OBJS += \
./src/side-of-contour/SideOfContour.o \
./src/side-of-contour/SideOfContour_io.o 

CPP_DEPS += \
./src/side-of-contour/SideOfContour.d \
./src/side-of-contour/SideOfContour_io.d 


# Each subdirectory must supply rules for building sources it contributes
src/side-of-contour/%.o: ../src/side-of-contour/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I$(PWD)/../include -I$(PWD)/../external/gauss-string/include -I$(PWD)/../external/array-operations/include -I$(PWD)/../external/lsm/include -I$(PWD)/../external/vector3d/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


