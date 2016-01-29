################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/subdir.mk
-include mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/subdir.mk
-include mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/subdir.mk
-include mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/TOOLCHAIN_GCC_ARM/subdir.mk
-include mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/subdir.mk
-include mbed/common/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Assignment_2_ACMP.axf

# Tool invocations
Assignment_2_ACMP.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C++ Linker'
	arm-none-eabi-g++ -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600\TOOLCHAIN_GCC_ARM\efm32lg.ld" --specs=nosys.specs -Xlinker --gc-sections -Xlinker -Map="Assignment_2_ACMP.map" --specs=nano.specs -o Assignment_2_ACMP.axf "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_acmp.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_adc.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_aes.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_assert.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_burtc.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_cmu.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_dac.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_dbg.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_dma.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_ebi.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_emu.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_gpio.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_i2c.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_idac.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_int.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_lcd.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_lesense.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_letimer.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_leuart.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_mpu.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_msc.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_opamp.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_pcnt.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_prs.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_rmu.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_rtc.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_system.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_timer.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_usart.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_vcmp.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/emlib/src/em_wdog.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/PeripheralPins.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/analogin_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/analogout_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/dma_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/gpio_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/gpio_irq_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/i2c_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/lp_ticker.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/mbed_overrides.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/pinmap.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/pinmap_function.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/port_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/pwmout_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/rtc_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/serial_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/sleep.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/spi_api.o" "./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/us_ticker.o" "./mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/TOOLCHAIN_GCC_ARM/startup_efm32lg.o" "./mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/cmsis_nvic.o" "./mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/system_efm32lg.o" "./mbed/common/BusIn.o" "./mbed/common/BusInOut.o" "./mbed/common/BusOut.o" "./mbed/common/CAN.o" "./mbed/common/CallChain.o" "./mbed/common/Ethernet.o" "./mbed/common/FileBase.o" "./mbed/common/FileLike.o" "./mbed/common/FilePath.o" "./mbed/common/FileSystemLike.o" "./mbed/common/I2C.o" "./mbed/common/I2CSlave.o" "./mbed/common/InterruptIn.o" "./mbed/common/InterruptManager.o" "./mbed/common/LocalFileSystem.o" "./mbed/common/RawSerial.o" "./mbed/common/SPI.o" "./mbed/common/SPISlave.o" "./mbed/common/Serial.o" "./mbed/common/SerialBase.o" "./mbed/common/Stream.o" "./mbed/common/Ticker.o" "./mbed/common/Timeout.o" "./mbed/common/Timer.o" "./mbed/common/TimerEvent.o" "./mbed/common/assert.o" "./mbed/common/board.o" "./mbed/common/error.o" "./mbed/common/gpio.o" "./mbed/common/lp_ticker_api.o" "./mbed/common/mbed_interface.o" "./mbed/common/pinmap_common.o" "./mbed/common/retarget.o" "./mbed/common/rtc_time.o" "./mbed/common/semihost_api.o" "./mbed/common/ticker_api.o" "./mbed/common/us_ticker_api.o" "./mbed/common/wait_api.o" "-lstdc++" "-lsupc++" "-lm" "-lnosys" "-lc" "-lgcc" "./src/main.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: Assignment_2_ACMP.hex'
	arm-none-eabi-objcopy -O ihex "Assignment_2_ACMP.axf" "Assignment_2_ACMP.hex"
	@echo ' '

	@echo 'Building bin file: Assignment_2_ACMP.bin'
	arm-none-eabi-objcopy -O binary "Assignment_2_ACMP.axf" "Assignment_2_ACMP.bin"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "Assignment_2_ACMP.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) Assignment_2_ACMP.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
