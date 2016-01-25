################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Documents/Coursework/ECEN5023/Simplicity_Workspace/Libraries/mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/PeripheralPins.c 

OBJS += \
./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/PeripheralPins.o 

C_DEPS += \
./mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/PeripheralPins.d 


# Each subdirectory must supply rules for building sources it contributes
mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/PeripheralPins.o: D:/Documents/Coursework/ECEN5023/Simplicity_Workspace/Libraries/mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/PeripheralPins.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32LG990F256=1' '-DDEBUG=1' -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\api" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\hal" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600\TOOLCHAIN_GCC_ARM" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib\inc" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib\src" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\common" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\emptyCxxProject\inc" -O0 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -fno-common -fomit-frame-pointer -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/PeripheralPins.d" -MT"mbed/targets/hal/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/PeripheralPins.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


