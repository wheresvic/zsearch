#!/usr/bin/python
# -*- coding: utf-8 -*-

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
	print key
	print value

data = {'input' : 'snoop < dawg', 'complex' : '72.8/km²'}

docId = z.addDocument(data)

print docId

d = z.getDocument(docId)

for key, value in d.iteritems():
	print key
	print value


d = z.getDocument(-1)


