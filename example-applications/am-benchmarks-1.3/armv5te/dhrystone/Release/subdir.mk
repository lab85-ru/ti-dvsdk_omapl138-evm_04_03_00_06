################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../dhry_1.c \
../dhry_2.c 

OBJS += \
./dhry_1.o \
./dhry_2.o 

C_DEPS += \
./dhry_1.d \
./dhry_2.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Linux GCC C Compiler'
	$(CROSS_COMPILE)gcc -O3 -Wa,-adhlns="$@.lst" -c -DTIME -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm926ej-s -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


