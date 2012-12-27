#!/usr/bin/python

import os, shutil
from zsearch import zsearch

def printText(txt):
    lines = txt.split('\n')
    for line in lines:
        print line.strip()


z = zsearch("http://localhost:8080")
print z

print z.getDocument(1)

