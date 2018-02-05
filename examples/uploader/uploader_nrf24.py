#!/usr/bin/env python
import argparse
import socket
import struct
import sys
import time

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--server", help="UDP server IP", default='192.168.0.30')
parser.add_argument("-p", "--port", help="UDP server port", type=int, default=6666)
parser.add_argument("-n", "--nrf24", help="Nrf24 device ID", type=int, default=0)
parser.add_argument("bitsy_file", help="Bitsy python bytecode file to upload")
args = parser.parse_args()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
address = (args.server, args.port)
prog = open(args.bitsy_file).read()
sock.settimeout(10)

def upload_program():
    try:
        total = (len(prog)-1)//30+1
        print total, bytearray([args.nrf24, total]+range(11, 122, 11))
        sock.sendto(bytearray([args.nrf24, total]+range(11, 122, 11)), address)
        while True:
            resp, _ = sock.recvfrom(1024)
            pos = ord(resp[0])
            print pos
            if pos>=0 and pos<total:
                time.sleep(0.02)
                sock.sendto(struct.pack('!B30sB', args.nrf24, prog[pos*30 : (pos+1)*30], pos), address)
            if pos == total-1: break
    except IOError as e:
        print 'Error occurred', e
        return False
    return True

if upload_program():
    print 'Uploaded Successfully'
