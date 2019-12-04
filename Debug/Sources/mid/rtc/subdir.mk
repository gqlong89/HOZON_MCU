################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/rtc/inrtc.c" \
"../Sources/mid/rtc/rtc.c" \

C_SRCS += \
../Sources/mid/rtc/inrtc.c \
../Sources/mid/rtc/rtc.c \

C_DEPS_QUOTED += \
"./Sources/mid/rtc/inrtc.d" \
"./Sources/mid/rtc/rtc.d" \

OBJS_QUOTED += \
"./Sources/mid/rtc/inrtc.o" \
"./Sources/mid/rtc/rtc.o" \

C_DEPS += \
./Sources/mid/rtc/inrtc.d \
./Sources/mid/rtc/rtc.d \

OBJS_OS_FORMAT += \
./Sources/mid/rtc/inrtc.o \
./Sources/mid/rtc/rtc.o \

OBJS += \
./Sources/mid/rtc/inrtc.o \
./Sources/mid/rtc/rtc.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/rtc/inrtc.o: ../Sources/mid/rtc/inrtc.c
	@echo 'Building file: $<'
	@echo 'Executing target #5 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/rtc/inrtc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/rtc/inrtc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/mid/rtc/rtc.o: ../Sources/mid/rtc/rtc.c
	@echo 'Building file: $<'
	@echo 'Executing target #6 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/rtc/rtc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/rtc/rtc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


