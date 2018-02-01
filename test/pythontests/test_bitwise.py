def main():
    a=503359680
    print a, a&0xFF, a>>8, a>>16, a>>24
    print ((a>>8)&0xFF), ((a>>16)&0xFF), ((a>>24)&0xFF)
