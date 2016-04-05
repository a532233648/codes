#! /usr/bin/env python
#coding = utf-8

'''
@version:??
@author:rock zhou
@file: ippre.py
@time: 2016/3/26 9:55
'''

def ippre(filepath,filenamein):
    filein=open(filepath+filenamein,'r')
    flag=0
    iplist=[]
    for line in filein.readlines():
        if(flag==0):
            iplist.append(line)
            flag=1
        else:
            flag=0
    filein.close()
    return iplist
def dealiplist(iplist):
    toiplist=[]
    for ele in iplist:
        onelist=[]
        onelist=ele.split('\t')
        ip=onelist[1]
        port=onelist[2]
        templist=ip+':'+port+'\n'
        toiplist.append(templist)
    return toiplist

def writeiplist(filepath,filenameout,iplist):
    fileout=open(filepath+filenameout,'a+')
    for elem in iplist:
        fileout.write(elem)
    fileout.close()

if __name__ == '__main__':
    filepath="F:\\temp\\"
    filenamein="inip.txt"
    filenameout="ouip.txt"
    iplist=ippre(filepath,filenamein)
    toiplist=dealiplist(iplist)
    writeiplist(filepath,filenameout,toiplist)

