#!/bin/bash
TESTS=( test1 test2 test3 test4 test5 test6 test_bitwise \
test_function1 test_condition1 test_loop1 test_import \
test_str test_list test_bytearray
test_looprange test_global )
TESTS=(test_global test_list)

PYTHONTESTDIR="$( cd "$(dirname "$0")" ; pwd -P )"
WORKSPACEDIR="$( cd "$(dirname "$PYTHONTESTDIR")/.." ; pwd -P )"
BUILD=$WORKSPACEDIR/build
BYTECODEGENDIR=$WORKSPACEDIR/bitsy_python_bytecodegen

export PYTHONPATH=$PYTHONPATH:.:$BYTECODEGENDIR

$BUILD/make.sh

for var in "${TESTS[@]}"
do
printf "\n\nTEST %s\n" "${var}"

cd $PYTHONTESTDIR
python main_helper.py ${var} > expected.txt

python $BYTECODEGENDIR/main.py ${var} ${var}.bitsy
$BUILD/bytecoderunner.out ${var}.bitsy > actual.txt

diff -Z expected.txt actual.txt | head -30

if [ $? -ne 0 ]; then
printf "TEST FAILED"
else
#rm ${var}.pyc ${var}.bitsy actual.txt expected.txt
cat actual.txt # expected.txt
fi
cd ~-

done
