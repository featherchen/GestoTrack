from time import sleep
import RPi.GPIO as GPIO
import socket

LEDPin = 17
BUZZPin = 27

GPIO.setmode(GPIO.BCM)
GPIO.setup(LEDPin, GPIO.OUT)
GPIO.output(LEDPin, GPIO.LOW)
GPIO.setup(BUZZPin, GPIO.OUT)
GPIO.output(BUZZPin, GPIO.LOW)

HOST = '192.168.50.114'
PORT = 8080

LIGHT_STATE = False

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print('Connect by: ', addr)
        try:
            while True:
                data = conn.recv(1024).decode('utf-8').strip()
                if data:
                    print('Receive: ', data)
                    if data[0] == '0':
                        GPIO.output(LEDPin, GPIO.LOW)
                    elif data[0] == '1':
                        GPIO.output(LEDPin, GPIO.HIGH)
                    else:
                        print("Unknown first bit: " + data[0])

                    if len(data) > 1:
                        if data[1] == '1':
                            GPIO.output(BUZZPin, GPIO.HIGH)
                            sleep(1)
                            GPIO.output(BUZZPin, GPIO.LOW)
                        elif data[1] != '0':
                            print("Unknown second bit: " + data)
                        else:
                            for i in range(10):
                                GPIO.output(LEDPin, GPIO.HIGH)
                                sleep(0.1)
                                GPIO.output(LEDPin, GPIO.LOW)
                                sleep(0.1)

                    else:
                        print("Missing second bit")
        except KeyboardInterrupt:
            GPIO.output(LEDPin, GPIO.LOW)
            GPIO.output(BUZZPin, GPIO.LOW)
            GPIO.cleanup()
            conn.close()
