from userlibs import arduino
from userlibs import esp8266wifitcp
from userlibs import nrf24

def on_nrf24_packet_recv(pkt):
    start=arduino.millis()
    print 'nrf24', len(pkt)
    esp8266wifitcp.send_response(pkt)
    print 'took', arduino.millis()-start

def on_wifi_packet_recv(pkt):
    start=arduino.millis()
    l = len(pkt)-1
    print 'wifi', l
    if l<=0: return
    send = bytearray(l)
    for i in range(l):
        send[i] = pkt[i+1]
    nrf24.send(pkt[0], send)
    print 'took', arduino.millis()-start

def main():
    nrf24.set_on_recv_callback(on_nrf24_packet_recv)
    esp8266wifitcp.set_on_recv_callback(on_wifi_packet_recv)
    arduino.delay(2000)
    ip = esp8266wifitcp.local_ip()
    print 'IP address', ip&0xFF, ((ip>>8)&0xFF), ((ip>>16)&0xFF), ((ip>>24)&0xFF)
    arduino.pinMode(2, 1) # 1=OUTPUT, 0=INPUT, 2=INPUT_PULLUP
    arduino.pinMode(0, 1) # 1=OUTPUT, 0=INPUT, 2=INPUT_PULLUP
    while True:
        arduino.digitalWrite(0, 1)
        arduino.digitalWrite(2, 0)
        arduino.delay(1000)
        arduino.digitalWrite(2, 1)
        arduino.digitalWrite(0, 0)
        arduino.delay(1000)
        print 'loop'
