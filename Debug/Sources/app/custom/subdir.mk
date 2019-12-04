################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/custom/custom.c" \

C_SRCS += \
../Sources/app/custom/custom.c \

C_DEPS_QUOTED += \
"./Sources/app/custom/custom.d" \

OBJS_QUOTED += \
"./Sources/app/custom/custom.o" \

C_DEPS += \
./Sources/app/custom/custom.d \

OBJS_OS_FORMAT += \
./Sources/app/custom/custom.o \

OBJS += \
./Sources/app/custom/custom.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/custom/custom.o: ../Sources/app/custom/custom.c
	@echo 'Building file: $<'
	@echo 'Executing target #58 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/custom/custom.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/custom/custom.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


