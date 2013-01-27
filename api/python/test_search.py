#!/usr/bin/python
# -*- coding: utf-8 -*-

import os, shutil
from zsearch import zsearch

z = zsearch("http://localhost:8080")
print z

print "<test.py> search for 'some'"
docIds = z.search("some")

print "<test.py> returned docIds:"
for docId in docIds:
	print docId

print "<test.py> search for 'some' with start and offset"
docIds = z.search("some", 0, 3)

print "<test.py> returned docIds:"
for docId in docIds:
	print docId

print "<test.py> bad start"
try:
	z.search("some", -1, 5)
except Exception, e:
	print e


print "<test.py> bad offset"
try:
	z.search("some", 0, 'snoop')
except Exception, e:
	print e

print "<test.py search for 'snoop'"
docIds = z.search("snoop")
for docId in docIds:
	print docId
