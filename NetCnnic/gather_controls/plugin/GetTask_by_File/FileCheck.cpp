/*************************************************************************
    > File Name: FileCheck.cpp
    > Author: lxf
    > Mail: blueskytoday@126.com 
    > Created Time: Fri 08 May 2015 08:47:15 PM CST
 ************************************************************************/

#include<iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include "FileCheck.h"
using namespace std;

/*
 * Desc
 *		constructor
 */
FileCheck::FileCheck(const char *dir, const char *filename, int timeout){
	//cout<<"filename:"<<filename<<endl;
	path = dir;
	if (path[path.length()-1]!='/')
		path+="/";
	//this->filename  = (string)dir+filename;
	this->filename  = filename;
	this->timeout = timeout;
	stat_file = path+string(".stat.")+filename;
	tmp_result_file = path+string(".tmp.")+filename;
	result_file = path+filename+string(".rst");

	undeal_num = 0;

	ofs_stat_file.open(stat_file.c_str(), ios::out);
	if (!ofs_stat_file.is_open()){
		perror("open stat file failed");
	}
	ofs_tmp_file.open(tmp_result_file.c_str(), ios::app);
	if (!ofs_tmp_file.is_open()){
		perror("open tmp result file failed");
	}
	//ofs_tmp_file<<"test"<<endl;

	//cout<<this->filename<<"\t"<<stat_file<<"\t"<<tmp_result_file<<"\t"<<result_file<<endl;
}

/*
 * Desc
 *		record stat of line
 * Return Value
 *		1	sucess
 */
int FileCheck::set_line_stat(string &line, int statcode){
	//line_map.insert(pair<string, int>(line, statcode));
	line_map[line] = statcode;
	ofs_stat_file<<line<<":\t"<<statcode<<endl;
	ofs_stat_file.flush();
	return 1;
}

int FileCheck::IsReady(){
	struct stat file_stat;
	string fname = path+filename;
	//cout<<"file:"<<this->filename<<endl;
	if (-1 == lstat(fname.c_str(), &file_stat)){
		perror("stat");
		return -1;
	}
	/*
	cout<<file_stat.st_dev<<endl;
	cout<<file_stat.st_ino<<endl;
	cout<<file_stat.st_mode<<endl;
	cout<<file_stat.st_nlink<<endl;
	cout<<file_stat.st_uid<<endl;
	cout<<file_stat.st_gid<<endl;
	cout<<file_stat.st_rdev<<endl;
	cout<<file_stat.st_size<<endl;
	cout<<file_stat.st_blksize<<endl;
	cout<<file_stat.st_blocks<<endl; 
	cout<<file_stat.st_atime<<endl;
	cout<<file_stat.st_mtime<<endl;  
	cout<<file_stat.st_ctime<<endl;
	cout<<"Now:"<<time(NULL)<<endl;
	*/
	if ((time(NULL) - file_stat.st_ctime)>timeout){
		cout<<__func__<<":file ready.\n";
		return 1;
	}
	cout<<__func__<<":file not ready.\n";
	return 0;
}

/*
 */
int FileCheck::IsDealed(){
	return !undeal_num;
}

/*
 * Desc
 *		display items in line_map
 */
void FileCheck::show_line_map(){
	//show line_map
	for(map<string,int>::iterator it = line_map.begin(); it!=line_map.end(); it++){
		cout<<it->first<<"\t"<<it->second<<endl;
	}
}

/*
 * Desc:
 *		read lines from file
 * Return Value:
 *		1	ok
 */
