def main():
    a = 100
    print 0 if a==100 else 1
    test1()

def test1():
    x = 5
    y = 10
    if x > y:
        print 0, x, y
    if y < x:
        print 1, x, y
    test2()

def test2():
    x = 5
    y = 8
    if x >= 55:
        print 2, x, y
    else:
        print 3, x, y
    test3()

def test3():
    x = 5
    y = 10
    z = 22
    if x > y:
        print 4, x, y, z
    elif x <= z:
        print 5, x, y, z
    else:
        print 6, x, y, z
