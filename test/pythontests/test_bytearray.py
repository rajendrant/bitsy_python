def main():
    s=bytearray(1)
    s[0]='A'
    print s
    iterate()
    indexing()
    length()
    a,b = unpack()
    print a,b
    search()
    copy()

def iterate():
    s=bytearray(4)
    s[0]='1'
    s[1]='2'
    s[2]='3'
    s[3]='4'
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

def unpack():
    s=bytearray(2)
    s[0] = 'A'
    s[1] = 'B'
    return s

def search():
    s=bytearray(2)
    s[0] = 'A'
    s[1] = 'B'
    if 'A' in s:
        print 'found A'
    if 'B' in s:
        print 'found B'
    if 'A' not in s:
        print 'not found A'
    if 'b' not in s:
        print 'not found b'

def do_copy(s):
    l=len(s)-1
    c=bytearray(l)
    for i in range(l):
      c[i]=s[i+1]
    print c

def copy():
    s=bytearray(4)
    s[0] = 'a'
    s[1] = 'b'
    s[2] = 'c'
    s[3] = 'd'
    do_copy(s)