int FileCheck::ReadLines(queue<string> &line_queue,queue<string> &temp_visited){
	string fname = path+filename;
	//get ok lines
	map<string, int> ok_lines;
	ifstream ifs_ok((path+".tmp."+filename).c_str());
    //cout<<"[Debug path +filename :]"<<path<<".tmp."<<filename<<endl;
	if (ifs_ok.is_open()){
		while (!ifs_ok.eof()){
			char tmp_ok_line[1024];
			char *p, *p_save;
			if (!ifs_ok.getline(tmp_ok_line, 1024).good() && !ifs_ok.eof()){
				ifs_ok.clear();
			}
			//cout<<"[Debug]tmp_ok_line:"<<tmp_ok_line<<endl;
			if (strcmp(tmp_ok_line, "")){
				if (tmp_ok_line[strlen(tmp_ok_line)-1] == '\r'){
					tmp_ok_line[strlen(tmp_ok_line)-1] = '\0';
				}
				p = strtok_r(tmp_ok_line, "\t", &p_save);
				if (!p)	continue;
				string str_ok_line = p;
				//cout<<"[Debug ok]:"<<str_ok_line<<endl;
				ok_lines.insert(pair<string,int>(str_ok_line, 1));
                
			}
		}
		ifs_ok.close();
	}
	//string fname = filename;
	ifstream ifs_file(fname.c_str());
    //cout<<"[Debug fname:]"<<fname<<endl;
	if (!ifs_file.is_open()){
		perror("ReadLines open file failed");
	}
	cout<<"[Debug]here\n";
	int linenum = 0;
	while (!ifs_file.eof()){
		linenum++;

		char tmp_line[1024];
		string str_line;
		ifs_file.getline(tmp_line, 1024);
		if (strcmp(tmp_line, "")){
			if (tmp_line[strlen(tmp_line)-1] == '\r'){
				tmp_line[strlen(tmp_line)-1] = '\0';
			}
			str_line = tmp_line;
			if (ok_lines.end() == ok_lines.find(str_line)){
				line_queue.push(filename+(string)"|"+str_line);
                temp_visited.push(str_line);
				set_line_stat(str_line, 0);
				undeal_num++;
				cout<<undeal_num<<"\t"<<tmp_line<<endl;
			}
		}
	}

	ifs_file.close();
	return 1;
}

int FileCheck::DealLine(string &line, string &result,int write_num){
	/*if (line_map.end() == line_map.find(line)){
		cerr<<"line not in file\n";
		return -1;
	}*/
	//write result into tmp_file
    if(write_num==1){
        cout<<"write:"<<line<<endl;
	    //ofs_tmp_file<<line<<"\t"<<result<<endl;
        ofs_tmp_file<<result<<endl;
	    ofs_tmp_file.flush();
    }
	//cout<<"write:"<<line<<endl;
	//ofs_tmp_file<<line<<"\t"<<result<<endl;
    //ofs_tmp_file<<result<<endl;
	//ofs_tmp_file.flush();
	//line_map[line] = 1;
	set_line_stat(line, 1);

	undeal_num--;

	//test
	//show_line_map();
	return 1;
}

/*
 * Desc
 *		check file, filt stat_file and result file
 */
int DirCheck::IsDatafile(char *filename){
	if ('.' == filename[0]){
		return 0;
	}
	if (!strcmp(filename+strlen(filename)-4, ".rst")){
		return 0;
	}
	//cout<<"It's data file.\n";
	return 1;
}

int DirCheck::IsNewfile(char *filename){
	for (vector<string>::iterator it=file_list.begin(); it!=file_list.end(); it++){
		if (*it == string(filename)){
			cout<<"not new file.\n";
			return 0;
		}
	}
	cout<<"new file.\n";
	return 1;
}

void DirCheck::show_taskqueue(){
	string tmp;
	cout<<"task size:"<<task_queue.size()<<endl;
	/*
	while (task_queue.size()){
		cout<<task_queue.front()<<endl;
		task_queue.pop();
	}
	*/
}

int DirCheck::IsDealed(char *filename){
	string fname = filename;
	for (vector<string>::iterator it=file_dealed_list.begin(); it!=file_dealed_list.end(); it++){
		if (*it == fname){
			return 1;
		}
	}
	return 0;
}

