################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/flash/flash.c" \

C_SRCS += \
../Sources/mid/flash/flash.c \

C_DEPS_QUOTED += \
"./Sources/mid/flash/flash.d" \

OBJS_QUOTED += \
"./Sources/mid/flash/flash.o" \

C_DEPS += \
./Sources/mid/flash/flash.d \

OBJS_OS_FORMAT += \
./Sources/mid/flash/flash.o \

OBJS += \
./Sources/mid/flash/flash.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/flash/flash.o: ../Sources/mid/flash/flash.c
	@echo 'Building file: $<'
	@echo 'Executing target #18 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/flash/flash.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/flash/flash.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


