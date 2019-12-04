################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/battery/battery.c" \
"../Sources/app/battery/panasonic.c" \
"../Sources/app/battery/varta.c" \

C_SRCS += \
../Sources/app/battery/battery.c \
../Sources/app/battery/panasonic.c \
../Sources/app/battery/varta.c \

C_DEPS_QUOTED += \
"./Sources/app/battery/battery.d" \
"./Sources/app/battery/panasonic.d" \
"./Sources/app/battery/varta.d" \

OBJS_QUOTED += \
"./Sources/app/battery/battery.o" \
"./Sources/app/battery/panasonic.o" \
"./Sources/app/battery/varta.o" \

C_DEPS += \
./Sources/app/battery/battery.d \
./Sources/app/battery/panasonic.d \
./Sources/app/battery/varta.d \

OBJS_OS_FORMAT += \
./Sources/app/battery/battery.o \
./Sources/app/battery/panasonic.o \
./Sources/app/battery/varta.o \

OBJS += \
./Sources/app/battery/battery.o \
./Sources/app/battery/panasonic.o \
./Sources/app/battery/varta.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/battery/battery.o: ../Sources/app/battery/battery.c
	@echo 'Building file: $<'
	@echo 'Executing target #67 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/battery/battery.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/battery/battery.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/battery/panasonic.o: ../Sources/app/battery/panasonic.c
	@echo 'Building file: $<'
	@echo 'Executing target #68 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/battery/panasonic.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/battery/panasonic.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/battery/varta.o: ../Sources/app/battery/varta.c
	@echo 'Building file: $<'
	@echo 'Executing target #69 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/battery/varta.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/battery/varta.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