int DirCheck::ScanDir(){
	cout<<"in "<<__func__<<" "<<file_count<<" "<<max_file_num<<endl;
	DIR *dp;
	struct dirent *dir_entry;
	if (NULL == (dp = opendir(path.c_str()))){
		//cerr<<"open dir failed.\n";
		perror("open dir failed:");
		return -1;
	}
	while (dir_entry=readdir(dp)){
		cout<<"find file:"<<dir_entry->d_name<<endl;
		//find new file
		if (IsDatafile(dir_entry->d_name) && IsNewfile(dir_entry->d_name)){
			//cout<<"[Debug]"<<file_count<<"\t"<<max_file_num<<endl;
			if (file_count != 0 && file_count >= max_file_num){
				//exceed file max num
				cout<<"[Error]exceed file max num\n";
				return -1;
				//continue;
			}
			files[file_ind] = new FileCheck(path.c_str(), dir_entry->d_name);
			if (!files[file_ind]){
				//new error
				cout<<"[Error]new FileCheck failed.\n";
				return -1;
			}
			if (files[file_ind]->IsReady()&& !IsDealed((char *)files[file_ind]->GetFilename().c_str())){
				file_list.push_back((string)dir_entry->d_name);
				//read
				files[file_ind]->ReadLines(task_queue,visited_ip_queue);
				file_ind++;
				cout<<"find new file.\n";
			} else{
				delete files[file_ind];
				file_ind--;
				return 0;
			}
		}
		//read file
		/*
		for(int i=0; i<file_ind; i++){
			if (files[i]->IsReady() && !IsDealed((char *)files[i]->GetFilename().c_str())){
				files[i]->ReadLines(task_queue);
			}
		}
		*/
		//test
		/*
		if (!IsDealed((char *)files[0]->GetFilename().c_str())){

			//string tmp_fn = "./data/test.data";
			string tmp_fn = "test.data";
			string tmp_line = "a";
			string tmp_res = "1";
			Dealed(tmp_fn, tmp_line, tmp_res);
			tmp_line = "b";
			tmp_res = "2";
			Dealed(tmp_fn, tmp_line, tmp_res);
			tmp_line = "c";
			tmp_res = "3";
			Dealed(tmp_fn, tmp_line, tmp_res);
		}
		*/
		//show_taskqueue();
		//file_dealed_list.push_back(files[0]->GetFilename());
	}
	closedir(dp);
	return 1;
}

int DirCheck::Dealed(string &filename, string &line, string &result,int write_num){
	cout<<"in "<<__func__<<" file:"<<filename<<endl;
	for(int i=0; i<file_ind; i++){
		if (files[i]->GetFilename() == filename){
			//string tmp_res = filename+"|"+result;
			//cout<<"[Debug file Getfilename]"<<files[i]->GetFilename()<<endl;
            files[i]->DealLine(line, result,write_num);
			if (files[i]->IsDealed()){
				file_dealed_list.push_back(filename);
			}
		}
	}
	return 1;
}

int DirCheck::Dealed(string &format_result,int write_num){
	char *tmp_result = new char[format_result.length()+1];
	strcpy(tmp_result, format_result.c_str());
	string fname = strtok(tmp_result, "|");
	string line = strtok(NULL, "|");
	string result = strtok(NULL, "|");
	return Dealed(fname, line ,result,write_num);
}

int DirCheck::GetTask(string &task){
	cout<<"[Debug task_queue size:]"<<task_queue.size()<<endl;
    cout<<"[Debug visiting_ip_queue size:]"<<visiting_ip_queue.size()<<endl;
	//cout<<"show taskque:\n";
	//show_taskqueue();
	if (task_queue.size()){
		task = task_queue.front();
        cout<<"[Debug get task:]"<<task<<endl;
		task_queue.pop();
		return 1;
	}
    else if(visiting_ip_queue.size()){
        task = visiting_ip_queue.front();
        cout<<"[Debug get visiting queue task:]"<<task<<endl;
		visiting_ip_queue.pop();
		return 1;
    }
	return 0;
}

