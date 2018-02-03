from userlibs import arduino
from userlibs import nrf24

def on_packet_recv(pkt):
    print 'nrf24', len(pkt), pkt

def main():
    nrf24.set_on_recv_callback(on_packet_recv)
    print 123456
    while True:
        print 123
        nrf24.recv_until(2000)
