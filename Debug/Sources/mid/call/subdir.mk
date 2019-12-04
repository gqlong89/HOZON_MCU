################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/call/call.c" \

C_SRCS += \
../Sources/mid/call/call.c \

C_DEPS_QUOTED += \
"./Sources/mid/call/call.d" \

OBJS_QUOTED += \
"./Sources/mid/call/call.o" \

C_DEPS += \
./Sources/mid/call/call.d \

OBJS_OS_FORMAT += \
./Sources/mid/call/call.o \

OBJS += \
./Sources/mid/call/call.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/call/call.o: ../Sources/mid/call/call.c
	@echo 'Building file: $<'
	@echo 'Executing target #27 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/call/call.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/call/call.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


