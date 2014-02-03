################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../httpd/main.cpp 

OBJS += \
./httpd/main.o 

CPP_DEPS += \
./httpd/main.d 


# Each subdirectory must supply rules for building sources it contributes
httpd/%.o: ../httpd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/Users/eddieaili/Documents/workspace/dfs -I/opt/local/include -I/usr/local/include/osxfuse -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


