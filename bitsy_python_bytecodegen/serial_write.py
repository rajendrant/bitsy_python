import serial
import sys
import time

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=2)

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

    ser.write('LEN %d\r\n'%(len(prog)))
    ser.write(prog)
    ser.write('END\r\n')
    read_until(ser, 'RESET\r\n')

time.sleep(2) # wait for arduino to initialize
send_bitsy_program(ser, open(sys.argv[1]).read())
