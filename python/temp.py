#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json

reload(sys)
sys.setdefaultencoding('utf-8')

def ipfiles():
	
	i=18734656
	while(i<18734687):
		output=open('E:\\s\\IP\\ipinfo\\temp\\temp.csv','r')
		for line in output.readlines():
			#print str(i) + 'test'
			#print isin(i,line)
			if(isin(i,line)==1):
				print "yes"
			#else :
				#print "no"
				#print i,
				#print line
		i=i+1
		#print i
		output.close()

def isin(ip,ipseg):
	templist=ipseg.split('\t')
	templist=map(int,templist)
	#print type(ip),
	#print type(templist[0]),
	#print type(templist[1])
	if(ip>=templist[0] and ip<=templist):
		return 1
	else:
		return 0

def votemax(address,flist):
	maxacc=0
	tempadd={}
	tempkey=""
	#if(len(address)==0):
	for i in range(len(address)):
		if(len(address[i])>maxacc):
			maxacc=len(address[i])
	for addnum in range(3):
		for i in range(len(address)):
			if(tempadd.has_key(address[i][addnum])==True):
				tempadd[address[i][addnum]]+=1
			else :
				tempadd[address[i][addnum]]=1
		tempkey=selmax(tempadd)
		address=delval(address,tempkey,addnum)
		flist.insert(addnum,tempkey)
		#tempkey=""
		#for add1 in tempadd:
			#print add1.decode('utf-8').encode('gbk'),
			#print tempadd[add1],
		tempadd={}
		#print "this ---- "

def delval(address,val,addnum):
	for key in address:
		if(key[addnum]!=val):
			address.remove(key)
	return address




def selmax(tempdic):
	max=0
	loc="\t"
	for key in tempdic:
		#print "this temokey"+'\t'+str(tempdic[key])+'\t'+str(max)+'\t'+"end temokey"
		if(tempdic[key]>max and key!="\t"):
			max=tempdic[key]
			loc=key
		#print "this temokey"+'\t'+loc.decode('utf-8').encode('gbk')+'\t'+key.decode('utf-8').encode('gbk')+'\t'+"end temokey"
	return loc

#def delval(tempdic):
	


if __name__=='__main__':
	address=[["泰国","京市","海淀区"],["泰国","京市","海淀区"],["泰国","京市","海淀区"],["中国","北京市","朝阳区"],["中国","湖南省","朝阳区"],["中国","北京市","\t"],["中国","湖北省","\t"]]
	flist=[]
	votemax(address,flist)
	for i in range(len(flist)):
		print flist[i].decode('utf-8').encode('gbk'),
	
	#ipfiles()
	#ipseg="16779264"+'\t'+"16781311"
	#ip=167792751
	#print isin(ip,ipseg)
        #test 123
