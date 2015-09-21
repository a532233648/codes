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
			#print ipinfo.decode('utf-8').encode(gb2312)
	output.close()
	
def compl(ipinfo,code):  #code=0,country;1:province;2:city
	sum=0
	i=16777470
	output=open('E:\\s\\IP\\ipinfo\\temp\\temp.csv','w')
	while(i<16777475):
		address=[]
		sum=0
		for basekey in ipinfo:  #每个库遍历
			#print basekey+'\t:\t'+str(i),
			for ipkey in ipinfo[basekey]:  #每个IP
				#output.write(ipkey+'\n')
				if(isin(i,ipkey)==1):     
					#print i
					sum=sum+1         #IP库个数
					#for valueaddr in ipinfo[basekey][ipkey]:  #IP地址
						#print valueaddr.decode('utf-8').encode('gbk'),
						#print address
					#print '\n'
					address.append(ipinfo[basekey][ipkey])
					break
				#else :
					#print i
					#i=i+1
		print sum
		for addnum in range(len(address)):
			for j in range(len(address[addnum])):
				print address[addnum][j].decode('utf-8').encode('gbk'),
			print '\n'
		votemax(address,sum)
		address[:]=[]
		i=i+1
	output.close()			

def votemax(address,flist):
	maxacc=0
	tempadd={}
	tempkey=""
	#if(len(address)==0):
	for i in range(len(address)):
		if(len(address[i])>maxacc):
			maxacc=len(address[i])
	for addnum in range(maxacc):
		for i in range(len(address)):
			if(tempadd.has_key(address[i][addnum])==True):
				tempadd[address[i][addnum]]+=1
			else :
				tempadd[address[i][addnum]]=1
		tempkey=selmax(tempadd)
		flist.insert(addnum,tempkey)
		tempkey=""
		#for add1 in tempadd:
			#print add1.decode('utf-8').encode('gbk'),
			#print tempadd[add1],
		tempadd={}
		#print "this ---- "

def selmax(tempdic):
	max=0
	for key in tempdic:
		#print "this temokey"+'\t'+str(tempdic[key])+'\t'+str(max)+'\t'+"end temokey"
		if(tempdic[key]>max):
			max=tempdic[key]
			loc=key
		#print "this temokey"+'\t'+loc.decode('utf-8').encode('gbk')+'\t'+key.decode('utf-8').encode('gbk')+'\t'+"end temokey"
	return loc

def isin(ip,ipseg):
    templist=ipseg.split('\t')
    templist=map(int,templist)
    if(ip>=templist[0] and ip<=templist[1]):
        return 1
    else:
        return 0


if __name__=='__main__':
	#file=open('/home/rock/rock/ipinfo/info_baidu.csv')
	ipinfo={}
	readfile(ipinfo)
	writefile(ipinfo)
	compl(ipinfo,0)
	#compl(ipinfo)

