
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
	$(CC) $(LDFLAGS) *.o -dynamiclib -o libs/libdfs.dylib 
	rm *.o
	$(CC) $(LDFLAGS) -Llibs -ldfs $(INCLUDE) $(CPP_FLAGS) test/test_dfsutil.cpp -o test/test_dfsutil
	$(CC) $(LDFLAGS) -Llibs -ldfs $(INCLUDE) $(CPP_FLAGS) httpd/main.cpp -o httpd/httpd
	$(CC) $(LDFLAGS) -Llibs -ldfs $(INCLUDE) $(CPP_FLAGS) cmd/*.cpp -o cmd/cmd
	$(CC) $(LDFLAGS) -Llibs -ldfs $(INCLUDE) $(CPP_FLAGS) dfs/*.cpp -o dfs/dfs
	


##==========================================================================
clean:
	@-	rm libs/libdfs.dylib
	@-	echo "Data Cleansing Done. Ready to Compile"
