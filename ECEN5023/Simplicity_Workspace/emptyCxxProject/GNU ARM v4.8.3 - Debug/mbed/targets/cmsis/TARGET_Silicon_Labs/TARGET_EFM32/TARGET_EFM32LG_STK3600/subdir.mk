################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Documents/Coursework/ECEN5023/Simplicity_Workspace/Libraries/mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/cmsis_nvic.c \
D:/Documents/Coursework/ECEN5023/Simplicity_Workspace/Libraries/mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/system_efm32lg.c 

OBJS += \
./mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/cmsis_nvic.o \
./mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/system_efm32lg.o 

C_DEPS += \
./mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/cmsis_nvic.d \
./mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/system_efm32lg.d 


# Each subdirectory must supply rules for building sources it contributes
mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/cmsis_nvic.o: D:/Documents/Coursework/ECEN5023/Simplicity_Workspace/Libraries/mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/cmsis_nvic.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32LG990F256=1' '-DDEBUG=1' -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\api" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\hal" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600\TOOLCHAIN_GCC_ARM" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib\inc" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib\src" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\common" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\emptyCxxProject\inc" -O0 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -fno-common -fomit-frame-pointer -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/cmsis_nvic.d" -MT"mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/cmsis_nvic.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/system_efm32lg.o: D:/Documents/Coursework/ECEN5023/Simplicity_Workspace/Libraries/mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/system_efm32lg.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32LG990F256=1' '-DDEBUG=1' -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\api" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\hal" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600\TOOLCHAIN_GCC_ARM" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib\inc" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib\src" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\common" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\emptyCxxProject\inc" -O0 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -fno-common -fomit-frame-pointer -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -std=c99 -MMD -MP -MF"mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/system_efm32lg.d" -MT"mbed/targets/cmsis/TARGET_Silicon_Labs/TARGET_EFM32/TARGET_EFM32LG_STK3600/system_efm32lg.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


