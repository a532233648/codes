#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os,sys
import json
import time

reload(sys)
sys.setdefaultencoding('utf-8')

if __name__=='__main__':
	inf=open('F:\\tempip\\1012\\merans.csv','r')
	ouf=open('F:\\tempip\\1012\\merans2.csv','a+')
	for line in inf.readlines():
		ouf.write("answer\t"+line)
	inf.close()
	ouf.close()