#!/bin/bash

# Run from the ori root directory
# Use file runtests_config.sh to selectively run tests
# TODO: nothing works if CWD has spaces in path


export ORIG_DIR=`pwd`
#export PATH=$PATH:$ORIG_DIR/
export CMD=$ORIG_DIR/bin/cmd
export HTTPD=$ORIG_DIR/bin/httpd
export DFS_EXE=$ORIG_DIR/bin/dfs
#export ORIDBG_EXE=$ORIG_DIR/build/oridbg/oridbg
# export ORI_TESTS=$ORIG_DIR/ori_tests

# On OS X and other systems this is umount
#export UMOUNT="fusermount -u"
export UMOUNT="umount"

export TEMP_DIR=$ORIG_DIR/tempdir
export SOURCE_FILES=$TEMP_DIR/files
export SOURCE_REPO=$TEMP_DIR/source_repo
export TEST_REPO2=$TEMP_DIR/test_repo2
export TEST_MERGEREPO=$TEMP_DIR/test_merge
export MTPOINT=$TEMP_DIR/mtpoint

export SOURCE_FS=oritest_source
export TEST_FS=oritest_fs
export TEST_FS2=oritest_fs2

export TEST_RESULTS=$ORIG_DIR/test_results.txt

# Check for tempdir
if [ -d $TEMP_DIR ]; then
    echo "Directory $TEMP_DIR already exists,"
    echo "please delete before running tests"
    exit
fi

if [ -d ~/.ori/$SOURCE_FS.ori ]; then
    echo "One or more test file systems have not been deleted!"
    exit
fi

mkdir $TEMP_DIR
mkdir $TEMP_DIR/$TEST_FS
mkdir $TEMP_DIR/$TEST_FS2

mkdir -p $SOURCE_FILES/a
echo "Hello, world!" > $SOURCE_FILES/a/a.txt
touch $SOURCE_FILES/a/empty
mkdir $SOURCE_FILES/a/empty_dir
mkdir $SOURCE_FILES/b
echo "Foo" > $SOURCE_FILES/b/b.txt

#cp $ORIG_DIR/libs $TEMP_DIR


printf "Ori Test Suite\n----------\nRUNNING TESTS\n----------\n"

# Run tests
echo "runtests.sh test results" > $TEST_RESULTS
date >> $TEST_RESULTS
echo >> $TEST_RESULTS

# =================================================

cd $TEMP_DIR
cp $ORIG_DIR/libdfs.dylib ./

# newfs
$CMD newfs $TEST_FS

# mount
$DFS_EXE $TEST_FS

sleep 1

cd $TEST_FS
ls > /dev/null
cd ../

$UMOUNT $TEST_FS

cd ~/.ori/$TEST_FS.ori/
#$ORIDBG_EXE verify
#$ORIDBG_EXE stats
cd $ORIG_DIR

# remove
#$CMD removefs $TEST_FS

# =================================================
# Clone local 

cd $TEMP_DIR
$CMD replicate $SOURCE_FS $TEST_FS

$DFS_EXE $SOURCE_FS
$DFS_EXE $TEST_FS

sleep 1

cd $TEST_FS
#$PYTHON $SCRIPTS/compare.py "$SOURCE_FS" "$TEST_FS"
cd ..

$UMOUNT $SOURCE_FS
$UMOUNT $TEST_FS

cd ~/.ori/$TEST_FS.ori
#$ORIDBG_EXE verify
#$ORIDBG_EXE stats

#bash -e $SCRIPTS/verify_refcounts.sh

cd $TEMP_DIR
$CMD removefs $TEST_FS



# =================================================




# Delete test repo, temp dir
if false; then
    cd $ORIG_DIR
    echo "Deleting directories"
    $CMD removefs $SOURCE_FS
    rm -rf $SOURCE_FILES
    rm -rf $TEMP_DIR
fi
