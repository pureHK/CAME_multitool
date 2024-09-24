################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HD44780_LCD/font.c \
../HD44780_LCD/hd44780.c \
../HD44780_LCD/hd44780_phy_layer.c 

OBJS += \
./HD44780_LCD/font.o \
./HD44780_LCD/hd44780.o \
./HD44780_LCD/hd44780_phy_layer.o 

C_DEPS += \
./HD44780_LCD/font.d \
./HD44780_LCD/hd44780.d \
./HD44780_LCD/hd44780_phy_layer.d 


# Each subdirectory must supply rules for building sources it contributes
HD44780_LCD/%.o: ../HD44780_LCD/%.c HD44780_LCD/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


