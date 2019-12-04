################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/pm/pm.c" \

C_SRCS += \
../Sources/mid/pm/pm.c \

C_DEPS_QUOTED += \
"./Sources/mid/pm/pm.d" \

OBJS_QUOTED += \
"./Sources/mid/pm/pm.o" \

C_DEPS += \
./Sources/mid/pm/pm.d \

OBJS_OS_FORMAT += \
./Sources/mid/pm/pm.o \

OBJS += \
./Sources/mid/pm/pm.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/pm/pm.o: ../Sources/mid/pm/pm.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/pm/pm.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/pm/pm.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


