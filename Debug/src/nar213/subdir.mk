################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/nar213/mips_cpu.cpp \
../src/nar213/test_mips.cpp 

OBJS += \
./src/nar213/mips_cpu.o \
./src/nar213/test_mips.o 

CPP_DEPS += \
./src/nar213/mips_cpu.d \
./src/nar213/test_mips.d 


# Each subdirectory must supply rules for building sources it contributes
src/nar213/%.o: ../src/nar213/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/Users/Nick/Documents/workspace/MIPS Simulator/include" -I"/Users/Nick/Documents/workspace/MIPS Simulator/src" -I"/Users/Nick/Documents/workspace/MIPS Simulator/src/shared" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


