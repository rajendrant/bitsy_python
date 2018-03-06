#!/bin/bash

SRC=( bitsy_alloc.cpp BlockStack.cpp ByteStack.cpp BitString.cpp \
FunctionStack.cpp  ExecStack.cpp \
Program.cpp instructions.cpp variable.cpp bitsy_python_vm.cpp \
BitsyHeap.cpp gc.cpp Builtins.cpp \
datatypes/datatype.cpp datatypes/iter.cpp)

BUILD="$( cd "$(dirname "$0")" ; pwd -P )"
WORKSPACEDIR="$( cd "$(dirname "$BUILD")" ; pwd -P )"
SRCDIR=$WORKSPACEDIR/bitsy_python_arduino

OPT="-std=c++11 -Wall -Wno-array-bounds -O3 -I$SRCDIR"
LIBS=

# Build the executable
for var in "${SRC[@]}"
do
#g++ -c $OPT $SRCDIR/${var}
LIBS="$LIBS $SRCDIR/${var}"
done

g++ $OPT $LIBS $SRCDIR/main.cpp -o $BUILD/bytecoderunner.out