int DirCheck::IpAddpre(string &task){
            //cout<<"[Debug ipaddpre Task]"<<task<<endl;
            char temp_task[1024];
            memset(temp_task,'\0',sizeof(temp_task));
            strcpy(temp_task,task.c_str());
            //cout<<"[Debug temp_task]:"<<temp_task<<endl;
            char *s_temp,*e_temp,*p_save;
            s_temp=strtok_r(temp_task,"|",&p_save);
            e_temp=strtok_r(NULL,"|",&p_save);
            //cout<<"[Debug s_temp e_temp]:"<<s_temp<<e_temp<<endl;
            //strcat(visi_ip,e_temp);
            task=s_temp+(string)"|"+(string)"ipwhois.cnnic.cn/bns/query/Query/ipwhoisQuery.do?queryOption=ipv4&txtquery="+e_temp;
            //cout<<"[Debug after temp_task]:"<<temp_task<<endl;
            //visited_ip_queue.push(e_temp);
            return 0;
        
}

int DirCheck::DividIp(char divid_ip[],char first_ip[],char last_ip[]){
    int divid_loc,i,j;
    for(i=0;i<strlen(divid_ip);i++){
        //if(strcmp(divid_ip[i],'-')==0&&strcmp(divid_ip[i-1]," ")==0&&strcmp(divid_ip[i+1]," ")==0)divid_loc=i;
        if(divid_ip[i]=='-'){
            divid_loc=i;
            break;
        }
    }
    strncpy(first_ip,divid_ip,divid_loc-1);
    for(i=divid_loc+2,j=0;i<strlen(divid_ip),j<strlen(divid_ip)-divid_loc+1;i++,j++){
        last_ip[j]=divid_ip[i];
    }
    return 0;
    
}

int DirCheck::myitoa(int i, char *string)
{
        int power=0,j=0;

        j=i;
        for( power=1;j>=10;j/=10)
                power*=10;

        for(;power>0;power/=10)
        {
                *string++='0'+i/power;
                i%=power;
        }
        *string='\0';
        //printf("%s\n",string);
        return 0;
}

int DirCheck::DividOneip(char ip[],int ip_num[]){
    char tmp_ip[1024],temp[1024];
    strcpy(tmp_ip,ip);
    int k=0,j=0;
    for(int i=0;i<=strlen(tmp_ip);i++){
        //cout<<"in "<<__func__ << k <<endl;
        temp[j]=tmp_ip[i];
        j++;
        if(tmp_ip[i]=='.'||tmp_ip[i]=='\0'){
            temp[j]='\0';
            ip_num[k]=atoi(temp);
            k++;
            j=0;
        }
    }
    k=0;
    return 0;

}

int DirCheck::AddQueue(char Add_ip[]){
    cout<<"[Debug Add_ip:]"<<Add_ip<<endl;
    string finame=files[0]->GetFilename();
    //cout<<"[Debug finame:]"<<finame<<endl;
    int ip_num[5];
    int j=3;
    char tp_ip[1024];
    char str_1[5],str_2[5],str_3[5],str_4[5],str[]=".";
    char *temp_ip;
    //first_ip_queue.push(first_ip);
    //last_ip_queue.push(last_ip);
    strcpy(tp_ip,Add_ip);
    DividOneip(tp_ip,ip_num);
    //for(int sa=0;sa<4;sa++)cout<<"[de:]"<<ip_num[sa]<<"\t";
    //cout<<"\n";
    ip_num[2]++;
    char str_one[5],str_two[5],str_thr[5],str_four[5];
    j=2;
    while(j>=0){
        if(ip_num[j]>255&&j>0){
            ip_num[j]=ip_num[j]-256;
            ip_num[j-1]=ip_num[j-1]+1;
        }
        j--;
    }
    myitoa(ip_num[0],str_1);
    myitoa(ip_num[1],str_2);
    myitoa(ip_num[2],str_3);
    myitoa(ip_num[3],str_4);
    //for(int sa=0;sa<4;sa++)cout<<"[lde:]"<<ip_num[sa]<<"\t";
    //cout<<"\n";
    //cout<<"[strde]"<<str_1<<"\t"<<str_2<<"\t"<<str_3<<"\t"<<str_4<<"\n";
    temp_ip=new char[1024];
    myitoa(ip_num[0],str_1);
    strcpy(temp_ip,str_1);
    strcat(temp_ip,".");
    myitoa(ip_num[1],str_2);
    strcat(temp_ip,str_2);
    strcat(temp_ip,".");
    myitoa(ip_num[2],str_3);
    strcat(temp_ip,str_3);
    strcat(temp_ip,".");
    myitoa(ip_num[3],str_4);
    strcat(temp_ip,str_4);
    //cout<<"[Debug visiting ip:]"<<temp_ip<<endl;
    visiting_ip_queue.push(finame+(string)"|"+temp_ip);
    //visiting_ip_queue.push(temp_ip);
    //tempoutput(visited_ip_queue);
    delete []temp_ip;
    return 0;
}

