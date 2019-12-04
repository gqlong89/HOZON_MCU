################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/ipc_spi/ipc_spi_list.c" \
"../Sources/mid/ipc_spi/ipc_spi_trans.c" \

C_SRCS += \
../Sources/mid/ipc_spi/ipc_spi_list.c \
../Sources/mid/ipc_spi/ipc_spi_trans.c \

C_DEPS_QUOTED += \
"./Sources/mid/ipc_spi/ipc_spi_list.d" \
"./Sources/mid/ipc_spi/ipc_spi_trans.d" \

OBJS_QUOTED += \
"./Sources/mid/ipc_spi/ipc_spi_list.o" \
"./Sources/mid/ipc_spi/ipc_spi_trans.o" \

C_DEPS += \
./Sources/mid/ipc_spi/ipc_spi_list.d \
./Sources/mid/ipc_spi/ipc_spi_trans.d \

OBJS_OS_FORMAT += \
./Sources/mid/ipc_spi/ipc_spi_list.o \
./Sources/mid/ipc_spi/ipc_spi_trans.o \

OBJS += \
./Sources/mid/ipc_spi/ipc_spi_list.o \
./Sources/mid/ipc_spi/ipc_spi_trans.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/ipc_spi/ipc_spi_list.o: ../Sources/mid/ipc_spi/ipc_spi_list.c
	@echo 'Building file: $<'
	@echo 'Executing target #14 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/ipc_spi/ipc_spi_list.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/ipc_spi/ipc_spi_list.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/mid/ipc_spi/ipc_spi_trans.o: ../Sources/mid/ipc_spi/ipc_spi_trans.c
	@echo 'Building file: $<'
	@echo 'Executing target #15 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/ipc_spi/ipc_spi_trans.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/ipc_spi/ipc_spi_trans.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


