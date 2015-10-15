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
	
def calratio(file,path,ipscmp,addrscmp):
	out=open('temp.txt','a+')
	out.write(file+'\n'),
	loc=0                   #698518
	i=0                     #10290
	j=0
	nosame=[0,0,0]
	all=[3741319168,3741319168,3741319168]
	flag=[0,0,0]
	ips,addrs=readin(file,path)
	tempipseq1=ips[i].split('\t')
	startip=int(tempipseq1[0])
	while (i<=len(ips)):
		out.write(str(i)+'\t'+str(loc)+'\t'),
		j=loc
		while (i<=len(ips) and j<=len(ipscmp)):
			if(j==len(ipscmp) or i==len(ips)):
				return nosame,all
			mark=seain(startip,ipscmp[j])
			if(mark==1):
				loc=j
				break
			else:
				j=j+1
		tempipseq1=ips[i].split('\t')
		out.write(str(tempipseq1)),
		out.write('\t'),
		tempipseq2=ipscmp[loc].split('\t')
		out.write(str(tempipseq2)),
		out.write('\n')
		flag=addrcmp(addrs[i],addrscmp[loc])
		if(int(tempipseq1[1])<int(tempipseq2[1])):
			tempnum=int(tempipseq1[1])-startip+1
			i=i+1
			if(i>=len(ips)):
				break
			tmpstartip=ips[i].split('\t')
			startip=int(tmpstartip[0])
			loc=loc
		elif(int(tempipseq1[1])==int(tempipseq2[1])):
			tempnum=int(tempipseq1[1])-startip+1
			loc=loc+1
			i=i+1
			if(i>=len(ips)):
				break
			tmpstartip=ips[i].split('\t')
			startip=int(tmpstartip[0])
		else:
			tempnum=int(tempipseq2[1])-startip+1
			startip=int(tempipseq2[1])+1
			i=i
			loc=loc+1
		#print "this is ",
		#print tempnum
		for k in range(len(flag)):
			#print flag[k],
			if(flag[k]==1):
				nosame[k]+=tempnum
		#print '\n'
	out.close()
	ips=addrs=[]
	ipscmp=addrscmp=[]
	return nosame,all

def addrcmp(addrs,addrscmp):
	flag=[0,0,0]
	for i in range(len(addrscmp)):
		tmpadd=addrscmp[i]
		tmpadd=tmpadd.split(' ')
		if(peraddr(addrs[i],tmpadd)==0):
			flag[i]+=1
	#print flag
	return flag

def peraddr(addr,addrs):
	if(addr==' ' or addr=='\t'):
		return 1
	for i in range(len(addrs)):
		if(addr==addrs[i]):
			return 1
	return 0



def seain(ip,ip2):
	ip2=ip2.strip('\n')
	ip2=ip2.split('\t')
	if(int(ip) >=int(ip2[0]) and int(ip) <=int(ip2[1])):
		return 1
	else :
		return 0





if __name__=='__main__':
	starttime=time.time()
	print time.localtime()
	path='F:\\tempip\\1012\\'
	final='ans.txt'
	files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_ip138.csv','info_sina.csv','info_taobao.csv']
	#files=['info_baidu.csv']
	anstxt='merans.csv'
	ipscmp,addrscmp=readin(anstxt,path)
	for file in files:
		print "----file-----"+file+"------file------"
		ansout=open(path+final,'a+')
		ansout.write(file+'\t'),
		nosame,all=calratio(file,path,ipscmp,addrscmp)
		for a in range(len(all)):
			ansout.write(str(all[a]-nosame[a])+' '+str(all[a])+'\t'),
			ansout.write(str(1.0-float(nosame[a])/all[a])),
			ansout.write('\t'),
			print 1.0-float(nosame[a])/all[a],
			print '\t',
			print all[a]-nosame[a],
			print '\t',
			print all[a],
			print '\t',
		ansout.write('\n')
		print '\n'
	endtime=time.time()
	alltime=endtime-starttime
	print time.localtime()
	print "time sec:",
	print alltime
	alltime=alltime/3600
	print "time hours:",
	print alltime	
