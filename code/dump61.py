#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys, re, serial, time
s = serial.Serial(sys.argv[1],timeout=10, baudrate=115200)
for i in range(69):
  s.write(b'cmd 50\r')
  s.flush()
  time.sleep(10)
  v=s.readline()
  print(',', len(v), v)