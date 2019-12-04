################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/msg/msg_parse.c" \

C_SRCS += \
../Sources/mid/msg/msg_parse.c \

C_DEPS_QUOTED += \
"./Sources/mid/msg/msg_parse.d" \

OBJS_QUOTED += \
"./Sources/mid/msg/msg_parse.o" \

C_DEPS += \
./Sources/mid/msg/msg_parse.d \

OBJS_OS_FORMAT += \
./Sources/mid/msg/msg_parse.o \

OBJS += \
./Sources/mid/msg/msg_parse.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/msg/msg_parse.o: ../Sources/mid/msg/msg_parse.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/msg/msg_parse.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/msg/msg_parse.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


