#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json
import time

reload(sys)
sys.setdefaultencoding('utf-8')

'''
def readfile(ipinfo):
    #ipinfo={}
    #path='E:\\s\\IP\\ipinfo\\temp\\'
    #path='E:\\s\\IP\\ipinfo\\'
    #path='/home/rock/rock/ipinfo/'
    #files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_ip138.csv','info_sina.csv','info_taobao.csv']
    #files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_taobao.csv']
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
'''


def searchip(ipnum):
    #path='E:\\s\\IP\\ipinfo\\temp\\' #win7 小文件
    #path='E:\\s\\IP\\ipinfo\\'      #win7
    #path='E:\\temp\\merge\\'
    #path='/home/rock/work/tempip/'   #ubuntu
    #path='/home/rock/work/tempipifno/'  #ubuntu 小文件
    path='/home/rock/work/tempip/10/'
    files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_ip138.csv','info_sina.csv','info_taobao.csv']
    #files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_taobao.csv']
    tempadd=[]
    address=[]
    tempendiplist=[]
    for file in files:
        output=open(path+file)
        for line in output.readlines():
            linespl=line.split('\t')
            ipseg=linespl[1]+'\t'+linespl[2]
            addr=[linespl[3],linespl[4],linespl[5]]
            if(isin(ipnum,ipseg)==1):
                address.append(addr)
                tempendiplist.append(linespl[2])
                break
        output.close()
    if(len(address)!=0 and len(tempendiplist)!=0):
        tempendiplist=map(int,tempendiplist)
        tempendip=min(tempendiplist)
    else :
        tempendip=0
    return address,tempendip



def compl(nimip,maxip):  
    sum=0
    ipnum=minip-1
    fians={}
    templist=[]
    ipaddress=[]
    markadd=[]
    mark=0
    startip=ipnum+1
    while(startip<maxip):
        ipnum=ipnum+1
        address=[]
        sum=0
        address,endip=searchip(ipnum)    #查找出与IP对应的各个库地址
        if(endip==0):
            continue
        votemax(address,templist)  #选出多数赞同的地址，保存在templist里
        if(mark==0):
            startip=ipnum
            markadd=templist
            markip=endip
        elif(mark>0):
            if((ipnum-markip)==1 and (markadd==templist)==True):
                markip=endip
            else :
                fians.setdefault(str(startip)+"\t"+str(markip),markadd)
                startip=ipnum
                markip=endip
                markadd=templist
                mark=0
            ipnum=endip
        if(ipnum>=(maxip-1)):
            fians.setdefault(str(startip)+"\t"+str(markip),markadd)
        templist=[]
        address[:]=[]
        mark=mark+1
        if(len(fians)==5):
            write_ans(fians)
            fians={}
    write_ans(fians)
	

def write_ans(fians):
    #output=open('E:\\s\\IP\\ipinfo\\temp\\temp.csv','a+')
    writeout=open('/home/rock/work/tempip/ans10.csv','a+')
    #writeout=open('E:\\s\\IP\\ipinfo\\temp\\ans.csv','a+')
    for ip in fians:
        writeout.write("answer"+'\t'+ip),
        for ansnum in range(len(fians[ip])):
            writeout.write('\t'+fians[ip][ansnum]) ,
        writeout.write("\n") 
    writeout.close()




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
    minip=3509387260
    maxip=3758096385
    #maxip=16778216
    starttime=time.time()
    print time.localtime()
    #ipinfo={}
    #readfile(ipinfo)
    compl(minip,maxip)
    endtime=time.time()
    time=endtime-starttime
    print time.localtime()
    print "time sec:",
    print time
    time=time/3600
    print "time hours:",
    print time	

