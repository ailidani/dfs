################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libfastlz/6pack.c \
../libfastlz/6unpack.c \
../libfastlz/fastlz.c 

OBJS += \
./libfastlz/6pack.o \
./libfastlz/6unpack.o \
./libfastlz/fastlz.o 

C_DEPS += \
./libfastlz/6pack.d \
./libfastlz/6unpack.d \
./libfastlz/fastlz.d 


# Each subdirectory must supply rules for building sources it contributes
libfastlz/%.o: ../libfastlz/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


