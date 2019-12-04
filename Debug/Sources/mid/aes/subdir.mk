################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/aes/aes.c" \

C_SRCS += \
../Sources/mid/aes/aes.c \

C_DEPS_QUOTED += \
"./Sources/mid/aes/aes.d" \

OBJS_QUOTED += \
"./Sources/mid/aes/aes.o" \

C_DEPS += \
./Sources/mid/aes/aes.d \

OBJS_OS_FORMAT += \
./Sources/mid/aes/aes.o \

OBJS += \
./Sources/mid/aes/aes.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/aes/aes.o: ../Sources/mid/aes/aes.c
	@echo 'Building file: $<'
	@echo 'Executing target #29 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/aes/aes.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/aes/aes.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


