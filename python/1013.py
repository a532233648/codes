#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json
import time

reload(sys)
sys.setdefaultencoding('utf-8')


def readin(file,path):
	ips=[]
	addrs=[]
	out=open(path+file,'r')
	for line in out.readlines():
		line=line.strip('\n')
		linespl=line.split('\t')
		ipseq=linespl[1]+'\t'+linespl[2]
		addr=[linespl[3],linespl[4],linespl[5]]
		ips.append(ipseq)
		addrs.append(addr)
	out.close()
	return ips,addrs
	
def merged(baseips,baseaddrs):
	ansips=[]
	ansaddrs=[]
	answer={}
	startip=16777216
	locflag=[[0,0],[0,0],[0,0],[0,0],[0,0],[0,0],[0,0]]  #标记查找到的IP在各个库位置,0未找到，1找到
	count=0
	while startip<=3758096382:
		#test=open('test.txt','a+')   #test
		#test.write("startip\t"+str(startip)+'\n')
		#test.close()
		tmpips=[]
		addrs=[]
		for i in range(7):
			tmploc=sealoc(startip,baseips[i],locflag[i][1])
			if(tmploc!=-1):
				locflag[i]=[1,tmploc]
			else:
				locflag[i][0]=0
		for j in range(7):
			if(locflag[j][0]!=0):
				tmpips.append(baseips[j][locflag[j][1]])
				addrs.append(baseaddrs[j][locflag[j][1]])
		tmpendip=selmin(tmpips)
		tmpendaddr=seladdr(addrs)
		ansips=str(startip)+'\t'+tmpendip
		answer.setdefault(ansips,tmpendaddr)
		count+=1
		startip=int(tmpendip)+1
		for k in range(7):
			locflag[i][0]=0
		if(count==1):
			mergedwrite(answer)
			answer={}
			count=0
	mergedwrite(answer)


def seladdr(address):
	tempadd={}
	templist=[]
	tempkey=[]
	for addnum in range(3):
		for i in range(len(address)):
			if(tempadd.has_key(address[i][addnum])==True):
				tempadd[address[i][addnum]]+=1
			else :
				tempadd[address[i][addnum]]=1
		tempkey=selmax(tempadd)
		templist.insert(addnum,tempkey)
		tempadd={}
	return templist


def selmax(tempdic):
	max=0
	addr=[]
	for key in tempdic:
		if(tempdic[key]>max and key!=""):
			max=tempdic[key]
	i=0
	for key in tempdic:
		if(tempdic[key]==max):
			addr.insert(i,key)
			i=i+1
	return addr


def selmin(ips):
	minip=3768096383
	#test=open('test.txt','a+')   #test
	for i in range(len(ips)):
		ips[i]=ips[i].split('\t')
		#for s in range(len(ips[i])):
			#test.write(ips[i][s]+' '),
		#test.write('||')
		if(int(ips[i][1])<minip):
			minip=int(ips[i][1])
	#test.write('\n')
	#test.close()
	return str(minip)
		
def sealoc(ip,ipseqs,locstart):
	loc=-1
	j=locstart
	while j<len(ipseqs):
		if(seain(ip,ipseqs[j])):
			loc=j
			#print "yes"+str(loc)
			break
		else:
			j=j+1
	return loc




def seain(ip,ip2):
	ip2=ip2.split('\t')
	if(int(ip) >=int(ip2[0]) and int(ip) <=int(ip2[1])):
		return 1
	else :
		return 0

def mergedwrite(answer):
	path='F:\\tempip\\1013\\'
	anstxt='merans.csv'
	out=open(path+anstxt,'a+')
	for key in answer:
		out.write(key+'\t'),
		for i in range(len(answer[key])):
			for k in range(len(answer[key][i])):
				out.write(answer[key][i][k]+' '),
			out.write('\t')
			#print value.decode('utf-8').encode('gbk'),
		out.write('\n')
	out.close()


if __name__=='__main__':
	starttime=time.time()
	print time.localtime()
	path='F:\\tempip\\1013\\'
	files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_ip138.csv','info_sina.csv','info_taobao.csv']
	baseips=[]
	baseaddrs=[]
	for file in files:
		ips,addrs=readin(file,path)
		baseips.append(ips)
		baseaddrs.append(addrs)
	merged(baseips,baseaddrs)
	endtime=time.time()
	alltime=endtime-starttime
	print time.localtime()
	print "time sec:",
	print alltime
	alltime=alltime/3600
	print "time hours:",
	print alltime	
