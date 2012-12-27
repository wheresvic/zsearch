
import httplib

import urllib
import urllib2

from xml.sax.saxutils import escape


class zsearch:

	def __init__(self, url):
		
		if (url[len(url) - 1] == '/'):
			self.url = url
		else:
			self.url = url + '/'

		# self.httpServ = httplib.HTTPConnection(self.url, self.port)

	def __str__(self):
		return "zearch " + self.url

	def __del__(self):	
		print "destroying zsearch"

	def getDocument(self, id):
		params = urllib.urlencode({'id' : str(id)})
		url = self.url + 'doc?' + params
		print url
		response = urllib2.urlopen(url)
		text = response.read()
		return text

