################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/clock/clock.c" \

C_SRCS += \
../Sources/mid/clock/clock.c \

C_DEPS_QUOTED += \
"./Sources/mid/clock/clock.d" \

OBJS_QUOTED += \
"./Sources/mid/clock/clock.o" \

C_DEPS += \
./Sources/mid/clock/clock.d \

OBJS_OS_FORMAT += \
./Sources/mid/clock/clock.o \

OBJS += \
./Sources/mid/clock/clock.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/clock/clock.o: ../Sources/mid/clock/clock.c
	@echo 'Building file: $<'
	@echo 'Executing target #23 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/clock/clock.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/clock/clock.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


