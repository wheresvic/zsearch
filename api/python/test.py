#!/usr/bin/python
# -*- coding: utf-8 -*-

import os, shutil
from zsearch import zsearch

z = zsearch("http://localhost:8080")
print z

try:

	print "<test.py> get document id 1"
	d = z.getDocument(1)

	print "<test.py> print document contents"
	for key, value in d.iteritems():
		print key
		print value

except Exception as err:
	print err

data = {'input' : 'snoop < dawg', 'complex' : '72.8/km²'}
print "<test.py> add document", data
docId = z.addDocument(data)
print "<test.py> returned docId:" + docId

print "<test.py> retrieve inserted document"
d = z.getDocument(docId)

print "<test.py> print document contents"
for key, value in d.iteritems():
	print key
	print value

print "<test.py> search for 'some'"
docIds = z.search("some")

print "<test.py> returned docIds:"
for docId in docIds:
	print docId

try:
	print "<test.py> try getting invalid documentId (-1)"
	d = z.getDocument(-1)
except Exception as err:
	print err
