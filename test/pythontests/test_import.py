from userlibs import testuserlib

def foo():
    testuserlib.printstr(1, 2)
    testuserlib.println(2, 3)

def math_test():
    print testuserlib.math_add(1, 2, 3)
    print testuserlib.math_add(100, -30)
    print testuserlib.math_add(-100, -30)
    print testuserlib.math_add(-100, 30)
    print testuserlib.math_power(2, 3)
    print testuserlib.math_power(2.5, 3.2)

def main():
    foo()
    math_test()
