################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/PWM_Control.c \
../src/main.c \
../src/system_K1921VK035.c 

OBJS += \
./src/PWM_Control.o \
./src/main.o \
./src/system_K1921VK035.o 

C_DEPS += \
./src/PWM_Control.d \
./src/main.d \
./src/system_K1921VK035.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\VectorIDE1.3.3_Examples\K1921VK035_POWER_BOARD/include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


