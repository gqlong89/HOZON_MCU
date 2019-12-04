################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/dbg_trace/dbg_trace.c" \

C_SRCS += \
../Sources/mid/dbg_trace/dbg_trace.c \

C_DEPS_QUOTED += \
"./Sources/mid/dbg_trace/dbg_trace.d" \

OBJS_QUOTED += \
"./Sources/mid/dbg_trace/dbg_trace.o" \

C_DEPS += \
./Sources/mid/dbg_trace/dbg_trace.d \

OBJS_OS_FORMAT += \
./Sources/mid/dbg_trace/dbg_trace.o \

OBJS += \
./Sources/mid/dbg_trace/dbg_trace.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/dbg_trace/dbg_trace.o: ../Sources/mid/dbg_trace/dbg_trace.c
	@echo 'Building file: $<'
	@echo 'Executing target #21 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/dbg_trace/dbg_trace.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/dbg_trace/dbg_trace.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


