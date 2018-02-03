#!/usr/bin/env python
import argparse
import serial
import struct
import sys
import time

parser = argparse.ArgumentParser()
parser.add_argument("-d", "--dev", help="serial device id ", default='/dev/USB0')
parser.add_argument("bitsy_file", help="Bitsy python bytecode file to upload")
args = parser.parse_args()

ser = serial.Serial(args.dev, 115200, timeout=2)

def read_until(ser, strr):
    t = time.time()
    while t+4 >= time.time():
        s = ser.readline()
        print s
        if s==strr:
            return True
    else:
        return False

def send_bitsy_program(ser, prog):
    ser.write('\r\nBEGIN\r\n')
    if not read_until(ser, 'READY\r\n'):
        return
    total = (len(prog)-1)//30+1
    ser.write('LEN %d\r\n'%(total))
    for pos in range(0, total):
        ser.write(struct.pack('!30sB', prog[pos*30 : (pos+1)*30], pos))

    if read_until(ser, 'RESET\r\n'):
        print 'Successfully wrote'

time.sleep(2) # wait for arduino to initialize
send_bitsy_program(ser, open(args.bitsy_file).read())
