#! /usr/bin/env python
# coding = utf-8

'''
@version:??
@author:rock zhou
@file: parser.py
@time: 2016/3/26 16:29
'''

import argparse
import os
import sys
import re


def a_parser():
    parser = argparse.ArgumentParser()
    # parser.add_argument("echo", help="echo the string you use here")
    parser.add_argument("square", help="display a square of a given number",type=int)
    args = parser.parse_args()
    # print args.echo
    print args.square**2

def filepathh():
    print os.path.dirname(__file__)
    cwd = os.path.abspath(__file__)
    print cwd

def recomp(paragr):
    MIN_SENTLENGTH = 100
    MAX_SENTLENGTH = 512
    terpat = re.compile('[\.\?!]+[A-Z\"]')
    ABBREV_LIST = ['mrs.', 'ms.', 'mr.', 'dr.', 'gov.', 'sr.', 'rev.', 'r.n.',
                   'pres.', 'treas.', 'sect.', 'maj.', 'ph.d.', 'ed. psy.',
                   'proc.', 'fr.', 'asst.', 'p.f.c.', 'prof.', 'admr.',
                   'engr.', 'mgr.', 'supt.', 'admin.', 'assoc.', 'voc.',
                   'hon.', 'm.d.', 'dpty.', 'sec.', 'capt.', 'c.e.o.',
                   'c.f.o.', 'c.i.o.', 'c.o.o.', 'c.p.a.', 'c.n.a.', 'acct.',
                   'llc.', 'inc.', 'dir.', 'esq.', 'lt.', 'd.d.', 'ed.',
                   'revd.', 'psy.d.', 'v.p.', 'senr.', 'gen.', 'prov.',
                   'cmdr.', 'sgt.', 'sen.', 'col.', 'lieut.', 'cpl.', 'pfc.',
                   'k.p.h.', 'cent.', 'deg.', 'doz.', 'Fahr.', 'Cel.', 'F.',
                   'C.', 'K.', 'ft.', 'fur.', 'gal.', 'gr.', 'in.', 'kg.',
                   'km.', 'kw.', 'l.', 'lat.', 'lb.', 'lb per sq in.', 'long.',
                   'mg.', 'mm.,, m.p.g.', 'm.p.h.', 'cc.', 'qr.', 'qt.', 'sq.',
                   't.', 'vol.', 'w.', 'wt.']

    sentlist = []
    terloc = terpat.search(paragr)
    while terloc:
        isok = True
        if paragr[terloc.start()] == '.':
            if (paragr[terloc.start() - 1].isupper() and
                    paragr[terloc.start() - 2] == ' '):
                        isok = False      # single initials
            else:
                # check abbreviations
                loc = paragr.rfind(' ', 0, terloc.start() - 1)
                if loc > 0:
                    if paragr[loc + 1:terloc.start() + 1].lower() in ABBREV_LIST:
                        isok = False
        if paragr[:terloc.start()].count('(') != paragr[:terloc.start()].count(')'):
            isok = False
        if paragr[:terloc.start()].count('"') % 2 != 0:
            isok = False
        if isok:
            if (len(paragr[:terloc.start()]) > MIN_SENTLENGTH and
                    len(paragr[:terloc.start()]) < MAX_SENTLENGTH):
                sentlist.append(paragr[:terloc.start()+1])
            paragr = paragr[terloc.end() - 1:]
            searchstart = 0
        else:
            searchstart = terloc.start() + 2
        terloc = terpat.search(paragr, searchstart)
    if (len(paragr) > MIN_SENTLENGTH and len(paragr) < MAX_SENTLENGTH):
        sentlist.append(paragr)
    return sentlist


if __name__ == '__main__':
    #a_parser()
    #filepathh()
    int=open('E:\example\\12345out.txt','r')
    paragr=int.read()
    #paragr=  paragr.replace('\n', '').replace('  ', '')
    int.close()
    sentlist=recomp(paragr)
    out=open('E:\example\\123out.txt','a+')
    for sent in sentlist:
       out.write(sent+'\n')
    out.close()
