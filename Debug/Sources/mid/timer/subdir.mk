################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/timer/timer.c" \

C_SRCS += \
../Sources/mid/timer/timer.c \

C_DEPS_QUOTED += \
"./Sources/mid/timer/timer.d" \

OBJS_QUOTED += \
"./Sources/mid/timer/timer.o" \

C_DEPS += \
./Sources/mid/timer/timer.d \

OBJS_OS_FORMAT += \
./Sources/mid/timer/timer.o \

OBJS += \
./Sources/mid/timer/timer.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/timer/timer.o: ../Sources/mid/timer/timer.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/timer/timer.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/timer/timer.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


