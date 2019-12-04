################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/nm_osek/nm_osekCore.c" \
"../Sources/app/nm_osek/nm_osekTimer.c" \

C_SRCS += \
../Sources/app/nm_osek/nm_osekCore.c \
../Sources/app/nm_osek/nm_osekTimer.c \

C_DEPS_QUOTED += \
"./Sources/app/nm_osek/nm_osekCore.d" \
"./Sources/app/nm_osek/nm_osekTimer.d" \

OBJS_QUOTED += \
"./Sources/app/nm_osek/nm_osekCore.o" \
"./Sources/app/nm_osek/nm_osekTimer.o" \

C_DEPS += \
./Sources/app/nm_osek/nm_osekCore.d \
./Sources/app/nm_osek/nm_osekTimer.d \

OBJS_OS_FORMAT += \
./Sources/app/nm_osek/nm_osekCore.o \
./Sources/app/nm_osek/nm_osekTimer.o \

OBJS += \
./Sources/app/nm_osek/nm_osekCore.o \
./Sources/app/nm_osek/nm_osekTimer.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/nm_osek/nm_osekCore.o: ../Sources/app/nm_osek/nm_osekCore.c
	@echo 'Building file: $<'
	@echo 'Executing target #54 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/nm_osek/nm_osekCore.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/nm_osek/nm_osekCore.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/app/nm_osek/nm_osekTimer.o: ../Sources/app/nm_osek/nm_osekTimer.c
	@echo 'Building file: $<'
	@echo 'Executing target #55 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/nm_osek/nm_osekTimer.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/nm_osek/nm_osekTimer.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


