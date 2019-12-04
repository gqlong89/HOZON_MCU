################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Generated_Code/Cpu.c" \
"../Generated_Code/SRSFTimer.c" \
"../Generated_Code/adc0.c" \
"../Generated_Code/can0_flexcan.c" \
"../Generated_Code/clockMan1.c" \
"../Generated_Code/dmaController1.c" \
"../Generated_Code/flash1.c" \
"../Generated_Code/lowFTimer.c" \
"../Generated_Code/lpi2c1.c" \
"../Generated_Code/lpspiCom.c" \
"../Generated_Code/lpuart1.c" \
"../Generated_Code/osif1.c" \
"../Generated_Code/pin_mux.c" \
"../Generated_Code/pwrMan1.c" \
"../Generated_Code/rtcTimer1.c" \
"../Generated_Code/watchdog1.c" \

C_SRCS += \
../Generated_Code/Cpu.c \
../Generated_Code/SRSFTimer.c \
../Generated_Code/adc0.c \
../Generated_Code/can0_flexcan.c \
../Generated_Code/clockMan1.c \
../Generated_Code/dmaController1.c \
../Generated_Code/flash1.c \
../Generated_Code/lowFTimer.c \
../Generated_Code/lpi2c1.c \
../Generated_Code/lpspiCom.c \
../Generated_Code/lpuart1.c \
../Generated_Code/osif1.c \
../Generated_Code/pin_mux.c \
../Generated_Code/pwrMan1.c \
../Generated_Code/rtcTimer1.c \
../Generated_Code/watchdog1.c \

C_DEPS_QUOTED += \
"./Generated_Code/Cpu.d" \
"./Generated_Code/SRSFTimer.d" \
"./Generated_Code/adc0.d" \
"./Generated_Code/can0_flexcan.d" \
"./Generated_Code/clockMan1.d" \
"./Generated_Code/dmaController1.d" \
"./Generated_Code/flash1.d" \
"./Generated_Code/lowFTimer.d" \
"./Generated_Code/lpi2c1.d" \
"./Generated_Code/lpspiCom.d" \
"./Generated_Code/lpuart1.d" \
"./Generated_Code/osif1.d" \
"./Generated_Code/pin_mux.d" \
"./Generated_Code/pwrMan1.d" \
"./Generated_Code/rtcTimer1.d" \
"./Generated_Code/watchdog1.d" \

OBJS_QUOTED += \
"./Generated_Code/Cpu.o" \
"./Generated_Code/SRSFTimer.o" \
"./Generated_Code/adc0.o" \
"./Generated_Code/can0_flexcan.o" \
"./Generated_Code/clockMan1.o" \
"./Generated_Code/dmaController1.o" \
"./Generated_Code/flash1.o" \
"./Generated_Code/lowFTimer.o" \
"./Generated_Code/lpi2c1.o" \
"./Generated_Code/lpspiCom.o" \
"./Generated_Code/lpuart1.o" \
"./Generated_Code/osif1.o" \
"./Generated_Code/pin_mux.o" \
"./Generated_Code/pwrMan1.o" \
"./Generated_Code/rtcTimer1.o" \
"./Generated_Code/watchdog1.o" \

C_DEPS += \
./Generated_Code/Cpu.d \
./Generated_Code/SRSFTimer.d \
./Generated_Code/adc0.d \
./Generated_Code/can0_flexcan.d \
./Generated_Code/clockMan1.d \
./Generated_Code/dmaController1.d \
./Generated_Code/flash1.d \
./Generated_Code/lowFTimer.d \
./Generated_Code/lpi2c1.d \
./Generated_Code/lpspiCom.d \
./Generated_Code/lpuart1.d \
./Generated_Code/osif1.d \
./Generated_Code/pin_mux.d \
./Generated_Code/pwrMan1.d \
./Generated_Code/rtcTimer1.d \
./Generated_Code/watchdog1.d \

