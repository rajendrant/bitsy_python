def f():
    print s

def f2():
    global s
    s = "Me too."
    print s

# Global scope
s=None

def main():
  global s
  s = "I love Geeksforgeeks"
  f()
  f2()
  print s
