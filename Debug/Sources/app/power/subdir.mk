################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/power/pm_mode.c" \
"../Sources/app/power/power.c" \

C_SRCS += \
../Sources/app/power/pm_mode.c \
../Sources/app/power/power.c \

C_DEPS_QUOTED += \
"./Sources/app/power/pm_mode.d" \
"./Sources/app/power/power.d" \

OBJS_QUOTED += \
"./Sources/app/power/pm_mode.o" \
"./Sources/app/power/power.o" \

C_DEPS += \
./Sources/app/power/pm_mode.d \
./Sources/app/power/power.d \

OBJS_OS_FORMAT += \
./Sources/app/power/pm_mode.o \
./Sources/app/power/power.o \

OBJS += \
./Sources/app/power/pm_mode.o \
./Sources/app/power/power.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/power/pm_mode.o: ../Sources/app/power/pm_mode.c
	@echo 'Building file: $<'
	@echo 'Executing target #52 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/power/pm_mode.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/power/pm_mode.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/power/power.o: ../Sources/app/power/power.c
	@echo 'Building file: $<'
	@echo 'Executing target #53 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/power/power.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/power/power.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


