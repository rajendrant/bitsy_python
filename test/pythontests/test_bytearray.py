def main():
    s=bytearray(4)
    print s
    iterate()
    indexing()
    length()

def iterate():
    s=bytearray(4)
    for ch in s:
        print ch

def indexing():
    s=bytearray(4)
    s[0]='a'
    s[1]='b'
    s[2]='c'
    print s[0]
    print s[1]
    print s[2]
    s[0]='A'
    s[1]='B'
    s[2]='C'
    print s

def length():
    s=bytearray(4)
    print len(s)
