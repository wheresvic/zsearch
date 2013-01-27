
import codecs
import urllib
import urllib2
from xml.dom.minidom import *

'''
TODO: add error handling and standardize errors returned from server
'''
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

	'''
	@param	id	the documentId to retrieve
	@return		a dictionary of the field : value pairs
	'''
	def getDocument(self, id):
		params = urllib.urlencode({'id' : str(id)})
		params = params.encode('utf-8')
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

				if (node.nodeType != node.TEXT_NODE):

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

	'''
	@param	data 	dictionary/map of field : value pairs, to be inserted as a single document
	@return			the docId of the inserted document
	'''
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

			params = {'data' : doc.toxml()}
			print params

			postData = urllib.urlencode(params)
			postData = postData.encode('utf-8')

			print postData
			url = self.url + "index";

			# print url
			response = urllib2.urlopen(url, postData)
			docId = response.read()

			return docId

		else:

			#TODO: raise exception here
			pass

	'''
	@param	query	the term to search for
	@param	field	the field to search for, defaults to None
	@return	 		a list of documentIds that match the search
	'''
	def search(self, query, start = 0, offset = 0, field = None):

		l = list()

		s = 0
		o = 0

		msg = "Invalid start (" + str(start) + ") and/or offset (" + str(offset) + "). They both need to be unsigned integers."

		try:
			s = int(start)
			o = int(offset)

			if (s < 0 or o < 0):
				raise Exception(msg)

		except ValueError:
			raise Exception(msg)

		p = dict()
		p['q'] = str(query)
		p['s'] = s
		p['o'] = o

		if field is not None:
			#TODO: add query for field
			pass

		params = urllib.urlencode(p)
		params = params.encode('utf-8')
		url = self.url + 'search?' + params
		print url

		try:

			response = urllib2.urlopen(url)
			text = response.read()

			l = (int(x) for x in text.split())

		except urllib2.HTTPError as err:

			if err.code == 404:
				pass
			else:
				pass

			#TODO: raise custom exception here
			raise err

		return l
