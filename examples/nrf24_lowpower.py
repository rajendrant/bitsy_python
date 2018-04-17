from userlibs import arduino
from userlibs import nrf24
from userlibs import lowpower
from userlibs import readvcc
from userlibs import servo

def on_packet_recv(pkt):
    print 'nrf24', len(pkt), pkt

def main():
    nrf24.set_on_recv_callback(on_packet_recv)
    print 123456
    a=bytearray(1)
    arduino.pinMode(6, 1)
    #servo.attach(4)
    while True:
        print 123
        nrf24.powerdown()
        lowpower.powerdown(4000)
        nrf24.powerup()
        vcc=readvcc.readvcc()
        a[0]=vcc/100
        print nrf24.send(0xA5, a)
        arduino.digitalWrite(6, 1)
        arduino.delay(5000)
        arduino.digitalWrite(6, 0)
        arduino.delay(5000)
