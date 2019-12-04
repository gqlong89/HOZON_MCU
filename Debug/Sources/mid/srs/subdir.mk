################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/srs/srs.c" \

C_SRCS += \
../Sources/mid/srs/srs.c \

C_DEPS_QUOTED += \
"./Sources/mid/srs/srs.d" \

OBJS_QUOTED += \
"./Sources/mid/srs/srs.o" \

C_DEPS += \
./Sources/mid/srs/srs.d \

OBJS_OS_FORMAT += \
./Sources/mid/srs/srs.o \

OBJS += \
./Sources/mid/srs/srs.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/srs/srs.o: ../Sources/mid/srs/srs.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/srs/srs.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/srs/srs.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


