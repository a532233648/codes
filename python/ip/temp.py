#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json
import time

reload(sys)
sys.setdefaultencoding('utf-8')


def seain(ip1,ip2):
	ip1=ip1.strip()
	ip2=ip2.strip()
	ipseq1=ip1.split('\t')
	ipseq2=ip2.split('\t')
	print ipseq1
	print ipseq2
	if(int(ipseq1[0])>=int(ipseq2[0]) and int(ipseq1[0])<=int(ipseq2[1])):
		return 1
	else :
		return 0
		
if __name__=='__main__':
	inp=open('1.csv','r')
	ip1=inp.readline()
	ip2=inp.readline()
	#print ip1
	#print ip2
	print seain(ip1,ip2)