################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/app/tp/proxy/Proxy.c" \

C_SRCS += \
../Sources/app/tp/proxy/Proxy.c \

C_DEPS_QUOTED += \
"./Sources/app/tp/proxy/Proxy.d" \

OBJS_QUOTED += \
"./Sources/app/tp/proxy/Proxy.o" \

C_DEPS += \
./Sources/app/tp/proxy/Proxy.d \

OBJS_OS_FORMAT += \
./Sources/app/tp/proxy/Proxy.o \

OBJS += \
./Sources/app/tp/proxy/Proxy.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/app/tp/proxy/Proxy.o: ../Sources/app/tp/proxy/Proxy.c
	@echo 'Building file: $<'
	@echo 'Executing target #41 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/app/tp/proxy/Proxy.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/app/tp/proxy/Proxy.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


