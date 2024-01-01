import Jetson.GPIO as GPIO
import time

pin = 18

GPIO.setmode(GPIO.BCM)
GPIO.setup(pin, GPIO.OUT)

def send_pulse():
    GPIO.output(pin, GPIO.HIGH)
    time.sleep(1)
    GPIO.output(pin, GPIO.LOW)

while True:
    time.sleep(2)
    print('run')
    send_pulse()
    time.sleep(0.1)
