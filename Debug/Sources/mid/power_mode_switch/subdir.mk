################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/power_mode_switch/power_mode_switch.c" \

C_SRCS += \
../Sources/mid/power_mode_switch/power_mode_switch.c \

C_DEPS_QUOTED += \
"./Sources/mid/power_mode_switch/power_mode_switch.d" \

OBJS_QUOTED += \
"./Sources/mid/power_mode_switch/power_mode_switch.o" \

C_DEPS += \
./Sources/mid/power_mode_switch/power_mode_switch.d \

OBJS_OS_FORMAT += \
./Sources/mid/power_mode_switch/power_mode_switch.o \

OBJS += \
./Sources/mid/power_mode_switch/power_mode_switch.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/power_mode_switch/power_mode_switch.o: ../Sources/mid/power_mode_switch/power_mode_switch.c
	@echo 'Building file: $<'
	@echo 'Executing target #7 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/power_mode_switch/power_mode_switch.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/power_mode_switch/power_mode_switch.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


