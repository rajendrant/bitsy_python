def main():
    foo()
    echo(1)
    add(1,2)
    print multiply(2,4)
    print pi()
    print divide(22.0, 16)
    print factorial(5)
    print factorial(10)
    print fib(10)
    print fib(15)

def foo():
    print 1,2,3

def echo(a):
    print a

def add(a,b):
    print a+b

def multiply(a,b):
    print a,b
    return a*b

def pi():
    return 3.14285

def divide(a, b):
    return a/b

def factorial(n):
    if n == 0:
        return 1
    else:
        return n * factorial(n-1)

def fib(n):
    if n<2:
        return n
    return fib(n-1)+fib(n-2)