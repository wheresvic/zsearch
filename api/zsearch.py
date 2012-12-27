
import httplib
import codecs
import urllib
import urllib2

from xml.sax.saxutils import escape

from xml.dom.minidom import *

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
		
		data = dict()
		
		try:

			response = urllib2.urlopen(url)
			text = response.read()
		
			# http://www.evanjones.ca/python-utf8.html		
			#text = text.lstrip( unicode( codecs.BOM_UTF8, "utf8" ) )
			#doc = parseString(text.encode("utf-8"))

			doc = parseString(text)

			roots = doc.getElementsByTagName("document")

		
			for node in roots[0].childNodes:
			
				field = node.tagName
				value = node.firstChild.nodeValue

				data[field] = value

			# print field.encode('utf-8') + " " + value.encode('utf-8')

		except urllib2.HTTPError as err:

			if err.code == 404:
				pass	
			else:	
				pass

			#TODO: raise custom exception here
			raise err
	
		return data

	def addDocument(self, data):
		
		if (len(data)):

			doc = xml.dom.minidom.Document()	
			
			root = doc.createElement("document")
			doc.appendChild(root)

			for key, value in data.iteritems():
				print key, value
				field = doc.createElement(key)
				text = doc.createTextNode(value)
				field.appendChild(text)
				root.appendChild(field)
			

			print(doc.toprettyxml())

