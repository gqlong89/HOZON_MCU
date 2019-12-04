################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/nm_autosar/nm_autosar.c" \

C_SRCS += \
../Sources/app/nm_autosar/nm_autosar.c \

C_DEPS_QUOTED += \
"./Sources/app/nm_autosar/nm_autosar.d" \

OBJS_QUOTED += \
"./Sources/app/nm_autosar/nm_autosar.o" \

C_DEPS += \
./Sources/app/nm_autosar/nm_autosar.d \

OBJS_OS_FORMAT += \
./Sources/app/nm_autosar/nm_autosar.o \

OBJS += \
./Sources/app/nm_autosar/nm_autosar.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/nm_autosar/nm_autosar.o: ../Sources/app/nm_autosar/nm_autosar.c
	@echo 'Building file: $<'
	@echo 'Executing target #56 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/nm_autosar/nm_autosar.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/nm_autosar/nm_autosar.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


