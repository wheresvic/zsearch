#!/usr/bin/python

import os, shutil
from zsearch import zsearch

def printText(txt):
    lines = txt.split('\n')
    for line in lines:
        print line.strip()


z = zsearch("http://localhost:8080")
print z

d = z.getDocument(1)

for key, value in d.iteritems():
	print key.encode('utf-8')
	print value.encode('utf-8')

data = {'input' : 'snoop < dawg', 'empty' : ''}

z.addDocument(data)

d = z.getDocument(-1)

