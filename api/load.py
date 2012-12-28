#!/usr/bin/python

import codecs
import httplib, urllib
import os, shutil

from xml.sax.saxutils import escape

def printText(txt):
    lines = txt.split('\n')
    for line in lines:
        print line.strip()


inputs = list()

httpServ = httplib.HTTPConnection("localhost", 8080)
httpServ.connect()

path = '../data/load/'

listing = os.listdir(path)

for file in listing:

	filename = os.path.join(path, file)

	if (os.path.isfile(filename)):

		#if (filename != '../data/load/document_42772.txt'):
		#	continue

		print filename
		# f = codecs.open(filename, "r", "utf-8")
		linestring = open(filename, "r").read()

		print linestring

		# params = urllib.urlencode({'data': escape(linestring)})
		params = urllib.urlencode({'data' : linestring})
		params = params.encode('utf-8')
		# params = {'data' : linestring}
		httpServ.request('POST', '/index', params)
		# inputs.append(linestring)

		response = httpServ.getresponse()

		# if response.status == httplib.OK:
		print "Output from POST request"
		printText (response.read())


# print "loaded " + str(len(inputs)) + " files"

httpServ.close()

