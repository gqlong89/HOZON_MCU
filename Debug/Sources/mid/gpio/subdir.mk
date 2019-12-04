################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/gpio/gpio.c" \

C_SRCS += \
../Sources/mid/gpio/gpio.c \

C_DEPS_QUOTED += \
"./Sources/mid/gpio/gpio.d" \

OBJS_QUOTED += \
"./Sources/mid/gpio/gpio.o" \

C_DEPS += \
./Sources/mid/gpio/gpio.d \

OBJS_OS_FORMAT += \
./Sources/mid/gpio/gpio.o \

OBJS += \
./Sources/mid/gpio/gpio.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/gpio/gpio.o: ../Sources/mid/gpio/gpio.c
	@echo 'Building file: $<'
	@echo 'Executing target #17 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/gpio/gpio.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/gpio/gpio.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


