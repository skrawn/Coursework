################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/empty_project.cpp 

OBJS += \
./src/empty_project.o 

CPP_DEPS += \
./src/empty_project.d 


# Each subdirectory must supply rules for building sources it contributes
src/empty_project.o: ../src/empty_project.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C++ Compiler'
	arm-none-eabi-g++ -g -gdwarf-2 -mcpu=cortex-m3 -mthumb '-DEFM32LG990F256=1' -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\api" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\hal" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\cmsis\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600\TOOLCHAIN_GCC_ARM" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\TARGET_EFM32LG_STK3600" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib\inc" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\targets\hal\TARGET_Silicon_Labs\TARGET_EFM32\emlib\src" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\Libraries\mbed\common" -I"D:\Documents\Coursework\ECEN5023\Simplicity_Workspace\emptyCxxProject\inc" -O0 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -fno-common -fomit-frame-pointer -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -MMD -MP -MF"src/empty_project.d" -MT"src/empty_project.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


