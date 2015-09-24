#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json

reload(sys)
sys.setdefaultencoding('utf-8')

def deltab(file):
	input=open('E:\\codes\\python\\'+file,'r')
	output=open('E:\\codes\\python\\ans-1.csv','a+')
	for line in input.readlines():
		if(len(line)==0 or line=='\t' or len(line)==1):
			continue
		else:
			output.write(line)
	output.close()
	input.close()

if __name__=='__main__':
	file='ans.csv'
	deltab(file)
	
	#ipfiles()
	#ipseg="16779264"+'\t'+"16781311"
	#ip=167792751
	#print isin(ip,ipseg)
        #test 123