int DirCheck::tempoutput(queue<string> ip_queue){
   if(ip_queue.size()){
        //cout<<"[Debug2 queue ip:]"<<ip_queue.back()<<endl;
        ip_queue.pop();
        //return 1;
        }
    return 0;
}

int DirCheck::judge_ip(char ip[]){
    int r1=strcmp(ip,"0.0.0.0");
    int r2=strcmp(ip,"255.255.255.255");
    if(r1==0)return 1;
    else if(r2==0)return 2;
    else return 0;
}


int DirCheck::comtwoip(char ip1[],char ip2[]){
    int ip1_num[5],ip2_num[5];
    if(strcmp(ip1,ip2)==0)return 0;
    DividOneip(ip1,ip1_num);
    DividOneip(ip2,ip2_num);
    //for(int i=0;i<4;i++)cout<<"[ip1_num]"<<ip1_num[i]<<'\t';
    //cout<<endl;
    //for(int i=0;i<4;i++)cout<<"[ip2_num]"<<ip2_num[i]<<'\t';
    //cout<<'\n';
    for(int i=0;i<4;i++){
        if(ip1_num[i]>ip2_num[i])return 1;
        else if(ip1_num[i]<ip2_num[i]) return 2;
    }
    return 3;
}

int DirCheck::comipqueue(char comp_ip[]){
    char ip[10][1024]={"0.0.0.255","25.0.0.255","50.0.0.255","75.0.0.255","103.0.0.255","125.0.0.255","150.0.0.255","175.0.0.255","200.0.0.255","225.0.0.255"};
    char temp[1024],first_ip[1024],last_ip[1024];
    DividIp(comp_ip,first_ip,last_ip);
    for(int i=0;i<10;i++){
        if(comtwoip(first_ip,ip[i])==2&&comtwoip(last_ip,ip[i])==1)return 1;
        //else if(comtwoip(first_ip,ip[i])==3||comtwoip(last_ip,ip[i])==3)return 1;
    }
    return 0;
   
}

int DirCheck::ipend(char end_ip[]){
    int ip_num[5];
    DividOneip(end_ip,ip_num);
    if(ip_num[0]==255&&ip_num[1]==255&&ip_num[2]>255)return 1;
    else return 0;
}

int DirCheck::LastPos(char s[],char ch) {
    int i,pos = 0; //
    for(i = 0; s[i]; ++i)
        if(s[i] == ch) pos = i + 1;
    return pos;
}
 
// 复制s[]中右边的n个字符到t[]中
int DirCheck::RightStr(char s[], char t[], int n) {
    int i,j,len = strlen(s);
    t[0] = '\0';
    if(n >= len) strcpy(t,s);
    else if(n > 0 && n < len) {
        i = len - n;
        j = 0;
        while(t[j++] = s[i++])
            ;
    }
    //t[i]='\0';
    return 0;
}
