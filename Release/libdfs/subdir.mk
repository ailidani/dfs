################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../libdfs/commit.cpp \
../libdfs/fchunker.cpp \
../libdfs/httpclient.cpp \
../libdfs/httprepo.cpp \
../libdfs/httpserver.cpp \
../libdfs/index.cpp \
../libdfs/largeblob.cpp \
../libdfs/localobject.cpp \
../libdfs/localrepo.cpp \
../libdfs/mergestate.cpp \
../libdfs/metadatalog.cpp \
../libdfs/peer.cpp \
../libdfs/remoterepo.cpp \
../libdfs/repo.cpp \
../libdfs/repostore.cpp \
../libdfs/rkchunker.cpp \
../libdfs/snapshotindex.cpp \
../libdfs/sshclient.cpp \
../libdfs/sshrepo.cpp \
../libdfs/tempdir.cpp \
../libdfs/tree.cpp \
../libdfs/treediff.cpp \
../libdfs/udsclient.cpp \
../libdfs/udsrepo.cpp \
../libdfs/udsserver.cpp \
../libdfs/varlink.cpp 

OBJS += \
./libdfs/commit.o \
./libdfs/fchunker.o \
./libdfs/httpclient.o \
./libdfs/httprepo.o \
./libdfs/httpserver.o \
./libdfs/index.o \
./libdfs/largeblob.o \
./libdfs/localobject.o \
./libdfs/localrepo.o \
./libdfs/mergestate.o \
./libdfs/metadatalog.o \
./libdfs/peer.o \
./libdfs/remoterepo.o \
./libdfs/repo.o \
./libdfs/repostore.o \
./libdfs/rkchunker.o \
./libdfs/snapshotindex.o \
./libdfs/sshclient.o \
./libdfs/sshrepo.o \
./libdfs/tempdir.o \
./libdfs/tree.o \
./libdfs/treediff.o \
./libdfs/udsclient.o \
./libdfs/udsrepo.o \
./libdfs/udsserver.o \
./libdfs/varlink.o 

CPP_DEPS += \
./libdfs/commit.d \
./libdfs/fchunker.d \
./libdfs/httpclient.d \
./libdfs/httprepo.d \
./libdfs/httpserver.d \
./libdfs/index.d \
./libdfs/largeblob.d \
./libdfs/localobject.d \
./libdfs/localrepo.d \
./libdfs/mergestate.d \
./libdfs/metadatalog.d \
./libdfs/peer.d \
./libdfs/remoterepo.d \
./libdfs/repo.d \
./libdfs/repostore.d \
./libdfs/rkchunker.d \
./libdfs/snapshotindex.d \
./libdfs/sshclient.d \
./libdfs/sshrepo.d \
./libdfs/tempdir.d \
./libdfs/tree.d \
./libdfs/treediff.d \
./libdfs/udsclient.d \
./libdfs/udsrepo.d \
./libdfs/udsserver.d \
./libdfs/varlink.d 


# Each subdirectory must supply rules for building sources it contributes
libdfs/%.o: ../libdfs/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/local/include -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


