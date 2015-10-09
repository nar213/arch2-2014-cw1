################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/shared/mips_mem_ram.cpp \
../src/shared/mips_test_framework.cpp 

OBJS += \
./src/shared/mips_mem_ram.o \
./src/shared/mips_test_framework.o 

CPP_DEPS += \
./src/shared/mips_mem_ram.d \
./src/shared/mips_test_framework.d 


# Each subdirectory must supply rules for building sources it contributes
src/shared/%.o: ../src/shared/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/Nick/Documents/workspace/MIPS Simulator/include" -I"/Users/Nick/Documents/workspace/MIPS Simulator/src" -I"/Users/Nick/Documents/workspace/MIPS Simulator/src/shared" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


