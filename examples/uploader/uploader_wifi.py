#!/usr/bin/env python
import argparse
import socket
import struct
import sys

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--server", help="UDP server IP", default='192.168.0.30')
parser.add_argument("-p", "--port", help="UDP server port", type=int, default=6666)
parser.add_argument("bitsy_file", help="Bitsy python bytecode file to upload")
args = parser.parse_args()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
address = (args.server, args.port)
prog = open(args.bitsy_file).read()
sock.settimeout(2)

def upload_program():
    try:
        total = (len(prog)-1)//30+1
        sock.sendto(bytearray([total]+range(11, 122, 11)), address)
        while True:
            resp, _ = sock.recvfrom(1024)
            pos = ord(resp[0])
            if pos>=0 and pos<total:
                sock.sendto(struct.pack('!30sB', prog[pos*30 : (pos+1)*30], pos), address)
            if pos == total-1: break
            if ord(resp[0]) != pos: return False
    except IOError as e:
        print 'Error occurred', e
        return False
    return True

if upload_program():
    print 'Uploaded Successfully'