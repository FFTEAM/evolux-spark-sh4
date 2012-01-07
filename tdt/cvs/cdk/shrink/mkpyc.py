#!/usr/bin/python

import sys
import py_compile

if len(sys.argv) < 2:
	print 'Usage: mkpyc.py filename.py'
	sys.exit()

#print "Kompiliere: " + sys.argv[1]
py_compile.compile(sys.argv[1])
