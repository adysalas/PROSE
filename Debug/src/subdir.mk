################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lcd.c \
../src/main.c \
../src/stm32f10x_it.c \
../src/syscalls.c 

OBJS += \
./src/lcd.o \
./src/main.o \
./src/stm32f10x_it.o \
./src/syscalls.o 

C_DEPS += \
./src/lcd.d \
./src/main.d \
./src/stm32f10x_it.d \
./src/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32F1 -DSTM32F103RBTx -DSTM32 -DDEBUG -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD -I"C:/Users/gonca/Documents/ISEP/Mestrado/2.� Ano/PROSE/PROSE/inc" -I"C:/Users/gonca/Documents/ISEP/Mestrado/2.� Ano/PROSE/PROSE/StdPeriph_Driver/inc" -I"C:/Users/gonca/Documents/ISEP/Mestrado/2.� Ano/PROSE/PROSE/CMSIS/core" -I"C:/Users/gonca/Documents/ISEP/Mestrado/2.� Ano/PROSE/PROSE/CMSIS/device" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


