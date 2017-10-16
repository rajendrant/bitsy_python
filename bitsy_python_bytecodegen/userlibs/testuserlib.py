def printstr(*arg):
    for i in arg: print i,

def println(*arg):
    for i in arg: print i,
    print ''

def math_add(*arg):
    res=0
    for i in arg: res+=i
    return res

def math_power(*arg):
    import math
    return math.pow(arg[0], arg[1])
