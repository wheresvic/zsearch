
Input simple text / key value pairs
	-> xml escape (api)
	-> wrap within xml frame (api)
	-> html escape (api)
	-> post (api)
	-> html unescape (server)
	-> build document and unescape (DocumentImpl / rapidxml)
	-> store
	-> query
	-> xml escape (DocumentImpl)
	-> return 	

