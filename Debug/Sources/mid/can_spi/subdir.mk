################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/can_spi/can_spi.c" \
"../Sources/mid/can_spi/mcp2515.c" \

C_SRCS += \
../Sources/mid/can_spi/can_spi.c \
../Sources/mid/can_spi/mcp2515.c \

C_DEPS_QUOTED += \
"./Sources/mid/can_spi/can_spi.d" \
"./Sources/mid/can_spi/mcp2515.d" \

OBJS_QUOTED += \
"./Sources/mid/can_spi/can_spi.o" \
"./Sources/mid/can_spi/mcp2515.o" \

C_DEPS += \
./Sources/mid/can_spi/can_spi.d \
./Sources/mid/can_spi/mcp2515.d \

OBJS_OS_FORMAT += \
./Sources/mid/can_spi/can_spi.o \
./Sources/mid/can_spi/mcp2515.o \

OBJS += \
./Sources/mid/can_spi/can_spi.o \
./Sources/mid/can_spi/mcp2515.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/can_spi/can_spi.o: ../Sources/mid/can_spi/can_spi.c
	@echo 'Building file: $<'
	@echo 'Executing target #24 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/can_spi/can_spi.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/can_spi/can_spi.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/mid/can_spi/mcp2515.o: ../Sources/mid/can_spi/mcp2515.c
	@echo 'Building file: $<'
	@echo 'Executing target #25 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/can_spi/mcp2515.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/can_spi/mcp2515.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


