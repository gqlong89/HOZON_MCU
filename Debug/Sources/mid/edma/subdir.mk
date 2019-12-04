################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/edma/edma.c" \

C_SRCS += \
../Sources/mid/edma/edma.c \

C_DEPS_QUOTED += \
"./Sources/mid/edma/edma.d" \

OBJS_QUOTED += \
"./Sources/mid/edma/edma.o" \

C_DEPS += \
./Sources/mid/edma/edma.d \

OBJS_OS_FORMAT += \
./Sources/mid/edma/edma.o \

OBJS += \
./Sources/mid/edma/edma.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/edma/edma.o: ../Sources/mid/edma/edma.c
	@echo 'Building file: $<'
	@echo 'Executing target #19 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/edma/edma.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/edma/edma.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


