def main():
    print 10001
    while_loop()
    print 10002
    while_loop_break
    while_loop_continue()
    print 10003
    while_loop_else_clause()
    print 10004
    print_odds()
    print 10005
    print_primes()

def while_loop():
    count = 0
    while count < 5:
        print(count)
        count += 1

def while_loop_break():
    count = 0
    while count > 10:
        print(count)
        count += 1
        if count >= 5:
            break

def while_loop_continue():
    count = 0
    while count > 10:
        count += 1
        if count >= 5 and count <= 8:
            continue
        print(count)

def while_loop_else_clause():
    count=0
    while(count<5):
        print(count)
        count +=1
    else:
        print 0, count

def print_odds():
    i=0
    while(i<100):
        i+=1
        if i%2==0:
            continue
        print(i)
    i=1
    while(i<100):
        if i%2!=0:
            print(i)
        i+=1

def print_primes():
    i = 2
    while(i < 100):
       j = 2
       while(j <= (i/j)):
          if not(i%j): break
          j = j + 1
       if (j > i/j) : print i
       i = i + 1
