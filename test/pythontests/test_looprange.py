def main():
    print 10001
    loop_else_clause()
    print 10002
    print_odds()
    print 10003
    print_primes()

def loop_else_clause():
    # Prints out 1,2,3,4
    for i in range(1, 10):
        if(i%5==0):
            break
        print(i)
    else:
        print 1, 0xfa11

def print_odds():
    for x in range(10):
        # Check if x is even
        if x % 2 == 0:
            continue
        print(x)

def print_primes():
    for i in range(3,100):
        is_prime = True
        for j in range(2, i//2):
            if not i%j:
                is_prime = False
        if is_prime:
            print i

"""
# below is not working
def print_primes():
    for i in range(3,12):
        for j in range(2, i//2):
            if not i%j: break
        else:
            print i
"""