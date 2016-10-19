################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/Components/stmpe811/stmpe811.c 

OBJS += \
./Utilities/Components/stmpe811/stmpe811.o 

C_DEPS += \
./Utilities/Components/stmpe811/stmpe811.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/Components/stmpe811/%.o: ../Utilities/Components/stmpe811/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -DSTM32F746G_DISCO -DSTM32F746NGHx -DSTM32F7 -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/inc" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/CMSIS/core" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/CMSIS/device" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/HAL_Driver/Inc" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/HAL_Driver/Inc/Legacy" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Fonts" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Log" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/ft5336" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/st7735" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/wm8994" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/exc7200" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/n25q512a" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/ts3510" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/s5k5cag" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/ampire480272" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/n25q128a" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/mfxstm32l152" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/ov9655" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/otm8009a" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/ft6x06" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/stmpe811" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/Common" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/ampire640480" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/mx25l512" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/Components/rk043fn48h" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Utilities/STM32746G-Discovery" -I"/home/miccio/Desktop/embedded/STM/workspace/stm32f7_audio2/Middlewares/ST/STM32_Audio/Addons/PDM" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


