#!/bin/bash
mkdir test_data 2> /dev/null
# Optional changes in source files to check different cases
# are maded manually, ie:
#
# * SIZE_FREQ/SIZE_TIME in script.py
# * .freq/.time in specgram
#   Changing size of arrays
#
# * UPPERCASE variables in script.py
# * defines in fingerprint.h
#   Changing rules of peaks detecting, sorting etc
echo "PYTHON RUNNING"
python2 script.py
echo "C COMPILING"
gcc sha1/sha1.c kiss_fft/kiss_fft.c specgram.c fingerprint.c -o fing.out -lm
echo "C RUNNING"
./fing.out
rm fing.out
echo "DIFFING RESULTS"
# TEST1 - test of finded peaks
TEST1_DIFF=$(diff test_data/py_test1.txt test_data/c_test1.txt)
if [ "$TEST1_DIFF" ]
  then 
    echo "[ERROR]  TEST 1 FAILED!!! ABORTING SCRIPT"
    echo "Check c_test1.txt and py_test1.txt for detail results"
  else
    echo "TEST 1 IS PASSED"
fi
# TEST2 - test of generated hashes
TEST2_DIFF=$(diff test_data/py_test2.txt test_data/c_test2.txt)
if [ "$TEST2_DIFF" ]
  then 
    echo "[ERROR]  TEST 2 FAILED!!! ABORTING SCRIPT"
    echo "Check c_test2.txt and py_test2.txt for detail results"
  else
    echo "TEST 2 IS PASSED"
fi