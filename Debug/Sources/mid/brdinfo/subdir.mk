################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/brdinfo/brdInfo.c" \

C_SRCS += \
../Sources/mid/brdinfo/brdInfo.c \

C_DEPS_QUOTED += \
"./Sources/mid/brdinfo/brdInfo.d" \

OBJS_QUOTED += \
"./Sources/mid/brdinfo/brdInfo.o" \

C_DEPS += \
./Sources/mid/brdinfo/brdInfo.d \

OBJS_OS_FORMAT += \
./Sources/mid/brdinfo/brdInfo.o \

OBJS += \
./Sources/mid/brdinfo/brdInfo.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/brdinfo/brdInfo.o: ../Sources/mid/brdinfo/brdInfo.c
	@echo 'Building file: $<'
	@echo 'Executing target #28 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/brdinfo/brdInfo.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/brdinfo/brdInfo.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


