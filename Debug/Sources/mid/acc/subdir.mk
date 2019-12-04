################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../../../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/mid/acc/acc.c" \
"../Sources/mid/acc/fxls_acc.c" \
"../Sources/mid/acc/icm_acc.c" \

C_SRCS += \
../Sources/mid/acc/acc.c \
../Sources/mid/acc/fxls_acc.c \
../Sources/mid/acc/icm_acc.c \

C_DEPS_QUOTED += \
"./Sources/mid/acc/acc.d" \
"./Sources/mid/acc/fxls_acc.d" \
"./Sources/mid/acc/icm_acc.d" \

OBJS_QUOTED += \
"./Sources/mid/acc/acc.o" \
"./Sources/mid/acc/fxls_acc.o" \
"./Sources/mid/acc/icm_acc.o" \

C_DEPS += \
./Sources/mid/acc/acc.d \
./Sources/mid/acc/fxls_acc.d \
./Sources/mid/acc/icm_acc.d \

OBJS_OS_FORMAT += \
./Sources/mid/acc/acc.o \
./Sources/mid/acc/fxls_acc.o \
./Sources/mid/acc/icm_acc.o \

OBJS += \
./Sources/mid/acc/acc.o \
./Sources/mid/acc/fxls_acc.o \
./Sources/mid/acc/icm_acc.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/mid/acc/acc.o: ../Sources/mid/acc/acc.c
	@echo 'Building file: $<'
	@echo 'Executing target #31 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/acc/acc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/acc/acc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/mid/acc/fxls_acc.o: ../Sources/mid/acc/fxls_acc.c
	@echo 'Building file: $<'
	@echo 'Executing target #32 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/acc/fxls_acc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/acc/fxls_acc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/mid/acc/icm_acc.o: ../Sources/mid/acc/icm_acc.c
	@echo 'Building file: $<'
	@echo 'Executing target #33 $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/mid/acc/icm_acc.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "Sources/mid/acc/icm_acc.o" "$<"
	@echo 'Finished building: $<'
	@echo ' '


