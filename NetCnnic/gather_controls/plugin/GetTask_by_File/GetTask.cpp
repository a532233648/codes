/*************************************************************************
    > File Name: GetTask.cpp
    > Author: lxf
    > Mail: blueskytoday@126.com 
    > Created Time: Sat 09 May 2015 03:17:43 PM CST
 ************************************************************************/

#include<iostream>
#include <string>
#include <string.h>

#include "FileCheck.h"
#include "GetTask.h"
#include "plugin_inter.h"
using namespace std;

int get_task_path(char *config, char *path){
	/*
	char *p, *p_save;
	strtok_r(tmp_conf, "\n", &p_save);
	*/
	char *path_head, *path_end;
	path_head = strstr(config, "FILEDIR:");
	if (!path_head){
		return 0;
	}
	path_head += 8;
	path_end = strstr(path_head, "\n");
	if ((path_end - path_head) <=0)
		return 0;
	memcpy(path, path_head, path_end - path_head);

	return 1;
}

static int task_num = 0;
map<string, long>task_time;
const int TIMEOUT = 60*10;	//10 minutes

long check_time = time(NULL);
void check_timeout(DirCheck &dc){
	if (time(NULL) - check_time < TIMEOUT){
		return;
	}
	check_time = time(NULL);
	for (map<string, long>::iterator it = task_time.begin(); it!=task_time.end(); it++){
		if ((time(NULL) - it->second) >= TIMEOUT){
			string tmp = it->first + "|timeout";
			dc.Dealed(tmp,0);
			task_time.erase(it);
			//dc.Dealed(str_filename, str_task, str_result);
		}
	}
}

