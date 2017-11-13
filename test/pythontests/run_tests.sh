#!/bin/bash
#test_looprange
TESTS=( test1 test2 test3 test4 test5 test6 \
test_function1 test_condition1 test_loop1 test_import )

PYTHONTESTDIR="$( cd "$(dirname "$0")" ; pwd -P )"
WORKSPACEDIR="$( cd "$(dirname "$PYTHONTESTDIR")/.." ; pwd -P )"
BUILD=$WORKSPACEDIR/build
BYTECODEGENDIR=$WORKSPACEDIR/bitsy_python_bytecodegen

export PYTHONPATH=$PYTHONPATH:.:$BYTECODEGENDIR

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
rm ${var}.pyc ${var}.bitsy actual.txt expected.txt
fi
cd ~-

done
