################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/kernel/debug.c" \
"../Sources/mid/kernel/task.c" \
"../Sources/mid/kernel/timing.c" \

C_SRCS += \
../Sources/mid/kernel/debug.c \
../Sources/mid/kernel/task.c \
../Sources/mid/kernel/timing.c \

C_DEPS_QUOTED += \
"./Sources/mid/kernel/debug.d" \
"./Sources/mid/kernel/task.d" \
"./Sources/mid/kernel/timing.d" \

OBJS_QUOTED += \
"./Sources/mid/kernel/debug.o" \
"./Sources/mid/kernel/task.o" \
"./Sources/mid/kernel/timing.o" \

C_DEPS += \
./Sources/mid/kernel/debug.d \
./Sources/mid/kernel/task.d \
./Sources/mid/kernel/timing.d \

OBJS_OS_FORMAT += \
./Sources/mid/kernel/debug.o \
./Sources/mid/kernel/task.o \
./Sources/mid/kernel/timing.o \

OBJS += \
./Sources/mid/kernel/debug.o \
./Sources/mid/kernel/task.o \
./Sources/mid/kernel/timing.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/kernel/debug.o: ../Sources/mid/kernel/debug.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/kernel/debug.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/kernel/debug.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/mid/kernel/task.o: ../Sources/mid/kernel/task.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/kernel/task.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/kernel/task.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/mid/kernel/timing.o: ../Sources/mid/kernel/timing.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/kernel/timing.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/kernel/timing.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


