################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/i2c/i2c.c" \

C_SRCS += \
../Sources/mid/i2c/i2c.c \

C_DEPS_QUOTED += \
"./Sources/mid/i2c/i2c.d" \

OBJS_QUOTED += \
"./Sources/mid/i2c/i2c.o" \

C_DEPS += \
./Sources/mid/i2c/i2c.d \

OBJS_OS_FORMAT += \
./Sources/mid/i2c/i2c.o \

OBJS += \
./Sources/mid/i2c/i2c.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/i2c/i2c.o: ../Sources/mid/i2c/i2c.c
	@echo 'Building file: $<'
	@echo 'Executing target #16 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/i2c/i2c.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/i2c/i2c.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


