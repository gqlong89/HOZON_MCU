################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/scom/scom_can_msg.c" \
"../Sources/app/scom/scom_para_msg.c" \
"../Sources/app/scom/scom_task.c" \
"../Sources/app/scom/scom_tl.c" \
"../Sources/app/scom/scom_tl_ex.c" \

C_SRCS += \
../Sources/app/scom/scom_can_msg.c \
../Sources/app/scom/scom_para_msg.c \
../Sources/app/scom/scom_task.c \
../Sources/app/scom/scom_tl.c \
../Sources/app/scom/scom_tl_ex.c \

C_DEPS_QUOTED += \
"./Sources/app/scom/scom_can_msg.d" \
"./Sources/app/scom/scom_para_msg.d" \
"./Sources/app/scom/scom_task.d" \
"./Sources/app/scom/scom_tl.d" \
"./Sources/app/scom/scom_tl_ex.d" \

OBJS_QUOTED += \
"./Sources/app/scom/scom_can_msg.o" \
"./Sources/app/scom/scom_para_msg.o" \
"./Sources/app/scom/scom_task.o" \
"./Sources/app/scom/scom_tl.o" \
"./Sources/app/scom/scom_tl_ex.o" \

C_DEPS += \
./Sources/app/scom/scom_can_msg.d \
./Sources/app/scom/scom_para_msg.d \
./Sources/app/scom/scom_task.d \
./Sources/app/scom/scom_tl.d \
./Sources/app/scom/scom_tl_ex.d \

OBJS_OS_FORMAT += \
./Sources/app/scom/scom_can_msg.o \
./Sources/app/scom/scom_para_msg.o \
./Sources/app/scom/scom_task.o \
./Sources/app/scom/scom_tl.o \
./Sources/app/scom/scom_tl_ex.o \

OBJS += \
./Sources/app/scom/scom_can_msg.o \
./Sources/app/scom/scom_para_msg.o \
./Sources/app/scom/scom_task.o \
./Sources/app/scom/scom_tl.o \
./Sources/app/scom/scom_tl_ex.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/scom/scom_can_msg.o: ../Sources/app/scom/scom_can_msg.c
	@echo 'Building file: $<'
	@echo 'Executing target #47 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/scom/scom_can_msg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/scom/scom_can_msg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/scom/scom_para_msg.o: ../Sources/app/scom/scom_para_msg.c
	@echo 'Building file: $<'
	@echo 'Executing target #48 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/scom/scom_para_msg.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/scom/scom_para_msg.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/scom/scom_task.o: ../Sources/app/scom/scom_task.c
	@echo 'Building file: $<'
	@echo 'Executing target #49 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/scom/scom_task.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/scom/scom_task.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/scom/scom_tl.o: ../Sources/app/scom/scom_tl.c
	@echo 'Building file: $<'
	@echo 'Executing target #50 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/scom/scom_tl.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/scom/scom_tl.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/scom/scom_tl_ex.o: ../Sources/app/scom/scom_tl_ex.c
	@echo 'Building file: $<'
	@echo 'Executing target #51 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/scom/scom_tl_ex.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/scom/scom_tl_ex.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


