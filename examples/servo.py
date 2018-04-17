from userlibs import arduino
from userlibs import esp8266wifitcp
from userlibs import servo

def on_wifi_packet_recv(pkt):
    esp8266wifitcp.send_response(pkt)

def delay(secs):
    for i in range(secs):
        arduino.delay(1000)

def humidifier_control(on):
    servo.attach(13)
    delay(1)
    for _ in range(1 if on else 4):
        servo.write(13, 90)
        delay(1)
        servo.write(13, 0)
        delay(1)
    arduino.pinMode(13, 1) # 1=OUTPUT, 0=INPUT, 2=INPUT_PULLUP
    #servo.detach(13)
    delay(1)
        
def main():
    esp8266wifitcp.set_on_recv_callback(on_wifi_packet_recv)
    ip = esp8266wifitcp.local_ip()
    print 'IP address', ip&0xFF, ((ip>>8)&0xFF), ((ip>>16)&0xFF), ((ip>>24)&0xFF)
    servo.attach(13)
    servo.write(13, 0)
    delay(1)
    while True:
        humidifier_control(True)
        delay(10*60)
        humidifier_control(False)
        delay(20*60)
        print 'loop'
