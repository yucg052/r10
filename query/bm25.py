import math

#f: the occurency of a word for the doc
#n: the number of doc that contains the word
#N: the total number of doc

def getBM25(f, n, N, dl, avgdl):
	#print f, n, N, dl, avgdl
	k=1.2
	b=0.75
	idf = math.log(((N-n)+0.5)/(n+0.5), 10)
	if idf < 0:
		idf = 0
	score = idf * (f*(k+1)/(f+k*(1-b+b*dl/avgdl)))
	return score

#print getBM25(10,10,100,55,40)