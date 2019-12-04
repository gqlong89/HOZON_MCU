################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/tp/uds/UDS_CAN.c" \
"../Sources/app/tp/uds/UDS_Config.c" \
"../Sources/app/tp/uds/UDS_LData.c" \
"../Sources/app/tp/uds/UDS_NSData.c" \
"../Sources/app/tp/uds/UDS_Task.c" \
"../Sources/app/tp/uds/UDS_Timer.c" \

C_SRCS += \
../Sources/app/tp/uds/UDS_CAN.c \
../Sources/app/tp/uds/UDS_Config.c \
../Sources/app/tp/uds/UDS_LData.c \
../Sources/app/tp/uds/UDS_NSData.c \
../Sources/app/tp/uds/UDS_Task.c \
../Sources/app/tp/uds/UDS_Timer.c \

C_DEPS_QUOTED += \
"./Sources/app/tp/uds/UDS_CAN.d" \
"./Sources/app/tp/uds/UDS_Config.d" \
"./Sources/app/tp/uds/UDS_LData.d" \
"./Sources/app/tp/uds/UDS_NSData.d" \
"./Sources/app/tp/uds/UDS_Task.d" \
"./Sources/app/tp/uds/UDS_Timer.d" \

OBJS_QUOTED += \
"./Sources/app/tp/uds/UDS_CAN.o" \
"./Sources/app/tp/uds/UDS_Config.o" \
"./Sources/app/tp/uds/UDS_LData.o" \
"./Sources/app/tp/uds/UDS_NSData.o" \
"./Sources/app/tp/uds/UDS_Task.o" \
"./Sources/app/tp/uds/UDS_Timer.o" \

C_DEPS += \
./Sources/app/tp/uds/UDS_CAN.d \
./Sources/app/tp/uds/UDS_Config.d \
./Sources/app/tp/uds/UDS_LData.d \
./Sources/app/tp/uds/UDS_NSData.d \
./Sources/app/tp/uds/UDS_Task.d \
./Sources/app/tp/uds/UDS_Timer.d \

OBJS_OS_FORMAT += \
./Sources/app/tp/uds/UDS_CAN.o \
./Sources/app/tp/uds/UDS_Config.o \
./Sources/app/tp/uds/UDS_LData.o \
./Sources/app/tp/uds/UDS_NSData.o \
./Sources/app/tp/uds/UDS_Task.o \
./Sources/app/tp/uds/UDS_Timer.o \

OBJS += \
./Sources/app/tp/uds/UDS_CAN.o \
./Sources/app/tp/uds/UDS_Config.o \
./Sources/app/tp/uds/UDS_LData.o \
./Sources/app/tp/uds/UDS_NSData.o \
./Sources/app/tp/uds/UDS_Task.o \
./Sources/app/tp/uds/UDS_Timer.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/tp/uds/UDS_CAN.o: ../Sources/app/tp/uds/UDS_CAN.c
	@echo 'Building file: $<'
	@echo 'Executing target #35 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/tp/uds/UDS_CAN.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/tp/uds/UDS_CAN.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/tp/uds/UDS_Config.o: ../Sources/app/tp/uds/UDS_Config.c
	@echo 'Building file: $<'
	@echo 'Executing target #36 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/tp/uds/UDS_Config.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/tp/uds/UDS_Config.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/tp/uds/UDS_LData.o: ../Sources/app/tp/uds/UDS_LData.c
	@echo 'Building file: $<'
	@echo 'Executing target #37 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/tp/uds/UDS_LData.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/tp/uds/UDS_LData.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/tp/uds/UDS_NSData.o: ../Sources/app/tp/uds/UDS_NSData.c
	@echo 'Building file: $<'
	@echo 'Executing target #38 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/tp/uds/UDS_NSData.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/tp/uds/UDS_NSData.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/tp/uds/UDS_Task.o: ../Sources/app/tp/uds/UDS_Task.c
	@echo 'Building file: $<'
	@echo 'Executing target #39 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/tp/uds/UDS_Task.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/tp/uds/UDS_Task.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/tp/uds/UDS_Timer.o: ../Sources/app/tp/uds/UDS_Timer.c
	@echo 'Building file: $<'
	@echo 'Executing target #40 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/tp/uds/UDS_Timer.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/tp/uds/UDS_Timer.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


