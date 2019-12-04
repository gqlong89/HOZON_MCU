################################################################################
# Automatically-generated file. Do not edit!
################################################################################

#Progress monitor hint: 133
first : all
-include ../makefile.init

# This file contains definitions of environment variables used in the makefiles and .args files if exist.
-include makefile.local

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Sources/mid/wdg/subdir.mk
-include Sources/mid/timer/subdir.mk
-include Sources/mid/srs/subdir.mk
-include Sources/mid/softrtc/subdir.mk
-include Sources/mid/rtc/subdir.mk
-include Sources/mid/power_mode_switch/subdir.mk
-include Sources/mid/pm/subdir.mk
-include Sources/mid/pin/subdir.mk
-include Sources/mid/msg/subdir.mk
-include Sources/mid/kernel/subdir.mk
-include Sources/mid/ipc_spi/subdir.mk
-include Sources/mid/i2c/subdir.mk
-include Sources/mid/gpio/subdir.mk
-include Sources/mid/flash/subdir.mk
-include Sources/mid/edma/subdir.mk
-include Sources/mid/dbg_uart/subdir.mk
-include Sources/mid/dbg_trace/subdir.mk
-include Sources/mid/config/subdir.mk
-include Sources/mid/clock/subdir.mk
-include Sources/mid/can_spi/subdir.mk
-include Sources/mid/can_flexcan/subdir.mk
-include Sources/mid/call/subdir.mk
-include Sources/mid/brdinfo/subdir.mk
-include Sources/mid/aes/subdir.mk
-include Sources/mid/adc/subdir.mk
-include Sources/mid/acc/subdir.mk
-include Sources/subdir.mk
-include Sources/app/tp/uds/subdir.mk
-include Sources/app/tp/proxy/subdir.mk
-include Sources/app/shell/subdir.mk
-include Sources/app/scom/subdir.mk
-include Sources/app/power/subdir.mk
-include Sources/app/nm_osek/subdir.mk
-include Sources/app/nm_autosar/subdir.mk
-include Sources/app/mcu_mgr/subdir.mk
-include Sources/app/custom/subdir.mk
-include Sources/app/can_mgr/subdir.mk
-include Sources/app/battery/subdir.mk
-include SDK/rtos/osif/subdir.mk
-include SDK/platform/hal/src/wdog/subdir.mk
-include SDK/platform/hal/src/smc/subdir.mk
-include SDK/platform/hal/src/sim/S32K144/subdir.mk
-include SDK/platform/hal/src/scg/subdir.mk
-include SDK/platform/hal/src/rtc/subdir.mk
-include SDK/platform/hal/src/rcm/subdir.mk
-include SDK/platform/hal/src/port/subdir.mk
-include SDK/platform/hal/src/pmc/subdir.mk
-include SDK/platform/hal/src/pcc/subdir.mk
-include SDK/platform/hal/src/lpuart/subdir.mk
-include SDK/platform/hal/src/lpspi/subdir.mk
-include SDK/platform/hal/src/lpi2c/subdir.mk
-include SDK/platform/hal/src/ftm/subdir.mk
-include SDK/platform/hal/src/flexcan/subdir.mk
-include SDK/platform/hal/src/edma/subdir.mk
-include SDK/platform/hal/src/dmamux/subdir.mk
-include SDK/platform/hal/src/adc/subdir.mk
-include SDK/platform/drivers/src/wdog/subdir.mk
-include SDK/platform/drivers/src/rtc/subdir.mk
-include SDK/platform/drivers/src/power/subdir.mk
-include SDK/platform/drivers/src/pins/subdir.mk
-include SDK/platform/drivers/src/lpuart/subdir.mk
-include SDK/platform/drivers/src/lpspi/subdir.mk
-include SDK/platform/drivers/src/lpi2c/subdir.mk
-include SDK/platform/drivers/src/interrupt/subdir.mk
-include SDK/platform/drivers/src/ftm/subdir.mk
-include SDK/platform/drivers/src/flexcan/subdir.mk
-include SDK/platform/drivers/src/flash/subdir.mk
-include SDK/platform/drivers/src/edma/subdir.mk
-include SDK/platform/drivers/src/clock/subdir.mk
-include SDK/platform/drivers/src/clock/S32K144/subdir.mk
-include SDK/platform/drivers/src/adc/subdir.mk
-include SDK/platform/devices/subdir.mk
-include SDK/platform/devices/S32K144/startup/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include Generated_Code/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
mcu_app.elf \

EXECUTABLES_QUOTED += \
"mcu_app.elf" \

EXECUTABLES_OS_FORMAT += \
mcu_app.elf \

SECONDARY_FLASH += \
mcu_app.bin \

SECONDARY_FLASH_QUOTED += \
"mcu_app.bin" \

SECONDARY_FLASH_OS_FORMAT += \
mcu_app.bin \

SECONDARY_LIST += \
mcu_app.lst \

SECONDARY_LIST_QUOTED += \
"mcu_app.lst" \

SECONDARY_LIST_OS_FORMAT += \
mcu_app.lst \

SECONDARY_SIZE += \
mcu_app.siz \

SECONDARY_SIZE_QUOTED += \
"mcu_app.siz" \

SECONDARY_SIZE_OS_FORMAT += \
mcu_app.siz \


# All Target
all: warning-messages explicit-dependencies mcu_app.elf secondary-outputs

# Tool invocations
mcu_app.elf: $(OBJS) C:/Users/INTEST/Desktop/MCU(2)/MCU/Project_Settings/Linker_Files/S32K144_64_flash.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Executing target #134 $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "mcu_app.elf" "@mcu_app.args"  $(USER_OBJS_QUOTED) 
	@echo 'Finished building target: $@'
	@echo ' '

mcu_app.bin: $(EXECUTABLES)
	@echo 'Executing target #135 $@'
	@echo 'Invoking: Standard S32DS Create Flash Image'
	arm-none-eabi-objcopy -O binary $(EXECUTABLES)  "mcu_app.bin"
	@echo 'Finished building: $@'
	@echo ' '

mcu_app.lst: $(EXECUTABLES)
	@echo 'Executing target #136 $@'
	@echo 'Invoking: Standard S32DS Create Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --line-numbers --wide $(EXECUTABLES) > "mcu_app.lst"
	@echo 'Finished building: $@'
	@echo ' '

mcu_app.siz: $(EXECUTABLES)
	@echo 'Executing target #137 $@'
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_FLASH_QUOTED) ./*/*.d ./*/*/*/*/*/*.d ./*/*/*/*.d ./*/*/*/*/*/*/*.d ./*/*/*/*/*.d  ./*/*.o ./*/*/*.o ./*/*/*/*/*/*.o ./*/*/*/*.o ./*/*/*/*/*/*/*.o ./*/*/*/*/*.o  $(SECONDARY_LIST_QUOTED) $(SECONDARY_SIZE_QUOTED) $(EXECUTABLES_QUOTED)
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents explicit-dependencies warning-messages
.SECONDARY:

-include ../makefile.targets

explicit-dependencies: 

warning-messages: 

