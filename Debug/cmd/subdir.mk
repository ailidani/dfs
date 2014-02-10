################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cmd/cmd_newfs.cpp \
../cmd/fuse_cmd.cpp \
../cmd/main.cpp \
../cmd/server.cpp 

CC_SRCS += \
../cmd/cmd_addkey.cc \
../cmd/cmd_branch.cc \
../cmd/cmd_branches.cc \
../cmd/cmd_checkout.cc \
../cmd/cmd_cleanup.cc \
../cmd/cmd_commit.cc \
../cmd/cmd_diff.cc \
../cmd/cmd_filelog.cc \
../cmd/cmd_findheads.cc \
../cmd/cmd_fsck.cc \
../cmd/cmd_gc.cc \
../cmd/cmd_graft.cc \
../cmd/cmd_init.cc \
../cmd/cmd_list.cc \
../cmd/cmd_listkeys.cc \
../cmd/cmd_log.cc \
../cmd/cmd_merge.cc \
../cmd/cmd_pull.cc \
../cmd/cmd_purgesnapshot.cc \
../cmd/cmd_rebuildindex.cc \
../cmd/cmd_rebuildrefs.cc \
../cmd/cmd_remote.cc \
../cmd/cmd_removefs.cc \
../cmd/cmd_removekey.cc \
../cmd/cmd_replicate.cc \
../cmd/cmd_setkey.cc \
../cmd/cmd_show.cc \
../cmd/cmd_snapshot.cc \
../cmd/cmd_snapshots.cc \
../cmd/cmd_status.cc \
../cmd/cmd_tip.cc \
../cmd/cmd_varlink.cc 

OBJS += \
./cmd/cmd_addkey.o \
./cmd/cmd_branch.o \
./cmd/cmd_branches.o \
./cmd/cmd_checkout.o \
./cmd/cmd_cleanup.o \
./cmd/cmd_commit.o \
./cmd/cmd_diff.o \
./cmd/cmd_filelog.o \
./cmd/cmd_findheads.o \
./cmd/cmd_fsck.o \
./cmd/cmd_gc.o \
./cmd/cmd_graft.o \
./cmd/cmd_init.o \
./cmd/cmd_list.o \
./cmd/cmd_listkeys.o \
./cmd/cmd_log.o \
./cmd/cmd_merge.o \
./cmd/cmd_newfs.o \
./cmd/cmd_pull.o \
./cmd/cmd_purgesnapshot.o \
./cmd/cmd_rebuildindex.o \
./cmd/cmd_rebuildrefs.o \
./cmd/cmd_remote.o \
./cmd/cmd_removefs.o \
./cmd/cmd_removekey.o \
./cmd/cmd_replicate.o \
./cmd/cmd_setkey.o \
./cmd/cmd_show.o \
./cmd/cmd_snapshot.o \
./cmd/cmd_snapshots.o \
./cmd/cmd_status.o \
./cmd/cmd_tip.o \
./cmd/cmd_varlink.o \
./cmd/fuse_cmd.o \
./cmd/main.o \
./cmd/server.o 

CC_DEPS += \
./cmd/cmd_addkey.d \
./cmd/cmd_branch.d \
./cmd/cmd_branches.d \
./cmd/cmd_checkout.d \
./cmd/cmd_cleanup.d \
./cmd/cmd_commit.d \
./cmd/cmd_diff.d \
./cmd/cmd_filelog.d \
./cmd/cmd_findheads.d \
./cmd/cmd_fsck.d \
./cmd/cmd_gc.d \
./cmd/cmd_graft.d \
./cmd/cmd_init.d \
./cmd/cmd_list.d \
./cmd/cmd_listkeys.d \
./cmd/cmd_log.d \
./cmd/cmd_merge.d \
./cmd/cmd_pull.d \
./cmd/cmd_purgesnapshot.d \
./cmd/cmd_rebuildindex.d \
./cmd/cmd_rebuildrefs.d \
./cmd/cmd_remote.d \
./cmd/cmd_removefs.d \
./cmd/cmd_removekey.d \
./cmd/cmd_replicate.d \
./cmd/cmd_setkey.d \
./cmd/cmd_show.d \
./cmd/cmd_snapshot.d \
./cmd/cmd_snapshots.d \
./cmd/cmd_status.d \
./cmd/cmd_tip.d \
./cmd/cmd_varlink.d 

CPP_DEPS += \
./cmd/cmd_newfs.d \
./cmd/fuse_cmd.d \
./cmd/main.d \
./cmd/server.d 


# Each subdirectory must supply rules for building sources it contributes
cmd/%.o: ../cmd/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/Users/eddieaili/Documents/workspace/dfs -I/opt/local/include -I/usr/local/include/osxfuse -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -D_FILE_OFFSET_BITS=64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

cmd/%.o: ../cmd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/Users/eddieaili/Documents/workspace/dfs -I/opt/local/include -I/usr/local/include/osxfuse -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -D_FILE_OFFSET_BITS=64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