OBJS_OS_FORMAT += \
./Generated_Code/Cpu.o \
./Generated_Code/SRSFTimer.o \
./Generated_Code/adc0.o \
./Generated_Code/can0_flexcan.o \
./Generated_Code/clockMan1.o \
./Generated_Code/dmaController1.o \
./Generated_Code/flash1.o \
./Generated_Code/lowFTimer.o \
./Generated_Code/lpi2c1.o \
./Generated_Code/lpspiCom.o \
./Generated_Code/lpuart1.o \
./Generated_Code/osif1.o \
./Generated_Code/pin_mux.o \
./Generated_Code/pwrMan1.o \
./Generated_Code/rtcTimer1.o \
./Generated_Code/watchdog1.o \

OBJS += \
./Generated_Code/Cpu.o \
./Generated_Code/SRSFTimer.o \
./Generated_Code/adc0.o \
./Generated_Code/can0_flexcan.o \
./Generated_Code/clockMan1.o \
./Generated_Code/dmaController1.o \
./Generated_Code/flash1.o \
./Generated_Code/lowFTimer.o \
./Generated_Code/lpi2c1.o \
./Generated_Code/lpspiCom.o \
./Generated_Code/lpuart1.o \
./Generated_Code/osif1.o \
./Generated_Code/pin_mux.o \
./Generated_Code/pwrMan1.o \
./Generated_Code/rtcTimer1.o \
./Generated_Code/watchdog1.o \


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/Cpu.o: ../Generated_Code/Cpu.c
	@echo 'Building file: $<'
	@echo 'Executing target #118 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/Cpu.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/Cpu.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/SRSFTimer.o: ../Generated_Code/SRSFTimer.c
	@echo 'Building file: $<'
	@echo 'Executing target #119 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/SRSFTimer.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/SRSFTimer.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/adc0.o: ../Generated_Code/adc0.c
	@echo 'Building file: $<'
	@echo 'Executing target #120 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/adc0.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/adc0.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/can0_flexcan.o: ../Generated_Code/can0_flexcan.c
	@echo 'Building file: $<'
	@echo 'Executing target #121 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/can0_flexcan.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/can0_flexcan.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/clockMan1.o: ../Generated_Code/clockMan1.c
	@echo 'Building file: $<'
	@echo 'Executing target #122 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/clockMan1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/clockMan1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/dmaController1.o: ../Generated_Code/dmaController1.c
	@echo 'Building file: $<'
	@echo 'Executing target #123 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/dmaController1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/dmaController1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/flash1.o: ../Generated_Code/flash1.c
	@echo 'Building file: $<'
	@echo 'Executing target #124 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/flash1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/flash1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/lowFTimer.o: ../Generated_Code/lowFTimer.c
	@echo 'Building file: $<'
	@echo 'Executing target #125 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/lowFTimer.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/lowFTimer.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/lpi2c1.o: ../Generated_Code/lpi2c1.c
	@echo 'Building file: $<'
	@echo 'Executing target #126 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/lpi2c1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/lpi2c1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/lpspiCom.o: ../Generated_Code/lpspiCom.c
	@echo 'Building file: $<'
	@echo 'Executing target #127 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/lpspiCom.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/lpspiCom.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/lpuart1.o: ../Generated_Code/lpuart1.c
	@echo 'Building file: $<'
	@echo 'Executing target #128 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/lpuart1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/lpuart1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/osif1.o: ../Generated_Code/osif1.c
	@echo 'Building file: $<'
	@echo 'Executing target #129 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/osif1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/osif1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/pin_mux.o: ../Generated_Code/pin_mux.c
	@echo 'Building file: $<'
	@echo 'Executing target #130 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/pin_mux.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/pin_mux.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/pwrMan1.o: ../Generated_Code/pwrMan1.c
	@echo 'Building file: $<'
	@echo 'Executing target #131 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/pwrMan1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/pwrMan1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/rtcTimer1.o: ../Generated_Code/rtcTimer1.c
	@echo 'Building file: $<'
	@echo 'Executing target #132 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/rtcTimer1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/rtcTimer1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/watchdog1.o: ../Generated_Code/watchdog1.c
	@echo 'Building file: $<'
	@echo 'Executing target #133 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Generated_Code/watchdog1.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Generated_Code/watchdog1.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


