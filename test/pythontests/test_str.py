def main():
    s='123'
    print s
    iterate()
    indexing()
    length()

def iterate():
    s='abc'
    for ch in s:
        print ch

def indexing():
    s='abc'
    print s[0]
    print s[1]
    print s[2]
    print s

def length():
    s='abcd'
    print len(s)
