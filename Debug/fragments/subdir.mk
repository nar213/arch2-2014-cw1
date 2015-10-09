################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../fragments/f_addu.c \
../fragments/f_fibonacci.c 

OBJS += \
./fragments/f_addu.o \
./fragments/f_fibonacci.o 

C_DEPS += \
./fragments/f_addu.d \
./fragments/f_fibonacci.d 


# Each subdirectory must supply rules for building sources it contributes
fragments/%.o: ../fragments/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


