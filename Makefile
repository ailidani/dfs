
#CC=/opt/local/bin/g++-mp-4.8
#C=/opt/local/bin/gcc-mp-4.8
CC=g++
C=gcc
CPP_FLAGS=-std=c++0x -D_FILE_OFFSET_BITS=64 -O0 -DDEBUG
C_FLAGS=-O0 -DDEBUG

LDFLAGS=-L/usr/local/lib/ -L/opt/local/lib/ -lz -levent -losxfuse -lssl -lresolv -lcrypto -llzma -lboost_iostreams-mt -lboost_serialization-mt -lboost_system-mt

INCLUDE=-I"/Users/eddieaili/Documents/workspace/dfs" -I/opt/local/include -I/usr/local/include/osxfuse

CPP_SOURCES=libdfsutil/*.cpp libdfs/*.cpp dfs/mds/*.cpp dfs/mds/*.hpp
CPP_OBJECTS=$(CPP_SOURCES:.cpp=.o)
C_SOURCES=libfastlz/fastlz.c libdiffmerge/*.c
C_OBJECTS=$(C_SOURCES:.c=.o)
EXECUTABLE=libs/libdfs.dylib

all:
	$(CC) $(INCLUDE) $(CPP_FLAGS) -c $(CPP_SOURCES)
	$(C) $(INCLUDE) $(C_FLAGS) -c $(C_SOURCES)
	#$(CC) $(LDFLAGS) *.o -dynamiclib -o libdfs.dylib
	ar rcs libdfs.a *.o
	rm *.o
	#cp libdfs.dylib bin/
	#$(CC) $(LDFLAGS) -L. -ldfs $(INCLUDE) $(CPP_FLAGS) test/test_dfsutil.cpp -o bin/test_dfsutil
	#$(CC) $(LDFLAGS) -L. -ldfs $(INCLUDE) $(CPP_FLAGS) httpd/main.cpp -o bin/httpd
	#$(CC) $(LDFLAGS) -L. -ldfs $(INCLUDE) $(CPP_FLAGS) cmd/*.cpp -o bin/cmd
	#$(CC) $(LDFLAGS) -L. -ldfs $(INCLUDE) $(CPP_FLAGS) dfs/*.cpp -o bin/dfs
	#$(CC) $(LDFLAGS) -L. -ldfs $(INCLUDE) $(CPP_FLAGS) dfssync/*.cpp -o bin/dfssync
	#$(CC) $(LDFLAGS) -L. -ldfs $(INCLUDE) $(CPP_FLAGS) mds/*.cpp -o bin/mds
	$(CC) $(LDFLAGS) $(INCLUDE) $(CPP_FLAGS) libdfs.a test/test_dfsutil.cpp -o bin/test_dfsutil
	$(CC) $(LDFLAGS) $(INCLUDE) $(CPP_FLAGS) libdfs.a httpd/main.cpp -o bin/httpd
	$(CC) $(LDFLAGS) $(INCLUDE) $(CPP_FLAGS) libdfs.a cmd/*.cpp -o bin/cmd
	$(CC) $(LDFLAGS) $(INCLUDE) $(CPP_FLAGS) libdfs.a dfs/*.cpp -o bin/dfs
	$(CC) $(LDFLAGS) $(INCLUDE) $(CPP_FLAGS) libdfs.a dfssync/*.cpp -o bin/dfssync
	$(CC) $(LDFLAGS) $(INCLUDE) $(CPP_FLAGS) libdfs.a mds/*.cpp -o bin/mds
	$(CC) $(LDFLAGS) $(INCLUDE) $(CPP_FLAGS) libdfs.a dfs/logging.cpp dfs/oricmd.cpp dfs/oripriv.cpp dfs/server.cpp test/udp_test.cpp -o bin/udp_test

mds:
	$(CC) $(INCLUDE) $(CPP_FLAGS) -c dfs/mds/*.cpp dfs/mds/*.hpp
	$(C) $(INCLUDE) $(C_FLAGS) -c $(C_SOURCES)
	ar rcs libdfs.a *.o
	rm *.o
	$(CC) $(LDFLAGS) $(INCLUDE) $(CPP_FLAGS) libdfs.a dfs/*.cpp -o bin/dfs

##==========================================================================
clean:
	@-	rm libs/libdfs.dylib
	@-	rm bin/*
	@-	echo "Data Cleansing Done. Ready to Compile"
