#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json

reload(sys)
sys.setdefaultencoding('utf-8')

def readfile(ipinfo):
	#ipinfo={}
	#path='E:\\s\\IP\\ipinfo\\temp\\'
	path='E:\\s\\IP\\ipinfo\\'
	#path='/home/rock/rock/ipinfo/'
	files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_ip138.csv','info_sina.csv','info_taobao.csv']
	#files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_taobao.csv']
	filesdict=[]
	for file in files:
		fileopen=open(path+file)
		for line in fileopen.readlines():
			linespl=line.split('\t')
			#line.encode('utf-8')
			ip=linespl[1]+'\t'+linespl[2]
			addr=[linespl[3],linespl[4],linespl[5]]
			if(linespl[0] in ipinfo):
				ipinfo[linespl[0]].update({ip:addr})
			else :
				ipinfo.setdefault(linespl[0],{ip:addr})
			#ipinfo.update({linespl[0],{ip:addr}})
	
def compl(ipinfo,nimip,maxip):  #code=0,country;1:province;2:city
	sum=0
	ipnum=minip
	fians={}
	templist=[]
	ipaddress=[]
	markadd=[]
	mark=0
	while(ipnum<maxip):
		endip=ipnum
		address=[]
		sum=0
		address=searchip(ipinfo,ipnum)    #查找出与IP对应的各个库地址
		if(len(address)==0):
			continue
		votemax(address,templist)  #选出多数赞同的地址，保存在templist里
		if(mark==0):
			startip=ipnum
			markadd=templist
			markip=endip
		elif(mark>0):
			if((endip-markip)==1 and (markadd==templist)==True):
				markip=endip
			else :
				fians.setdefault(str(startip)+"\t"+str(markip),markadd)
				startip=endip
				markip=endip
				markadd=templist
				mark=0
		if(ipnum==(maxip-1)):
			fians.setdefault(str(startip)+"\t"+str(markip),markadd)
		templist=[]
		address[:]=[]
		ipnum=ipnum+1
		mark=mark+1
		if(len(fians)==5):
			write_ans(fians)
			fians={}
	write_ans(fians)
	

def searchip(ipinfo,ipnum):
	tempadd=[]
	for basekey in ipinfo:  #每个库遍历
		for ipkey in ipinfo[basekey]:  #每个IP
			if(isin(ipnum,ipkey)==1):   
				tempadd.append(ipinfo[basekey][ipkey])
				break
	return tempadd
	

def write_ans(fians):
	output=open('E:\\s\\IP\\ipinfo\\temp\\temp.csv','a+')
	for ip in fians:
		output.write(ip),
		for ansnum in range(len(fians[ip])):
			output.write('\t'+fians[ip][ansnum]) ,
		output.write("\n") 
	output.close()




def votemax(address,flist):
	tempadd={}
	tempkey=""
	for addnum in range(3):
		for i in range(len(address)):
			if(tempadd.has_key(address[i][addnum])==True):
				tempadd[address[i][addnum]]+=1
			else :
				tempadd[address[i][addnum]]=1
		tempkey=selmax(tempadd)
		address=delval(address,tempkey,addnum)
		flist.insert(addnum,tempkey)
		tempadd={}

def delval(address,val,addnum):
	for key in address:
		if(key[addnum]!=val):
			address.remove(key)
	return address

def outputsome(address):
	for add in address:
		print add.decode('utf-8').encode('gbk')+"\t"+str(address[add])



def selmax(tempdic):
	max=0
	loc="\t"
	for key in tempdic:
		if(tempdic[key]>max and key!=""):
			max=tempdic[key]
			loc=key
	return loc

def isin(ip,ipseg):
	templist=ipseg.split('\t')
	templist=map(int,templist)
	if(ip>=templist[0] and ip<=templist[1]):
		return 1
	else:
		return 0


if __name__=='__main__':
	minip=16777452
	maxip=16807979
	ipinfo={}
	readfile(ipinfo)
	compl(ipinfo,minip,maxip)
	

