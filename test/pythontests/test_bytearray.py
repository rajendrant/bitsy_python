def main():
    s=bytearray(1)
    s[0]='A'
    print s
    #iterate()
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
    s[3]='d'
    print s
    s[0]='A'
    s[1]='B'
    s[2]='C'
    s[3]='D'
    print s

def length():
    s=bytearray(4)
    print len(s)