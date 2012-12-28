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

print "<test.py search for 'snoop'"
docIds = z.search("snoop")
for docId in docIds:
	print docId
