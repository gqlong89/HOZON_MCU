################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/dbg_uart/dbg_uart.c" \

C_SRCS += \
../Sources/mid/dbg_uart/dbg_uart.c \

C_DEPS_QUOTED += \
"./Sources/mid/dbg_uart/dbg_uart.d" \

OBJS_QUOTED += \
"./Sources/mid/dbg_uart/dbg_uart.o" \

C_DEPS += \
./Sources/mid/dbg_uart/dbg_uart.d \

OBJS_OS_FORMAT += \
./Sources/mid/dbg_uart/dbg_uart.o \

OBJS += \
./Sources/mid/dbg_uart/dbg_uart.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/dbg_uart/dbg_uart.o: ../Sources/mid/dbg_uart/dbg_uart.c
	@echo 'Building file: $<'
	@echo 'Executing target #20 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/dbg_uart/dbg_uart.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/dbg_uart/dbg_uart.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


