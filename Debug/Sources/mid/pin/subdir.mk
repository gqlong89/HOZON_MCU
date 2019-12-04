################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/pin/pin.c" \

C_SRCS += \
../Sources/mid/pin/pin.c \

C_DEPS_QUOTED += \
"./Sources/mid/pin/pin.d" \

OBJS_QUOTED += \
"./Sources/mid/pin/pin.o" \

C_DEPS += \
./Sources/mid/pin/pin.d \

OBJS_OS_FORMAT += \
./Sources/mid/pin/pin.o \

OBJS += \
./Sources/mid/pin/pin.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/pin/pin.o: ../Sources/mid/pin/pin.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/pin/pin.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/pin/pin.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


