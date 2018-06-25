def crawl (url) :
	global a
	fURL.write('[' + str(int(a)-10000) + ']./public_html/' + url)
	if(int(a) != 10023) : fURL.write('\n')

	fname = 'Output_' + a[1] + a[2] + a[3] + a[4] + '.txt'
	f = open(fname, "w")
	a = str( int(a) +1 )

	r = requests_session.get(path + url)
	soup = BeautifulSoup(r.content, "html.parser")
	
	f.write(soup.get_text())
	
	l = soup.find_all('a')
	for link in l :
		href = link.get('href')

		if ( (requests_session.get(path + href)).content == None ) : continue
		if href not in linklis :
			linklis.append(href)
		else : continue
		
		crawl(href)

	f.close()

	

import requests
import os
from lib_LocalFileAdapter import LocalFileAdapter
requests_session = requests.session()
requests_session.mount('file://',LocalFileAdapter())


from bs4 import BeautifulSoup
path = 'file://' + os.getcwd() + '/public_html/'
linklis = ['index.html']
a = str(10001)

fURL = open("URL.txt", "w")
crawl('index.html')

fURL.close()

