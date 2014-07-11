#!/usr/bin/env python
import serial
import random
import time

def usage():
  msg = "\n".join([
    'Usage: %s <device>' % sys.argv[0],
    '',
    " device:  the *duino serial port",
    "          Ex: COM3 (Windows) or /dev/tty.usbserial-AM01QKUP (Unix, Mac)",
    ''
  ])
  sys.exit(msg)

if len(sys.argv) < 2:
  usage()

ser = serial.Serial(sys.argv[1], 9600, 8, parity=serial.PARITY_NONE, timeout=1, rtscts=1, dsrdtr=1)

while True:
  value = random.randint(0,100)
  transition_time = random.randint(200,1000)
  print("value: %s, transition time: %s" % (value, transition_time))
  ser.write("%s,%s\n" % (value, transition_time))
  time.sleep(float(transition_time)/1000)
  print(ser.readline()),
