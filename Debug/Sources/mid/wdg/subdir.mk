################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/wdg/wdg.c" \

C_SRCS += \
../Sources/mid/wdg/wdg.c \

C_DEPS_QUOTED += \
"./Sources/mid/wdg/wdg.d" \

OBJS_QUOTED += \
"./Sources/mid/wdg/wdg.o" \

C_DEPS += \
./Sources/mid/wdg/wdg.d \

OBJS_OS_FORMAT += \
./Sources/mid/wdg/wdg.o \

OBJS += \
./Sources/mid/wdg/wdg.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/wdg/wdg.o: ../Sources/mid/wdg/wdg.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/wdg/wdg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/wdg/wdg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


