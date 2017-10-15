#!/bin/bash
SRC=( bitsy_alloc.cpp BlockStack.cpp ByteStack.cpp BitStack.cpp BitString.cpp \
FunctionStack.cpp  ExecStack.cpp \
Program.cpp instructions.cpp variable.cpp bitsy_python_vm.cpp )
OPT="-std=c++11 -Wall -g -I../bitsy_python_arduino/"
BUILD=../build

LIBS=
for var in "${SRC[@]}"
do
LIBS="$LIBS ../bitsy_python_arduino/${var}"
done

g++ $OPT $LIBS ../bitsy_python_arduino/main.cpp -o $BUILD/bytecoderunner.out


g++ $OPT $LIBS BlockStack_test.cpp -o $BUILD/BlockStack_test.out
$BUILD/BlockStack_test.out
g++ $OPT $LIBS ByteStack_test.cpp -o $BUILD/ByteStack_test.out
$BUILD/ByteStack_test.out
g++ $OPT $LIBS BitStack_test.cpp -o $BUILD/BitStack_test.out
$BUILD/BitStack_test.out

g++ $OPT $LIBS FunctionStack_test.cpp -o $BUILD/FunctionStack_test.out
$BUILD/FunctionStack_test.out
g++ $OPT $LIBS ExecStack_test.cpp -o $BUILD/ExecStack_test.out
$BUILD/ExecStack_test.out

g++ $OPT $LIBS BitString_test.cpp -o $BUILD/BitString_test.out
$BUILD/BitString_test.out

g++ $OPT $LIBS Program_test.cpp -o $BUILD/Program_test.out
$BUILD/Program_test.out
