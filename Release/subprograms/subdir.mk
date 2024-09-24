################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../subprograms/calibration.c \
../subprograms/common_subprograms.c \
../subprograms/count_kpm.c \
../subprograms/encoder_counter.c \
../subprograms/encoder_test.c \
../subprograms/info.c \
../subprograms/limit_sw_test.c \
../subprograms/settings.c 

OBJS += \
./subprograms/calibration.o \
./subprograms/common_subprograms.o \
./subprograms/count_kpm.o \
./subprograms/encoder_counter.o \
./subprograms/encoder_test.o \
./subprograms/info.o \
./subprograms/limit_sw_test.o \
./subprograms/settings.o 

C_DEPS += \
./subprograms/calibration.d \
./subprograms/common_subprograms.d \
./subprograms/count_kpm.d \
./subprograms/encoder_counter.d \
./subprograms/encoder_test.d \
./subprograms/info.d \
./subprograms/limit_sw_test.d \
./subprograms/settings.d 


# Each subdirectory must supply rules for building sources it contributes
subprograms/%.o: ../subprograms/%.c subprograms/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


