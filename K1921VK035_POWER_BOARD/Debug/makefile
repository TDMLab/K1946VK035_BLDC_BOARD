################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include asm/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
K1921VK035_POWER_BOARD.hex \

SECONDARY_SIZE += \
K1921VK035_POWER_BOARD.siz \


# All Target
all: K1921VK035_POWER_BOARD.elf secondary-outputs

# Tool invocations
K1921VK035_POWER_BOARD.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM Cross C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -T "C:\VectorIDE1.3.3_Examples\K1921VK035_POWER_BOARD\cmd\build.ld" -nostartfiles -nodefaultlibs -nostdlib -Xlinker --gc-sections -L"C:\VectorIDE1.3.3_Examples\K1921VK035_POWER_BOARD\lib" -Xlinker -L "C:\VectorIDE1.3.3_Examples\K1921VK035_POWER_BOARD\cmd" -Wl,-Map,"K1921VK035_POWER_BOARD.map" -o "K1921VK035_POWER_BOARD.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

K1921VK035_POWER_BOARD.hex: K1921VK035_POWER_BOARD.elf
	@echo 'Invoking: GNU ARM Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex "K1921VK035_POWER_BOARD.elf"  "K1921VK035_POWER_BOARD.hex"
	@echo 'Finished building: $@'
	@echo ' '

K1921VK035_POWER_BOARD.siz: K1921VK035_POWER_BOARD.elf
	@echo 'Invoking: GNU ARM Cross Print Size'
	arm-none-eabi-size --format=berkeley "K1921VK035_POWER_BOARD.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) K1921VK035_POWER_BOARD.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
