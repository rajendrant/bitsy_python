#!/bin/bash

SRC=( bitsy_alloc.cpp BlockStack.cpp ByteStack.cpp BitStack.cpp BitString.cpp \
FunctionStack.cpp  ExecStack.cpp \
Program.cpp instructions.cpp variable.cpp )

UNITTESTS=( BlockStack_test ByteStack_test BitStack_test \
FunctionStack_test ExecStack_test BitString_test Program_test )

UNITTESTDIR="$( cd "$(dirname "$0")" ; pwd -P )"
WORKSPACEDIR="$( cd "$(dirname "$UNITTESTDIR")/.." ; pwd -P )"
BUILD=$WORKSPACEDIR/build
SRCDIR=$WORKSPACEDIR/bitsy_python_arduino

OPT="-std=c++11 -Wall -Wno-array-bounds -O3 -I$SRCDIR"
LIBS=

# Build the executable
$BUILD/make.sh

for var in "${SRC[@]}"
do
LIBS="$LIBS $SRCDIR/${var}"
done

# Build and run the unittests
for var in "${UNITTESTS[@]}"
do
g++ $OPT $LIBS $UNITTESTDIR/$var.cpp -o $BUILD/$var.out
$BUILD/$var.out
done
