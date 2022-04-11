#!/usr/bin/env python3

import argparse
import serial  # 导入串口包
import time  # 导入时间包
import sys
import signal


parser = argparse.ArgumentParser(description='serial-reader.')
parser.add_argument("-p", "--port", default="/dev/ttyUSB0",help="serial-port")
parser.add_argument("-b", "--baud", default="115200",help="baud rate")
args = parser.parse_args()

ser = serial.Serial(args.port,args.baud,timeout = 5)
ser.flushInput()

def quit(signum, frame):
    if ser.isOpen():
        ser.close()
    sys.exit(0)

def main():
    while True:
        count = ser.inWaiting()
        if count !=0 :
            recv = ser.read(ser.in_waiting).decode("gbk")
            print(recv)
        time.sleep(0.1)  

if __name__ == '__main__':
    signal.signal(signal.SIGINT, quit)
    main()
