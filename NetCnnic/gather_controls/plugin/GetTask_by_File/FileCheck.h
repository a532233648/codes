/*************************************************************************
    > File Name: FileCheck.h
    > Author: lxf
    > Mail: blueskytoday@126.com 
    > Created Time: Fri 08 May 2015 08:28:41 PM CST
 ************************************************************************/
#ifndef _H_CLASS_FILECHECK_H_
#define _H_CLASS_FILECHECK_H_
#include<iostream>
#include <stdio.h>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <string>
using namespace std;

class FileCheck{
	private:
		string path;
		string filename;	//filename
		string stat_file;	//name of stat_file
		string tmp_result_file;	//name of tmp_result_file
		string result_file;	//name of result_file
		ofstream ofs_stat_file;	//stat_file handle
		ofstream ofs_tmp_file;	//stat_file handle
		int timeout;		//time-out value

		map<string, int> line_map;	//line,statcode
		int undeal_num;
		int set_line_stat(string &line, int statcode);	//0-processing	1-processed

		//func for test
		void show_line_map();
	public:
		//FileCheck(){ }
		FileCheck(const char *dir, const char *filename, int timeout=10);
		~FileCheck(){
			ofs_stat_file.close();
			ofs_tmp_file.close();
		}
		int IsReady();
		int IsDealed();
		//int ReadLines(){ };
		//int ReadLines(queue<string> &line_queue,queue<string> &temp_visited);
        int ReadLines(queue<string> &line_queue,queue<string> &temp_visited);
		int DealLine(string &line, string &result,int write_num);
		string &GetFilename(){
			return filename;
		}
        
};

class DirCheck{
	private:
		string path;	//dir path
		int max_file_num;		//max num of file in dir
		FileCheck **files;	//file in dir
		int file_ind;
		int file_count;
		int timeout;	//time-out value

		queue<string> task_queue, result_queue;
        queue<string> first_ip_queue, last_ip_queue,visit_ip_queue;//start ip,end ip,visit ip
        queue<string> visiting_ip_queue;
        queue<string> visited_ip_queue;
		vector<string> file_list;	//file list that not has been dealed
		vector<string> file_dealed_list;	//already dealed
		//map<string, int> file_map;
        //char visi_ip[1024];
		int IsDatafile(char *filename);
		int IsNewfile(char *filename);
		int IsDealed(char *filename);

		//test
		void show_taskqueue();
	public:
         //char visi_ip[1024];
		DirCheck(char *dir, int num=10, int timeout=10){
			max_file_num = num;
			this->timeout = timeout;
			path = dir;
			file_ind = 0;
			file_count = 0;
			files = new FileCheck*[num];
		}
		~DirCheck(){
			//???
			for (int i=0; i<max_file_num; i++){
				/*
				if (files[i]->GetFilename() != ""){
					cout<<"delete:"<<i<<endl;
					cout<<files[i]->GetFilename()<<endl;
					//delete files[i];
				}
				*/
			}
			delete []files;
		}
		int ScanDir();	//scan dir,search for ready file
		int Dealed(string &filename, string &line, string &result,int write_num);
		int Dealed(string &format_result,int write_num);
		int GetTask(string &);
        
        int IpAddpre(string &task);
        int DividIp(char divid_ip[],char first_ip[],char last_ip[]);
        int AddQueue(char Add_ip[]);
        int DividOneip(char ip[],int ip_num[]);
        int myitoa(int num,char *string);
        int tempoutput(queue<string>ip_queue);
        int judge_ip(char ip[]);
        int comtwoip(char ip1[],char ip2[]);
        int comipqueue(char comp_ip[]);
        int ipend(char end_ip[]);
        int LastPos(char s[], char ch);
        int RightStr(char s[], char t[], int n);

};

#endif

