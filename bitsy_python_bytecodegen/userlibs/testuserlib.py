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

callback=None
def init_callback(f):
    global callback
    callback=f
    callback(123)

def trigger_callback():
    global callback
    if callback:
        callback(123)
