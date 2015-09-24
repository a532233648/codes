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
		linespl=line.split('\t')
		basename=linespl[0]
		startip=linespl[1]
		endip=linespl[2]
		addr=[linespl[3],linespl[4],linespl[5]]
		if(mark==0):
			markstartip=startip
			markendip=endip
			markaddr=addr
			mark=mark+1
			print mark
		else :
			if(int(startip)-(int(markendip))==1 and (markaddr==addr)==True):
				markendip=endip
			else :
				writeans(file,basename,markstartip,markendip,markaddr)
				markstartip=startip
				markendip=endip
				markaddr=addr
				mark=mark+1
	input.close()

def writeans(file,basename,markstartip,markendip,markaddr):
	path='E:\\temp\\merge\\'
	out=open(path+file,'a+')
	out.write(basename+'\t'),
	out.write(markstartip),
	out.write('\t'),
	out.write(markendip),
	out.write('\t'),
	for i in range(len(markaddr)):
		out.write(markaddr[i]+'\t'),
	out.write('\n')
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
	output.close()

def addbasename(file):
	path='E:\\temp\\'
	input=open(path+file,'r')
	out=open('E:\\temp\\source\\'+file,'a+')
	f=file.split('.')
	basename="answer"
	for line in input.readlines():
		out.write(basename+'\t'+line)
	input.close()
	out.close()
	
	
if  __name__=='__main__':
	
	starttime=time.time()
	print starttime
	#files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_ip138.csv','info_sina.csv','info_taobao.csv']
	file='ans.csv'
	#for file in files:
	addbasename(file)
	sortip(file)
	mergeip(file)
	endtime=time.time()
	print endtime
	time=endtime-starttime
	print "time sec:",
	print time
	time=time/3600
	print "time hours:",
	print time	
