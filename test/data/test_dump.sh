#!/bin/bash
#test_looprange
DATA=( test1 test2 test3 test4 test5 test6 test_function1 test_condition1 test_loop1 test_import )
export PYTHONPATH=$PYTHONPATH:.:../../bitsy_python_bytecodegen/

for var in "${DATA[@]}"
do
printf "\n\nTEST %s\n" "${var}"

python main_helper.py ${var} > expected.txt

python ../../bitsy_python_bytecodegen/main.py ${var} ${var}.bitsy
../../build/bytecoderunner.out ${var}.bitsy > actual.txt

diff -Z expected.txt actual.txt | head -30

if [ $? -ne 0 ]; then
printf "TEST FAILED"
else
rm ${var}.pyc ${var}.bitsy actual.txt expected.txt
fi

done