void *Thread_Main(void *arg){
	CONTROL_PLUGIN_INFO_T *pinfo = (CONTROL_PLUGIN_INFO_T*)arg;
	char path[50] = "";
	
	if(pinfo==NULL)
		return NULL;
	if (!get_task_path(pinfo->config, path)){
		return NULL;
	}
		
	//cout<<"[Debug]"<<path<<endl;
	DirCheck dc(path);
	dc.ScanDir();
    //char visi_ip[1024];
	while(1){ 
		PLUGIN_INTER_STRUCT_T	result;
		
		if(pinfo->result_queue->Pop(&result)>0){
			//ask for task
			/*
			 * task format
			 *	filename|line
			 */
			if( result.task_rest_head.packet_type==PACKET_TYPE_REQUEST){
				string task;
				dc.ScanDir();
				/*
				do{
					dc.ScanDir();
					//sleep(10);
				}while(dc.GetTask(task));
				*/

				if (!dc.GetTask(task)){
					cout<<"get task stay.\n";
					continue;
				}
                
                /*
                //add preip ,dc.IpAddpre();
		        char temp_task[1024];
                strcpy(temp_task,task.c_str());
                //cout<<"[Debug temp_task]:"<<temp_task<<endl;
                char *s_temp,*e_temp,*p_save;
                s_temp=strtok_r(temp_task,"|",&p_save);
                e_temp=strtok_r(NULL,"|",&p_save);
                //cout<<"[Debug s_temp e_temp]:"<<s_temp<<e_temp<<endl;
                task=s_temp+(string)"|"+(string)"ipwhois.cnnic.cn/bns/query/Query/ipwhoisQuery.do?queryOption=ipv4&txtquery="+e_temp;
                //cout<<"[Debug after task]:"<<task<<endl;
				*/
                //cout<<"[Debug gettask ipaddpre Task]"<<task<<endl;
                if(!task.empty()){
                    dc.IpAddpre(task);
                }
                //dc.IpAddpre(task);
                //cout<<"[Debug visi_ip:]"<<visi_ip<<endl;
                //cout<<"[Debug after task]:"<<task<<endl;
                result.task_rest_head.control_id = pinfo->id;
				result.task_rest_head.packet_type = PACKET_TYPE_TASK;
				result.task_rest_head.cont_size = sizeof(uint32_t)+task.length()+1;
				result.content = (char *)realloc(result.content,result.task_rest_head.cont_size);
				strcpy(result.content+sizeof(uint32_t),task.c_str());
			 	while(pinfo->task_queue->Push(&result)<=0){
					usleep(10);
				}
				task_num ++;
				//record time
				task_time.insert(pair<string,long>(task, time(NULL)));
				//cout<<"[Debug]"<<task_num<<"new task:"<<task<<endl;
			}else if( result.task_rest_head.packet_type==PACKET_TYPE_RESULT){
				//has result
				/*
				 * result format:
				 *	filename|line|result
				 */
				char *p_filename, *p_task, *p_result,*p_save;
                char ip_content[1024];
				cout<<"in "<<__func__<<" get result size="<<result.task_rest_head.cont_size<<endl;
				if(result.task_rest_head.cont_size <= 8)
					continue;
				//cout<<"[Debug]"<<result.task_rest_head.cont_size-sizeof(uint32_t)<<endl;
				char *tmp_result = new char[result.task_rest_head.cont_size-2*sizeof(uint32_t)];
				if (!tmp_result){
					cout<<"[Error]new tmp_result failed.\n";
					continue;
				}
                bzero(tmp_result, sizeof(tmp_result));
                //cout<<"[Debug result in :] "<<result.content<<endl;
				strcpy(tmp_result, result.content+2*sizeof(uint32_t));
                //char *temp_s;
                //cout<<"[Debug]tmp_result = "<<strlen(tmp_result)<<endl;
                //temp_s=new char[strlen(tmp_result)];
                char temp_s[4096];
                bzero(temp_s, sizeof(temp_s));
                strcpy(temp_s,tmp_result);
				cout<<"in "<<__func__<<" get result:"<<tmp_result<<endl;
                //cout<<"[Debug after trans:]"<<tmp_result<<endl;
				p_filename = strtok_r(tmp_result, "|", &p_save);
				p_task = strtok_r(NULL, "|", &p_save);
				p_result = strtok_r(NULL, "|", &p_save);
                //ip_content = strtok_r(NULL,"|",&p_save);
                int temp_loc=dc.LastPos(temp_s,'|');
                dc.RightStr(temp_s,ip_content,strlen(temp_s)-temp_loc);
                //if (temp_s) cout<<"[Debug]"<<strlen(temp_s)-temp_loc<<"temp_s = "<<strlen(temp_s)<<endl;
                //if (temp_s) delete []temp_s;
                //ip_content=strrchr(temp_s,c);
                cout<<"[Debug IP_content :]"<<ip_content<<endl;
				//cout<<"[Debug in :] "<<p_filename<<" "<<p_task<<" "<<endl;
                //cout<<"[Debug]in "<<__func__<<":"<<p_filename<<" "<<p_task<<" "<<p_result<<endl;
                char old_ip[1024];
                strcpy(old_ip,p_task);
                char first_ip[1024],last_ip[1024],divid_ip[1024];
                strcpy(divid_ip,ip_content);
                memset(first_ip,'\0',sizeof(first_ip));
                memset(last_ip,'\0',sizeof(last_ip));
                dc.DividIp(divid_ip,first_ip,last_ip);
                if(dc.judge_ip(first_ip)==1&&dc.judge_ip(last_ip)==2){
                    //cout<<"[Debug judge 1:]"<<old_ip<<endl;
                    if(dc.ipend(old_ip)==0)dc.AddQueue(old_ip);
                }
                
                else if(dc.comipqueue(divid_ip)==0){
                    if(dc.judge_ip(first_ip)==0&&dc.judge_ip(last_ip)==2){
                            //cout<<"[Debug judge 2:]"<<endl;
                    }else if(dc.judge_ip(first_ip)==0&&dc.judge_ip(last_ip)==0){
                            //cout<<"[Debug judge 3:]"<<endl;
                            if(dc.ipend(old_ip)==0)dc.AddQueue(last_ip);
                            }
                }
				if (p_filename && p_task && p_result){
					cout<<"in "<<__func__<<" get result:"<<p_filename<<endl;
					cout<<"in "<<__func__<<" get result:"<<p_task<<endl;
					cout<<"in "<<__func__<<" get result:"<<p_result<<endl;
					string str_filename(p_filename), str_task(p_task), str_result(p_result);
                    if(dc.judge_ip(first_ip)==1&&dc.judge_ip(last_ip)==2){
                        dc.Dealed(str_filename, str_task, str_result,0);
                    }else {
                        dc.Dealed(str_filename, str_task, str_result,1);
                    }
					//cout<<"[Debug str_task]"<<str_task<<endl;
					map<string, long>::iterator tmp_it = task_time.find(str_filename+"|"+str_task);
					if(tmp_it != task_time.end()){
						task_time.erase(tmp_it);
					}
				} else{
					cout<<"[WARNING]result error.\n";
				}
               
                //cout<<"[Debug ip divid:]"<<first_ip<<"\n"<<last_ip<<endl;
                //cout<<"[Debug ip divid:]"<<divid_ip<<endl;
                
                int num123=0;
                //dc.tempoutput();
				delete []tmp_result;
				Release_PLUGIN_INTER_STRUCT_T(&result);
			} else{
			}
		}else{
			check_timeout(dc);
			usleep(1);
		}
	}
	
	return NULL;
}
