#!/usr/bin/env python
import argparse
import socket
import struct
import sys
import time

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--server", help="TCP server IP", default='192.168.0.30')
parser.add_argument("-p", "--port", help="TCP server port", type=int, default=6666)
parser.add_argument("bitsy_file", help="Bitsy python bytecode file to upload")
args = parser.parse_args()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((args.server, args.port))
sock.settimeout(2)
prog = open(args.bitsy_file).read()

def recv_pkt():
    l = sock.recv(1)
    return sock.recv(ord(l[0]))

def upload_program():
    try:
        total = (len(prog)-1)//30+1
        started = False
        while True:
            if not started:
                sock.send(bytearray([12, total]+range(11, 122, 11)))
                time.sleep(0.3)
            resp = recv_pkt()
            pos = ord(resp[0])
            if pos>=0 and pos<total:
                part = prog[pos*30 : (pos+1)*30]
                sock.send(chr(len(part)+1) + part + chr(pos))
                started=True
            elif pos==total:
                sock.send(bytearray([1, total]))
                return True
    except IOError as e:
        print 'Error occurred', e
    return False

if upload_program():
    print 'Uploaded Successfully'
