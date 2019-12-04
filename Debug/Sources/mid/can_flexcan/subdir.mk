################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/can_flexcan/can_flexcan.c" \

C_SRCS += \
../Sources/mid/can_flexcan/can_flexcan.c \

C_DEPS_QUOTED += \
"./Sources/mid/can_flexcan/can_flexcan.d" \

OBJS_QUOTED += \
"./Sources/mid/can_flexcan/can_flexcan.o" \

C_DEPS += \
./Sources/mid/can_flexcan/can_flexcan.d \

OBJS_OS_FORMAT += \
./Sources/mid/can_flexcan/can_flexcan.o \

OBJS += \
./Sources/mid/can_flexcan/can_flexcan.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/can_flexcan/can_flexcan.o: ../Sources/mid/can_flexcan/can_flexcan.c
	@echo 'Building file: $<'
	@echo 'Executing target #26 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/can_flexcan/can_flexcan.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/can_flexcan/can_flexcan.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


