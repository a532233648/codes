#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json
import time

reload(sys)
sys.setdefaultencoding('utf-8')

def mergeip(file):
	path='E:\\temp\\sorted\\'	  #win7
	input=open(path+file,'r')
	mark=0
	for line in input.readlines():
		line=line.strip('\n')
		linespl=line.split('\t')
		basename=linespl[0]
		startip=linespl[1]
		endip=linespl[2]
		for j in range(3):
			if(linespl[j+3]=='\n' or linespl[j+3]=="\n"):
				linespl[j+3]='\t'
		addr=[linespl[3],linespl[4],linespl[5]]
		if(mark==0):
			markstartip=startip
			markendip=endip
			markaddr=addr
			mark=mark+1
			#print mark
		else :
			if(int(startip)-(int(markendip))==1 and (markaddr==addr)==True):
				markendip=endip
			else :
				writeans(file,basename,markstartip,markendip,markaddr)
				markstartip=startip
				markendip=endip
				markaddr=addr
				#mark=mark+1
	writeans(file,basename,markstartip,markendip,markaddr)
	input.close()
	
	
def writeans(file,basename,markstartip,markendip,markaddr):
	path='E:\\temp\\merge\\'
	out=open(path+file,'a+')
	out.write(basename+'\t'),
	out.write(markstartip),
	out.write('\t'),
	out.write(markendip),
	#out.write('\t'),
	for i in range(3):
		out.write('\t'+markaddr[i]),
	out.write('\n')
	basename=''
	markstartip=''
	markendip=''
	markaddr=[]
	out.close()
	
	
def sortip(file):
	pathsource='E:\\temp\\source\\'
	pathsorted='E:\\temp\\sorted\\'
	output=open(pathsorted+file,'a+')
	input=open(pathsource+file,'r')
	iplist=[]
	ipdict={}
	for line in input.readlines():
		linespl=line.split('\t')
		iplist.append(int(linespl[1]))
		ipdict.setdefault(linespl[1],line)
	iplist.sort()
	input.close()
	for i in range(len(iplist)):
		output.write(ipdict[str(iplist[i])])
		#output.write(ipdict[iplist[i]])
	output.close()
	

	
if __name__=='__main__':
	
	starttime=time.time()
	print starttime
	print time.localtime()
	files=['ans1.csv','ans2.csv','ans3.csv','ans4.csv','ans5.csv','ans6.csv','ans7.csv','ans8.csv','ans9.csv','ans10.csv',]
	#file='ans6.csv'
	for file in files:
		#sortip(file)
		mergeip(file)
	endtime=time.time()
	print endtime
	time=endtime-starttime
	print "time sec:",
	print time
	time=time/3600
	print "time hours:",
	print time	