import datetime
import time
import sys
import MeCab
from pymongo import MongoClient
import math
import operator

import pprint
class MyPrettyPrinter(pprint.PrettyPrinter):
    def format(self, _object, context, maxlevels, level):
        if isinstance(_object, unicode):
            return "'%s'" % _object.encode('utf8'), True, False
        elif isinstance(_object, str):
            _object = unicode(_object,'utf8')
            return "'%s'" % _object.encode('utf8'), True, False
        return pprint.PrettyPrinter.format(self, _object, context, maxlevels, level)


def printMenu():
    print "1. WordCount"
    print "2. TF-IDF"
    print "3. Similarity"
    print "4. MorpAnalysis"
    print "5. CopyData"

def MorpAnalysis(docs, col_tfidf):
    print "MorpAnalysis"
    objid = raw_input('Insert Object ID: ')
    a=0

    t = MeCab.Tagger('-d/usr/local/lib/mecab/dic/mecab-ko-dic')

    stop_word = {}
    f = open("wordList.txt", 'r')
    while True:
        line = f.readline()
        if not line: break
        stop_word[line.strip('\n')] = line.strip('\n')
    f.close()

    for doc in docs:
        if str(doc['_id']) == objid:
            a=1
        content = doc['content']
        nodes = t.parseToNode(content.encode('utf-8'))

        MorpList = []
        while nodes:
            if nodes.feature[0] == 'N' and nodes.feature[1] == 'N':
                w = nodes.surface
                if not w in stop_word:
                    MorpList.append(w)
            nodes = nodes.next

        contentDic = {}
        for key in doc.keys():
            contentDic[key] = doc[key]
        contentDic['morp'] = MorpList
        col_tfidf.update({'_id':contentDic['_id']}, contentDic, True)

        if a==1:
            print MyPrettyPrinter().pformat(MorpList)
            a=2

    if a==0:
        print "object id not in newslist"


def WordCount(docs, col_tfidf):
    print "WordCount"
    objid = raw_input('Insert Object ID: ')
    a=0
    for doc in docs:
        if str(doc['_id']) == objid:
            a=1
        MorpDic = {}
        for word in doc['morp']:
            if word not in MorpDic:
                MorpDic[word] = 1
            else:
                MorpDic[word] += 1

        contentDic = {}
        for key in doc.keys():
            contentDic[key] = doc[key]
        contentDic['WordCount'] = MorpDic
        col_tfidf.update({'_id':contentDic['_id']}, contentDic, True)

        if a==1:
            print MyPrettyPrinter().pformat(MorpDic)
            a=2

    if a==0:
        print "object id not in newslist"


def TfIdf(docs, col_tfidf):
    print "TF-IDF"
    
    objid = raw_input('Insert Object ID: ')
    a=0
    D=0
    targetDic = {}
    sigma = 0
    tfDic = {}
    idfDic = {}
    tfidfDic = {}
    for doc in docs:
        if str(doc['_id']) == objid:
            a=1
            targetDic = doc['WordCount']

            for word in doc['WordCount']:
                sigma += doc['WordCount'][word]

            for word in doc['WordCount']:
                tfDic[word] = float(doc['WordCount'][word]) / sigma
        
            break
    if a==0:
        print "object id not in newslist"
        return

    for doc in col_tfidf.find():
        D += 1
        for word in targetDic:
            if word in doc['WordCount']:
                if word not in idfDic:
                    idfDic[word] = 1
                else:
                    idfDic[word] += 1

    for word in targetDic:
        tfidfDic[word] = tfDic[word] * math.log(float(D) / idfDic[word])

    for doc in col_tfidf.find():
        contentDic = {}
        if str(doc['_id']) == objid:
            for key in doc.keys():
                contentDic[key] = doc[key]
            contentDic['TfIdf'] = tfidfDic
            col_tfidf.update({'_id':contentDic['_id']}, contentDic, True)

    sort = sorted(tfidfDic, key=lambda k : tfidfDic[k], reverse=True)
    for i in range(0, 10):
        s = sort[i] + u'\t' + str(tfidfDic[sort[i]])
        print MyPrettyPrinter().pformat(s)

#    sort = sorted(tfidfDic.items(), key=operator.itemgetter(1), reverse=True)
#    for i in range(0, 10):
#        print MyPrettyPrinter().pformat(sort[i])


def Similarity(docs, col_tfidf):
    print "Similarity"

    objid1 = raw_input('Insert Object ID(1): ')
    objid2 = raw_input('Insert Object ID(2): ')
    a1=0
    a2=0
    simDic1 = {}
    simDic2 = {}
    sigma1=0.0
    sigma2=0.0
    sigma3=0.0
    result = 0.0
    for doc in docs:
        if str(doc['_id']) == objid1:
            a1=1
        elif str(doc['_id']) == objid2:
            a2=1

    if a1==0 or a2==0:
        print "object id not in newslist"
        return

    for doc in col_tfidf.find():
        if str(doc['_id']) == objid1:
            if 'TfIdf' not in doc.keys():
                print "TF-IDF not calculated"
                return
            else:
                simDic1 = doc['TfIdf']
        elif str(doc['_id']) == objid2:
            if 'TfIdf' not in doc.keys():
                print "TF-IDF not calculated"
                return
            else:
                simDic2 = doc['TfIdf']

    for word in simDic1:
        if word not in simDic2:
            simDic2[word] = 0.0
    for word in simDic2:
        if word not in simDic1:
            simDic1[word] = 0.0

    for word in simDic1:
        sigma1 += simDic1[word] * simDic2[word]
    
    for word in simDic1:
        sigma2 += simDic1[word] * simDic1[word]
    sigma2 = math.sqrt(sigma2)
    
    for word in simDic2:
        sigma3 += simDic2[word] * simDic2[word]
    sigma3 = math.sqrt(sigma3)

    result = sigma1 / (sigma2 * sigma3)
    print result



def copyData(docs, col_tfidf):
    col_tfidf.drop()
    for doc in docs:
        contentDic = {}
        for key in doc.keys():
            if key != "_id":
                contentDic[key] = doc[key]
        col_tfidf.insert(contentDic)

conn = MongoClient('localhost')
db = conn['db20141583']
db.authenticate('db20141583', 'db20141583')
col = db['news']
col_tfidf = db['news_tfidf']

printMenu()
selector = input()

docs = col_tfidf.find()
if selector == 1:
    WordCount(docs, col_tfidf)
elif selector == 2:
    TfIdf(docs, col_tfidf)
elif selector == 3:
    Similarity(docs, col_tfidf)
elif selector == 4:
    MorpAnalysis(docs, col_tfidf)
elif selector == 5:
    docs = col.find()
    copyData(docs, col_tfidf)
