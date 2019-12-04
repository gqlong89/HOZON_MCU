################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/softrtc/softrtc.c" \

C_SRCS += \
../Sources/mid/softrtc/softrtc.c \

C_DEPS_QUOTED += \
"./Sources/mid/softrtc/softrtc.d" \

OBJS_QUOTED += \
"./Sources/mid/softrtc/softrtc.o" \

C_DEPS += \
./Sources/mid/softrtc/softrtc.d \

OBJS_OS_FORMAT += \
./Sources/mid/softrtc/softrtc.o \

OBJS += \
./Sources/mid/softrtc/softrtc.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/softrtc/softrtc.o: ../Sources/mid/softrtc/softrtc.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/softrtc/softrtc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/softrtc/softrtc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


