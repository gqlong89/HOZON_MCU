################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/shell/shell_cmd.c" \
"../Sources/app/shell/shell_list.c" \
"../Sources/app/shell/shell_redirect.c" \
"../Sources/app/shell/shell_task.c" \
"../Sources/app/shell/shell_transfer.c" \

C_SRCS += \
../Sources/app/shell/shell_cmd.c \
../Sources/app/shell/shell_list.c \
../Sources/app/shell/shell_redirect.c \
../Sources/app/shell/shell_task.c \
../Sources/app/shell/shell_transfer.c \

C_DEPS_QUOTED += \
"./Sources/app/shell/shell_cmd.d" \
"./Sources/app/shell/shell_list.d" \
"./Sources/app/shell/shell_redirect.d" \
"./Sources/app/shell/shell_task.d" \
"./Sources/app/shell/shell_transfer.d" \

OBJS_QUOTED += \
"./Sources/app/shell/shell_cmd.o" \
"./Sources/app/shell/shell_list.o" \
"./Sources/app/shell/shell_redirect.o" \
"./Sources/app/shell/shell_task.o" \
"./Sources/app/shell/shell_transfer.o" \

C_DEPS += \
./Sources/app/shell/shell_cmd.d \
./Sources/app/shell/shell_list.d \
./Sources/app/shell/shell_redirect.d \
./Sources/app/shell/shell_task.d \
./Sources/app/shell/shell_transfer.d \

OBJS_OS_FORMAT += \
./Sources/app/shell/shell_cmd.o \
./Sources/app/shell/shell_list.o \
./Sources/app/shell/shell_redirect.o \
./Sources/app/shell/shell_task.o \
./Sources/app/shell/shell_transfer.o \

OBJS += \
./Sources/app/shell/shell_cmd.o \
./Sources/app/shell/shell_list.o \
./Sources/app/shell/shell_redirect.o \
./Sources/app/shell/shell_task.o \
./Sources/app/shell/shell_transfer.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/shell/shell_cmd.o: ../Sources/app/shell/shell_cmd.c
	@echo 'Building file: $<'
	@echo 'Executing target #42 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/shell/shell_cmd.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/shell/shell_cmd.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/shell/shell_list.o: ../Sources/app/shell/shell_list.c
	@echo 'Building file: $<'
	@echo 'Executing target #43 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/shell/shell_list.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/shell/shell_list.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/shell/shell_redirect.o: ../Sources/app/shell/shell_redirect.c
	@echo 'Building file: $<'
	@echo 'Executing target #44 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/shell/shell_redirect.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/shell/shell_redirect.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/shell/shell_task.o: ../Sources/app/shell/shell_task.c
	@echo 'Building file: $<'
	@echo 'Executing target #45 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/shell/shell_task.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/shell/shell_task.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/shell/shell_transfer.o: ../Sources/app/shell/shell_transfer.c
	@echo 'Building file: $<'
	@echo 'Executing target #46 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/shell/shell_transfer.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/shell/shell_transfer.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


