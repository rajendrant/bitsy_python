from userlibs import arduino
from userlibs import esp8266wifiudp
from userlibs import nrf24

def on_nrf24_packet_recv(pkt):
    print 'nrf24 pkts', len(pkt), pkt

def on_packet_recv(pkt):
    print 'wifi pkt', len(pkt), pkt

def main():
    nrf24.set_on_recv_callback(on_nrf24_packet_recv)
    esp8266wifiudp.set_on_recv_callback(on_packet_recv)
    arduino.delay(10000)
    ip = esp8266wifiudp.local_ip()
    print 'IP address', ip&0xFF, ((ip>>8)&0xFF), ((ip>>16)&0xFF), ((ip>>24)&0xFF)
    arduino.delay(10000)
    while True:
        arduino.delay(10)
