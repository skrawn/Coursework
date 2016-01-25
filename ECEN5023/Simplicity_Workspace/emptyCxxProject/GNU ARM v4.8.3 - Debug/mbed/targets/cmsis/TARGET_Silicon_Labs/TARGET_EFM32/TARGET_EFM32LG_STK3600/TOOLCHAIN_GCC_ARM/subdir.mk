################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
D:/Documents/Coursework/ECEN5023/Simplicity_Workspace/Libraries/mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/TOOLCHAIN_GCC_ARM/startup_efm32lg.S 

OBJS += \
./mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/TOOLCHAIN_GCC_ARM/startup_efm32lg.o 


# Each subdirectory must supply rules for building sources it contributes
mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/TOOLCHAIN_GCC_ARM/startup_efm32lg.o: D:/Documents/Coursework/ECEN5023/Simplicity_Workspace/Libraries/mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/TOOLCHAIN_GCC_ARM/startup_efm32lg.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Assembler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -c -x assembler-with-cpp -I"C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/Device/SiliconLabs/EFM32LG/Include" -I"C:/SiliconLabs/SimplicityStudio/v3/developer/sdks/efm32/v2/kits/EFM32LG_STK3600/config" '-DEFM32LG990F256=1' -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


