from userlibs import arduino
from userlibs import nrf24
from userlibs import lowpower

def on_packet_recv(pkt):
    print 'nrf24', len(pkt), pkt

def main():
    nrf24.set_on_recv_callback(on_packet_recv)
    print 123456
    while True:
        print 123
        lowpower.powerdown(5000)
        arduino.delay(2000)
