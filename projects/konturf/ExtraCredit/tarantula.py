import subprocess
import random

def parseTxtCounter(txtList = [], numList = []):
	i = 1
	lineCtr = 1
	found = False
	iterTxtList = iter(txtList)
	# There is no Line 0, so just put 0 into numList for Line 0
	# since Line 0 will never be covered
	if(len(numList) == 0):
		numList.append(0)
	for line in txtList[1: ]:
		# Find the first line in the program
		while(not found):
			j = 0
			while(txtList[lineCtr][j] != ':'):
				j += 1
			while(not (txtList[lineCtr][j].isdigit())):
				j += 1
			if(txtList[lineCtr][j] != '1'):
				next(iterTxtList)
				lineCtr += 1
			else:
				found = True
		# Determine line-by-line if a line was covered. If so, add 1 to
		# the numList element for that line
		j = 0
		if(i >= len(numList)):
			numList.append(0)
		while(line[j] == ' '):
			j += 1
		if(line[j].isdigit()):
			numList[i] += 1
		i += 1	

def parseTxtLines(txtList = [], linesList = []):
	i = 1
	lineCtr = 1
	found = False
	iterTxtList = iter(txtList)
	# No Line 0, so just append 0 for Line 0
	linesList.append(0)
	for line in txtList[1: ]:
		# Find the first line in the program
		while(not found):
			j = 0
			while(txtList[lineCtr][j] != ':'):
				j += 1
			while(not (txtList[lineCtr][j].isdigit())):
				j += 1
			if(txtList[lineCtr][j] != '1'):
				next(iterTxtList)
				lineCtr += 1
			else:
				found = True
		# Record each line in linesList, skipping the initial part of
		# the line added by gcov
		j = 0
		while(line[j] != ':'):
			j += 1
		while(not (line[j].isdigit())):
			j += 1
		linesList.append(line[j: ])

def calcRes(totGdRes, totBdRes, lstGdRes = [], lstBdRes = [], fnlRes = []):
	for i in range(0, (len(lstGdRes) - 1)):
		passedRatio = lstGdRes[i] / float(totGdRes)
		failedRatio = lstBdRes[i] / float(totBdRes)
		if((passedRatio + failedRatio) > 0.0):
			susp = failedRatio / (passedRatio + failedRatio)
		else:
			susp = -1
		info = {}
		info['lineNum'] = i
		info['suspiciousness'] = susp
		fnlRes.append(info)
	

totGoodResult = 0
totBadResult = 0
numUsedGoodResult = []
numUsedBadResult = []
linesList = []
tarantulaResList = []
compileStr = "gcc randomtestcard2.c dominion.c rngs.c -coverage -lm"
gcovStr = "gcov dominion.c > /dev/null"
coverageOutputStr = "cat dominion.c.gcov >> randomTestResult.txt"
rmFileStr = "rm -f randomTestResult.txt"
numResults = 30
#numResults = input("How many of the most suspicious lines do you want? ")
for seed in range(1, 2001):
	testResultList = []
	runStr = "a.out " + str(seed) + " > randomTestResult.txt"
	subprocess.call(compileStr, shell=True)
	subprocess.call([runStr], shell=True)
	subprocess.call([gcovStr], shell=True)
	subprocess.call([coverageOutputStr], shell=True)
	f = open("randomTestResult.txt", "r")
	for line in f:
		testResultList.append(line)
	f.close()
	subprocess.call([rmFileStr], shell=True)
	# Check to see if the first line is "Correct Result" or 
	# "Incorrect Result"
	if(testResultList[0][0] == 'C'):
		totGoodResult += 1
		parseTxtCounter(testResultList, numUsedGoodResult)
	else:
		totBadResult += 1
		parseTxtCounter(testResultList, numUsedBadResult)
	# For the first time through, save the lines of the program to a list
	if(seed == 1):
		parseTxtLines(testResultList, linesList) 
subprocess.call("rm -f a.out", shell=True)
calcRes(totGoodResult, totBadResult, numUsedGoodResult, numUsedBadResult, tarantulaResList)
for i in range(0, len(tarantulaResList)):
	print(tarantulaResList[i]['suspiciousness']),
	print("\t"),
	print(linesList[tarantulaResList[i]['lineNum']]),
#tarantulaResList.sort(key = lambda res: res['suspiciousness'], reverse = True)
#for i in range(0, numResults):
#	print(tarantulaResList[i]['suspiciousness']),
#	print(" "),
#	print(linesList[tarantulaResList[i]['lineNum']])
