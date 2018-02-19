def f():
    print s

def f2():
    global s
    s = "Me too."
    print s

# Global scope
s = "I love Geeksforgeeks"
def main():
  f()
  f2()
  print s
