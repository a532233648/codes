#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json

reload(sys)
sys.setdefaultencoding('utf-8')

def readfile(ipinfo):
	#ipinfo={}
	path='E:\\s\\IP\\ipinfo\\temp\\'
	#path='/home/rock/rock/ipinfo/'
	#files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_ip138.csv','info_sina.csv','info_taobao.csv']
	files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_taobao.csv']
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
	
	
def writefile(ipinfo):
	output=open('E:\\s\\IP\\ipinfo\\temp\\answer.csv','w')
	for basekey in ipinfo:  #每个库遍历
		for ipkey in ipinfo[basekey]:  #每个IP
			#output.write(basekey+':'+valueaddr+':'+ipkey[valueaddr])
			#print basekey+':'+ipkey+':',
			output.write(basekey+':'+ipkey+' ')
			#print json.dumps(ipinfo[basekey][ipkey], encoding='utf-8', ensure_ascii=False)
			#print str(ipinfo[basekey][ipkey]).decode('utf-8').encode('gbk')
			#print ipinfo[basekey][ipkey][0].decode('utf-8').encode('gbk')
			output.write(ipinfo[basekey][ipkey][0]+'\t')
			output.write(ipinfo[basekey][ipkey][1]+'\t')
			output.write(ipinfo[basekey][ipkey][2]+'\t')
			output.write('\n')
			#print json.dumps(ipinfo, encoding='utf-8', ensure_ascii=False) 
			#print ipinfo.decode('utf-8').encode('gbk')
	output.close()
	
def compl(ipinfo,code):  #code=0,country;1:province;2:city
	sum=0
	ipnum=16777472
	output=open('E:\\s\\IP\\ipinfo\\temp\\temp.csv','w')
	fians={}
	templist=[]
	while(ipnum<18234879):
		address=[]
		sum=0
		for basekey in ipinfo:  #每个库遍历
			for ipkey in ipinfo[basekey]:  #每个IP
				if(isin(ipnum,ipkey)==1):     
					sum=sum+1         #IP库个数
					address.append(ipinfo[basekey][ipkey])
					break
		votemax(address,templist)
		fians.setdefault(str(ipnum),templist)
		templist=[]
		address[:]=[]
		ipnum=ipnum+1
	for ip in fians:
		output.write(ip)
		for ansnum in range(3):
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
	ipinfo={}
	readfile(ipinfo)
	writefile(ipinfo)
	compl(ipinfo,0)

