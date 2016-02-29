#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json
import time

reload(sys)
sys.setdefaultencoding('utf-8')

def cal(file,code):
	path='E:\\tempip\\'
	inputfile=open(path+file,'r')
	addr=[]
	allsame=0
	allnum=0
	for line in inputfile.readlines():
		line=line.strip('\n')
		linespl=line.split('\t')
		basename=linespl[0]
		startip=linespl[1]
		endip=linespl[2]
		addr=[linespl[3],linespl[4],linespl[5]]
		samenum,num=calnum(startip,endip,addr,code)
		allsame=allsame+samenum
		allnum=allnum+num
		print allsame,allnum
	ratio=allsame/allnum
	input.close()
	return ratio
		
		
		
def calnum(startip,endip,addr,code):
	path='E:\\tempip\\'
	ansfiles=['ans1.csv','ans2.csv','ans3.csv','ans4.csv','ans5.csv','ans6.csv','ans7.csv','ans8.csv','ans9.csv','ans10.csv']
	comsip=startip
	samenum=0
	allnum=0
	for ansfile in ansfiles:
		inputcomp=open(path+ansfile,'r')
		line=inputcomp.readline()
		while line:
			line=line.strip('\n')
			linespl=line.split('\t')
			firstip=linespl[1]
			secondip=linespl[2]
			addr2=[linespl[3],linespl[4],linespl[5]]
			if(isin(comsip,firstip,secondip)==1):
				if(int(endip)<=int(secondip)):
					comeip=endip
				else:
					comeip=secondip
				total=int(comeip)-int(comsip)+1
				comsip=str(int(comeip)+1)
				if(comaddr(addr,addr2,code)==1):
					samenum=samenum+total
				else:
					samenum=samenum
			if(int(comsip)>int(endip)):
				inputcomp.close()
				break
			line=inputcomp.readline()
		inputcomp.close()
	allnum=int(endip)-int(startip)+1
	return samenum,allnum

def comaddr(addr,addr2,code):
	#outputs(addr)
	#outputs(addr2)
	flag=0
	if(code==1):
		if(addr[0]==addr2[0]):
			flag=1
	elif(code==2):
		if((addr[0]==addr2[0])and (addr[1]==addr2[1])):
			flag=1
	elif(code==3):
		if((addr[0]==addr2[0])and (addr[1]==addr2[1])and (addr[2]==addr2[2])):
			flag=1
	return flag
	
def outputs(addr):
	for add in addr:
		print add.decode('utf-8').encode('gbk'),
	print "\t"
	
def isin(ip,firstip,secondip):
	if(int(ip)>=int(firstip) and int(ip)<=int(secondip)):
		return 1
	else:
		return 0
	
	
	
	
if  __name__=='__main__':
	
	starttime=time.time()
	print starttime
	files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_ip138.csv','info_sina.csv','info_taobao.csv']
	#file='info_baidu.csv'
	out=open('ratio.txt','a+')
	for file in files:
		for i in range(3):
			ratio=cal(file,i+1)
			out.write(ratio),
			out.write('\t')
		out.write("\n")
	endtime=time.time()
	print endtime
	time=endtime-starttime
	print "time sec:",
	print time
	time=time/3600
	print "time hours:",
	print time	
