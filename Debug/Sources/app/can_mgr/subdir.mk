################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/can_mgr/can_basic.c" \
"../Sources/app/can_mgr/can_basic_cfg.c" \
"../Sources/app/can_mgr/can_broadCast.c" \
"../Sources/app/can_mgr/can_hardware.c" \
"../Sources/app/can_mgr/can_mgr.c" \
"../Sources/app/can_mgr/can_sendFifo.c" \
"../Sources/app/can_mgr/can_voltage.c" \
"../Sources/app/can_mgr/canbus_off.c" \

C_SRCS += \
../Sources/app/can_mgr/can_basic.c \
../Sources/app/can_mgr/can_basic_cfg.c \
../Sources/app/can_mgr/can_broadCast.c \
../Sources/app/can_mgr/can_hardware.c \
../Sources/app/can_mgr/can_mgr.c \
../Sources/app/can_mgr/can_sendFifo.c \
../Sources/app/can_mgr/can_voltage.c \
../Sources/app/can_mgr/canbus_off.c \

C_DEPS_QUOTED += \
"./Sources/app/can_mgr/can_basic.d" \
"./Sources/app/can_mgr/can_basic_cfg.d" \
"./Sources/app/can_mgr/can_broadCast.d" \
"./Sources/app/can_mgr/can_hardware.d" \
"./Sources/app/can_mgr/can_mgr.d" \
"./Sources/app/can_mgr/can_sendFifo.d" \
"./Sources/app/can_mgr/can_voltage.d" \
"./Sources/app/can_mgr/canbus_off.d" \

OBJS_QUOTED += \
"./Sources/app/can_mgr/can_basic.o" \
"./Sources/app/can_mgr/can_basic_cfg.o" \
"./Sources/app/can_mgr/can_broadCast.o" \
"./Sources/app/can_mgr/can_hardware.o" \
"./Sources/app/can_mgr/can_mgr.o" \
"./Sources/app/can_mgr/can_sendFifo.o" \
"./Sources/app/can_mgr/can_voltage.o" \
"./Sources/app/can_mgr/canbus_off.o" \

C_DEPS += \
./Sources/app/can_mgr/can_basic.d \
./Sources/app/can_mgr/can_basic_cfg.d \
./Sources/app/can_mgr/can_broadCast.d \
./Sources/app/can_mgr/can_hardware.d \
./Sources/app/can_mgr/can_mgr.d \
./Sources/app/can_mgr/can_sendFifo.d \
./Sources/app/can_mgr/can_voltage.d \
./Sources/app/can_mgr/canbus_off.d \

OBJS_OS_FORMAT += \
./Sources/app/can_mgr/can_basic.o \
./Sources/app/can_mgr/can_basic_cfg.o \
./Sources/app/can_mgr/can_broadCast.o \
./Sources/app/can_mgr/can_hardware.o \
./Sources/app/can_mgr/can_mgr.o \
./Sources/app/can_mgr/can_sendFifo.o \
./Sources/app/can_mgr/can_voltage.o \
./Sources/app/can_mgr/canbus_off.o \

OBJS += \
./Sources/app/can_mgr/can_basic.o \
./Sources/app/can_mgr/can_basic_cfg.o \
./Sources/app/can_mgr/can_broadCast.o \
./Sources/app/can_mgr/can_hardware.o \
./Sources/app/can_mgr/can_mgr.o \
./Sources/app/can_mgr/can_sendFifo.o \
./Sources/app/can_mgr/can_voltage.o \
./Sources/app/can_mgr/canbus_off.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/can_mgr/can_basic.o: ../Sources/app/can_mgr/can_basic.c
	@echo 'Building file: $<'
	@echo 'Executing target #59 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/can_mgr/can_basic.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/can_mgr/can_basic.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/can_mgr/can_basic_cfg.o: ../Sources/app/can_mgr/can_basic_cfg.c
	@echo 'Building file: $<'
	@echo 'Executing target #60 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/can_mgr/can_basic_cfg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/can_mgr/can_basic_cfg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/can_mgr/can_broadCast.o: ../Sources/app/can_mgr/can_broadCast.c
	@echo 'Building file: $<'
	@echo 'Executing target #61 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/can_mgr/can_broadCast.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/can_mgr/can_broadCast.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/can_mgr/can_hardware.o: ../Sources/app/can_mgr/can_hardware.c
	@echo 'Building file: $<'
	@echo 'Executing target #62 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/can_mgr/can_hardware.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/can_mgr/can_hardware.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/can_mgr/can_mgr.o: ../Sources/app/can_mgr/can_mgr.c
	@echo 'Building file: $<'
	@echo 'Executing target #63 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/can_mgr/can_mgr.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/can_mgr/can_mgr.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/can_mgr/can_sendFifo.o: ../Sources/app/can_mgr/can_sendFifo.c
	@echo 'Building file: $<'
	@echo 'Executing target #64 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/can_mgr/can_sendFifo.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/can_mgr/can_sendFifo.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/can_mgr/can_voltage.o: ../Sources/app/can_mgr/can_voltage.c
	@echo 'Building file: $<'
	@echo 'Executing target #65 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/can_mgr/can_voltage.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/can_mgr/can_voltage.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/can_mgr/canbus_off.o: ../Sources/app/can_mgr/canbus_off.c
	@echo 'Building file: $<'
	@echo 'Executing target #66 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/can_mgr/canbus_off.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/can_mgr/canbus_off.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


