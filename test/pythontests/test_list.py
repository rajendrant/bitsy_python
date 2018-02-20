def main():
    test1()
    indexing()
    length()
    listoflist()
    slicing()
    append()
    fromrange()

def test1():
    a=[1,2,3]
    print a

def indexing():
    l=[1,2,3]
    print l[0]
    print l[1]
    print l[2]
    l[0]=4
    l[1]=5
    l[2]=6
    print l

def length():
    l=[1,2,3]
    print len(l)

def listoflist():
  l=[[1,2], [3,4], [5, [6,7]]]
  print l

def slicing():
  l=[1, 2, 3, 4, 5, 6, 7, 8]
  print l
  print l[1:4]
  print l[2:]
  print l[:5]
  #print l[1:4:2]
  #print l[::-1]  # reverse
  print l[:]
  print l[-5:-2]
  print l[-2:]
  print l[:-3]
  #print l[-6:-1:2]

def append():
  a=[1,2,3]
  a+=[4]
  print a
  a=a+[5,6]
  print a
  print a+[7,8]

def fromrange():
  l=list(range(10))
  print l
