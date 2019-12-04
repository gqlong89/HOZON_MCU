################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/mcu_mgr/mcu_task.c" \

C_SRCS += \
../Sources/app/mcu_mgr/mcu_task.c \

C_DEPS_QUOTED += \
"./Sources/app/mcu_mgr/mcu_task.d" \

OBJS_QUOTED += \
"./Sources/app/mcu_mgr/mcu_task.o" \

C_DEPS += \
./Sources/app/mcu_mgr/mcu_task.d \

OBJS_OS_FORMAT += \
./Sources/app/mcu_mgr/mcu_task.o \

OBJS += \
./Sources/app/mcu_mgr/mcu_task.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/mcu_mgr/mcu_task.o: ../Sources/app/mcu_mgr/mcu_task.c
	@echo 'Building file: $<'
	@echo 'Executing target #57 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/mcu_mgr/mcu_task.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/mcu_mgr/mcu_task.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


