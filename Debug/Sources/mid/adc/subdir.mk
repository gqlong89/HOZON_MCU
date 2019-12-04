################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/adc/adc.c" \

C_SRCS += \
../Sources/mid/adc/adc.c \

C_DEPS_QUOTED += \
"./Sources/mid/adc/adc.d" \

OBJS_QUOTED += \
"./Sources/mid/adc/adc.o" \

C_DEPS += \
./Sources/mid/adc/adc.d \

OBJS_OS_FORMAT += \
./Sources/mid/adc/adc.o \

OBJS += \
./Sources/mid/adc/adc.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/adc/adc.o: ../Sources/mid/adc/adc.c
	@echo 'Building file: $<'
	@echo 'Executing target #30 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/adc/adc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/adc/adc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


