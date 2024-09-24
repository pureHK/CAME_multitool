################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../data.c \
../helpers.c \
../led.c \
../main.c \
../math_calculations.c \
../menu.c \
../program_manager.c 

OBJS += \
./data.o \
./helpers.o \
./led.o \
./main.o \
./math_calculations.o \
./menu.o \
./program_manager.o 

C_DEPS += \
./data.d \
./helpers.d \
./led.d \
./main.d \
./math_calculations.d \
./menu.d \
./program_manager.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


