#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os
import sys


def readtxt(path, txtfile):
    """

    Parameters
    ----------
    path
    txtfile

    Returns
    -------

    """
    newslist = []
    txtout = open(path + txtfile, 'r')
    for line in txtout.readlines():
        newslist.append(line)
    txtout.close()
    return newslist


def writexml(newslist, path, xmlfile):
    out = open(path + xmlfile, 'a+')
    for line in newslist:
        line = line.split('\t')
        date = '"' + line[0] + '"'
        id = '"' + line[1] + '"'
        source = '"' + line[2] + '"'
        news = line[3]
        out.write("<Sentence date = " + date + " id =" + id + " source = " + source + ' sentence = "True">' + '\n')
        out.write("<Text>" + '\n' + news + "</Text>" + '\n')
        out.write("<Parse>" + '\n' + "</Parse>" + '\n')
        out.write("</Sentence>" + '\n')
    out.close()


def exepetrarch(xmlfile, path):
    outfile = 'test_out.txt'
    command = 'petrarch parse -i ' + path + xmlfile + ' -o ' + path + outfile
    os.system(command)


if __name__ == '__main__':
    path = "E:\\example\\"
    txtfile = "bbcnews1.txt"
    xmlfile = "example2.xml"
    newslist = readtxt(path, txtfile)
    out = open(path + xmlfile, 'a')
    out.write("<Sentences>" + '\n')
    out.close()
    writexml(newslist, path, xmlfile)
    out = open(path + xmlfile, 'a+')
    out.write("</Sentences>")
    out.close()
    # os.system('petrarch parse -i
    # E:\\work\\event\\petrarch_win32\\example\\example2.xml -o
    # E:\\work\\event\\petrarch_win32\\example\\out.txt')
    exepetrarch(xmlfile, path)
    # 'petrarch parse -i
# E:\\example\\example2.xml -o
# E:\\example\\test_out.txt'
