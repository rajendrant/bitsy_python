import serial
import struct
import sys
import time

ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=2)

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
    for pos in range(0, total):
        ser.write(struct.pack('!30sBB', prog[pos*30 : (pos+1)*30], pos, total))

    if read_until(ser, 'RESET\r\n'):
        print 'Successfully wrote'

time.sleep(2) # wait for arduino to initialize
send_bitsy_program(ser, open(sys.argv[1]).read())
