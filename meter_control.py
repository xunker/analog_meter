#!/usr/bin/env python
#
# Use in conjunction with analog_meter.ino sketch to move the needle of a
# PWM-connected gauge.
#
# NOTE: This code assumes the baud-rate in the Arduino sketch is set at
# 9600. If you have changed that value in the sketch, change it here too:

CONNECTION_SPEED = 9600

# This script also assumes 8 data bits with with no parity, XON/XOFF is off
# and both CTS/RTS and DTR/DSR are on, which should work in most cases.

import serial
import sys
import re
import math
import time

def usage():
  msg = "\n".join([
    'Usage: %s <device> <position> (<time>)' % sys.argv[0],
    '',
    " device:  the *duino serial port",
    "          Ex: COM3 (Windows) or /dev/tty.usbserial-AM01QKUP (Unix, Mac)",
    " positon: the needle position betweeen 0 and 100",
    " time:    (optional) time to move the needle, 0-9999 milliseconds, default 0",
    ''
  ])
  sys.exit(msg)

if len(sys.argv) < 3:
  usage()

value = re.sub(r"[^\d]", '', sys.argv[2])
if (len(value) < 1) or (len(value) > 3): usage()
value = int(value)
if (value > 100) or (value < 0): usage()

try:
  transition_time = re.sub(r"[^\d]", '', sys.argv[3])
  if (len(transition_time) < 1) or (len(transition_time) > 4): usage()
  transition_time = int(transition_time)
  if (transition_time > 9999) or (transition_time < 0): usage()
except IndexError:
  transition_time = 0

# create serial connection
ser = serial.Serial(sys.argv[1], CONNECTION_SPEED, 8, parity=serial.PARITY_NONE, timeout=1, rtscts=1, dsrdtr=1)

# send the command
ser.write("%s,%s\n" % (value, transition_time))

# Calculate how long we need to wait for a response.
# It may take at least transition_time milliseconds, but it may return sooner
# if the previous position was the same. Here we poll for a response with a
# one second timeout to see if we get an ACK early so we don't have to wait
# transition_time milliseconds if there is no transition going on.
response_wait_time = 1
if transition_time > 0:
  response_wait_time = int(math.ceil((float(transition_time) / 1000.0)))+1

command_sent = int(time.time())
last_response = ""
while (not last_response) and (int(time.time()) < (command_sent + response_wait_time)):
  for response in iter(ser.readline, ""):
    response.strip()
    if len(response) > 0:
      last_response = response
      continue

if last_response:
  ack = re.match(r"^ACK\.", last_response)
  if ack:
    print "SUCCESS:",
  else:
    print "ERROR:",
  print(response),
else:
  print 'ERROR: Did not receive response within expected %s second timeframe.' % response_wait_time
  