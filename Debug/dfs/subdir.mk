################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../dfs/logging.cpp \
../dfs/oricmd.cpp \
../dfs/orifuse.cpp \
../dfs/oripriv.cpp \
../dfs/server.cpp 

OBJS += \
./dfs/logging.o \
./dfs/oricmd.o \
./dfs/orifuse.o \
./dfs/oripriv.o \
./dfs/server.o 

CPP_DEPS += \
./dfs/logging.d \
./dfs/oricmd.d \
./dfs/orifuse.d \
./dfs/oripriv.d \
./dfs/server.d 


# Each subdirectory must supply rules for building sources it contributes
dfs/%.o: ../dfs/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/Users/eddieaili/Documents/workspace/dfs -I/opt/local/include -I/usr/local/include/osxfuse -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -D_FILE_OFFSET_BITS=64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


