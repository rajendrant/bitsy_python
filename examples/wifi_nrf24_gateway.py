from userlibs import arduino
from userlibs import esp8266wifiudp
from userlibs import nrf24

def on_nrf24_packet_recv(pkt):
    start=arduino.millis()
    print 'nrf24', len(pkt)
    esp8266wifiudp.send_response(pkt, len(pkt))
    print 'took', arduino.millis()-start

def on_wifi_packet_recv(pkt):
    start=arduino.millis()
    l = len(pkt)-1
    print 'wifi', l
    if l<=0: return
    send = bytearray(l)
    for i in range(l):
        send[i] = pkt[i+1]
    print 'took', arduino.millis()-start
    nrf24.send(pkt[0], send, l)
    print 'took', arduino.millis()-start

def main():
    nrf24.set_on_recv_callback(on_nrf24_packet_recv)
    esp8266wifiudp.set_on_recv_callback(on_wifi_packet_recv)
    arduino.delay(2000)
    ip = esp8266wifiudp.local_ip()
    print 'IP address', ip&0xFF, ((ip>>8)&0xFF), ((ip>>16)&0xFF), ((ip>>24)&0xFF)
    while True:
        arduino.delay(100)
