#!/usr/bin/env python
# coding=utf-8


'''
@version:??
@author:rock zhou
@file: chntoeng.py
@time: 2016/4/6 22:07
'''
import sys
reload(sys)
sys.setdefaultencoding('utf-8')

def strQ2B(ustring):
    """全角转半角"""
    rstring = ""
    ustring = ustring.encode('gbk', 'ignore').decode('cp936')
    #ustring = ustring.decode("cp936")
    for uchar in ustring:
        inside_code=ord(uchar)
        if inside_code == 12288:                              #全角空格直接转换
            inside_code = 32
        elif (inside_code >= 65281 and inside_code <= 65374): #全角字符（除空格）根据关系转化
            inside_code -= 65248
        rstring += unichr(inside_code)
    return rstring

if __name__ == "__main__":
    # test Q2B and B2Q
    filename = "123.txt"
    path = "E:\example\\"
    int = open(path + filename, 'r')
    teststr = int.read()
    test2str = strQ2B(teststr)
    int.close()
    print test2str
    out = open(path+"e"+filename, 'a+')
    out.write(test2str)
    out.close()
