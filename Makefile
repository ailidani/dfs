
CC=g++
C=gcc
CPP_FLAGS=-Wall -std=c++0x -D_FILE_OFFSET_BITS=64
C_FLAGS=-Wall

LDFLAGS=-L/usr/local/lib/ -L/opt/local/lib/ -lz -levent -losxfuse -lssl -lresolv -lcrypto -llzma

INCLUDE=-I"/Users/eddieaili/Documents/workspace/dfs" -I/opt/local/include -I/usr/local/include/osxfuse

CPP_SOURCES=libdfsutil/*.cpp libdfs/*.cpp
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



##==========================================================================
clean:
	@-	rm libs/libdfs.dylib
	@-	echo "Data Cleansing Done. Ready to Compile"
