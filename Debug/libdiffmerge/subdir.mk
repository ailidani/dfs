################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libdiffmerge/blob.c \
../libdiffmerge/diff.c \
../libdiffmerge/encode.c \
../libdiffmerge/file.c \
../libdiffmerge/fossil_wrapper.c \
../libdiffmerge/merge3.c \
../libdiffmerge/printf.c 

OBJS += \
./libdiffmerge/blob.o \
./libdiffmerge/diff.o \
./libdiffmerge/encode.o \
./libdiffmerge/file.o \
./libdiffmerge/fossil_wrapper.o \
./libdiffmerge/merge3.o \
./libdiffmerge/printf.o 

C_DEPS += \
./libdiffmerge/blob.d \
./libdiffmerge/diff.d \
./libdiffmerge/encode.d \
./libdiffmerge/file.d \
./libdiffmerge/fossil_wrapper.d \
./libdiffmerge/merge3.d \
./libdiffmerge/printf.d 


# Each subdirectory must supply rules for building sources it contributes
libdiffmerge/%.o: ../libdiffmerge/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


