#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import time

def searchiptime(ipnum):
    path='/home/rock/rock/ipinfo/'   #ubuntu
    files=['info_baidu.csv','info_cz.csv','info_geoip.csv','info_ip2location.csv','info_ip138.csv','info_sina.csv','info_taobao.csv']
    tempadd=[]
    address=[]
    for file in files:
        output=open(path+file)
        for line in output:
            linespl=line.split('\t')
            ipseg=linespl[1]+'\t'+linespl[2]
            addr=[linespl[3],linespl[4],linespl[5]]
            if(isin(ipnum,ipseg)==1):
                print line
                address.append(addr)
                break
        output.close()
    #return address

def isin(ip,ipseg):
    templist=ipseg.split('\t')
    templist=map(int,templist)
    if(ip>=templist[0] and ip<=templist[1]):
        return 1
    else:
        return 0

def counttime():
    i=1
    while(i<3758096383):
        i=i+1

if __name__=='__main__':
    starttime=time.time()
    i=3758095380
    while(i<3758096380):
        searchiptime(i)
        i=i+1
    endtime=time.time()
    time=endtime-starttime
    print "time sec:",
    print time
    time=time/3600
    print "time hours:",
    print time
