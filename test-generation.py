INPUT = open('input.txt', 'w')
OUTPUT = open('output.txt', 'w')

def printio(s, msg, onlyOut=False):
    print(s)
    if onlyOut == False:
        print(s, file=INPUT)
    print(msg, s, file=OUTPUT)

# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.

from collections import defaultdict
from random import randint
from random import random
from random import sample
import re

defValue = lambda : 0

def createPoly(numTerms):
    p = defaultdict(defValue)
    while(len(p) < numTerms):
        #p[(randint(0,int(numTerms ** 0.5)), randint(0,int(numTerms ** 0.5)))] = round((random()-0.5)*numTerms, randint(0,2))
        p[(randint(0, int(numTerms ** 0.5)), randint(0, int(numTerms ** 0.5)))] = int((random() - 0.5) * numTerms)
    return p

def printPoly(p, msg='', onlyOut=False):
    l = []
    for k in sorted(p.keys(), key=lambda x:(x[0]+x[1], x[0]), reverse=True):
        if (p[k] != 0):
            l.append("(%d,%d,%g)"%(k[0], k[1], p[k]))
    printio(','.join(l), msg, onlyOut)

def addPoly(p1, p2):
    res = defaultdict(defValue)
    for k in p1.keys():
        res[k] = p1[k]
    for k in p2.keys():
        res[k] += p2[k]
    return res

def mulPoly(p1, p2):
    res = defaultdict(defValue)
    for i in p1.keys():
        for j in p2.keys():
            res[(i[0]+j[0], i[1]+j[1])] += p1[i]*p2[j]
    return res

def generatePolyTest():
    p1 = createPoly(randint(4,20))
    p2 = createPoly(randint(4,20))
    op = ''
    if randint(0,1) == 0:
        p3 = addPoly(p1, p2)
        op = '+'
    else:
        p3 = mulPoly(p1, p2)
        op = '*'

    printio(op, 'Please enter the operator(+/*):')
    printPoly(p1, 'Please enter polynomial A:')
    printPoly(p2, 'Please enter polynomial B:')
    printPoly(p3, 'A'+op+'B:', True)
    printio('y','Do you want to continue? (y/n):')
    printio('', '', True)

##############################################################################
##############################################################################
NUMBER_RANGE = 300

chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHINJKLMNOPQRSTUVWXYZ123456789'
cmap = {}

if NUMBER_RANGE < len(chars):
    chars = chars[:NUMBER_RANGE]
randArray = sample(range(0, NUMBER_RANGE + int(NUMBER_RANGE/4)), len(chars))
for i in range(len(chars)):
    cmap[chars[i]] = randArray[i]-int(NUMBER_RANGE/20)

def createSeq(numTerms):
    s = ''
    for i in range(numTerms):
        s += chars[randint(0,len(chars)-1)]
    return s

def printSeq(s, msg='', onlyOut=False):
    global cmap
    res = ''
    for i in range(len(s)):
        res += str(cmap[s[i]])+','
    res += '$'
    printio(res, msg, onlyOut)

def printPatternSeq(s):
    printSeq(s, "Please enter the pattern to be replaced:")

def printTextSeq(s):
    printSeq(s, "Please enter the replacement text:")

def printSeqAndAskForContinue(s):
    printSeq(s, "After replacement, S:", True)
    printio('y', "Do you want to continue? (y/n):")
    printio('', "", True)

def generateResetStep(s):
    if randint(0,100) > 5:
        return s

    printSeqAndAskForContinue(s)

    printPatternSeq(s)
    printTextSeq('')
    printSeqAndAskForContinue('')

    #printio('RESET *****', "", True)

    printPatternSeq('')
    text = createSeq(10)
    printTextSeq(text)
    return text

def generateSeqStep(s):
    start = randint(0,len(s)-1)
    end = randint(start,len(s))
    pattern = s[start:end]
    printPatternSeq(pattern)
    text = createSeq(150)
    printTextSeq(text)
    if pattern == '':
        s = text + s
    else:
        s = re.sub(pattern,text,s)
    return generateResetStep(s)

def generateSequenceTest(numSteps):
    s = createSeq(randint(4, 100))
    printSeq(s, "Please enter the sequence S:")
    for i in range(numSteps):
        s = generateSeqStep(s)
        printSeqAndAskForContinue(s)

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    for i in range(5000):
        generatePolyTest()
